#pragma once

class CTexture
{
private:
	TEXTURE_TYPE			    m_Type{};

	ComPtr<ID3D12Resource>		m_D3D12Texture{};
	ComPtr<ID3D12Resource>		m_D3D12TextureUploadBuffer{};
	D3D12_GPU_DESCRIPTOR_HANDLE m_D3D12GpuDescriptorHandle{};

public:
	CTexture() = default;
	~CTexture() = default;

	void LoadTextureFromDDSFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, TEXTURE_TYPE TextureType, const tstring& FileName);
	void CreateTexture2D(ID3D12Device* D3D12Device, TEXTURE_TYPE TextureType, const UINT64& Width, UINT Height, D3D12_RESOURCE_STATES D3D12ResourceStates, D3D12_RESOURCE_FLAGS D3D12ResourceFlags, DXGI_FORMAT DxgiFormat, const D3D12_CLEAR_VALUE& D3D12ClearValue);

	void UpdateShaderVariable(ID3D12GraphicsCommandList* D3D12GraphicsCommandList);

	void ReleaseUploadBuffers();

	TEXTURE_TYPE GetTextureType() const;
	ID3D12Resource* GetResource();

	void SetGpuDescriptorHandle(const D3D12_GPU_DESCRIPTOR_HANDLE& D3D12GpuDescriptorHandle);
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetGpuDescriptorHandle();
};
