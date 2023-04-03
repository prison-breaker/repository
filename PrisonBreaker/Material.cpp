#include "pch.h"
#include "Material.h"

#include "AssetManager.h"

#include "Texture.h"
#include "Shader.h"

CMaterial::CMaterial() :
	m_albedoColor(1.0f, 1.0f, 1.0f, 1.0f),
	m_emissionColor(),
	m_smoothness(),
	m_metallic(),
	m_textureMask(),
	m_textureScale(1.0f, 1.0f),
	m_textures(),
	m_shaders(),
	m_stateNum()
{
}

CMaterial::~CMaterial()
{
}

void CMaterial::SetStateNum(int stateNum)
{
	if (stateNum < 0)
	{
		return;
	}

	m_stateNum = stateNum;
}

int CMaterial::GetStateNum()
{
	return m_stateNum;
}

void CMaterial::Load(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList, ifstream& in)
{
	string str;

	while (true)
	{
		File::ReadStringFromFile(in, str);

		if (str == "<Name>")
		{
			File::ReadStringFromFile(in, str);
			SetName(str);
		}
		else if (str == "<StateNum>")
		{
			in.read(reinterpret_cast<char*>(&m_stateNum), sizeof(int));
			m_shaders.push_back(CAssetManager::GetInstance()->GetShader("Object"));
			m_shaders.push_back(CAssetManager::GetInstance()->GetShader("DepthWrite"));
		}
		else if (str == "<TextureScale>")
		{
			in.read(reinterpret_cast<char*>(&m_textureScale), sizeof(XMFLOAT2));
		}
		else if (str == "<AlbedoColor>")
		{
			in.read(reinterpret_cast<char*>(&m_albedoColor), sizeof(XMFLOAT4));
		}
		else if (str == "<EmissionColor>")
		{
			in.read(reinterpret_cast<char*>(&m_emissionColor), sizeof(XMFLOAT4));
		}
		else if (str == "<Smoothness>")
		{
			in.read(reinterpret_cast<char*>(&m_smoothness), sizeof(float));
		}
		else if (str == "<Metallic>")
		{
			in.read(reinterpret_cast<char*>(&m_metallic), sizeof(float));
		}
		else if (str == "<AlbedoMap>")
		{
			File::ReadStringFromFile(in, str);

			if (str != "None")
			{
				CTexture* texture = CAssetManager::GetInstance()->GetTexture(str);

				if (texture != nullptr)
				{
					m_textureMask |= TEXTURE_MASK_ALBEDO_MAP;
					m_textures.push_back(texture);
				}
			}
		}
		else if (str == "<MetallicMap>")
		{
			File::ReadStringFromFile(in, str);

			if (str != "None")
			{
				CTexture* texture = CAssetManager::GetInstance()->GetTexture(str);

				if (texture != nullptr)
				{
					m_textureMask |= TEXTURE_MASK_METALLIC_MAP;
					m_textures.push_back(texture);
				}
			}
		}
		else if (str == "<NormalMap>")
		{
			File::ReadStringFromFile(in, str);

			if (str != "None")
			{
				CTexture* texture = CAssetManager::GetInstance()->GetTexture(str);

				if (texture != nullptr)
				{
					m_textureMask |= TEXTURE_MASK_NORMAL_MAP;
					m_textures.push_back(texture);
				}
			}
		}
		else if (str == "</Material>")
		{
			break;
		}
	}
}

void CMaterial::AddTexture(CTexture* texture)
{
	if (texture != nullptr)
	{
		switch (texture->GetType())
		{
		case TEXTURE_TYPE::ALBEDO_MAP:
			m_textureMask |= TEXTURE_MASK_ALBEDO_MAP;
			break;
		case TEXTURE_TYPE::METALLIC_MAP:
			m_textureMask |= TEXTURE_MASK_METALLIC_MAP;
			break;
		case TEXTURE_TYPE::NORMAL_MAP:
			m_textureMask |= TEXTURE_MASK_NORMAL_MAP;
			break;
		case TEXTURE_TYPE::SHADOW_MAP:
			m_textureMask |= TEXTURE_MASK_SHADOW_MAP;
			break;
		}

		m_textures.push_back(texture);
	}
}

void CMaterial::AddShader(CShader* shader)
{
	if (shader != nullptr)
	{
		m_shaders.push_back(shader);
	}
}

void CMaterial::UpdateShaderVariables(ID3D12GraphicsCommandList* d3d12GraphicsCommandList)
{
	d3d12GraphicsCommandList->SetGraphicsRoot32BitConstants((UINT)ROOT_PARAMETER_TYPE::OBJECT, 4, &m_albedoColor, 16);
	d3d12GraphicsCommandList->SetGraphicsRoot32BitConstants((UINT)ROOT_PARAMETER_TYPE::OBJECT, 1, &m_textureMask, 20);
	d3d12GraphicsCommandList->SetGraphicsRoot32BitConstants((UINT)ROOT_PARAMETER_TYPE::OBJECT, 2, &m_textureScale, 21);

	for (const auto& texture : m_textures)
	{
		texture->UpdateShaderVariable(d3d12GraphicsCommandList);
	}
}

void CMaterial::SetPipelineState(ID3D12GraphicsCommandList* d3d12GraphicsCommandList, RENDER_TYPE RenderType)
{
	m_shaders[(int)RenderType]->SetPipelineState(d3d12GraphicsCommandList, m_stateNum);
}
