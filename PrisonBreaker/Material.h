#pragma once
#include "Texture.h"

class CCamera;
class CShader;

class CMaterial
{
private:
	XMFLOAT4			         m_AlbedoColor{};

	vector<shared_ptr<CTexture>> m_Textures{};
	UINT						 m_TextureMask{};

	vector<shared_ptr<CShader>>  m_Shaders{};
	UINT						 m_StateNum{};

public:
	CMaterial() = default;
	~CMaterial() = default;

	void LoadMaterialFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, tifstream& InFile, bool IsSkinnedMesh);

	void UpdateShaderVariables(ID3D12GraphicsCommandList* D3D12GraphicsCommandList);

	void RegisterTexture(const shared_ptr<CTexture>& Texture);
	void RegisterShader(const shared_ptr<CShader>& Shader);

	void SetStateNum(SHADER_TYPE ShaderType);
	void SetPipelineState(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, RENDER_TYPE RenderType);
};