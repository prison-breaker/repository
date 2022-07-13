#pragma once
#pragma comment(lib, "ws2_32")

#define DEBUG_MODE
#define READ_BINARY_FILE

#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define EPSILON			          1.0e-10f

#define CLIENT_WIDTH		      1920
#define	CLIENT_HEIGHT		      1080
#define DEPTH_BUFFER_WIDTH	      2048
#define DEPTH_BUFFER_HEIGHT       2048
#define PLANE_WIDTH				  400
#define PLANE_HEIGHT			  400

#define MAX_CLIENT_CAPACITY		  2
#define MAX_NPC_COUNT			  15	
#define MAX_TITLE_LENGTH	      64
#define MAX_LIGHTS                2
#define MAX_BOUNDINGBOX_INDICES	  36
#define MAX_BONES				  100
#define MAX_BGM_SOUNDS			  3
#define MAX_SFX_SOUNDS			  11

#define INPUT_MASK_NONE			  0x0000
#define INPUT_MASK_W	          0x0001
#define INPUT_MASK_S              0x0002
#define INPUT_MASK_A              0x0004
#define INPUT_MASK_D              0x0008
#define INPUT_MASK_F			  0x0010
#define INPUT_MASK_SHIFT          0x0020
#define INPUT_MASK_TAB            0x0040
#define INPUT_MASK_LMB            0x0080
#define INPUT_MASK_RMB            0x0100
#define INPUT_MASK_NUM1			  0x0200
#define INPUT_MASK_NUM2			  0x0400

#define TEXTURE_MASK_ALBEDO_MAP   0x01
#define TEXTURE_MASK_METALLIC_MAP 0x02
#define TEXTURE_MASK_NORMAL_MAP   0x04
#define TEXTURE_MASK_SHADOW_MAP   0x08

enum MSG_TYPE
{
	MSG_TYPE_NONE                = 0x0000,
	MSG_TYPE_TITLE               = 0x0001,
	MSG_TYPE_INGAME              = 0x0002,
	MSG_TYPE_TRIGGER             = 0x0004,
	MSG_TYPE_PLAYER1_WEAPON_SWAP = 0x0008,
	MSG_TYPE_PLAYER2_WEAPON_SWAP = 0x0010
};

enum OBJECT_TYPE
{
	OBJECT_TYPE_PLAYER,
	OBJECT_TYPE_NPC,
	OBJECT_TYPE_TERRAIN,
	OBJECT_TYPE_STRUCTURE
};

enum BILBOARD_OBJECT_TYPE
{
	BILBOARD_OBJECT_TYPE_SKYBOX,
	BILBOARD_OBJECT_TYPE_UI
};

enum WEAPON_TYPE
{
	WEAPON_TYPE_PUNCH,
	WEAPON_TYPE_PISTOL
};

enum ANIMATION_TYPE
{
	ANIMATION_TYPE_LOOP,
	ANIMATION_TYPE_ONCE,
	ANIMATION_TYPE_ONCE_REVERSE
};

enum ANIMATION_CLIP_TYPE
{
	ANIMATION_CLIP_TYPE_PLAYER_IDLE = 0,
	ANIMATION_CLIP_TYPE_PLAYER_WALK_FORWARD_AND_BACK,
	ANIMATION_CLIP_TYPE_PLAYER_WALK_LEFT,
	ANIMATION_CLIP_TYPE_PLAYER_WALK_RIGHT,
	ANIMATION_CLIP_TYPE_PLAYER_RUN_FORWARD,
	ANIMATION_CLIP_TYPE_PLAYER_RUN_LEFT,
	ANIMATION_CLIP_TYPE_PLAYER_RUN_RIGHT,
	ANIMATION_CLIP_TYPE_PLAYER_PUNCH,
	ANIMATION_CLIP_TYPE_PLAYER_PISTOL_IDLE,
	ANIMATION_CLIP_TYPE_PLAYER_SHOOT,
	ANIMATION_CLIP_TYPE_PLAYER_DIE,

