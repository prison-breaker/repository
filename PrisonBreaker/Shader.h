#pragma once
#include "Player.h"
#include "SkyBox.h"
#include "Texture.h"

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
	CGraphicsShader() = default;
	virtual ~CGraphicsShader() = default;

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout(UINT PSONum = 0);
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState(UINT PSONum = 0);
	virtual D3D12_BLEND_DESC CreateBlendState(UINT PSONum = 0);
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState(UINT PSONum = 0);
	virtual D3D12_STREAM_OUTPUT_DESC CreateStreamOutputState(UINT PSONum = 0);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum = 0);
	virtual D3D12_SHADER_BYTECODE CreateHullShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum = 0);
	virtual D3D12_SHADER_BYTECODE CreateDomainShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum = 0);
	virtual D3D12_SHADER_BYTECODE CreateGeometryShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum = 0);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum = 0);

	virtual D3D12_PRIMITIVE_TOPOLOGY_TYPE GetPrimitiveType(UINT PSONum);
	virtual DXGI_FORMAT GetRTVFormat(UINT PSONum, UINT RTVNum = 0);
	virtual DXGI_FORMAT GetDSVFormat(UINT PSONum = 0);

	virtual void CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature, UINT PSONum = 0);

	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);
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

	virtual void CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature, const XMUINT3& ThreadGroups);

	virtual void Dispatch(ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	virtual void Dispatch(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, const XMUINT3& ThreadGroups);
};
