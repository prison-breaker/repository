#define MAX_LIGHTS		                              1
#define POINT_LIGHT		                              1
#define SPOT_LIGHT		                              2
#define DIRECTIONAL_LIGHT			                  3

#define DEPTH_BUFFER_WIDTH		                   2048
#define DEPTH_BUFFER_HEIGHT		                   2048
#define DELTA_X             (1.0f / DEPTH_BUFFER_WIDTH)
#define DELTA_Y            (1.0f / DEPTH_BUFFER_HEIGHT)

struct LIGHT
{
	bool					m_IsActive;

	float3					m_Position;
	float3					m_Direction;

	int						m_Type;

	float4					m_Color;

	float3					m_Attenuation;
	float 					m_Falloff;
	float					m_Range;
	float 					m_Theta;
	float					m_Phi;

	float					PADDING;

	float4x4				m_ToTexCoordMatrix;
};

// ====================================== ROOT SIGNATURE ======================================

cbuffer CB_FRAMEWORKINFO : register(b0)
{
	float TotalTime   : packoffset(c0.x);
	float ElapsedTime : packoffset(c0.y);
};

cbuffer CB_CAMERA : register(b1)
{
	matrix ViewMatrix		: packoffset(c0);
	matrix ProjectionMatrix : packoffset(c4);

	float3 CameraPosition   : packoffset(c8);
};

cbuffer CB_LIGHT : register(b2)
{
	LIGHT Lights[MAX_LIGHTS];
};

cbuffer CB_OBJECT : register(b3)
{
	matrix WorldMatrix : packoffset(c0);
};

Texture2D DiffuseMapTexture		  : register(t0);
Texture2D NormalMapTexture		  : register(t1);
Texture2D<float> ShadowMapTexture : register(t2);

SamplerState Sampler                    : register(s0);
SamplerComparisonState PCFShadowSampler : register(s1);

// ====================================== SHADOW FUNCTION ======================================

float Get3x3ShadowFactor(float2 ShadowTexCoord, float Depth)
{
	float Percent = ShadowMapTexture.SampleCmpLevelZero(PCFShadowSampler, ShadowTexCoord, Depth).r;
	Percent += ShadowMapTexture.SampleCmpLevelZero(PCFShadowSampler, ShadowTexCoord + float2(-DELTA_X, 0.0f), Depth).r;
	Percent += ShadowMapTexture.SampleCmpLevelZero(PCFShadowSampler, ShadowTexCoord + float2(+DELTA_X, 0.0f), Depth).r;
	Percent += ShadowMapTexture.SampleCmpLevelZero(PCFShadowSampler, ShadowTexCoord + float2(0.0f, -DELTA_Y), Depth).r;
	Percent += ShadowMapTexture.SampleCmpLevelZero(PCFShadowSampler, ShadowTexCoord + float2(0.0f, +DELTA_Y), Depth).r;
	Percent += ShadowMapTexture.SampleCmpLevelZero(PCFShadowSampler, ShadowTexCoord + float2(-DELTA_X, -DELTA_Y), Depth).r;
	Percent += ShadowMapTexture.SampleCmpLevelZero(PCFShadowSampler, ShadowTexCoord + float2(-DELTA_X, +DELTA_Y), Depth).r;
	Percent += ShadowMapTexture.SampleCmpLevelZero(PCFShadowSampler, ShadowTexCoord + float2(+DELTA_X, -DELTA_Y), Depth).r;
	Percent += ShadowMapTexture.SampleCmpLevelZero(PCFShadowSampler, ShadowTexCoord + float2(+DELTA_X, +DELTA_Y), Depth).r;

	return Percent /= 9.0f;
}

// ====================================== LIGHT FUNCTION ======================================

float4 DirectionalLight(int Index, float3 Normal, float2 TexCoord, float3 ToCamera)
{
	float3 ToLight = -Lights[Index].m_Direction;
	float DiffuseFactor = dot(ToLight, Normal);
	float4 DiffuseColor = DiffuseMapTexture.Sample(Sampler, TexCoord);

	return Lights[Index].m_Color * DiffuseFactor * DiffuseColor;
}