	ANIMATION_CLIP_TYPE_NPC_IDLE = 0,
	ANIMATION_CLIP_TYPE_NPC_WALK_FORWARD,
	ANIMATION_CLIP_TYPE_NPC_RUN_FORWARD,
	ANIMATION_CLIP_TYPE_NPC_SHOOT,
	ANIMATION_CLIP_TYPE_NPC_HIT,
	ANIMATION_CLIP_TYPE_NPC_DIE
};

enum TRIGGER_TYPE
{
	TRIGGER_TYPE_OPEN_DOOR,
	TRIGGER_TYPE_OPEN_ELEC_PANEL,
	TRIGGER_TYPE_SIREN,
	TRIGGER_TYPE_OPEN_GATE
};

enum RENDER_TYPE
{
	RENDER_TYPE_STANDARD,
	RENDER_TYPE_DEPTH_WRITE
};

enum SHADER_TYPE
{
	SHADER_TYPE_STANDARD,
	SHADER_TYPE_WITH_SKINNING,
};

enum LIGHT_TYPE
{
	LIGHT_TYPE_POINT = 1,
	LIGHT_TYPE_SPOT,
	LIGHT_TYPE_DIRECTIONAL
};

enum ROOT_PARAMETER_TYPE
{
	ROOT_PARAMETER_TYPE_FRAMEWORK_INFO,
	ROOT_PARAMETER_TYPE_CAMERA,
	ROOT_PARAMETER_TYPE_LIGHT,
	ROOT_PARAMETER_TYPE_FOG,
	ROOT_PARAMETER_TYPE_OBJECT,
	ROOT_PARAMETER_TYPE_BONE_OFFSET,
	ROOT_PARAMETER_TYPE_BONE_TRANSFORM,
	ROOT_PARAMETER_TYPE_ALBEDO_MAP,
	ROOT_PARAMETER_TYPE_METALLIC_MAP,
	ROOT_PARAMETER_TYPE_NORMAL_MAP,
	ROOT_PARAMETER_TYPE_SHADOW_MAP
};

enum TEXTURE_TYPE
{
	TEXTURE_TYPE_ALBEDO_MAP,
	TEXTURE_TYPE_METALLIC_MAP,
	TEXTURE_TYPE_NORMAL_MAP,
	TEXTURE_TYPE_SHADOW_MAP,
};

enum SOUND_TYPE
{
	// BGM
	SOUND_TYPE_TITLE_BGM,
	SOUND_TYPE_INGAME_BGM_1,
	SOUND_TYPE_INGAME_BGM_2,

	// SFX
	SOUND_TYPE_BUTTON_OVER,
	SOUND_TYPE_OPEN_DOOR,
	SOUND_TYPE_OPEN_EP,
	SOUND_TYPE_OPEN_GATE,
	SOUND_TYPE_POWER_DOWN,
	SOUND_TYPE_BREATH,
	SOUND_TYPE_GRUNT_1,
	SOUND_TYPE_GRUNT_2,
	SOUND_TYPE_PISTOL_SHOT,
	SOUND_TYPE_PISTOL_EMPTY,
	SOUND_TYPE_SIREN
};

// C Header
#include <winsock2.h>
#include <SDKDDKVer.h>
#include <stdlib.h>
#include <memory.h>
#include <tchar.h>
#include <wrl.h>
#include <shellapi.h>

// C++ Header
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <queue>
#include <thread>
#include <chrono>
#include <numeric>
#include <random>
using namespace std;
using namespace std::chrono;

// DirectX Header
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#include "D3DX12.h"
#include <D3D11on12.h>
#include <D3Dcompiler.h>
#include <D2D1_3.h>
#include <DXGI1_4.h>
#include <DXGIDebug.h>
#include <DWrite.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
using namespace DirectX;
using namespace DirectX::PackedVector;
using Microsoft::WRL::ComPtr;

#ifdef _UNICODE
#define tcout wcout
#define tcin  wcin
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#else
#define tcout cout
#define tcin  cin
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif

