#include "pch.h"
#include "Material.h"

#include "AssetManager.h"

#include "Texture.h"
#include "Shader.h"

CMaterial::CMaterial() :
	m_color(1.0f, 1.0f, 1.0f, 1.0f),
	m_textureMask(),
	m_textureScale(1.0f, 1.0f),
	m_textures(),
	m_shaders(),
	m_stateNum()
{
	m_textures.resize(static_cast<size_t>(TEXTURE_TYPE::COUNT));
}

CMaterial::~CMaterial()
{
}

void CMaterial::SetColor(const XMFLOAT4& color)
{
	m_color = color;
}

const XMFLOAT4& CMaterial::GetColor()
{
	return m_color;
}

void CMaterial::SetStateNum(int stateNum)
{
	if (stateNum >= 0)
	{
		m_stateNum = stateNum;
	}
}

int CMaterial::GetStateNum()
{
	return m_stateNum;
}

void CMaterial::SetTexture(CTexture* texture)
{
	if (texture != nullptr)
	{
		switch (texture->GetType())
		{
		case TEXTURE_TYPE::ALBEDO_MAP:
			m_textureMask |= TEXTURE_MASK_ALBEDO_MAP;
			break;
		case TEXTURE_TYPE::NORMAL_MAP:
			m_textureMask |= TEXTURE_MASK_NORMAL_MAP;
			break;
		case TEXTURE_TYPE::SHADOW_MAP:
			m_textureMask |= TEXTURE_MASK_SHADOW_MAP;
			break;
		}

		m_textures[static_cast<int>(texture->GetType())] = texture;
	}
}

const vector<CTexture*>& CMaterial::GetTextures()
{
	return m_textures;
}

void CMaterial::AddShader(CShader* shader)
{
	if (shader != nullptr)
	{
		m_shaders.push_back(shader);
	}
}

const vector<CShader*>& CMaterial::GetShaders()
{
	return m_shaders;
}

void CMaterial::Load(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList, ifstream& in)
{
	string str;

	while (true)
	{
		File::ReadStringFromFile(in, str);

		if (str == "<Name>")
		{
			File::ReadStringFromFile(in, m_name);
		}
		else if (str == "<Color>")
		{
			in.read(reinterpret_cast<char*>(&m_color), sizeof(XMFLOAT4));
		}
		else if (str == "<TextureScale>")
		{
			in.read(reinterpret_cast<char*>(&m_textureScale), sizeof(XMFLOAT2));
		}
		else if (str == "<AlbedoMap>" || str == "<NormalMap>")
		{
			File::ReadStringFromFile(in, str);

			CTexture* texture = CAssetManager::GetInstance()->GetTexture(str);

			if (texture != nullptr)
			{
				SetTexture(texture);
			}
		}
		else if (str == "<Shaders>")
		{
			int shaderCount = 0;

			in.read(reinterpret_cast<char*>(&shaderCount), sizeof(int));

			for (int i = 0; i < shaderCount; ++i)
			{
				File::ReadStringFromFile(in, str);
				
				CShader* shader = CAssetManager::GetInstance()->GetShader(str);

				AddShader(shader);
			}
		}
		else if (str == "<StateNum>")
		{
			in.read(reinterpret_cast<char*>(&m_stateNum), sizeof(int));
		}
		else if (str == "</Material>")
		{
			break;
		}
	}
}

void CMaterial::UpdateShaderVariables(ID3D12GraphicsCommandList* d3d12GraphicsCommandList)
{
	d3d12GraphicsCommandList->SetGraphicsRoot32BitConstants(static_cast<UINT>(ROOT_PARAMETER_TYPE::OBJECT), 4, &m_color, 16);
	d3d12GraphicsCommandList->SetGraphicsRoot32BitConstants(static_cast<UINT>(ROOT_PARAMETER_TYPE::OBJECT), 1, &m_textureMask, 20);
	d3d12GraphicsCommandList->SetGraphicsRoot32BitConstants(static_cast<UINT>(ROOT_PARAMETER_TYPE::OBJECT), 2, &m_textureScale, 21);

	for (const auto& texture : m_textures)
	{
		if (texture != nullptr)
		{
			texture->UpdateShaderVariable(d3d12GraphicsCommandList);
		}
	}
}

void CMaterial::SetPipelineState(ID3D12GraphicsCommandList* d3d12GraphicsCommandList, RENDER_TYPE renderType)
{
	m_shaders[static_cast<int>(renderType)]->SetPipelineState(d3d12GraphicsCommandList, m_stateNum);
}
