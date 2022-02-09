#pragma once
#include "Shader.h"

struct LIGHT;

class CDepthWriteShader : public CGraphicsShader
{
private:
	vector<LIGHT>&					 m_Lights;
	shared_ptr<CCamera>			     m_LightCamera{};

	shared_ptr<CPlayer>&             m_Player;
	vector<shared_ptr<CGameObject>>& m_Police;
	vector<shared_ptr<CGameObject>>& m_Structures;
					 
	ComPtr<ID3D12DescriptorHeap>     m_D3D12RtvDescriptorHeap{};
	ComPtr<ID3D12DescriptorHeap>     m_D3D12DsvDescriptorHeap{};
									 
	ComPtr<ID3D12Resource>		     m_D3D12DepthBuffer{};
	shared_ptr<CTexture>		     m_DepthTexture{};
									 
	XMFLOAT4X4						 m_ProjectionMatrixToTexture{};

public:
	CDepthWriteShader(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, vector<LIGHT>& Lights, shared_ptr<CPlayer>& Player, vector<shared_ptr<CGameObject>>& Police, vector<shared_ptr<CGameObject>>& Structures);
	virtual ~CDepthWriteShader() = default;

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout(UINT PSONum = 0);
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState(UINT PSONum = 0);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum = 0);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum = 0);

	virtual DXGI_FORMAT GetRTVFormat(UINT PSONum, UINT RTVNum = 0);
	virtual DXGI_FORMAT GetDSVFormat(UINT PSONum = 0);

	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);

	void CreateRtvAndDsvDescriptorHeaps(ID3D12Device* D3D12Device);
	void CreateRenderTargetViews(ID3D12Device* D3D12Device);
	void CreateDepthStencilView(ID3D12Device* D3D12Device);

	void CreateShadowMap(ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
};

//=========================================================================================================================

class CShadowMapShader : public CGraphicsShader
{
private:
	shared_ptr<CPlayer>& m_Player;
	vector<shared_ptr<CGameObject>>& m_Police;
	vector<shared_ptr<CGameObject>>& m_Structures;

public:
	CShadowMapShader(shared_ptr<CPlayer>& Player, vector<shared_ptr<CGameObject>>& Police, vector<shared_ptr<CGameObject>>& Structures);
	virtual ~CShadowMapShader() = default;

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout(UINT PSONum = 0);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum = 0);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum = 0);

	virtual void CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature, UINT PSONum = 0);

	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);
};
