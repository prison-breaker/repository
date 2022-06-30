#define MAX_LIGHTS		             3
#define LIGHT_TYPE_POINT		     1
#define LIGHT_TYPE_SPOT		         2
#define LIGHT_TYPE_DIRECTIONAL	     3

#define MAX_BONES				     100
#define MAX_BONE_INFLUENCE_TO_VERTEX 4
				
#define CLIENT_WIDTH		         1920
#define	CLIENT_HEIGHT		         1080
#define DEPTH_BUFFER_WIDTH	         2048
#define DEPTH_BUFFER_HEIGHT	         2048
#define DELTA_X                      (1.0f / DEPTH_BUFFER_WIDTH)
#define DELTA_Y                      (1.0f / DEPTH_BUFFER_HEIGHT)
								     
#define TEXTURE_MASK_ALBEDO_MAP	     0x01
#define TEXTURE_MASK_METALLIC_MAP    0x02
#define TEXTURE_MASK_NORMAL_MAP      0x04
#define TEXTURE_MASK_SHADOW_MAP      0x08

struct MATERIAL
{
	float4 m_AlbedoColor;

	uint   m_TextureMask;
	float2 m_TextureScale;
};

struct FOG
{
	float4 m_Color;

	float  m_Density;
};

struct LIGHT
{
	bool	 m_IsActive;
			 
	float3	 m_Position;
	float3	 m_Direction;
			 
	uint	 m_Type;
			 
	float4	 m_Color;
			 
	float3	 m_Attenuation;
	float 	 m_Falloff;
	float	 m_Range;
	float 	 m_Theta;
	float	 m_Phi;
			 
	bool	 m_ShadowMapping;
			 
	float4x4 m_ToTexCoordMatrix;
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

cbuffer CB_FOG : register(b3)
{
	FOG Fog;
}

cbuffer CB_OBJECT : register(b4)
{
	matrix   WorldMatrix : packoffset(c0);

	MATERIAL Material    : packoffset(c4);
};

cbuffer CB_BONE_OFFSET : register(b5)
{
	matrix BoneOffsetMatrix[MAX_BONES];
}

cbuffer CB_BONE_TRANSFORM : register(b6)
{
	matrix BoneTransformMatrix[MAX_BONES];
}

Texture2D        AlbedoMapTexture   : register(t0);
Texture2D        MetallicMapTexture	: register(t1);
Texture2D        NormalMapTexture   : register(t2);
Texture2D<float> ShadowMapTexture   : register(t3);

SamplerState           Sampler          : register(s0);
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

// ====================================== FOG FUNCTION ======================================

float4 Fogging(float4 Color, float3 Position)
{
    float3 ToLight = Lights[2].m_Position - Position;
    float Distance = length(ToLight);
	
    if (Distance > Lights[2].m_Range)
    {
        float3 ToCamera = CameraPosition - Position;
        float DistanceToCamera = length(ToCamera);
        float FogFactor = 1.0f / exp(pow(DistanceToCamera * Fog.m_Density, 2));
        float4 FoggedColor = float4(lerp(Fog.m_Color.rgb, Color.rgb, FogFactor), Color.a);
		
        return FoggedColor;
    }
	
    return Color;
}

// ====================================== LIGHT FUNCTION ======================================

float4 PointLight(int Index, float3 Position, float3 Normal, float3 ToCamera)
{
    float3 ToLight = Lights[Index].m_Position - Position;
    float Distance = length(ToLight);
	
    if (Distance <= Lights[Index].m_Range)
    {
        ToLight /= Distance;

        float AlbedoFactor = dot(ToLight, Normal);
        float AttenuationFactor = 1.0f / dot(Lights[Index].m_Attenuation, float3(1.0f, Distance, Distance * Distance));
		
        return Lights[Index].m_Color * AlbedoFactor * AttenuationFactor;
    }
	
    return float4(0.0f, 0.0f, 0.0f, 0.0f);
}

float4 SpotLight(int Index, float3 Position, float3 Normal, float3 ToCamera)
{
	float3 ToLight = Lights[Index].m_Position - Position;
	float Distance = length(ToLight);

	if (Distance <= Lights[Index].m_Range)
	{
		ToLight /= Distance;

		float AlbedoFactor = dot(ToLight, Normal);
        float Alpha = max(dot(-ToLight, Lights[Index].m_Direction), 0.0f);
		float SpotFactor = pow(max((Alpha - Lights[Index].m_Phi) / (Lights[Index].m_Theta - Lights[Index].m_Phi), 0.0f), Lights[Index].m_Falloff);
		float AttenuationFactor = 1.0f / dot(Lights[Index].m_Attenuation, float3(1.0f, Distance, Distance * Distance));

		return Lights[Index].m_Color * AlbedoFactor * SpotFactor * AttenuationFactor;
	}

	return float4(0.0f, 0.0f, 0.0f, 0.0f);
}

float4 DirectionalLight(int Index, float3 Normal, float3 ToCamera)
{
    float3 ToLight = -Lights[Index].m_Direction;
    float AlbedoFactor = dot(ToLight, Normal);

    return Lights[Index].m_Color * AlbedoFactor;
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

			if (Lights[i].m_ShadowMapping)
			{
				ShadowFactor = Get3x3ShadowFactor(ShadowTexCoord.xy / ShadowTexCoord.ww, ShadowTexCoord.z / ShadowTexCoord.w);
			}

			switch (Lights[i].m_Type)
			{
			case LIGHT_TYPE_POINT:
                Color += ShadowFactor * PointLight(i, Position, Normal, ToCamera);
				break;
			case LIGHT_TYPE_SPOT:
				Color += ShadowFactor * SpotLight(i, Position, Normal, ToCamera);
				break;
			case LIGHT_TYPE_DIRECTIONAL:
				Color += ShadowFactor * DirectionalLight(i, Normal, ToCamera);
				break;
			}
		}
	}

	return Color;
}