typedef basic_string<TCHAR>        tstring;
typedef basic_istream<TCHAR>       tistream;
typedef basic_ostream<TCHAR>       tostream;
typedef basic_fstream<TCHAR>       tfstream;
typedef basic_ifstream<TCHAR>      tifstream;
typedef basic_ofstream<TCHAR>      tofstream;
typedef basic_stringstream<TCHAR>  tstringstream;
typedef basic_istringstream<TCHAR> tistringstream;
typedef basic_ostringstream<TCHAR> tostringstream;

struct SOCKET_INFO
{
	UINT		m_ID{};
	SOCKET      m_Socket{};
	SOCKADDR_IN m_SocketAddress{};
};

struct CLIENT_TO_SERVER_DATA
{
	UINT	   m_InputMask{};
	XMFLOAT4X4 m_WorldMatrix{};
};

struct TRIGGER_DATA
{
	UINT m_Size{};

	UINT m_TriggerIndexStack[2]{};
	UINT m_CallerIndexStack[2]{};
};

struct SERVER_TO_CLIENT_DATA
{
	MSG_TYPE			m_MsgType;

	XMFLOAT4X4          m_PlayerWorldMatrices[MAX_CLIENT_CAPACITY]{};
	ANIMATION_CLIP_TYPE m_PlayerAnimationClipTypes[MAX_CLIENT_CAPACITY]{};

	XMFLOAT4X4          m_NPCWorldMatrices[MAX_NPC_COUNT]{};
	ANIMATION_CLIP_TYPE m_NPCAnimationClipTypes[MAX_NPC_COUNT]{};

	XMFLOAT3            m_TowerLightDirection{};

	// 1. Scene's State - UINT
	// 4. UI and Trigger's Activation Condition - bool
	// 6. Sound Play Condition and Volume - bool and float
};

namespace DX
{
	void ThrowIfFailed(HRESULT Result);

	ComPtr<ID3D12Resource> CreateTextureResource(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, void* Data, const UINT64& Bytes, D3D12_HEAP_TYPE D3D12HeapType, D3D12_RESOURCE_STATES D3D12ResourceStates, ID3D12Resource** D3D12UploadBuffer, const UINT64& Width, UINT Height, UINT16 DepthOrArraySize, UINT16 MipLevels, D3D12_RESOURCE_DIMENSION D3D12ResourceDimension, D3D12_RESOURCE_FLAGS D3D12ResourceFlags, DXGI_FORMAT DXGIFormat);
	ComPtr<ID3D12Resource> CreateBufferResource(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, void* Data, const UINT64& Bytes, D3D12_HEAP_TYPE D3D12HeapType, D3D12_RESOURCE_STATES D3D12ResourceStates, ID3D12Resource** D3D12UploadBuffer);
	ComPtr<ID3D12Resource> CreateTexture2DResource(ID3D12Device* D3D12Device, const UINT64& Width, UINT Height, UINT16 DepthOrArraySize, UINT16 MipLevels, D3D12_RESOURCE_STATES D3D12ResourceStates, D3D12_RESOURCE_FLAGS D3D12ResourceFlags, DXGI_FORMAT DXGIFormat, const D3D12_CLEAR_VALUE& ClearValue);
	ComPtr<ID3D12Resource> CreateTextureResourceFromDDSFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, const tstring& FileName, D3D12_RESOURCE_STATES D3D12ResourceStates, ID3D12Resource** D3D12UploadBuffer);

	void ResourceTransition(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, ID3D12Resource* Resource, D3D12_RESOURCE_STATES BeforeState, D3D12_RESOURCE_STATES AfterState);
}

namespace Time
{
	void MesureStart();
	void MesureEnd();
}

namespace File
{
	UINT ReadIntegerFromFile(tifstream& InFile);
	float ReadFloatFromFile(tifstream& InFile);
	void ReadStringFromFile(tifstream& InFile, tstring& Token);
}

namespace Random
{
	float Range(float Min, float Max);
}

namespace Math
{
	bool IsZero(float Value);
	bool IsEqual(float Value1, float Value2);

	float InverseSqrt(float Value);

	float Distance(const XMFLOAT3& Vector1, const XMFLOAT3& Vector2);