float4 Lighting(float3 Position, float3 Normal, float2 TexCoord, float4 ShadowTexCoord)
{
	float3 ToCamera = normalize(CameraPosition - Position);
	float4 Color = float4(0.0f, 0.0f, 0.0f, 0.0f);

	[unroll(MAX_LIGHTS)] for (int i = 0; i < MAX_LIGHTS; ++i)
	{
		if (Lights[i].m_IsActive)
		{
			float ShadowFactor = 1.0f;
			ShadowFactor = Get3x3ShadowFactor(ShadowTexCoord.xy / ShadowTexCoord.ww, ShadowTexCoord.z / ShadowTexCoord.w);

			if (Lights[i].m_Type == DIRECTIONAL_LIGHT)
			{
				Color += ShadowFactor * DirectionalLight(i, Normal, TexCoord, ToCamera);
			}
		}
	}

	return Color;
}


//float4 SpotLight(int Index, float3 Position, float3 Normal, float3 ToCamera, float2 TexCoord)
//{
//	float3 ToLight = Lights[Index].m_Position - Position;
//	float Distance = length(ToLight);
//	float DiffuseFactor = dot(ToLight, Normal);
//	float4 DiffuseColor = DiffuseMapTexture.Sample(Sampler, TexCoord);
//
//	if (Distance <= Lights[Index].m_Range)
//	{
//		ToLight /= Distance;
//
//		float Alpha = max(dot(-ToLight, Lights[Index].m_Direction), 0.0f);
//		float SpotFactor = pow(max(((Alpha - Lights[Index].m_Phi) / (Lights[Index].m_Theta - Lights[Index].m_Phi)), 0.0f), Lights[Index].m_Falloff);
//		float AttenuationFactor = 1.0f / dot(Lights[Index].m_Attenuation, float3(1.0f, Distance, Distance * Distance));
//
//		return Lights[Index].m_Color * DiffuseColor * DiffuseFactor;//2.0f * Lights[Index].m_Color + DiffuseColor * DiffuseFactor * SpotFactor * AttenuationFactor;
//	}
//
//	return float4(0.0f, 0.0f, 0.0f, 0.0f);
//}
//
//float4 Lighting(float3 Position, float3 Normal, float2 TexCoord, float4 ShadowTexCoord)
//{
//	float3 ToCamera = normalize(CameraPosition - Position);
//	float4 Color = float4(0.0f, 0.0f, 0.0f, 0.0f);
//
//	[unroll(MAX_LIGHTS)] for (int i = 0; i < MAX_LIGHTS; ++i)
//	{
//		if (Lights[i].m_IsActive)
//		{
//			float ShadowFactor = 1.0f;
//			ShadowFactor = Get3x3ShadowFactor(ShadowTexCoord.xy / ShadowTexCoord.ww, ShadowTexCoord.z / ShadowTexCoord.w);
//
//			if (Lights[i].m_Type == DIRECTIONAL_LIGHT)
//			{
//				Color += DirectionalLight(i, Normal, TexCoord, ToCamera) * ShadowFactor;
//			}
//			else if (Lights[i].m_Type == SPOT_LIGHT)
//			{
//				Color += SpotLight(i, Position, Normal, ToCamera, TexCoord);// *ShadowFactor;
//			}
//		}
//	}
//
//	return Color;
//}

// ====================================== BASIC SHADER ======================================

struct VS_INPUT
{
	float3 m_Position  : POSITION;
	float3 m_Normal    : NORMAL;
	float2 m_TexCoord  : TEXCOORD;
};

struct VS_OUTPUT
{
	float4 m_Position   : SV_POSITION;
	float3 m_PositionW  : POSITION;
	float3 m_NormalW    : NORMAL;
	float2 m_TexCoord   : TEXCOORD;
};

VS_OUTPUT VS_Main(VS_INPUT Input)
{
	VS_OUTPUT Output;

	Output.m_PositionW = mul(float4(Input.m_Position, 1.0f), WorldMatrix).xyz;
	Output.m_Position = mul(mul(float4(Output.m_PositionW, 1.0f), ViewMatrix), ProjectionMatrix);
	Output.m_NormalW = mul(Input.m_Normal, (float3x3)WorldMatrix);
	Output.m_TexCoord = Input.m_TexCoord;

	return Output;
}

//float4 PS_Main(VS_OUTPUT Input) : SV_TARGET
//{
//	float4 AlbedoColor = DiffuseMapTexture.Sample(Sampler, Input.m_TexCoord);
//
//	return AlbedoColor + Lighting(Input.m_PositionW, Input.m_NormalW);
//}

// ====================================== BASIC SHADER WITH NORMALMAP ======================================

struct VS_INPUT_WITH_NORMALMAP
{
	float3 m_Position  : POSITION;
	float3 m_Normal    : NORMAL;
	float3 m_Tangent   : TANGENT;
	float3 m_BiTangent : BITANGENT;
	float2 m_TexCoord  : TEXCOORD;
};

