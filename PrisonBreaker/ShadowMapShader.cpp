#include "stdafx.h"
#include "ShadowMapShader.h"
#include "GameScene.h"

CDepthWriteShader::CDepthWriteShader(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, vector<LIGHT>& Lights, shared_ptr<CPlayer>& Player, vector<shared_ptr<CGameObject>>& Police, vector<shared_ptr<CGameObject>>& Structures) :
	m_Lights{ Lights },
	m_Player{ Player },
	m_Police{ Police },
	m_Structures{ Structures },
	m_ProjectionMatrixToTexture{ 0.5f,  0.0f, 0.0f, 0.0f,
	                             0.0f, -0.5f, 0.0f, 0.0f,
	                             0.0f,  0.0f, 1.0f, 0.0f,
	                             0.5f,  0.5f, 0.0f, 1.0f }
{
	m_LightCamera = make_shared<CCamera>();
	m_LightCamera->SetViewport(0, 0, DEPTH_BUFFER_WIDTH, DEPTH_BUFFER_HEIGHT, 0.0f, 1.0f);
	m_LightCamera->SetScissorRect(0, 0, DEPTH_BUFFER_WIDTH, DEPTH_BUFFER_HEIGHT);
	m_LightCamera->CreateShaderVariables(D3D12Device, D3D12GraphicsCommandList);

	m_DepthTexture = make_shared<CTexture>();
	m_DepthTexture->CreateTexture2D(D3D12Device, TEXTURE_TYPE_SHADOW_MAP, DEPTH_BUFFER_WIDTH, DEPTH_BUFFER_HEIGHT, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, DXGI_FORMAT_R32_FLOAT, D3D12_CLEAR_VALUE{ DXGI_FORMAT_R32_FLOAT, { 1.0f, 1.0f, 1.0f, 1.0f } });
	CTextureManager::GetInstance()->RegisterTexture(TEXT("ShadowMap"), m_DepthTexture);

	CreateRtvAndDsvDescriptorHeaps(D3D12Device);
	CreateRenderTargetViews(D3D12Device);
	CreateDepthStencilView(D3D12Device);
}

D3D12_INPUT_LAYOUT_DESC CDepthWriteShader::CreateInputLayout(UINT PSONum)
{
	const UINT InputElementCount{ 5 };
	D3D12_INPUT_ELEMENT_DESC* D3D12InputElementDescs{ new D3D12_INPUT_ELEMENT_DESC[InputElementCount] };

	D3D12InputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12InputElementDescs[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12InputElementDescs[2] = { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 2, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12InputElementDescs[3] = { "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 3, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12InputElementDescs[4] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 4, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC D3D12InputLayoutDesc{};

	D3D12InputLayoutDesc.pInputElementDescs = D3D12InputElementDescs;
	D3D12InputLayoutDesc.NumElements = InputElementCount;

	return D3D12InputLayoutDesc;
}

D3D12_RASTERIZER_DESC CDepthWriteShader::CreateRasterizerState(UINT PSONum)
{
	D3D12_RASTERIZER_DESC D3D12RasterizerDesc{};

	D3D12RasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	D3D12RasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	D3D12RasterizerDesc.FrontCounterClockwise = false;
	D3D12RasterizerDesc.DepthBias = 50000;
	D3D12RasterizerDesc.DepthBiasClamp = 0.0f;
	D3D12RasterizerDesc.SlopeScaledDepthBias = 1.0f;
	D3D12RasterizerDesc.DepthClipEnable = true;
	D3D12RasterizerDesc.MultisampleEnable = false;
	D3D12RasterizerDesc.AntialiasedLineEnable = false;
	D3D12RasterizerDesc.ForcedSampleCount = 0;
	D3D12RasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return D3D12RasterizerDesc;
}

D3D12_SHADER_BYTECODE CDepthWriteShader::CreateVertexShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum)
{
	return CGraphicsShader::CompileShaderFromFile(L"GameSceneShader.hlsl", "VS_Main", "vs_5_1", D3D12ShaderBlob);
}

D3D12_SHADER_BYTECODE CDepthWriteShader::CreatePixelShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum)
{
	return CGraphicsShader::CompileShaderFromFile(L"GameSceneShader.hlsl", "PS_DetphWrite", "ps_5_1", D3D12ShaderBlob);
}

DXGI_FORMAT CDepthWriteShader::GetRTVFormat(UINT PSONum, UINT RTVNum)
{
	return DXGI_FORMAT_R32_FLOAT;
}

DXGI_FORMAT CDepthWriteShader::GetDSVFormat(UINT PSONum)
{
	return DXGI_FORMAT_D32_FLOAT;
}

void CDepthWriteShader::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera)
{
	if (Camera)
	{
		Camera->RSSetViewportsAndScissorRects(D3D12GraphicsCommandList);
		Camera->UpdateShaderVariables(D3D12GraphicsCommandList);
	}

	CGraphicsShader::Render(D3D12GraphicsCommandList, Camera);

	m_Player->UpdateTransform(Matrix4x4::Identity());
	m_Player->Render(D3D12GraphicsCommandList, Camera);

	for (const auto& Police : m_Police)
	{
		Police->UpdateTransform(Matrix4x4::Identity());
		Police->Render(D3D12GraphicsCommandList, Camera);
	}

	for (const auto& Structure : m_Structures)
	{
		Structure->UpdateTransform(Matrix4x4::Identity());
		Structure->Render(D3D12GraphicsCommandList, Camera);
	}
}

