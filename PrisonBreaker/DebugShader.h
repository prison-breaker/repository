#pragma once
#include "Shader.h"

class CDebugShader : public CGraphicsShader
{
public:
	CDebugShader() = default;
	virtual ~CDebugShader() = default;

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout(UINT PSONum = 0);
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState(UINT PSONum = 0);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum = 0);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum = 0);
};