// ====================================== STANDARD SHADER ======================================

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
	VS_OUTPUT Output = (VS_OUTPUT)0;

	float4 PositionW = mul(float4(Input.m_Position, 1.0f), WorldMatrix);

	Output.m_PositionW = PositionW.xyz;
	Output.m_Position = mul(mul(PositionW, ViewMatrix), ProjectionMatrix);
	Output.m_NormalW = mul(Input.m_Normal, (float3x3)WorldMatrix);
	Output.m_TangentW = mul(Input.m_Tangent, (float3x3)WorldMatrix);
	Output.m_BiTangentW = mul(Input.m_BiTangent, (float3x3)WorldMatrix);
	Output.m_TexCoord = Material.m_TextureScale * Input.m_TexCoord;
	Output.m_ShadowTexCoord = mul(PositionW, Lights[1].m_ToTexCoordMatrix);

	return Output;
}

float4 PS_Main(VS_OUTPUT Input) : SV_TARGET
{
	float4 Color = float4(0.0f, 0.0f, 0.0f, 0.0f);

	if (Material.m_TextureMask & TEXTURE_MASK_ALBEDO_MAP)
	{
		Color += AlbedoMapTexture.Sample(Sampler, Input.m_TexCoord) * Material.m_AlbedoColor;
	}
	else
	{
		Color += Material.m_AlbedoColor;
	}

	//if (Material.m_TextureMask & TEXTURE_MASK_METALLIC_MAP)
	//{
	//	Color += MetallicMapTexture.Sample(Sampler, Input.m_TexCoord);
	//}

	float3 NormalW = float3(0.0f, 0.0f, 0.0f);

	if (Material.m_TextureMask & TEXTURE_MASK_NORMAL_MAP)
	{
		float3x3 TBN = float3x3(Input.m_TangentW, Input.m_BiTangentW, Input.m_NormalW);

		NormalW = normalize(mul(2.0f * NormalMapTexture.Sample(Sampler, Input.m_TexCoord).rgb - 1.0f, TBN));
	}
	else
	{
		NormalW = normalize(Input.m_NormalW);
	}
	
	float4 Illumination = Lighting(Input.m_PositionW, NormalW, Input.m_ShadowTexCoord);

	Color = float4(lerp(Color.rgb, Illumination.rgb, 0.75f).rgb, Color.a);
	Color = Fogging(Color, Input.m_PositionW);

	return Color;
}

// ====================================== STANDARD SKINNING SHADER ======================================

struct VS_INPUT_SKINNING
{
	float3 m_Position    : POSITION;
	float3 m_Normal      : NORMAL;
	float3 m_Tangent     : TANGENT;
	float3 m_BiTangent   : BITANGENT;
	float2 m_TexCoord    : TEXCOORD;
	uint4  m_BoneIndices : BONEINDEX;
	float4 m_BoneWeights : BONEWEIGHT;
};

