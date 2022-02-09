#include "stdafx.h"
#include "DDSTextureLoader12.h"

namespace DX
{
	void ThrowIfFailed(HRESULT Result)
	{
		if (FAILED(Result))
		{
			TCHAR DebugMessage[64]{};

			_stprintf_s(DebugMessage, _countof(DebugMessage), _T("[Error] HRESULT of 0x%x\n"), Result);
			OutputDebugString(DebugMessage);
		}
	}

	ComPtr<ID3D12Resource> CreateTextureResource(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList,
		void* Data, const UINT64& Bytes, D3D12_HEAP_TYPE D3D12HeapType, D3D12_RESOURCE_STATES D3D12ResourceStates, ID3D12Resource** D3D12UploadBuffer,
		const UINT64& Width, UINT Height, UINT16 DepthOrArraySize, UINT16 MipLevels, D3D12_RESOURCE_DIMENSION D3D12ResourceDimension, D3D12_RESOURCE_FLAGS D3D12ResourceFlags, DXGI_FORMAT DXGIFormat)
	{
		CD3DX12_HEAP_PROPERTIES D3D12HeapProperties{ D3D12HeapType };
		CD3DX12_RESOURCE_DESC D3D12ResourceDesc{
			D3D12ResourceDimension,
			(D3D12ResourceDimension == D3D12_RESOURCE_DIMENSION_BUFFER) ? (UINT64)D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT : 0,
			(D3D12ResourceDimension == D3D12_RESOURCE_DIMENSION_BUFFER) ? Bytes : Width,
			(D3D12ResourceDimension == D3D12_RESOURCE_DIMENSION_BUFFER) ? (UINT16)1 : Height,
			(D3D12ResourceDimension == D3D12_RESOURCE_DIMENSION_BUFFER) ? (UINT16)1 : DepthOrArraySize,
			(D3D12ResourceDimension == D3D12_RESOURCE_DIMENSION_BUFFER) ? (UINT16)1 : MipLevels,
			(D3D12ResourceDimension == D3D12_RESOURCE_DIMENSION_BUFFER) ? DXGI_FORMAT_UNKNOWN : DXGIFormat,
			1,
			0,
			(D3D12ResourceDimension == D3D12_RESOURCE_DIMENSION_BUFFER) ? D3D12_TEXTURE_LAYOUT_ROW_MAJOR : D3D12_TEXTURE_LAYOUT_UNKNOWN,
			D3D12ResourceFlags
		};
		ComPtr<ID3D12Resource> D3D12Buffer{};
		D3D12_RANGE D3D12ReadRange{};
		UINT8* DataBuffer{};

		switch (D3D12HeapType)
		{
		case D3D12_HEAP_TYPE_DEFAULT:
		{
			D3D12_RESOURCE_STATES D3D12ResourceInitialStates{ (D3D12UploadBuffer && Data) ? D3D12_RESOURCE_STATE_COPY_DEST : D3D12ResourceStates };

			DX::ThrowIfFailed(D3D12Device->CreateCommittedResource(&D3D12HeapProperties, D3D12_HEAP_FLAG_NONE, &D3D12ResourceDesc, D3D12ResourceInitialStates, nullptr,
				__uuidof(ID3D12Resource), (void**)D3D12Buffer.GetAddressOf()));

			if (D3D12UploadBuffer && Data)
			{
				// ���ε� ���۸� �����Ѵ�.
				D3D12HeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;

				D3D12ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
				D3D12ResourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
				D3D12ResourceDesc.Width = Bytes;
				D3D12ResourceDesc.Height = 1;
				D3D12ResourceDesc.DepthOrArraySize = 1;
				D3D12ResourceDesc.MipLevels = 1;
				D3D12ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
				D3D12ResourceDesc.SampleDesc.Count = 1;
				D3D12ResourceDesc.SampleDesc.Quality = 0;
				D3D12ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
				D3D12ResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

				DX::ThrowIfFailed(D3D12Device->CreateCommittedResource(&D3D12HeapProperties, D3D12_HEAP_FLAG_NONE, &D3D12ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
					__uuidof(ID3D12Resource), (void**)D3D12UploadBuffer));

				// ���ε� ���۸� �����Ͽ� �����͸� �����Ѵ�.
				DX::ThrowIfFailed((*D3D12UploadBuffer)->Map(0, &D3D12ReadRange, (void**)&DataBuffer));
				memcpy(DataBuffer, Data, Bytes);
				(*D3D12UploadBuffer)->Unmap(0, nullptr);

				// ���ε� ������ ������ ����Ʈ ���ۿ� �����Ѵ�.
				D3D12GraphicsCommandList->CopyResource(D3D12Buffer.Get(), *D3D12UploadBuffer);

				// ���ҽ� ���¸� �����Ѵ�.
				CD3DX12_RESOURCE_BARRIER D3D12ResourceBarrier{};

				D3D12ResourceBarrier = D3D12ResourceBarrier.Transition(D3D12Buffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12ResourceStates, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, D3D12_RESOURCE_BARRIER_FLAG_NONE);
				D3D12GraphicsCommandList->ResourceBarrier(1, &D3D12ResourceBarrier);
			}
			break;
		}
		case D3D12_HEAP_TYPE_UPLOAD:
		{
			D3D12ResourceStates |= D3D12_RESOURCE_STATE_GENERIC_READ;

			DX::ThrowIfFailed(D3D12Device->CreateCommittedResource(&D3D12HeapProperties, D3D12_HEAP_FLAG_NONE, &D3D12ResourceDesc, D3D12ResourceStates, nullptr,
				__uuidof(ID3D12Resource), (void**)D3D12Buffer.GetAddressOf()));

			if (Data)
			{
				DX::ThrowIfFailed(D3D12Buffer->Map(0, &D3D12ReadRange, (void**)&DataBuffer));
				memcpy(DataBuffer, Data, Bytes);
				D3D12Buffer->Unmap(0, nullptr);
			}
			break;
		}
		case D3D12_HEAP_TYPE_READBACK:
		{
			D3D12ResourceStates |= D3D12_RESOURCE_STATE_COPY_DEST;

			DX::ThrowIfFailed(D3D12Device->CreateCommittedResource(&D3D12HeapProperties, D3D12_HEAP_FLAG_NONE, &D3D12ResourceDesc, D3D12ResourceStates, nullptr,
				__uuidof(ID3D12Resource), (void**)D3D12Buffer.GetAddressOf()));

			if (Data)
			{
				DX::ThrowIfFailed(D3D12Buffer->Map(0, &D3D12ReadRange, (void**)&DataBuffer));
				memcpy(DataBuffer, Data, Bytes);
				D3D12Buffer->Unmap(0, nullptr);
			}
			break;
		}
		}

		return D3D12Buffer;
	}

