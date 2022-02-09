#include "stdafx.h"
#include "Texture.h"

void CTexture::LoadTextureFromDDSFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, UINT TextureType, LPCTSTR FileName)
{
	if (TextureType < TEXTURE_TYPE_DIFFUSEMAP || TextureType > TEXTURE_TYPE_SHADOWMAP)
	{
		return;
	}

	TEXTURE Texture{};

	Texture.m_Type = TextureType;
	Texture.m_D3D12Texture = DX::CreateTextureResourceFromDDSFile(D3D12Device, D3D12GraphicsCommandList, FileName, D3D12_RESOURCE_STATE_GENERIC_READ, Texture.m_D3D12TextureUploadBuffer.GetAddressOf());

	m_Textures.push_back(Texture);
}

void CTexture::CreateTexture2D(ID3D12Device* D3D12Device, UINT TextureType, const UINT64& Width, UINT Height, D3D12_RESOURCE_STATES D3D12ResourceStates, D3D12_RESOURCE_FLAGS D3D12ResourceFlags, DXGI_FORMAT DxgiFormat, const D3D12_CLEAR_VALUE& D3D12ClearValue)
{
	if (TextureType < TEXTURE_TYPE_DIFFUSEMAP || TextureType > TEXTURE_TYPE_SHADOWMAP)
	{
		return;
	}

	TEXTURE Texture{};

	Texture.m_Type = TextureType;
	Texture.m_D3D12Texture = DX::CreateTexture2DResource(D3D12Device, Width, Height, 1, 0, D3D12ResourceStates, D3D12ResourceFlags, DxgiFormat, D3D12ClearValue);

	m_Textures.push_back(Texture);
}

void CTexture::UpdateShaderVariable(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, UINT RootParameter, UINT TextureNum)
{
	if (TextureNum < 0 || TextureNum >= m_Textures.size())
	{
		return;
	}

	D3D12GraphicsCommandList->SetGraphicsRootDescriptorTable(RootParameter, m_Textures[TextureNum].m_D3D12GpuDescriptorHandle);
}

void CTexture::UpdateShaderVariables(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	for (UINT i = 0; i < m_Textures.size(); ++i)
	{
		switch (m_Textures[i].m_Type)
		{
		case TEXTURE_TYPE_DIFFUSEMAP:
			D3D12GraphicsCommandList->SetGraphicsRootDescriptorTable(ROOT_PARAMETER_DIFFUSEMAP, m_Textures[i].m_D3D12GpuDescriptorHandle);
			break;
		case TEXTURE_TYPE_NORMALMAP:
			D3D12GraphicsCommandList->SetGraphicsRootDescriptorTable(ROOT_PARAMETER_NORMALMAP, m_Textures[i].m_D3D12GpuDescriptorHandle);
			break;
		case TEXTURE_TYPE_SHADOWMAP:
			D3D12GraphicsCommandList->SetGraphicsRootDescriptorTable(ROOT_PARAMETER_SHADOWMAP, m_Textures[i].m_D3D12GpuDescriptorHandle);
			break;
		}
	}
}

void CTexture::ReleaseUploadBuffers()
{
	for (auto& Texture : m_Textures)
	{
		Texture.m_D3D12TextureUploadBuffer.ReleaseAndGetAddressOf();
	}
}

UINT CTexture::GetTextureCount() const
{
	return (UINT)m_Textures.size();
}

ID3D12Resource* CTexture::GetResource(UINT Index)
{
	if (Index < 0 || Index >= m_Textures.size())
	{
		return nullptr;
	}

	return m_Textures[Index].m_D3D12Texture.Get();
}

void CTexture::SetGpuDescriptorHandle(UINT Index, const D3D12_GPU_DESCRIPTOR_HANDLE& D3D12GpuDescriptorHandle)
{
	if (Index < 0 || Index >= m_Textures.size())
	{
		return;
	}

	m_Textures[Index].m_D3D12GpuDescriptorHandle = D3D12GpuDescriptorHandle;
}

const D3D12_GPU_DESCRIPTOR_HANDLE& CTexture::GetGpuDescriptorHandle(UINT Index)
{
	return m_Textures[Index].m_D3D12GpuDescriptorHandle;
}
