#pragma once
#include "Texture.h"

class CCamera;
class CShader;

class CMaterial
{
private:
	XMFLOAT4			         m_AlbedoColor{};

	UINT						 m_TextureMask{};
	vector<shared_ptr<CTexture>> m_Textures{};

	shared_ptr<CShader>			 m_Shader{};

public:
	CMaterial() = default;
	~CMaterial() = default;

	void LoadMaterialFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, tifstream& InFile);

	void UpdateShaderVariables(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);

	const XMFLOAT4& GetAlbedoColor() const;

	void SetShader(const shared_ptr<CShader>& Shader);
	const shared_ptr<CShader>& GetShader() const;

	void RegisterTexture(const shared_ptr<CTexture>& Texture);
};
