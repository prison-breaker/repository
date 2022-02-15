#pragma once

#define DEBUG_MODE
#define WIN32_LEAN_AND_MEAN

#define EPSILON			      1.0e-10f

#define MAX_TITLE_LENGTH	        64
#define MAX_LIGHTS                   1

#define CLIENT_WIDTH		      1920
#define	CLIENT_HEIGHT		      1080
#define DEPTH_BUFFER_WIDTH	      2048
#define DEPTH_BUFFER_HEIGHT       2048
#define PLANE_WIDTH			       500
#define PLANE_HEIGHT		       500

#define TEXTURE_MASK_ALBEDO_MAP   0x01
#define TEXTURE_MASK_METALLIC_MAP 0x02
#define TEXTURE_MASK_NORMAL_MAP   0x04
#define TEXTURE_MASK_SHADOW_MAP   0x08

enum OBJECT_TYPE
{
	OBJECT_TYPE_PLAYER = 1,
	OBJECT_TYPE_GUARD,
	OBJECT_TYPE_STRUCTURE
};

enum LIGHT_TYPE
{
	LIGHT_TYPE_POINT = 1,
	LIGHT_TYPE_SPOT,
	LIGHT_TYPE_DIRECTIONAL,
};

enum ROOT_PARAMETER_TYPE
{
	ROOT_PARAMETER_TYPE_FRAMEWORK_INFO,
	ROOT_PARAMETER_TYPE_CAMERA,
	ROOT_PARAMETER_TYPE_LIGHT,
	ROOT_PARAMETER_TYPE_OBJECT,
	ROOT_PARAMETER_TYPE_ALBEDO_MAP,
	ROOT_PARAMETER_TYPE_METALLIC_MAP,
	ROOT_PARAMETER_TYPE_NORMAL_MAP,
	ROOT_PARAMETER_TYPE_SHADOW_MAP,
};

enum TEXTURE_TYPE
{
	TEXTURE_TYPE_ALBEDO_MAP,
	TEXTURE_TYPE_METALLIC_MAP,
	TEXTURE_TYPE_NORMAL_MAP,
	TEXTURE_TYPE_SHADOW_MAP,
};

// C Header
#include <windows.h>
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
using namespace std;

// DirectX Header
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#include "D3DX12.h"
#include <D3Dcompiler.h>
#include <DXGI1_4.h>
#include <DXGIDebug.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
using namespace DirectX;
using namespace DirectX::PackedVector;
using Microsoft::WRL::ComPtr;

#ifdef _UNICODE
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
typedef std::wstring   tstring;
typedef std::wifstream tifstream;
#define tcout wcout
#define tcin  wcin
#else
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
typedef std::string	   tstring;
typedef std::ifstream  tifstream;
#define tcout cout
#define tcin  cin
#endif

// Managers
#include "SceneManager.h"
#include "TextureManager.h"

namespace DX
{
	void ThrowIfFailed(HRESULT Result);

	ComPtr<ID3D12Resource> CreateTextureResource(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, void* Data, const UINT64& Bytes, D3D12_HEAP_TYPE D3D12HeapType, D3D12_RESOURCE_STATES D3D12ResourceStates, ID3D12Resource** D3D12UploadBuffer, const UINT64& Width, UINT Height, UINT16 DepthOrArraySize, UINT16 MipLevels, D3D12_RESOURCE_DIMENSION D3D12ResourceDimension, D3D12_RESOURCE_FLAGS D3D12ResourceFlags, DXGI_FORMAT DXGIFormat);
	ComPtr<ID3D12Resource> CreateBufferResource(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, void* Data, const UINT64& Bytes, D3D12_HEAP_TYPE D3D12HeapType, D3D12_RESOURCE_STATES D3D12ResourceStates, ID3D12Resource** D3D12UploadBuffer);
	ComPtr<ID3D12Resource> CreateTexture2DResource(ID3D12Device* D3D12Device, const UINT64& Width, UINT Height, UINT16 DepthOrArraySize, UINT16 MipLevels, D3D12_RESOURCE_STATES D3D12ResourceStates, D3D12_RESOURCE_FLAGS D3D12ResourceFlags, DXGI_FORMAT DXGIFormat, const D3D12_CLEAR_VALUE& ClearValue);
	ComPtr<ID3D12Resource> CreateTextureResourceFromDDSFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, const tstring& FileName, D3D12_RESOURCE_STATES D3D12ResourceStates, ID3D12Resource** D3D12UploadBuffer);

	void ResourceTransition(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, ID3D12Resource* Resource, D3D12_RESOURCE_STATES BeforeState, D3D12_RESOURCE_STATES AfterState);
}

namespace Random
{
	float Random(float Min, float Max);
}

namespace Math
{
	bool IsZero(float Value);
	bool IsEqual(float Value1, float Value2);

	float InverseSqrt(float Value);
}

namespace Vector3
{
	bool IsZero(const XMFLOAT3& Vector);

	float Length(const XMFLOAT3& Vector);
	XMFLOAT3 Normalize(const XMFLOAT3& Vector);

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
