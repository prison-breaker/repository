#include "pch.h"
#include "Texture.h"

#include "AssetManager.h"

CTexture::CTexture() :
	m_type(),
	m_d3d12Texture(),
	m_d3d12UploadBuffer(),
	m_d3d12GpuDescriptorHandle()
{
}

CTexture::~CTexture()
{
}

TEXTURE_TYPE CTexture::GetType()
{
	return m_type;
}

ID3D12Resource* CTexture::GetTexture()
{
	return m_d3d12Texture.Get();
}

void CTexture::SetGpuDescriptorHandle(const D3D12_GPU_DESCRIPTOR_HANDLE& d3d12GpuDescriptorHandle)
{
	m_d3d12GpuDescriptorHandle = d3d12GpuDescriptorHandle;
}

const D3D12_GPU_DESCRIPTOR_HANDLE& CTexture::GetGpuDescriptorHandle()
{
	return m_d3d12GpuDescriptorHandle;
}

void CTexture::Create(ID3D12Device* d3d12Device, const UINT64& Width, UINT Height, D3D12_RESOURCE_STATES D3D12ResourceStates, D3D12_RESOURCE_FLAGS D3D12ResourceFlags, DXGI_FORMAT DxgiFormat, const D3D12_CLEAR_VALUE& D3D12ClearValue, TEXTURE_TYPE textureType)
{
	m_type = textureType;
	m_d3d12Texture = DX::CreateTexture2DResource(d3d12Device, Width, Height, 1, 0, D3D12ResourceStates, D3D12ResourceFlags, DxgiFormat, D3D12ClearValue);
}

void CTexture::Load(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList, ifstream& in)
{
	string filePath = CAssetManager::GetInstance()->GetAssetPath() + "Texture\\";
	string str;

	while (true)
	{
		File::ReadStringFromFile(in, str);

		if (str == "<FileName>")
		{
			File::ReadStringFromFile(in, str);

			// .dds 부분을 제외한 나머지를 이름으로 설정한다.
			m_name = str.substr(0, str.length() - 4);
			m_d3d12Texture = DX::CreateTextureResourceFromDDSFile(d3d12Device, d3d12GraphicsCommandList, filePath + str, D3D12_RESOURCE_STATE_GENERIC_READ, m_d3d12UploadBuffer.GetAddressOf());
		}
		else if (str == "<Type>")
		{
			in.read(reinterpret_cast<char*>(&m_type), sizeof(int));
		}
		else if (str == "</Texture>")
		{
			break;
		}
	}
}

void CTexture::Load(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList, const string& fileName, TEXTURE_TYPE textureType)
{
	string filePath = CAssetManager::GetInstance()->GetAssetPath() + "Texture\\" + fileName;

	// .dds 부분을 제외한 나머지를 이름으로 설정한다.
	m_name = fileName.substr(0, fileName.length() - 4);
	m_type = textureType;
	m_d3d12Texture = DX::CreateTextureResourceFromDDSFile(d3d12Device, d3d12GraphicsCommandList, filePath, D3D12_RESOURCE_STATE_GENERIC_READ, m_d3d12UploadBuffer.GetAddressOf());
}

void CTexture::UpdateShaderVariable(ID3D12GraphicsCommandList* d3d12GraphicsCommandList)
{
	switch (m_type)
	{
	case TEXTURE_TYPE::ALBEDO_MAP:
		d3d12GraphicsCommandList->SetGraphicsRootDescriptorTable((UINT)ROOT_PARAMETER_TYPE::ALBEDO_MAP, m_d3d12GpuDescriptorHandle);
		break;
	case TEXTURE_TYPE::METALLIC_MAP:
		d3d12GraphicsCommandList->SetGraphicsRootDescriptorTable((UINT)ROOT_PARAMETER_TYPE::METALLIC_MAP, m_d3d12GpuDescriptorHandle);
		break;
	case TEXTURE_TYPE::NORMAL_MAP:
		d3d12GraphicsCommandList->SetGraphicsRootDescriptorTable((UINT)ROOT_PARAMETER_TYPE::NORMAL_MAP, m_d3d12GpuDescriptorHandle);
		break;
	case TEXTURE_TYPE::SHADOW_MAP:
		d3d12GraphicsCommandList->SetGraphicsRootDescriptorTable((UINT)ROOT_PARAMETER_TYPE::SHADOW_MAP, m_d3d12GpuDescriptorHandle);
		break;
	}
}

void CTexture::ReleaseUploadBuffers()
{
	if (m_d3d12UploadBuffer.Get() != nullptr)
	{
		m_d3d12UploadBuffer.Reset();
	}
}
