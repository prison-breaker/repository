#include "stdafx.h"
#include "TextureManager.h"
#include "Texture.h"

CTextureManager* CTextureManager::GetInstance()
{
	static CTextureManager Instance{};

	return &Instance;
}

void CTextureManager::CreateCbvSrvUavDescriptorHeaps(ID3D12Device* D3D12Device)
{
	D3D12_DESCRIPTOR_HEAP_DESC D3D12DescriptorHeapDesc{};

	D3D12DescriptorHeapDesc.NumDescriptors = GetRegisteredTextureCount();
	D3D12DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	D3D12DescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	D3D12DescriptorHeapDesc.NodeMask = 0;

	DX::ThrowIfFailed(D3D12Device->CreateDescriptorHeap(&D3D12DescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)m_D3D12SrvDescriptorHeap.GetAddressOf()));
}

void CTextureManager::CreateShaderResourceViews(ID3D12Device* D3D12Device)
{
	D3D12_CPU_DESCRIPTOR_HANDLE D3D12CpuDescriptorHandle{ m_D3D12SrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };
	D3D12_GPU_DESCRIPTOR_HANDLE D3D12GpuDescriptorHandle{ m_D3D12SrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart() };
	UINT DescriptorIncrementSize{ D3D12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) };

	for (const auto& Texture : m_Textures)
	{
		D3D12Device->CreateShaderResourceView(Texture.second->GetResource(), nullptr, D3D12CpuDescriptorHandle);
		Texture.second->SetGpuDescriptorHandle(D3D12GpuDescriptorHandle);

		D3D12CpuDescriptorHandle.ptr += DescriptorIncrementSize;
		D3D12GpuDescriptorHandle.ptr += DescriptorIncrementSize;
	}
}

void CTextureManager::ReleaseUploadBuffers()
{
	for (const auto& Texture : m_Textures)
	{
		Texture.second->ReleaseUploadBuffers();
	}
}

void CTextureManager::RegisterTexture(const tstring& TextureName, const shared_ptr<CTexture>& Texture)
{
	if (Texture)
	{
		m_Textures.emplace(TextureName, Texture);
	}
}

shared_ptr<CTexture> CTextureManager::GetTexture(const tstring& TextureName)
{
	if (m_Textures.count(TextureName))
	{
		return m_Textures[TextureName];
	}

	return nullptr;
}

UINT CTextureManager::GetRegisteredTextureCount() const
{
	return (UINT)m_Textures.size();
}

void CTextureManager::SetDescriptorHeap(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	if (m_D3D12SrvDescriptorHeap)
	{
		D3D12GraphicsCommandList->SetDescriptorHeaps(1, m_D3D12SrvDescriptorHeap.GetAddressOf());
	}
}