VS_OUTPUT VS_Main_Skinning(VS_INPUT_SKINNING Input)
{
	VS_OUTPUT Output = (VS_OUTPUT)0;
	float4x4 SkinnedWorldMatrix = (float4x4)0.0f;

	for (int i = 0; i < MAX_BONE_INFLUENCE_TO_VERTEX; ++i)
	{
		SkinnedWorldMatrix += Input.m_BoneWeights[i] * mul(BoneOffsetMatrix[Input.m_BoneIndices[i]], BoneTransformMatrix[Input.m_BoneIndices[i]]);
	}

	float4 PositionW = mul(float4(Input.m_Position, 1.0f), SkinnedWorldMatrix);

	Output.m_PositionW = PositionW.xyz;
	Output.m_Position = mul(mul(PositionW, ViewMatrix), ProjectionMatrix);
	Output.m_NormalW = mul(Input.m_Normal, (float3x3)SkinnedWorldMatrix);
	Output.m_TangentW = mul(Input.m_Tangent, (float3x3)SkinnedWorldMatrix);
	Output.m_BiTangentW = mul(Input.m_BiTangent, (float3x3)SkinnedWorldMatrix);
	Output.m_TexCoord = Material.m_TextureScale * Input.m_TexCoord;
	Output.m_ShadowTexCoord = mul(PositionW, Lights[1].m_ToTexCoordMatrix);

	return Output;
}

// ====================================== SKYBOX SHADER ======================================

struct VS_INPUT_SKYBOX
{
	float3 m_PositionW : POSITION;
	float2 m_SizeW	   : SIZE;
};

struct VS_OUTPUT_SKYBOX
{
	float3 m_CenterW : POSITION;
	float2 m_SizeW	 : SIZE;
};

struct GS_OUTPUT_SKYBOX
{
	float4 m_Position : SV_POSITION;
	float2 m_TexCoord : TEXCOORD;
};

VS_OUTPUT_SKYBOX VS_SkyBox(VS_INPUT_SKYBOX Input)
{
	VS_OUTPUT_SKYBOX Output = (VS_OUTPUT_SKYBOX)0;

	float4x4 CameraWorldMatrix = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		 CameraPosition,  1.0f
	};

	Output.m_CenterW = (float3)mul(float4(Input.m_PositionW, 1.0f), CameraWorldMatrix);
	Output.m_SizeW = Input.m_SizeW;

	return Output;
}

