#pragma once
#include "Texture.h"

class CMaterial
{
private:
	XMFLOAT4			         m_AlbedoColor{};

	UINT						 m_TextureMask{};
	vector<shared_ptr<CTexture>> m_Textures{};

public:
	CMaterial() = default;
	~CMaterial() = default;

	void LoadMaterialFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, tifstream& InFile);

	void UpdateShaderVariables(ID3D12GraphicsCommandList* D3D12GraphicsCommandList);

	const XMFLOAT4& GetAlbedoColor() const;

	void RegisterTexture(const shared_ptr<CTexture>& Texture);
};
