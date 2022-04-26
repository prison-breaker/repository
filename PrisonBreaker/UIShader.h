#pragma once
#include "Shader.h"

class CUIShader : public CGraphicsShader
{
public:
	CUIShader() = default;
	virtual ~CUIShader() = default;

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout(UINT StateNum);
	virtual D3D12_BLEND_DESC CreateBlendState(UINT StateNum);
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState(UINT StateNum);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob* D3D12ShaderBlob, UINT StateNum);
	virtual D3D12_SHADER_BYTECODE CreateGeometryShader(ID3DBlob* D3D12ShaderBlob, UINT StateNum);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob* D3D12ShaderBlob, UINT StateNum);

	virtual D3D12_PRIMITIVE_TOPOLOGY_TYPE GetPrimitiveType(UINT StateNum);

	virtual void SetPipelineState(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, UINT StateNum);
};