[maxvertexcount(4)]
void GS_SkyBox(point VS_OUTPUT_SKYBOX Input[1], inout TriangleStream<GS_OUTPUT_SKYBOX> OutStream)
{
	float3 Look = normalize(CameraPosition - Input[0].m_CenterW);
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

	float HalfWidth = 0.5f * Input[0].m_SizeW.x;
	float HalfHeight = 0.5f * Input[0].m_SizeW.y;

	float4 Vertices[4] = {
		float4(Input[0].m_CenterW + HalfWidth * Right - HalfHeight * Up, 1.0f),
		float4(Input[0].m_CenterW + HalfWidth * Right + HalfHeight * Up, 1.0f),
		float4(Input[0].m_CenterW - HalfWidth * Right - HalfHeight * Up, 1.0f),
		float4(Input[0].m_CenterW - HalfWidth * Right + HalfHeight * Up, 1.0f)
	};

	float2 TexCoords[4] = {
		float2(0.0f, 1.0f),
		float2(0.0f, 0.0f),
		float2(1.0f, 1.0f),
		float2(1.0f, 0.0f)
	};

	GS_OUTPUT_SKYBOX Output = (GS_OUTPUT_SKYBOX)0;

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

// ====================================== UI IMAGE SHADER ======================================

struct VS_INPUT_IMAGE
{
	float3 m_PositionS  : POSITION;
	float2 m_SizeS      : SIZE;
    float  m_AlphaColor : ALPHACOLOR;
	uint2  m_CellCount  : CELLCOUNT;
	uint   m_CellIndex  : CELLINDEX;
};

struct VS_OUTPUT_IMAGE
{
	float3 m_CenterS    : POSITION;
	float2 m_SizeS      : SIZE;
    float  m_AlphaColor : ALPHACOLOR;
	uint2  m_CellCount  : CELLCOUNT;
	uint   m_CellIndex  : CELLINDEX;
};

struct GS_OUTPUT_IMAGE
{
	float4 m_Position   : SV_POSITION;
	float2 m_TexCoord   : TEXCOORD;
    float  m_AlphaColor : ALPHACOLOR;
};

float3 TransScreenToCamera(float Xpos, float Ypos)
{
	// 뷰포트 좌표계를 카메라 좌표계로 변환한다.
	return float3(2.0f * Xpos / CLIENT_WIDTH - 1.0f, -2.0f * Ypos / CLIENT_HEIGHT + 1.0f, 0.0f);
}

VS_OUTPUT_IMAGE VS_Image(VS_INPUT_IMAGE Input)
{
	VS_OUTPUT_IMAGE Output = (VS_OUTPUT_IMAGE)0;

	Output.m_CenterS = Input.m_PositionS;
	Output.m_SizeS = Input.m_SizeS;
    Output.m_AlphaColor = Input.m_AlphaColor;
	Output.m_CellCount = Input.m_CellCount;
	Output.m_CellIndex = Input.m_CellIndex;

	return Output;
}

[maxvertexcount(4)]
void GS_Image(point VS_OUTPUT_IMAGE Input[1], inout TriangleStream<GS_OUTPUT_IMAGE> OutStream)
{
	float2 ScreenCoord = float2(Input[0].m_CenterS.x, Input[0].m_CenterS.y);
	float2 HalfLength = float2(0.5f * Input[0].m_SizeS.x, 0.5f * Input[0].m_SizeS.y); // x: Width, y: Height

	float4 Vertices[4] = {
		float4(TransScreenToCamera(ScreenCoord.x - HalfLength.x, ScreenCoord.y + HalfLength.y), 1.0f),
		float4(TransScreenToCamera(ScreenCoord.x - HalfLength.x, ScreenCoord.y - HalfLength.y), 1.0f),
		float4(TransScreenToCamera(ScreenCoord.x + HalfLength.x, ScreenCoord.y + HalfLength.y), 1.0f),
		float4(TransScreenToCamera(ScreenCoord.x + HalfLength.x, ScreenCoord.y - HalfLength.y), 1.0f)
	};

	float2 TexCoords[4] = {
		float2(0.0f, 1.0f),
		float2(0.0f, 0.0f),
		float2(1.0f, 1.0f),
		float2(1.0f, 0.0f)
	};

	if (Input[0].m_CellCount.x > 0 && Input[0].m_CellCount.y > 0)
	{
		float2 CellSize = float2(1.0f / Input[0].m_CellCount.y, 1.0f / Input[0].m_CellCount.x);

		for (int i = 0; i < 4; ++i)
		{
			TexCoords[i].x = CellSize.x * TexCoords[i].x + CellSize.x * (Input[0].m_CellIndex % Input[0].m_CellCount.y);
			TexCoords[i].y = CellSize.y * TexCoords[i].y + CellSize.y * (Input[0].m_CellIndex / Input[0].m_CellCount.y);
		}
	}

	GS_OUTPUT_IMAGE Output = (GS_OUTPUT_IMAGE)0;

	for (int j = 0; j < 4; ++j)
	{
		Output.m_Position = Vertices[j];
		Output.m_TexCoord = TexCoords[j];
        Output.m_AlphaColor = Input[0].m_AlphaColor;

		OutStream.Append(Output);
	}
}

float4 PS_Image(GS_OUTPUT_IMAGE Input) : SV_TARGET
{
    float4 Color = AlbedoMapTexture.Sample(Sampler, Input.m_TexCoord);
	
    Color.a *= Input.m_AlphaColor;
	
    return Color;
}

// ====================================== STANDARD DEPTH WRITE SHADER ======================================

struct PS_OUTPUT_DEPTH
{
	float m_Position : SV_Target;
	float m_Depth    : SV_Depth;
};

float4 VS_Position(float3 Input : POSITION) : SV_POSITION
{
	return mul(mul(mul(float4(Input, 1.0f), WorldMatrix), ViewMatrix), ProjectionMatrix);
}

PS_OUTPUT_DEPTH PS_DepthWrite(float4 Input : SV_POSITION)
{
	PS_OUTPUT_DEPTH Output = (PS_OUTPUT_DEPTH)0;

	Output.m_Position = Output.m_Depth = Input.z;

	return Output;
}

// ====================================== SKINNING DEPTH WRITE SHADER ======================================

struct VS_INPUT_DEPTH_SKINNING
{
	float3 m_Position    : POSITION;
	uint4  m_BoneIndices : BONEINDEX;
	float4 m_BoneWeights : BONEWEIGHT;
};

float4 VS_Position_Skinning(VS_INPUT_DEPTH_SKINNING Input) : SV_POSITION
{
	VS_OUTPUT Output = (VS_OUTPUT)0;
	float4x4 SkinnedWorldMatrix = (float4x4)0.0f;

	for (int i = 0; i < MAX_BONE_INFLUENCE_TO_VERTEX; ++i)
	{
		SkinnedWorldMatrix += Input.m_BoneWeights[i] * mul(BoneOffsetMatrix[Input.m_BoneIndices[i]], BoneTransformMatrix[Input.m_BoneIndices[i]]);
	}

	return mul(mul(mul(float4(Input.m_Position, 1.0f), SkinnedWorldMatrix), ViewMatrix), ProjectionMatrix);
}

// ====================================== BOUNDINGBOX SHADER ======================================

float4 PS_BoundingBox(float4 Input : SV_POSITION) : SV_TARGET
{
	return float4(0.0f, 1.0f, 0.0f, 1.0f);
}