	ComPtr<ID3D12Resource> CreateBufferResource(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList,
		void* Data, const UINT64& Bytes, D3D12_HEAP_TYPE D3D12HeapType, D3D12_RESOURCE_STATES D3D12ResourceStates, ID3D12Resource** D3D12UploadBuffer)
	{
		return CreateTextureResource(D3D12Device, D3D12GraphicsCommandList, Data, Bytes, D3D12HeapType, D3D12ResourceStates, D3D12UploadBuffer,  Bytes, 1, 1, 1, D3D12_RESOURCE_DIMENSION_BUFFER, D3D12_RESOURCE_FLAG_NONE, DXGI_FORMAT_UNKNOWN);
	}

	ComPtr<ID3D12Resource> CreateTexture2DResource(ID3D12Device* D3D12Device, const UINT64& Width, UINT Height, UINT16 DepthOrArraySize, UINT16 MipLevels,
		D3D12_RESOURCE_STATES D3D12ResourceStates, D3D12_RESOURCE_FLAGS D3D12ResourceFlags, DXGI_FORMAT DXGIFormat, const D3D12_CLEAR_VALUE& ClearValue)
	{
		ComPtr<ID3D12Resource> Texture{};
		CD3DX12_HEAP_PROPERTIES D3D12HeapProperties{ D3D12_HEAP_TYPE_DEFAULT };
		CD3DX12_RESOURCE_DESC D3D12ResourceDesc{ D3D12_RESOURCE_DIMENSION_TEXTURE2D, 0, Width, Height, DepthOrArraySize, MipLevels, DXGIFormat, 1, 0, D3D12_TEXTURE_LAYOUT_UNKNOWN, D3D12ResourceFlags };

		DX::ThrowIfFailed(D3D12Device->CreateCommittedResource(&D3D12HeapProperties, D3D12_HEAP_FLAG_NONE, &D3D12ResourceDesc, D3D12ResourceStates, &ClearValue, __uuidof(ID3D12Resource), (void**)Texture.GetAddressOf()));

		return Texture;
	}

