#include "stdafx.h"
#include "UIShader.h"

D3D12_INPUT_LAYOUT_DESC CUIShader::CreateInputLayout(UINT StateNum)
{
	const UINT InputElementCount{ 4 };
	D3D12_INPUT_ELEMENT_DESC* D3D12InputElementDescs{ new D3D12_INPUT_ELEMENT_DESC[InputElementCount] };

	D3D12InputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12InputElementDescs[1] = { "SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12InputElementDescs[2] = { "CELLCOUNT", 0, DXGI_FORMAT_R32G32_UINT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12InputElementDescs[3] = { "CELLINDEX", 0, DXGI_FORMAT_R32_UINT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC D3D12InputLayoutDesc{};

	D3D12InputLayoutDesc.pInputElementDescs = D3D12InputElementDescs;
	D3D12InputLayoutDesc.NumElements = InputElementCount;

	return D3D12InputLayoutDesc;
}

D3D12_BLEND_DESC CUIShader::CreateBlendState(UINT StateNum)
{
	D3D12_BLEND_DESC D3D12BlendDesc{};

	D3D12BlendDesc.AlphaToCoverageEnable = true;
	D3D12BlendDesc.IndependentBlendEnable = false;
	D3D12BlendDesc.RenderTarget[0].BlendEnable = true;
	D3D12BlendDesc.RenderTarget[0].LogicOpEnable = false;
	D3D12BlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	D3D12BlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	D3D12BlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	D3D12BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	D3D12BlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	D3D12BlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	D3D12BlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	D3D12BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return D3D12BlendDesc;
}

D3D12_SHADER_BYTECODE CUIShader::CreateVertexShader(ID3DBlob* D3D12ShaderBlob, UINT StateNum)
{
	return CGraphicsShader::CompileShaderFromFile(L"GameSceneShader.hlsl", "VS_Image", "vs_5_1", D3D12ShaderBlob);
}

D3D12_SHADER_BYTECODE CUIShader::CreateGeometryShader(ID3DBlob* D3D12ShaderBlob, UINT StateNum)
{
	return CGraphicsShader::CompileShaderFromFile(L"GameSceneShader.hlsl", "GS_Image", "gs_5_1", D3D12ShaderBlob);
}

D3D12_SHADER_BYTECODE CUIShader::CreatePixelShader(ID3DBlob* D3D12ShaderBlob, UINT StateNum)
{
	return CGraphicsShader::CompileShaderFromFile(L"GameSceneShader.hlsl", "PS_Image", "ps_5_1", D3D12ShaderBlob);
}

D3D12_PRIMITIVE_TOPOLOGY_TYPE CUIShader::GetPrimitiveType(UINT StateNum)
{
	return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
}

void CUIShader::SetPipelineState(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, UINT StateNum)
{
	if (CShaderManager::GetInstance()->SetPipelineState(TEXT("UIShader"), StateNum))
	{
		if (m_D3D12PipelineStates[StateNum])
		{
			D3D12GraphicsCommandList->SetPipelineState(m_D3D12PipelineStates[StateNum].Get());
		}
	}
}
