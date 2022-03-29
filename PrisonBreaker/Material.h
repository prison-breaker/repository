#pragma once
#include "Texture.h"

class CCamera;
class CShader;

class CMaterial
{
private:
	tstring						 m_Name{};

	XMFLOAT4			         m_AlbedoColor{};

	vector<shared_ptr<CTexture>> m_Textures{};
	UINT						 m_TextureMask{};
	XMFLOAT2					 m_TextureScale{ 1.0f, 1.0f }; // Tiling

	vector<shared_ptr<CShader>>  m_Shaders{};
	UINT						 m_StateNum{};

public:
	CMaterial() = default;
	~CMaterial() = default;

	void LoadMaterialInfoFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, tifstream& InFile);

	void UpdateShaderVariables(ID3D12GraphicsCommandList* D3D12GraphicsCommandList);

	void RegisterTexture(const shared_ptr<CTexture>& Texture);
	void RegisterShader(const shared_ptr<CShader>& Shader);

	void SetName(const tstring & Name);
	const tstring& GetName() const;

	void SetStateNum(SHADER_TYPE ShaderType);

	void SetPipelineState(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, RENDER_TYPE RenderType);
};