	ComPtr<ID3D12Resource> CreateTextureResourceFromDDSFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList,
		const tstring& FileName, D3D12_RESOURCE_STATES D3D12ResourceStates, ID3D12Resource** D3D12UploadBuffer)
	{
		ComPtr<ID3D12Resource> D3D12Texture{};
		vector<D3D12_SUBRESOURCE_DATA> Subresources{};
		unique_ptr<uint8_t[]> DDSData{};
		DDS_ALPHA_MODE DDSAlphaMode{ DDS_ALPHA_MODE_UNKNOWN };
		bool IsCubeMap{};

		DX::ThrowIfFailed(DirectX::LoadDDSTextureFromFileEx(D3D12Device, FileName.c_str(), 0, D3D12_RESOURCE_FLAG_NONE, DDS_LOADER_DEFAULT, D3D12Texture.GetAddressOf(),
			DDSData, Subresources, &DDSAlphaMode, &IsCubeMap));

		UINT64 Bytes{ GetRequiredIntermediateSize(D3D12Texture.Get(), 0, (UINT)Subresources.size()) };
		CD3DX12_HEAP_PROPERTIES D3D12HeapPropertiesDesc{ D3D12_HEAP_TYPE_UPLOAD, 1, 1 };
		CD3DX12_RESOURCE_DESC D3D12ResourceDesc{ D3D12_RESOURCE_DIMENSION_BUFFER, 0, Bytes, 1, 1, 1, DXGI_FORMAT_UNKNOWN, 1, 0, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_NONE };

		DX::ThrowIfFailed(D3D12Device->CreateCommittedResource(&D3D12HeapPropertiesDesc, D3D12_HEAP_FLAG_NONE, &D3D12ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, __uuidof(ID3D12Resource), (void**)D3D12UploadBuffer));
		UpdateSubresources(D3D12GraphicsCommandList, D3D12Texture.Get(), *D3D12UploadBuffer, 0, 0, (UINT)Subresources.size(), Subresources.data());

		CD3DX12_RESOURCE_BARRIER D3D12ResourceBarrier{};

		D3D12ResourceBarrier = D3D12ResourceBarrier.Transition(D3D12Texture.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12ResourceStates, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, D3D12_RESOURCE_BARRIER_FLAG_NONE);

		D3D12GraphicsCommandList->ResourceBarrier(1, &D3D12ResourceBarrier);

		return D3D12Texture;
	}

	void ResourceTransition(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, ID3D12Resource* Resource, D3D12_RESOURCE_STATES BeforeState, D3D12_RESOURCE_STATES AfterState)
	{
		CD3DX12_RESOURCE_BARRIER D3D12ResourceBarrier{};

		D3D12ResourceBarrier = D3D12ResourceBarrier.Transition(Resource, BeforeState, AfterState, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, D3D12_RESOURCE_BARRIER_FLAG_NONE);
		D3D12GraphicsCommandList->ResourceBarrier(1, &D3D12ResourceBarrier);
	}
}

namespace Random
{
	float Random(float Min, float Max)
	{
		return Min + (Max - Min) * ((float)rand() / (RAND_MAX));
	}
}

namespace Math
{
	bool IsZero(float Value)
	{
		return fabsf(Value) < EPSILON;
	}

	bool IsEqual(float Value1, float Value2)
	{
		return IsZero(Value1 - Value2);
	}

	float InverseSqrt(float Value)
	{
		return 1.0f / sqrtf(Value);
	}
}

namespace Vector3
{
	bool IsZero(const XMFLOAT3& Vector)
	{
		if (Math::IsZero(Vector.x) && Math::IsZero(Vector.y) && Math::IsZero(Vector.z))
		{
			return true;
		}

		return false;
	}