struct VS_OUTPUT_WITH_NORMALMAP
{
	float4 m_Position   : SV_POSITION;
	float3 m_PositionW  : POSITION;
	float3 m_NormalW    : NORMAL;
	float3 m_TangentW   : TANGENT;
	float3 m_BiTangentW : BITANGENT;
	float2 m_TexCoord   : TEXCOORD;
};

//VS_OUTPUT_WITH_NORMALMAP VS_Main_With_NormalMap(VS_INPUT_WITH_NORMALMAP Input)
//{
//	VS_OUTPUT_WITH_NORMALMAP Output;
//
//	Output.m_PositionW = mul(float4(Input.m_Position, 1.0f), WorldMatrix).xyz;
//	Output.m_Position = mul(mul(float4(Output.m_PositionW, 1.0f), ViewMatrix), ProjectionMatrix);
//	Output.m_NormalW = mul(Input.m_Normal, (float3x3)WorldMatrix);
//	Output.m_TangentW = mul(Input.m_Tangent, (float3x3)WorldMatrix);
//	Output.m_BiTangentW = mul(Input.m_BiTangent, (float3x3)WorldMatrix);
//	Output.m_TexCoord = Input.m_TexCoord;
//
//	return Output;
//}
//
//float4 PS_Main_With_NormalMap(VS_OUTPUT_WITH_NORMALMAP Input) : SV_TARGET
//{
//	float4 AlbedoColor = DiffuseMapTexture.Sample(Sampler, Input.m_TexCoord);
//
//	float3 Normal = NormalMapTexture.Sample(Sampler, Input.m_TexCoord).rgb;
//	Normal = 2.0f * Normal - 1.0f;
//
//	float3x3 TBN = float3x3(Input.m_TangentW, Input.m_BiTangentW, Input.m_NormalW);
//	float3 NormalW = mul(Normal, TBN);
//	
//	return AlbedoColor + Lighting(Input.m_PositionW, NormalW);
//}

// ====================================== SKYBOX SHADER ======================================

struct VS_INPUT_SKYBOX
{
	float3 m_Position : POSITION;
	float2 m_Size	  : SIZE;
};

struct VS_OUTPUT_SKYBOX
{
	float3 m_Center   : POSITION;
	float2 m_Size	  : SIZE;
};

struct GS_OUTPUT_SKYBOX
{
	float4 m_Position : SV_POSITION;
	float2 m_TexCoord : TEXCOORD;
};

VS_OUTPUT_SKYBOX VS_SkyBox(VS_INPUT_SKYBOX Input)
{
	VS_OUTPUT_SKYBOX Output;

	float4x4 CameraWorldMatrix = { 1.0f, 0.0f, 0.0f, 0.0f,
								   0.0f, 1.0f, 0.0f, 0.0f,
								   0.0f, 0.0f, 1.0f, 0.0f,
									CameraPosition,  1.0f };

	Output.m_Center = (float3)mul(float4(Input.m_Position, 1.0f), CameraWorldMatrix);
	Output.m_Size = Input.m_Size;

	return Output;
}

[maxvertexcount(4)]
void GS_SkyBox(point VS_OUTPUT_SKYBOX Input[1], inout TriangleStream<GS_OUTPUT_SKYBOX> OutStream)
{
	float3 Look = normalize(CameraPosition - Input[0].m_Center);
	float3 Up = float3(0.0f, 1.0f, 0.0f);

	if (Look.y < 0.0f)
	{
		Up = float3(0.0f, 0.0f, -1.0f);
	}
	else if (Look.y > 0.0f)
	{
		Up = float3(0.0f, 0.0f, +1.0f);
	}

	float3 Right = cross(Up, Look);

	float HalfWidth = 0.5f * Input[0].m_Size.x;
	float HalfHeight = 0.5f * Input[0].m_Size.y;

	float4 Vertices[4];

	Vertices[0] = float4(Input[0].m_Center + HalfWidth * Right - HalfHeight * Up, 1.0f);
	Vertices[1] = float4(Input[0].m_Center + HalfWidth * Right + HalfHeight * Up, 1.0f);
	Vertices[2] = float4(Input[0].m_Center - HalfWidth * Right - HalfHeight * Up, 1.0f);
	Vertices[3] = float4(Input[0].m_Center - HalfWidth * Right + HalfHeight * Up, 1.0f);

	float2 TexCoords[4];

	TexCoords[0] = float2(0.0f, 1.0f);
	TexCoords[1] = float2(0.0f, 0.0f);
	TexCoords[2] = float2(1.0f, 1.0f);
	TexCoords[3] = float2(1.0f, 0.0f);

	GS_OUTPUT_SKYBOX Output;

	for (int i = 0; i < 4; ++i)
	{
		Output.m_Position = mul(mul(Vertices[i], ViewMatrix), ProjectionMatrix).xyww;
		Output.m_TexCoord = TexCoords[i];

		OutStream.Append(Output);
	}
}

