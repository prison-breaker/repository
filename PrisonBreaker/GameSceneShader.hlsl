#define MAX_LIGHTS		          1
#define LIGHT_TYPE_POINT		  1
#define LIGHT_TYPE_SPOT		      2
#define LIGHT_TYPE_DIRECTIONAL	  3
							      
#define DEPTH_BUFFER_WIDTH	      2048
#define DEPTH_BUFFER_HEIGHT	      2048
#define DELTA_X                   (1.0f / DEPTH_BUFFER_WIDTH)
#define DELTA_Y                   (1.0f / DEPTH_BUFFER_HEIGHT)

#define TEXTURE_MASK_ALBEDO_MAP	  0x01
#define TEXTURE_MASK_METALLIC_MAP 0x02
#define TEXTURE_MASK_NORMAL_MAP   0x04
#define TEXTURE_MASK_SHADOW_MAP   0x08

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

	float4 AlbedoColor : packoffset(c4);
	uint   TextureMask : packoffset(c5);
};

Texture2D AlbedoMapTexture		  : register(t0);
Texture2D MetallicMapTexture	  : register(t1);
Texture2D NormalMapTexture		  : register(t2);
Texture2D<float> ShadowMapTexture : register(t3);

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

float4 DirectionalLight(int Index, float3 Normal, float3 ToCamera)
{
	float3 ToLight = -Lights[Index].m_Direction;
	float AlbedoFactor = max(0.35f, dot(ToLight, Normal));

	return Lights[Index].m_Color * AlbedoFactor * AlbedoColor;
}

float4 Lighting(float3 Position, float3 Normal, float4 ShadowTexCoord)
{
	float3 ToCamera = normalize(CameraPosition - Position);
	float4 Color = float4(0.0f, 0.0f, 0.0f, 0.0f);

	[unroll(MAX_LIGHTS)] for (int i = 0; i < MAX_LIGHTS; ++i)
	{
		if (Lights[i].m_IsActive)
		{
			float ShadowFactor = 1.0f;
			ShadowFactor = max(0.3f, Get3x3ShadowFactor(ShadowTexCoord.xy / ShadowTexCoord.ww, ShadowTexCoord.z / ShadowTexCoord.w));

			if (Lights[i].m_Type == LIGHT_TYPE_DIRECTIONAL)
			{
				Color += ShadowFactor * DirectionalLight(i, Normal, ToCamera);
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
//	float4 DiffuseColor = AlbedoMapTexture.Sample(Sampler, TexCoord);
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

// ====================================== BASIC SHADER ======================================

struct VS_INPUT
{
	float3 m_Position  : POSITION;
	float3 m_Normal    : NORMAL;
	float3 m_Tangent   : TANGENT;
	float3 m_BiTangent : BITANGENT;
	float2 m_TexCoord  : TEXCOORD;
};

struct VS_OUTPUT
{
	float4 m_Position       : SV_POSITION;
	float3 m_PositionW      : POSITION;
	float3 m_NormalW        : NORMAL;
	float3 m_TangentW       : TANGENT;
	float3 m_BiTangentW     : BITANGENT;
	float2 m_TexCoord       : TEXCOORD0;
	float4 m_ShadowTexCoord : TEXCOORD1;
};

VS_OUTPUT VS_Main(VS_INPUT Input)
{
	VS_OUTPUT Output;

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

float4 PS_Main(VS_OUTPUT Input) : SV_TARGET
{
	float4 Color = float4(0.0f, 0.0f, 0.0f, 0.0f);

	if (TextureMask & TEXTURE_MASK_ALBEDO_MAP)
	{
		Color += AlbedoMapTexture.Sample(Sampler, Input.m_TexCoord);
	}

	if (TextureMask & TEXTURE_MASK_METALLIC_MAP)
	{
		Color += MetallicMapTexture.Sample(Sampler, Input.m_TexCoord);
	}

	float3 NormalW = float3(0.0f, 0.0f, 0.0f);

	if (TextureMask & TEXTURE_MASK_NORMAL_MAP)
	{
		float3x3 TBN = float3x3(Input.m_TangentW, Input.m_BiTangentW, Input.m_NormalW);
		NormalW = normalize(mul(2.0f * NormalMapTexture.Sample(Sampler, Input.m_TexCoord).rgb - 1.0f, TBN));
	}
	else
	{
		NormalW = normalize(Input.m_NormalW);
	}

	float4 Illumination = Lighting(Input.m_PositionW, NormalW, Input.m_ShadowTexCoord);

	return lerp(Color, Illumination, 0.65f); //Color * Illumination;
}

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
	return AlbedoMapTexture.Sample(Sampler, Input.m_TexCoord);
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