	UINT CalculateTriangleArea(const XMFLOAT3& Vertex1, const XMFLOAT3& Vertex2, const XMFLOAT3& Vertex3);
	bool IsInTriangle(const XMFLOAT3& Vertex1, const XMFLOAT3& Vertex2, const XMFLOAT3& Vertex3, const XMFLOAT3& NewPosition);

	int CounterClockWise(const XMFLOAT3& Vertex1, const XMFLOAT3& Vertex2, const XMFLOAT3& Vertex3);
	bool LineIntersection(const XMFLOAT3& L1V1, const XMFLOAT3& L1V2, const XMFLOAT3& L2V1, const XMFLOAT3& L2V2);
}

namespace Vector3
{
	bool IsZero(const XMFLOAT3& Vector);

	bool IsEqual(const XMFLOAT3& Vector1, const XMFLOAT3& Vector2);

	float Length(const XMFLOAT3& Vector);
	XMFLOAT3 Normalize(const XMFLOAT3& Vector);
	XMFLOAT3 Inverse(const XMFLOAT3& Vector);

	XMFLOAT3 XMVectorToXMFloat3(const XMVECTOR& Vector);

	XMFLOAT3 Add(const XMFLOAT3& Vector1, const XMFLOAT3& Vector2);
	XMFLOAT3 Subtract(const XMFLOAT3& Vector1, const XMFLOAT3& Vector2);

	XMFLOAT3 ScalarProduct(float Scalar, const XMFLOAT3& Vector, bool Normalize);
	float DotProduct(const XMFLOAT3& Vector1, const XMFLOAT3& Vector2);
	XMFLOAT3 CrossProduct(const XMFLOAT3& Vector1, const XMFLOAT3& Vector2, bool Normalize);

	float Angle(const XMVECTOR& Vector1, const XMVECTOR& Vector2);
	float Angle(const XMFLOAT3& Vector1, const XMFLOAT3& Vector2);

	XMFLOAT3 TransformNormal(const XMFLOAT3& Vector, const XMMATRIX& Matrix);
	XMFLOAT3 TransformNormal(const XMFLOAT3& Vector, const XMFLOAT4X4& Matrix);
	XMFLOAT3 TransformCoord(const XMFLOAT3& Vector, const XMMATRIX& Matrix);
	XMFLOAT3 TransformCoord(const XMFLOAT3& Vector, const XMFLOAT4X4& Matrix);
}

namespace Matrix4x4
{
	XMFLOAT4X4 Identity();
	XMFLOAT4X4 Inverse(const XMFLOAT4X4& Matrix);
	XMFLOAT4X4 Transpose(const XMFLOAT4X4& Matrix);

	XMFLOAT4X4 Multiply(const XMFLOAT4X4& Matrix1, const XMFLOAT4X4& Matrix2);
	XMFLOAT4X4 Multiply(const XMFLOAT4X4& Matrix1, const XMMATRIX& Matrix2);
	XMFLOAT4X4 Multiply(const XMMATRIX& Matrix1, const XMFLOAT4X4& Matrix2);

	XMFLOAT4X4 Scale(float Pitch, float Yaw, float Roll);

	XMFLOAT4X4 RotationYawPitchRoll(float Pitch, float Yaw, float Roll);
	XMFLOAT4X4 RotationAxis(const XMFLOAT3& Axis, float Angle);

	XMFLOAT4X4 OrthographicFovLH(float Width, float Height, float NearZ, float FarZ);
	XMFLOAT4X4 PerspectiveFovLH(float FovAngleY, float AspectRatio, float NearZ, float FarZ);

	XMFLOAT4X4 LookAtLH(const XMFLOAT3& Position, const XMFLOAT3& FocusPosition, const XMFLOAT3& UpDirection);
	XMFLOAT4X4 LookToLH(const XMFLOAT3& Position, const XMFLOAT3& Look, const XMFLOAT3& WorldUp);
}

namespace Server
{
	void ErrorQuit(const char* Msg);
	void ErrorDisplay(const char* Msg);
}

// Managers
#include "SceneManager.h"
#include "TextureManager.h"
#include "ShaderManager.h"
#include "SoundManager.h"
