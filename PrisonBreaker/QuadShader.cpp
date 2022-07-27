#include "stdafx.h"
#include "QuadShader.h"

D3D12_INPUT_LAYOUT_DESC CQuadShader::CreateInputLayout(UINT StateNum)
{
	const UINT InputElementCount{ 5 };
	D3D12_INPUT_ELEMENT_DESC* D3D12InputElementDescs{ new D3D12_INPUT_ELEMENT_DESC[InputElementCount] };

	D3D12InputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12InputElementDescs[1] = { "SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12InputElementDescs[2] = { "ALPHACOLOR", 0, DXGI_FORMAT_R32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12InputElementDescs[3] = { "CELLCOUNT", 0, DXGI_FORMAT_R32G32_UINT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12InputElementDescs[4] = { "CELLINDEX", 0, DXGI_FORMAT_R32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC D3D12InputLayoutDesc{};

	D3D12InputLayoutDesc.pInputElementDescs = D3D12InputElementDescs;
	D3D12InputLayoutDesc.NumElements = InputElementCount;

	return D3D12InputLayoutDesc;
}

D3D12_BLEND_DESC CQuadShader::CreateBlendState(UINT StateNum)
{
	D3D12_BLEND_DESC D3D12BlendDesc{ CGraphicsShader::CreateBlendState(StateNum) };

	switch (StateNum)
	{
	case 0: // Bilboard
	case 1: // SkyBox
		D3D12BlendDesc.AlphaToCoverageEnable = true;
		D3D12BlendDesc.RenderTarget[0].BlendEnable = true;
		D3D12BlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		D3D12BlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		break;
	case 2: // UI
		D3D12BlendDesc.RenderTarget[0].BlendEnable = true;
		D3D12BlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		D3D12BlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		break;
	}

	return D3D12BlendDesc;
}

D3D12_DEPTH_STENCIL_DESC CQuadShader::CreateDepthStencilState(UINT StateNum)
{
	D3D12_DEPTH_STENCIL_DESC D3D12DepthStencilDesc{ CGraphicsShader::CreateDepthStencilState(StateNum) };

	switch (StateNum)
	{
	case 1: // SkyBox
		D3D12DepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		D3D12DepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		break;
	case 2: // UI
		D3D12DepthStencilDesc.DepthEnable = false;
		D3D12DepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		break;
	}

	return D3D12DepthStencilDesc;
}

D3D12_SHADER_BYTECODE CQuadShader::CreateVertexShader(ID3DBlob* D3D12ShaderBlob, UINT StateNum)
{
	switch (StateNum)
	{
	case 0: // Bilboard
	case 2: // UI
		return CGraphicsShader::CompileShaderFromFile(L"GameSceneShader.hlsl", "VS_Quad", "vs_5_1", D3D12ShaderBlob);
	case 1: // SkyBox
		return CGraphicsShader::CompileShaderFromFile(L"GameSceneShader.hlsl", "VS_SkyBox", "vs_5_1", D3D12ShaderBlob);
	}

	return CGraphicsShader::CreateVertexShader(D3D12ShaderBlob, StateNum);
}

D3D12_SHADER_BYTECODE CQuadShader::CreateGeometryShader(ID3DBlob* D3D12ShaderBlob, UINT StateNum)
{
	switch (StateNum)
	{
	case 0: // Bilboard
		return CGraphicsShader::CompileShaderFromFile(L"GameSceneShader.hlsl", "GS_Bilboard", "gs_5_1", D3D12ShaderBlob);
	case 1: // SkyBox
		return CGraphicsShader::CompileShaderFromFile(L"GameSceneShader.hlsl", "GS_SkyBox", "gs_5_1", D3D12ShaderBlob);
	case 2: // UI
		return CGraphicsShader::CompileShaderFromFile(L"GameSceneShader.hlsl", "GS_UI", "gs_5_1", D3D12ShaderBlob);
	}

	return CGraphicsShader::CreateGeometryShader(D3D12ShaderBlob, StateNum);
}

D3D12_SHADER_BYTECODE CQuadShader::CreatePixelShader(ID3DBlob* D3D12ShaderBlob, UINT StateNum)
{
	switch (StateNum)
	{
	case 0: // Bilboard
		return CGraphicsShader::CompileShaderFromFile(L"GameSceneShader.hlsl", "PS_Bilboard", "ps_5_1", D3D12ShaderBlob);
	case 1: // SkyBox
	case 2: // UI
		return CGraphicsShader::CompileShaderFromFile(L"GameSceneShader.hlsl", "PS_Quad", "ps_5_1", D3D12ShaderBlob);
	}

	return CGraphicsShader::CreatePixelShader(D3D12ShaderBlob, StateNum);
}

D3D12_PRIMITIVE_TOPOLOGY_TYPE CQuadShader::GetPrimitiveType(UINT StateNum)
{
	return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
}

void CQuadShader::SetPipelineState(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, UINT StateNum)
{
	if (CShaderManager::GetInstance()->SetPipelineState(TEXT("QuadShader"), StateNum))
	{
		if (m_D3D12PipelineStates[StateNum])
		{
			D3D12GraphicsCommandList->SetPipelineState(m_D3D12PipelineStates[StateNum].Get());
		}
	}
}
