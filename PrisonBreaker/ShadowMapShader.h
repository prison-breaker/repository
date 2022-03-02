#pragma once
#include "Shader.h"

struct LIGHT;

class CDepthWriteShader : public CGraphicsShader
{
private:
	vector<LIGHT>&					m_Lights;
	shared_ptr<CCamera>			    m_LightCamera{};

	vector<shared_ptr<CGameObject>> m_ShadyObjects{};
					 
	ComPtr<ID3D12DescriptorHeap>    m_D3D12RtvDescriptorHeap{};
	ComPtr<ID3D12DescriptorHeap>    m_D3D12DsvDescriptorHeap{};
									 
	ComPtr<ID3D12Resource>		    m_D3D12DepthBuffer{};
	shared_ptr<CTexture>		    m_DepthTexture{};
									 
	XMFLOAT4X4						m_ProjectionMatrixToTexture{};

public:
	CDepthWriteShader(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, vector<LIGHT>& Lights, vector<shared_ptr<CGameObject>>& ShadyObjects, UINT StateCount = 1);
	virtual ~CDepthWriteShader() = default;

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout(UINT StateNum = 0);
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState(UINT StateNum = 0);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob* D3D12ShaderBlob, UINT StateNum = 0);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob* D3D12ShaderBlob, UINT StateNum = 0);

	virtual DXGI_FORMAT GetRTVFormat(UINT StateNum = 0, UINT RenderTargetNum = 0);
	virtual DXGI_FORMAT GetDSVFormat(UINT StateNum = 0);

	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera, UINT StateNum = 0);

	void CreateRtvAndDsvDescriptorHeaps(ID3D12Device* D3D12Device);
	void CreateRenderTargetViews(ID3D12Device* D3D12Device);
	void CreateDepthStencilView(ID3D12Device* D3D12Device);

	void PrepareShadowMap(ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
};

//=========================================================================================================================

class CShadowMapShader : public CGraphicsShader
{
private:
	vector<shared_ptr<CGameObject>> m_ShadyObjects{};

public:
	CShadowMapShader(vector<shared_ptr<CGameObject>>& ShadyObjects, UINT StateCount = 1);
	virtual ~CShadowMapShader() = default;

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout(UINT StateNum = 0);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob* D3D12ShaderBlob, UINT StateNum = 0);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob* D3D12ShaderBlob, UINT StateNum = 0);

	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera, UINT StateNum = 0);
};
