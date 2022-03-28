#include "stdafx.h"
#include "Material.h"
#include "Shader.h"

void CMaterial::LoadMaterialFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, tifstream& InFile, bool IsSkinnedMesh)
{
	tstring Token{};
	shared_ptr<CTexture> Texture{};

#ifdef READ_BINARY_FILE
	while (true)
	{
		File::ReadStringFromFile(InFile, Token);

		if (Token == TEXT("<Material>"))
		{
			File::ReadStringFromFile(InFile, m_Name);

			RegisterShader(CShaderManager::GetInstance()->GetShader("ShadowMapShader"));
			RegisterShader(CShaderManager::GetInstance()->GetShader("DepthWriteShader"));

			m_StateNum = (IsSkinnedMesh) ? SHADER_TYPE_WITH_SKINNING : SHADER_TYPE_STANDARD;
		}
		else if (Token == TEXT("<AlbedoColor>"))
		{
			InFile.read(reinterpret_cast<TCHAR*>(&m_AlbedoColor), sizeof(XMFLOAT4));
		}
		else if (Token == TEXT("<AlbedoMap>"))
		{
			File::ReadStringFromFile(InFile, Token);

			if (Token != TEXT("Null"))
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
		else if (Token == TEXT("<MetallicMap>"))
		{
			File::ReadStringFromFile(InFile, Token);

			if (Token != TEXT("Null"))
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
		else if (Token == TEXT("<NormalMap>"))
		{
			File::ReadStringFromFile(InFile, Token);

			if (Token != TEXT("Null"))
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
		else if (Token == TEXT("</Material>"))
		{
			break;
		}
	}
#else
	while (InFile >> Token)
	{
		if (Token == TEXT("<Material>"))
		{
			RegisterShader(CShaderManager::GetInstance()->GetShader("ShadowMapShader"));
			RegisterShader(CShaderManager::GetInstance()->GetShader("DepthWriteShader"));

			m_StateNum = (IsSkinnedMesh) ? SHADER_TYPE_WITH_SKINNING : SHADER_TYPE_STANDARD;
		}
		else if (Token == TEXT("<AlbedoColor>"))
		{
			InFile >> m_AlbedoColor.x >> m_AlbedoColor.y >> m_AlbedoColor.z >> m_AlbedoColor.w;
		}
		else if (Token == TEXT("<AlbedoMap>"))
		{
			InFile >> Token;

			if (Token != TEXT("Null"))
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
		else if (Token == TEXT("<MetallicMap>"))
		{
			InFile >> Token;

			if (Token != TEXT("Null"))
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
		else if (Token == TEXT("<NormalMap>"))
		{
			InFile >> Token;

			if (Token != TEXT("Null"))
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
		else if (Token == TEXT("</Material>"))
		{
			break;
		}
	}
#endif
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

void CMaterial::RegisterTexture(const shared_ptr<CTexture>& Texture)
{
	if (Texture)
	{
		m_Textures.push_back(Texture);
	}
}

void CMaterial::RegisterShader(const shared_ptr<CShader>& Shader)
{
	if (Shader)
	{
		m_Shaders.push_back(Shader);
	}
}

void CMaterial::SetName(const tstring& Name)
{
	m_Name = Name;
}

const tstring& CMaterial::GetName() const
{
	return m_Name;
}

void CMaterial::SetStateNum(SHADER_TYPE ShaderType)
{
	switch (ShaderType)
	{
	case SHADER_TYPE_STANDARD:
	case SHADER_TYPE_WITH_SKINNING:
		m_StateNum = ShaderType;
		break;
	}
}

void CMaterial::SetPipelineState(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, RENDER_TYPE RenderType)
{
	switch (RenderType)
	{
	case RENDER_TYPE_STANDARD:
		static_pointer_cast<CGraphicsShader>(m_Shaders[RenderType])->SetPipelineState(D3D12GraphicsCommandList, m_StateNum);
		break;
	case RENDER_TYPE_DEPTH_WRITE:
		static_pointer_cast<CGraphicsShader>(m_Shaders[RenderType])->SetPipelineState(D3D12GraphicsCommandList, m_StateNum);
		break;
	}
}
