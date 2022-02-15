#include "stdafx.h"
#include "Material.h"

void CMaterial::LoadMaterialFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, tifstream& InFile)
{
	tstring Token{};
	shared_ptr<CTexture> Texture{};

	while (InFile >> Token)
	{
		if (!Token.compare(TEXT("<Material>")))
		{
			continue;
		}
		else if (!Token.compare(TEXT("<AlbedoColor>")))
		{
			InFile >> m_AlbedoColor.x >> m_AlbedoColor.y >> m_AlbedoColor.z >> m_AlbedoColor.w;
		}
		else if (!Token.compare(TEXT("<AlbedoMap>")))
		{
			InFile >> Token;

			if (Token.compare(TEXT("Null")))
			{
				Texture = CTextureManager::GetInstance()->GetTexture(Token);

				if (!Texture)
				{
					Texture = make_shared<CTexture>();
					Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, TEXTURE_TYPE_ALBEDO_MAP, Token);
				}

				m_TextureMask |= TEXTURE_MASK_ALBEDO_MAP;
				m_Textures.push_back(Texture);
				CTextureManager::GetInstance()->RegisterTexture(Token, Texture);
			}
		}
		else if (!Token.compare(TEXT("<MetallicMap>")))
		{
			InFile >> Token;

			if (Token.compare(TEXT("Null")))
			{
				Texture = CTextureManager::GetInstance()->GetTexture(Token);

				if (!Texture)
				{
					Texture = make_shared<CTexture>();
					Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, TEXTURE_TYPE_METALLIC_MAP, Token);
				}

				m_TextureMask |= TEXTURE_MASK_METALLIC_MAP;
				m_Textures.push_back(Texture);
				CTextureManager::GetInstance()->RegisterTexture(Token, Texture);
			}
		}
		else if (!Token.compare(TEXT("<NormalMap>")))
		{
			InFile >> Token;

			if (Token.compare(TEXT("Null")))
			{
				Texture = CTextureManager::GetInstance()->GetTexture(Token);

				if (!Texture)
				{
					Texture = make_shared<CTexture>();
					Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, TEXTURE_TYPE_NORMAL_MAP, Token);
				}
				
				m_TextureMask |= TEXTURE_MASK_NORMAL_MAP;
				m_Textures.push_back(Texture);
				CTextureManager::GetInstance()->RegisterTexture(Token, Texture);
			}
		}
		else if (!Token.compare(TEXT("</Material>")))
		{
			break;
		}
	}
}

void CMaterial::UpdateShaderVariables(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	D3D12GraphicsCommandList->SetGraphicsRoot32BitConstants(ROOT_PARAMETER_TYPE_OBJECT, 4, &m_AlbedoColor, 16);
	D3D12GraphicsCommandList->SetGraphicsRoot32BitConstants(ROOT_PARAMETER_TYPE_OBJECT, 1, &m_TextureMask, 20);

	for (const auto& Texture : m_Textures)
	{
		if (Texture)
		{
			Texture->UpdateShaderVariable(D3D12GraphicsCommandList);
		}
	}
}

const XMFLOAT4& CMaterial::GetAlbedoColor() const
{
	return m_AlbedoColor;
}

void CMaterial::RegisterTexture(const shared_ptr<CTexture>& Texture)
{
	if (Texture)
	{
		m_Textures.push_back(Texture);
	}
}
