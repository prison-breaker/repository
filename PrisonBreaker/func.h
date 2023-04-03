#pragma once

namespace Utility
{
	wstring ConvertString(const string& str);
	string ConvertString(const wstring& str);

	template <typename T>
	inline void SafeDelete(vector<T>& v)
	{
		for (int i = 0; i < v.size(); ++i)
		{
			if (v[i] != nullptr)
			{
				delete v[i];
				v[i] = nullptr;
			}
		}

		v.clear();
	}

	template <typename T, typename U>
	inline void SafeDelete(unordered_map<T, U>& um)
	{
		for (auto& p : um)
		{
			if (p.second != nullptr)
			{
				delete p.second;
				p.second = nullptr;
			}
		}

		um.clear();
	}
}

namespace DX
{
	void ThrowIfFailed(HRESULT Result);

	ComPtr<ID3D12Resource> CreateTextureResource(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList, void* Data, const UINT64& Bytes, D3D12_HEAP_TYPE D3D12HeapType, D3D12_RESOURCE_STATES D3D12ResourceStates, ID3D12Resource** D3D12UploadBuffer, const UINT64& Width, UINT Height, UINT16 DepthOrArraySize, UINT16 MipLevels, D3D12_RESOURCE_DIMENSION D3D12ResourceDimension, D3D12_RESOURCE_FLAGS D3D12ResourceFlags, DXGI_FORMAT DXGIFormat);
	ComPtr<ID3D12Resource> CreateBufferResource(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList, void* Data, const UINT64& Bytes, D3D12_HEAP_TYPE D3D12HeapType, D3D12_RESOURCE_STATES D3D12ResourceStates, ID3D12Resource** D3D12UploadBuffer);
	ComPtr<ID3D12Resource> CreateTexture2DResource(ID3D12Device* d3d12Device, const UINT64& Width, UINT Height, UINT16 DepthOrArraySize, UINT16 MipLevels, D3D12_RESOURCE_STATES D3D12ResourceStates, D3D12_RESOURCE_FLAGS D3D12ResourceFlags, DXGI_FORMAT DXGIFormat, const D3D12_CLEAR_VALUE& ClearValue);
	ComPtr<ID3D12Resource> CreateTextureResourceFromDDSFile(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList, const string& FileName, D3D12_RESOURCE_STATES D3D12ResourceStates, ID3D12Resource** D3D12UploadBuffer);

	void ResourceTransition(ID3D12GraphicsCommandList* d3d12GraphicsCommandList, ID3D12Resource* Resource, D3D12_RESOURCE_STATES BeforeState, D3D12_RESOURCE_STATES AfterState);
}

namespace File
{
	void ReadStringFromFile(ifstream& in, string& str);
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
