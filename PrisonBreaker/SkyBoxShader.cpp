#include "stdafx.h"
#include "SkyBoxShader.h"

CSkyBoxShader::CSkyBoxShader(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	shared_ptr<CTexture> Texture{ make_shared<CTexture>() };
	Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, TEXTURE_TYPE_DIFFUSEMAP, TEXT("Texture/SkyBox_Back.dds"));
	Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, TEXTURE_TYPE_DIFFUSEMAP, TEXT("Texture/SkyBox_Front.dds"));
	Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, TEXTURE_TYPE_DIFFUSEMAP, TEXT("Texture/SkyBox_Right.dds"));
	Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, TEXTURE_TYPE_DIFFUSEMAP, TEXT("Texture/SkyBox_Left.dds"));
	Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, TEXTURE_TYPE_DIFFUSEMAP, TEXT("Texture/SkyBox_Top.dds"));
	Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, TEXTURE_TYPE_DIFFUSEMAP, TEXT("Texture/SkyBox_Bottom.dds"));
	CTextureManager::GetInstance()->RegisterTexture(TEXT("SkyBox"), Texture);

	m_SkyBox = make_shared<CSkyBox>(D3D12Device, D3D12GraphicsCommandList);
	m_SkyBox->SetTexture(Texture);
}

D3D12_INPUT_LAYOUT_DESC CSkyBoxShader::CreateInputLayout(UINT PSONum)
{
	const UINT InputElementCount{ 2 };
	D3D12_INPUT_ELEMENT_DESC* D3D12InputElementDescs{ new D3D12_INPUT_ELEMENT_DESC[InputElementCount] };

	D3D12InputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12InputElementDescs[1] = { "SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC D3D12InputLayoutDesc{};

	D3D12InputLayoutDesc.pInputElementDescs = D3D12InputElementDescs;
	D3D12InputLayoutDesc.NumElements = InputElementCount;

	return D3D12InputLayoutDesc;
}

D3D12_DEPTH_STENCIL_DESC CSkyBoxShader::CreateDepthStencilState(UINT PSONum)
{
	D3D12_DEPTH_STENCIL_DESC D3D12DepthStencilDesc{};

	D3D12DepthStencilDesc.DepthEnable = true;
	D3D12DepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	D3D12DepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	D3D12DepthStencilDesc.StencilEnable = false;
	D3D12DepthStencilDesc.StencilReadMask = 0xff;
	D3D12DepthStencilDesc.StencilWriteMask = 0xff;
	D3D12DepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	D3D12DepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	D3D12DepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	D3D12DepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	D3D12DepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	D3D12DepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	D3D12DepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	D3D12DepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return D3D12DepthStencilDesc;
}

D3D12_SHADER_BYTECODE CSkyBoxShader::CreateVertexShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum)
{
	return CGraphicsShader::CompileShaderFromFile(L"GameSceneShader.hlsl", "VS_SkyBox", "vs_5_1", D3D12ShaderBlob);
}

D3D12_SHADER_BYTECODE CSkyBoxShader::CreateGeometryShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum)
{
	return CGraphicsShader::CompileShaderFromFile(L"GameSceneShader.hlsl", "GS_SkyBox", "gs_5_1", D3D12ShaderBlob);
}

D3D12_SHADER_BYTECODE CSkyBoxShader::CreatePixelShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum)
{
	return CGraphicsShader::CompileShaderFromFile(L"GameSceneShader.hlsl", "PS_SkyBox", "ps_5_1", D3D12ShaderBlob);
}

D3D12_PRIMITIVE_TOPOLOGY_TYPE CSkyBoxShader::GetPrimitiveType(UINT PSONum)
{
	return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
}

void CSkyBoxShader::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera)
{
	CGraphicsShader::Render(D3D12GraphicsCommandList, Camera);

	if (m_SkyBox)
	{
		m_SkyBox->Render(D3D12GraphicsCommandList, Camera);
	}
}