	float Length(const XMFLOAT3& Vector)
	{
		XMFLOAT3 Result{};

		XMStoreFloat3(&Result, XMVector3Length(XMLoadFloat3(&Vector)));

		return Result.x;
	}

	XMFLOAT3 Normalize(const XMFLOAT3& Vector)
	{
		XMFLOAT3 Result{};

		XMStoreFloat3(&Result, XMVector3Normalize(XMLoadFloat3(&Vector)));

		return Result;
	}

	XMFLOAT3 XMVectorToXMFloat3(const XMVECTOR& Vector)
	{
		XMFLOAT3 Result{};

		XMStoreFloat3(&Result, Vector);

		return Result;
	}

	XMFLOAT3 Add(const XMFLOAT3& Vector1, const XMFLOAT3& Vector2)
	{
		XMFLOAT3 Result{};

		XMStoreFloat3(&Result, XMLoadFloat3(&Vector1) + XMLoadFloat3(&Vector2));

		return Result;
	}

	XMFLOAT3 Subtract(const XMFLOAT3& Vector1, const XMFLOAT3& Vector2)
	{
		XMFLOAT3 Result{};

		XMStoreFloat3(&Result, XMLoadFloat3(&Vector1) - XMLoadFloat3(&Vector2));

		return Result;
	}

	XMFLOAT3 ScalarProduct(float Scalar, const XMFLOAT3& Vector, bool Normalize)
	{
		XMFLOAT3 Result{};

		if (Normalize)
		{
			XMStoreFloat3(&Result, XMVector3Normalize(Scalar * XMLoadFloat3(&Vector)));
		}
		else
		{
			XMStoreFloat3(&Result, XMLoadFloat3(&Vector) * Scalar);
		}

		return Result;
	}

	float DotProduct(const XMFLOAT3& Vector1, const XMFLOAT3& Vector2)
	{
		XMFLOAT3 Result{};

		XMStoreFloat3(&Result, XMVector3Dot(XMLoadFloat3(&Vector1), XMLoadFloat3(&Vector2)));

		return Result.x;
	}

	XMFLOAT3 CrossProduct(const XMFLOAT3& Vector1, const XMFLOAT3& Vector2, bool Normalize)
	{
		XMFLOAT3 Result{};

		if (Normalize)
		{
			XMStoreFloat3(&Result, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&Vector1), XMLoadFloat3(&Vector2))));
		}
		else
		{
			XMStoreFloat3(&Result, XMVector3Cross(XMLoadFloat3(&Vector1), XMLoadFloat3(&Vector2)));
		}

		return Result;
	}

	float Angle(const XMVECTOR& Vector1, const XMVECTOR& Vector2)
	{
		XMVECTOR Result{ XMVector3AngleBetweenNormals(Vector1, Vector2) };

		return XMConvertToDegrees(XMVectorGetX(Result));
	}

	float Angle(const XMFLOAT3& Vector1, const XMFLOAT3& Vector2)
	{
		return Angle(XMLoadFloat3(&Vector1), XMLoadFloat3(&Vector2));
	}

	XMFLOAT3 TransformNormal(const XMFLOAT3& Vector, const XMMATRIX& Matrix)
	{
		XMFLOAT3 Result{};

		XMStoreFloat3(&Result, XMVector3TransformNormal(XMLoadFloat3(&Vector), Matrix));

		return Result;
	}

	XMFLOAT3 TransformNormal(const XMFLOAT3& Vector, const XMFLOAT4X4& Matrix)
	{
		return TransformNormal(Vector, XMLoadFloat4x4(&Matrix));
	}

	XMFLOAT3 TransformCoord(const XMFLOAT3& Vector, const XMMATRIX& Matrix)
	{
		XMFLOAT3 Result{};

		XMStoreFloat3(&Result, XMVector3TransformCoord(XMLoadFloat3(&Vector), Matrix));

		return Result;
	}

	XMFLOAT3 TransformCoord(const XMFLOAT3& Vector, const XMFLOAT4X4& Matrix)
	{
		return TransformCoord(Vector, XMLoadFloat4x4(&Matrix));
	}
}

