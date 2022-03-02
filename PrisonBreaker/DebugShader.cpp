#include "stdafx.h"
#include "DebugShader.h"

D3D12_INPUT_LAYOUT_DESC CDebugShader::CreateInputLayout(UINT StateNum)
{
	const UINT InputElementCount{ 1 };
	D3D12_INPUT_ELEMENT_DESC* D3D12InputElementDescs{ new D3D12_INPUT_ELEMENT_DESC[InputElementCount] };

	D3D12InputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC D3D12InputLayoutDesc{};

	D3D12InputLayoutDesc.pInputElementDescs = D3D12InputElementDescs;
	D3D12InputLayoutDesc.NumElements = InputElementCount;

	return D3D12InputLayoutDesc;
}

D3D12_RASTERIZER_DESC CDebugShader::CreateRasterizerState(UINT StateNum)
{
	D3D12_RASTERIZER_DESC D3D12RasterizerDesc{};

	D3D12RasterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;
	D3D12RasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	D3D12RasterizerDesc.FrontCounterClockwise = false;
	D3D12RasterizerDesc.DepthBias = 0;
	D3D12RasterizerDesc.DepthBiasClamp = 0.0f;
	D3D12RasterizerDesc.SlopeScaledDepthBias = 0.0f;
	D3D12RasterizerDesc.DepthClipEnable = true;
	D3D12RasterizerDesc.MultisampleEnable = false;
	D3D12RasterizerDesc.AntialiasedLineEnable = false;
	D3D12RasterizerDesc.ForcedSampleCount = 0;
	D3D12RasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return D3D12RasterizerDesc;
}

D3D12_SHADER_BYTECODE CDebugShader::CreateVertexShader(ID3DBlob* D3D12ShaderBlob, UINT StateNum)
{
	return CGraphicsShader::CompileShaderFromFile(L"GameSceneShader.hlsl", "VS_Position", "vs_5_1", D3D12ShaderBlob);
}

D3D12_SHADER_BYTECODE CDebugShader::CreatePixelShader(ID3DBlob* D3D12ShaderBlob, UINT StateNum)
{
	return CGraphicsShader::CompileShaderFromFile(L"GameSceneShader.hlsl", "PS_BoundingBox", "ps_5_1", D3D12ShaderBlob);
}
