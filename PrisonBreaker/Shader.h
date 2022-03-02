#pragma once
#include "Player.h"
#include "SkyBox.h"

class CCamera;

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
	CGraphicsShader(UINT StateCount = 1);
	virtual ~CGraphicsShader() = default;

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout(UINT StateNum = 0);
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState(UINT StateNum = 0);
	virtual D3D12_BLEND_DESC CreateBlendState(UINT StateNum = 0);
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState(UINT StateNum = 0);
	virtual D3D12_STREAM_OUTPUT_DESC CreateStreamOutputState(UINT StateNum = 0);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob* D3D12ShaderBlob, UINT StateNum = 0);
	virtual D3D12_SHADER_BYTECODE CreateHullShader(ID3DBlob* D3D12ShaderBlob, UINT StateNum = 0);
	virtual D3D12_SHADER_BYTECODE CreateDomainShader(ID3DBlob* D3D12ShaderBlob, UINT StateNum = 0);
	virtual D3D12_SHADER_BYTECODE CreateGeometryShader(ID3DBlob* D3D12ShaderBlob, UINT StateNum = 0);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob* D3D12ShaderBlob, UINT StateNum = 0);

	virtual D3D12_PRIMITIVE_TOPOLOGY_TYPE GetPrimitiveType(UINT StateNum = 0);
	virtual DXGI_FORMAT GetRTVFormat(UINT StateNum = 0, UINT RenderTargetNum = 0);
	virtual DXGI_FORMAT GetDSVFormat(UINT StateNum = 0);

	virtual void CreatePipelineState(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature, UINT StateNum = 0);
	
	virtual void ReleaseUploadBuffers();

	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera, UINT StateNum = 0);
};

//=========================================================================================================================

class CComputeShader : public CShader
{
protected:
	XMUINT3 m_ThreadGroups{};

public:
	CComputeShader() = default;
	virtual ~CComputeShader() = default;

	virtual D3D12_SHADER_BYTECODE CreateComputeShader(ID3DBlob* D3D12ShaderBlob);

	virtual void CreatePipelineStates(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature, const XMUINT3& ThreadGroups);

	virtual void Dispatch(ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	virtual void Dispatch(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, const XMUINT3& ThreadGroups);
};