float4 PS_SkyBox(GS_OUTPUT_SKYBOX Input) : SV_TARGET
{
	return DiffuseMapTexture.Sample(Sampler, Input.m_TexCoord);
}

// ====================================== DEPTH WRITE SHADER ======================================

struct PS_OUTPUT_DEPTH
{
	float m_Position : SV_Target;
	float m_Depth    : SV_Depth;
};

PS_OUTPUT_DEPTH PS_DetphWrite(VS_OUTPUT Input)
{
	PS_OUTPUT_DEPTH Output;

	Output.m_Position = Output.m_Depth = Input.m_Position.z;

	return Output;
}

// ====================================== SHADOWMAP SHADER ======================================

struct VS_OUTPUT_SHADOW
{
	float4 m_Position		: SV_POSITION;
	float3 m_PositionW	    : POSITION;
	float3 m_NormalW	    : NORMAL;
	float2 m_TexCoord		: TEXCOORD0;
	float4 m_ShadowTexCoord : TEXCOORD1;
};

VS_OUTPUT_SHADOW VS_Shadow(VS_INPUT Input)
{
	VS_OUTPUT_SHADOW Output;

	float4 PositionW = mul(float4(Input.m_Position, 1.0f), WorldMatrix);
	Output.m_PositionW = PositionW.xyz;
	Output.m_Position = mul(mul(PositionW, ViewMatrix), ProjectionMatrix);
	Output.m_NormalW = mul(Input.m_Normal, (float3x3)WorldMatrix);
	Output.m_TexCoord = Input.m_TexCoord;

	if (Lights[0].m_IsActive)
	{
		Output.m_ShadowTexCoord = mul(PositionW, Lights[0].m_ToTexCoordMatrix);
	}

	return Output;
}

float4 PS_Shadow(VS_OUTPUT_SHADOW Input) : SV_TARGET
{
	return Lighting(Input.m_PositionW, Input.m_NormalW, Input.m_TexCoord, Input.m_ShadowTexCoord);
}

// ====================================== BASIC SHADER WITH NORMALMAP ======================================

struct VS_OUTPUT_SHADOW_WITH_NORMALMAP
{
	float4 m_Position		: SV_POSITION;
	float3 m_PositionW	    : POSITION;
	float3 m_NormalW	    : NORMAL;
	float3 m_TangentW	    : TANGENT;
	float3 m_BiTangentW	    : BITANGENT;
	float2 m_TexCoord	    : TEXCOORD0;
	float4 m_ShadowTexCoord : TEXCOORD1;
};

VS_OUTPUT_SHADOW_WITH_NORMALMAP VS_Shadow_With_NormalMap(VS_INPUT_WITH_NORMALMAP Input)
{
	VS_OUTPUT_SHADOW_WITH_NORMALMAP Output;

	float4 PositionW = mul(float4(Input.m_Position, 1.0f), WorldMatrix);
	Output.m_PositionW = PositionW.xyz;
	Output.m_Position = mul(mul(PositionW, ViewMatrix), ProjectionMatrix);
	Output.m_NormalW = mul(Input.m_Normal, (float3x3)WorldMatrix);
	Output.m_TangentW = mul(Input.m_Tangent, (float3x3)WorldMatrix);
	Output.m_BiTangentW = mul(Input.m_BiTangent, (float3x3)WorldMatrix);
	Output.m_TexCoord = Input.m_TexCoord;

	if (Lights[0].m_IsActive)
	{
		Output.m_ShadowTexCoord = mul(PositionW, Lights[0].m_ToTexCoordMatrix);
	}

	return Output;
}

float4 PS_Shadow_With_NormalMap(VS_OUTPUT_SHADOW_WITH_NORMALMAP Input) : SV_TARGET
{
	float3 TexNormal = 2.0f * NormalMapTexture.Sample(Sampler, Input.m_TexCoord).rgb - 1.0f;
	float3x3 TBN = float3x3(Input.m_TangentW, Input.m_BiTangentW, Input.m_NormalW);
	float3 NormalW = mul(TexNormal, TBN);

	return Lighting(Input.m_PositionW, NormalW, Input.m_TexCoord, Input.m_ShadowTexCoord);
}
