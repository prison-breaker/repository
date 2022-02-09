#pragma once
#include "Shader.h"

class CSkyBoxShader : public CGraphicsShader
{
private:
	shared_ptr<CSkyBox> m_SkyBox{};

public:
	CSkyBoxShader(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	virtual ~CSkyBoxShader() = default;

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout(UINT PSONum = 0);
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState(UINT PSONum);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum);
	virtual D3D12_SHADER_BYTECODE CreateGeometryShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum);

	virtual D3D12_PRIMITIVE_TOPOLOGY_TYPE GetPrimitiveType(UINT PSONum = 0);

	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);
};
