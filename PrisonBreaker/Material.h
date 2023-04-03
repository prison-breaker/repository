#pragma once
#include "Asset.h"

class CTexture;
class CShader;

class CMaterial : public CAsset
{
	friend class CAssetManager;

private:
	XMFLOAT4		  m_albedoColor;
	XMFLOAT4		  m_emissionColor;
	float			  m_smoothness;
	float			  m_metallic;

	int				  m_textureMask;
	XMFLOAT2		  m_textureScale;
	vector<CTexture*> m_textures;

	vector<CShader*>  m_shaders;
	int	              m_stateNum;

private:
	CMaterial();

public:
	virtual ~CMaterial();

	void SetStateNum(int stateNum);
	int GetStateNum();

	void Load(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList, ifstream& in);

	void AddTexture(CTexture* texture);
	void AddShader(CShader* shader);

	void UpdateShaderVariables(ID3D12GraphicsCommandList* d3d12GraphicsCommandList);
	void SetPipelineState(ID3D12GraphicsCommandList* d3d12GraphicsCommandList, RENDER_TYPE RenderType);
};
