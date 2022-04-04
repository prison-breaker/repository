#pragma once
#include "Player.h"
#include "Guard.h"
#include "SkyBox.h"

class CShader
{
protected:
	vector<ComPtr<ID3D12PipelineState>> m_D3D12PipelineStates{};

public:
	CShader() = default;
	virtual ~CShader() = default;

	D3D12_SHADER_BYTECODE CompileShaderFromFile(LPCWSTR FileName, LPCSTR ShaderName, LPCSTR ShaderModelName, ID3DBlob* D3D12CodeBlob);
};

//=========================================================================================================================

class CGraphicsShader : public CShader
{
public:
	CGraphicsShader() = default;
	virtual ~CGraphicsShader() = default;

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout(UINT StateNum);
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState(UINT StateNum);
	virtual D3D12_BLEND_DESC CreateBlendState(UINT StateNum);
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState(UINT StateNum);
	virtual D3D12_STREAM_OUTPUT_DESC CreateStreamOutputState(UINT StateNum);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob* D3D12ShaderBlob, UINT StateNum);
	virtual D3D12_SHADER_BYTECODE CreateHullShader(ID3DBlob* D3D12ShaderBlob, UINT StateNum);
	virtual D3D12_SHADER_BYTECODE CreateDomainShader(ID3DBlob* D3D12ShaderBlob, UINT StateNum);
	virtual D3D12_SHADER_BYTECODE CreateGeometryShader(ID3DBlob* D3D12ShaderBlob, UINT StateNum);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob* D3D12ShaderBlob, UINT StateNum);

	virtual D3D12_PRIMITIVE_TOPOLOGY_TYPE GetPrimitiveType(UINT StateNum);
	virtual DXGI_FORMAT GetRTVFormat(UINT StateNum, UINT RenderTargetNum);
	virtual DXGI_FORMAT GetDSVFormat(UINT StateNum);

	virtual void CreatePipelineState(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature, UINT StateNum);
	virtual void SetPipelineState(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, UINT StateNum);

	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera, const vector<vector<shared_ptr<CGameObject>>>& GameObjects, UINT StateNum);
};

//=========================================================================================================================

class CComputeShader : public CShader
{
protected:
	XMUINT3 m_ThreadGroups{};

public:
	CComputeShader() = default;
	virtual ~CComputeShader() = default;

	virtual D3D12_SHADER_BYTECODE CreateComputeShader(ID3DBlob* D3D12ShaderBlob, UINT StateNum);

	virtual void CreatePipelineStates(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature, const XMUINT3& ThreadGroups, UINT StateNum);

	virtual void Dispatch(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, UINT StateNum);
	virtual void Dispatch(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, const XMUINT3& ThreadGroups, UINT StateNum);
};