void CDepthWriteShader::CreateRtvAndDsvDescriptorHeaps(ID3D12Device* D3D12Device)
{
	D3D12_DESCRIPTOR_HEAP_DESC D3D12DescriptorHeapDesc{};

	D3D12DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	D3D12DescriptorHeapDesc.NumDescriptors = 1;
	D3D12DescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	D3D12DescriptorHeapDesc.NodeMask = 0;
	DX::ThrowIfFailed(D3D12Device->CreateDescriptorHeap(&D3D12DescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)m_D3D12RtvDescriptorHeap.GetAddressOf()));

	D3D12DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	DX::ThrowIfFailed(D3D12Device->CreateDescriptorHeap(&D3D12DescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)m_D3D12DsvDescriptorHeap.GetAddressOf()));
}

void CDepthWriteShader::CreateRenderTargetViews(ID3D12Device* D3D12Device)
{
	D3D12_RENDER_TARGET_VIEW_DESC D3D12RenderTargetViewDesc{};

	D3D12RenderTargetViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
	D3D12RenderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	D3D12RenderTargetViewDesc.Texture2D.MipSlice = 0;
	D3D12RenderTargetViewDesc.Texture2D.PlaneSlice = 0;

	D3D12Device->CreateRenderTargetView(m_DepthTexture->GetResource(), &D3D12RenderTargetViewDesc, m_D3D12RtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
}

void CDepthWriteShader::CreateDepthStencilView(ID3D12Device* D3D12Device)
{
	D3D12_DEPTH_STENCIL_VIEW_DESC D3D12DepthStencilViewDesc{};

	D3D12DepthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	D3D12DepthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	D3D12DepthStencilViewDesc.Flags = D3D12_DSV_FLAG_NONE;

	m_D3D12DepthBuffer = DX::CreateTexture2DResource(D3D12Device, DEPTH_BUFFER_WIDTH, DEPTH_BUFFER_HEIGHT, 1, 1, D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL, DXGI_FORMAT_D32_FLOAT, D3D12_CLEAR_VALUE{ DXGI_FORMAT_D32_FLOAT, { 1.0f, 0.0f } });
	D3D12Device->CreateDepthStencilView(m_D3D12DepthBuffer.Get(), &D3D12DepthStencilViewDesc, m_D3D12DsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
}

void CDepthWriteShader::CreateShadowMap(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	if (m_Lights[0].m_IsActive)
	{
		XMFLOAT3 Position{ m_Lights[0].m_Position };
		XMFLOAT3 Look{ m_Lights[0].m_Direction };
		float NearPlaneDistance{ 1.0f }, FarPlaneDistance{ m_Lights[0].m_Range };

		switch (m_Lights[0].m_Type)
		{
		case LIGHT_TYPE_SPOT:
			m_LightCamera->GeneratePerspectiveProjectionMatrix(60.0f, (float)DEPTH_BUFFER_WIDTH / (float)DEPTH_BUFFER_HEIGHT, NearPlaneDistance, FarPlaneDistance);
			break;
		case LIGHT_TYPE_DIRECTIONAL:
			m_LightCamera->GenerateOrthographicsProjectionMatrix((float)PLANE_WIDTH, (float)PLANE_HEIGHT, NearPlaneDistance, FarPlaneDistance);
			break;
		}

		m_LightCamera->GenerateViewMatrix(Position, Look);

		XMMATRIX ToTexCoordMatrix{ XMMatrixTranspose(XMLoadFloat4x4(&m_LightCamera->GetViewMatrix()) * XMLoadFloat4x4(&m_LightCamera->GetProjectionMatrix()) * XMLoadFloat4x4(&m_ProjectionMatrixToTexture)) };
		XMStoreFloat4x4(&m_Lights[0].m_ToTexCoordMatrix, ToTexCoordMatrix);

		ID3D12Resource* DepthTexture{ m_DepthTexture->GetResource() };
		DX::ResourceTransition(D3D12GraphicsCommandList, DepthTexture, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_RENDER_TARGET);
		
		CD3DX12_CPU_DESCRIPTOR_HANDLE D3D12RtvCPUDescriptorHandle{ m_D3D12RtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };
		CD3DX12_CPU_DESCRIPTOR_HANDLE D3D12DsvCPUDescriptorHandle{ m_D3D12DsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };

		D3D12GraphicsCommandList->ClearRenderTargetView(D3D12RtvCPUDescriptorHandle, Colors::White, 0, nullptr);
		D3D12GraphicsCommandList->ClearDepthStencilView(D3D12DsvCPUDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
		D3D12GraphicsCommandList->OMSetRenderTargets(1, &D3D12RtvCPUDescriptorHandle, TRUE, &D3D12DsvCPUDescriptorHandle);

		Render(D3D12GraphicsCommandList, m_LightCamera.get());

		DX::ResourceTransition(D3D12GraphicsCommandList, DepthTexture, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COMMON);
	}
}

//=========================================================================================================================

CShadowMapShader::CShadowMapShader(shared_ptr<CPlayer>& Player, vector<shared_ptr<CGameObject>>& Police, vector<shared_ptr<CGameObject>>& Structures) :
	m_Player{ Player },
	m_Police{ Police },
	m_Structures{ Structures }
{

}

D3D12_INPUT_LAYOUT_DESC CShadowMapShader::CreateInputLayout(UINT PSONum)
{
	const UINT InputElementCount{ 5 };
	D3D12_INPUT_ELEMENT_DESC* D3D12InputElementDescs{ new D3D12_INPUT_ELEMENT_DESC[InputElementCount] };

	D3D12InputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12InputElementDescs[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12InputElementDescs[2] = { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 2, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12InputElementDescs[3] = { "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 3, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12InputElementDescs[4] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 4, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC D3D12InputLayoutDesc{};

	D3D12InputLayoutDesc.pInputElementDescs = D3D12InputElementDescs;
	D3D12InputLayoutDesc.NumElements = InputElementCount;

	return D3D12InputLayoutDesc;
}

D3D12_SHADER_BYTECODE CShadowMapShader::CreateVertexShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum)
{
	return CGraphicsShader::CompileShaderFromFile(L"GameSceneShader.hlsl", "VS_Main", "vs_5_1", D3D12ShaderBlob);
}

D3D12_SHADER_BYTECODE CShadowMapShader::CreatePixelShader(ID3DBlob* D3D12ShaderBlob, UINT PSONum)
{
	return CGraphicsShader::CompileShaderFromFile(L"GameSceneShader.hlsl", "PS_Main", "ps_5_1", D3D12ShaderBlob);
}

void CShadowMapShader::CreatePipelineStateObject(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature, UINT PSONum)
{
	CGraphicsShader::CreatePipelineStateObject(D3D12Device, D3D12RootSignature);
}

void CShadowMapShader::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera)
{
	CTextureManager::GetInstance()->GetTexture(TEXT("ShadowMap"))->UpdateShaderVariable(D3D12GraphicsCommandList);
	CGraphicsShader::Render(D3D12GraphicsCommandList, Camera);

	m_Player->UpdateTransform(Matrix4x4::Identity());
	m_Player->Render(D3D12GraphicsCommandList, Camera);

	for (const auto& Police : m_Police)
	{
		Police->UpdateTransform(Matrix4x4::Identity());
		Police->Render(D3D12GraphicsCommandList, Camera);
	}

	for (const auto& Structure : m_Structures)
	{
		Structure->UpdateTransform(Matrix4x4::Identity());
		Structure->Render(D3D12GraphicsCommandList, Camera);
	}
}