namespace Matrix4x4
{
	XMFLOAT4X4 Identity()
	{
		XMFLOAT4X4 Result{};

		XMStoreFloat4x4(&Result, XMMatrixIdentity());

		return Result;
	}

	XMFLOAT4X4 Inverse(const XMFLOAT4X4& Matrix)
	{
		XMFLOAT4X4 Result{};

		XMStoreFloat4x4(&Result, XMMatrixInverse(nullptr, XMLoadFloat4x4(&Matrix)));

		return Result;
	}

	XMFLOAT4X4 Transpose(const XMFLOAT4X4& Matrix)
	{
		XMFLOAT4X4 Result{};

		XMStoreFloat4x4(&Result, XMMatrixTranspose(XMLoadFloat4x4(&Matrix)));

		return Result;
	}

	XMFLOAT4X4 Multiply(const XMFLOAT4X4& Matrix1, const XMFLOAT4X4& Matrix2)
	{
		XMFLOAT4X4 Result{};

		XMStoreFloat4x4(&Result, XMLoadFloat4x4(&Matrix1) * XMLoadFloat4x4(&Matrix2));

		return Result;
	}

	XMFLOAT4X4 Multiply(const XMFLOAT4X4& Matrix1, const XMMATRIX& Matrix2)
	{
		XMFLOAT4X4 Result{};

		XMStoreFloat4x4(&Result, XMLoadFloat4x4(&Matrix1) * Matrix2);

		return Result;
	}

	XMFLOAT4X4 Multiply(const XMMATRIX& Matrix1, const XMFLOAT4X4& Matrix2)
	{
		XMFLOAT4X4 Result{};

		XMStoreFloat4x4(&Result, Matrix1 * XMLoadFloat4x4(&Matrix2));

		return Result;
	}

	XMFLOAT4X4 Scale(float Pitch, float Yaw, float Roll)
	{
		XMFLOAT4X4 Result{};

		XMStoreFloat4x4(&Result, XMMatrixScaling(Pitch, Yaw, Roll));

		return Result;
	}

	XMFLOAT4X4 RotationYawPitchRoll(float Pitch, float Yaw, float Roll)
	{
		XMFLOAT4X4 Result{};

		XMStoreFloat4x4(&Result, XMMatrixRotationRollPitchYaw(XMConvertToRadians(Pitch), XMConvertToRadians(Yaw), XMConvertToRadians(Roll)));

		return Result;
	}

	XMFLOAT4X4 RotationAxis(const XMFLOAT3& Axis, float Angle)
	{
		XMFLOAT4X4 Result{};

		XMStoreFloat4x4(&Result, XMMatrixRotationAxis(XMLoadFloat3(&Axis), XMConvertToRadians(Angle)));

		return Result;
	}

	XMFLOAT4X4 OrthographicFovLH(float ViewWidth, float ViewHeight, float NearZ, float FarZ)
	{
		XMFLOAT4X4 Result{};

		XMStoreFloat4x4(&Result, XMMatrixOrthographicLH(ViewWidth, ViewHeight, NearZ, FarZ));

		return Result;
	}

	XMFLOAT4X4 PerspectiveFovLH(float FovAngleY, float AspectRatio, float NearZ, float FarZ)
	{
		XMFLOAT4X4 Result{};

		XMStoreFloat4x4(&Result, XMMatrixPerspectiveFovLH(FovAngleY, AspectRatio, NearZ, FarZ));

		return Result;
	}

	XMFLOAT4X4 LookAtLH(const XMFLOAT3& Position, const XMFLOAT3& FocusPosition, const XMFLOAT3& UpDirection)
	{
		XMFLOAT4X4 Result{};

		XMStoreFloat4x4(&Result, XMMatrixLookAtLH(XMLoadFloat3(&Position), XMLoadFloat3(&FocusPosition), XMLoadFloat3(&UpDirection)));

		return Result;
	}

	XMFLOAT4X4 LookToLH(const XMFLOAT3& Position, const XMFLOAT3& Look, const XMFLOAT3& WorldUp)
	{
		XMFLOAT4X4 Result{};

		XMStoreFloat4x4(&Result, XMMatrixLookToLH(XMLoadFloat3(&Position), XMLoadFloat3(&Look), XMLoadFloat3(&WorldUp)));

		return Result;
	}
}