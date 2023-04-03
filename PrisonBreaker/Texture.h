#pragma once
#include "Asset.h"

class CTexture : public CAsset
{
	friend class CAssetManager;

private:
	TEXTURE_TYPE			    m_type;

	ComPtr<ID3D12Resource>		m_d3d12Texture;
	ComPtr<ID3D12Resource>		m_d3d12UploadBuffer;
	D3D12_GPU_DESCRIPTOR_HANDLE m_d3d12GpuDescriptorHandle;

private:
	CTexture();

public:
	virtual ~CTexture();

	TEXTURE_TYPE GetType();
	ID3D12Resource* GetTexture();

	void SetGpuDescriptorHandle(const D3D12_GPU_DESCRIPTOR_HANDLE& D3D12GpuDescriptorHandle);
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGpuDescriptorHandle();

	void Create(ID3D12Device* d3d12Device, const UINT64& Width, UINT Height, D3D12_RESOURCE_STATES D3D12ResourceStates, D3D12_RESOURCE_FLAGS D3D12ResourceFlags, DXGI_FORMAT DxgiFormat, const D3D12_CLEAR_VALUE& D3D12ClearValue, TEXTURE_TYPE textureType);
	void Load(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList, ifstream& in);
	void Load(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList, const string& fileName, TEXTURE_TYPE textureType);

	void UpdateShaderVariable(ID3D12GraphicsCommandList* d3d12GraphicsCommandList);

	void ReleaseUploadBuffers();
};
