#include "stdafx.h"
#include "Texture.h"

void CTexture::LoadTextureFromDDSFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, TEXTURE_TYPE TextureType, const tstring& FileName)
{
	if (TextureType < TEXTURE_TYPE_ALBEDO_MAP || TextureType > TEXTURE_TYPE_SHADOW_MAP)
	{
		return;
	}

	m_Type = TextureType;
	m_D3D12Texture = DX::CreateTextureResourceFromDDSFile(D3D12Device, D3D12GraphicsCommandList, FileName, D3D12_RESOURCE_STATE_GENERIC_READ, m_D3D12TextureUploadBuffer.GetAddressOf());
}

void CTexture::CreateTexture2D(ID3D12Device* D3D12Device, TEXTURE_TYPE TextureType, const UINT64& Width, UINT Height, D3D12_RESOURCE_STATES D3D12ResourceStates, D3D12_RESOURCE_FLAGS D3D12ResourceFlags, DXGI_FORMAT DxgiFormat, const D3D12_CLEAR_VALUE& D3D12ClearValue)
{
	if (TextureType < TEXTURE_TYPE_ALBEDO_MAP || TextureType > TEXTURE_TYPE_SHADOW_MAP)
	{
		return;
	}

	m_Type = TextureType;
	m_D3D12Texture = DX::CreateTexture2DResource(D3D12Device, Width, Height, 1, 0, D3D12ResourceStates, D3D12ResourceFlags, DxgiFormat, D3D12ClearValue);
}

void CTexture::UpdateShaderVariable(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	switch (m_Type)
	{
	case TEXTURE_TYPE_ALBEDO_MAP:
		D3D12GraphicsCommandList->SetGraphicsRootDescriptorTable(ROOT_PARAMETER_TYPE_ALBEDO_MAP, m_D3D12GpuDescriptorHandle);
		break;
	case TEXTURE_TYPE_METALLIC_MAP:
		D3D12GraphicsCommandList->SetGraphicsRootDescriptorTable(ROOT_PARAMETER_TYPE_METALLIC_MAP, m_D3D12GpuDescriptorHandle);
		break;
	case TEXTURE_TYPE_NORMAL_MAP:
		D3D12GraphicsCommandList->SetGraphicsRootDescriptorTable(ROOT_PARAMETER_TYPE_NORMAL_MAP, m_D3D12GpuDescriptorHandle);
		break;
	case TEXTURE_TYPE_SHADOW_MAP:
		D3D12GraphicsCommandList->SetGraphicsRootDescriptorTable(ROOT_PARAMETER_TYPE_SHADOW_MAP, m_D3D12GpuDescriptorHandle);
		break;
	}
}

void CTexture::ReleaseUploadBuffers()
{
	if (m_D3D12TextureUploadBuffer)
	{
		m_D3D12TextureUploadBuffer.Reset();
	}
}

TEXTURE_TYPE CTexture::GetTextureType() const
{
	return m_Type;
}

ID3D12Resource* CTexture::GetResource()
{
	if (m_D3D12Texture)
	{
		return m_D3D12Texture.Get();
	}

	return nullptr;
}

void CTexture::SetGpuDescriptorHandle(const D3D12_GPU_DESCRIPTOR_HANDLE& D3D12GpuDescriptorHandle)
{
	m_D3D12GpuDescriptorHandle = D3D12GpuDescriptorHandle;
}

const D3D12_GPU_DESCRIPTOR_HANDLE& CTexture::GetGpuDescriptorHandle()
{
	return m_D3D12GpuDescriptorHandle;
}
