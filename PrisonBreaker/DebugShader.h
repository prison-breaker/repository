#pragma once
#include "Shader.h"

class CDebugShader : public CGraphicsShader
{
public:
	CDebugShader() = default;
	virtual ~CDebugShader() = default;

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout(UINT StateNum);
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState(UINT StateNum);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob* D3D12ShaderBlob, UINT StateNum);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob* D3D12ShaderBlob, UINT StateNum);

	virtual void SetPipelineState(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, UINT StateNum);

	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);
};
