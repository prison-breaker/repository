#include "stdafx.h"
#include "ShadowMapShader.h"
#include "GameScene.h"

CDepthWriteShader::CDepthWriteShader(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList) :
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

D3D12_INPUT_LAYOUT_DESC CDepthWriteShader::CreateInputLayout(UINT StateNum)
{
	const UINT InputElementCount{ 1 };
	D3D12_INPUT_ELEMENT_DESC* D3D12InputElementDescs{ new D3D12_INPUT_ELEMENT_DESC[InputElementCount] };

	D3D12InputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC D3D12InputLayoutDesc{};

	D3D12InputLayoutDesc.pInputElementDescs = D3D12InputElementDescs;
	D3D12InputLayoutDesc.NumElements = InputElementCount;

	return D3D12InputLayoutDesc;
}

D3D12_RASTERIZER_DESC CDepthWriteShader::CreateRasterizerState(UINT StateNum)
{
	D3D12_RASTERIZER_DESC D3D12RasterizerDesc{};

	D3D12RasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	D3D12RasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	D3D12RasterizerDesc.FrontCounterClockwise = false;
	D3D12RasterizerDesc.DepthBias = 10000;
	D3D12RasterizerDesc.DepthBiasClamp = 0.0f;
	D3D12RasterizerDesc.SlopeScaledDepthBias = 1.0f;
	D3D12RasterizerDesc.DepthClipEnable = true;
	D3D12RasterizerDesc.MultisampleEnable = false;
	D3D12RasterizerDesc.AntialiasedLineEnable = false;
	D3D12RasterizerDesc.ForcedSampleCount = 0;
	D3D12RasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return D3D12RasterizerDesc;
}

D3D12_SHADER_BYTECODE CDepthWriteShader::CreateVertexShader(ID3DBlob* D3D12ShaderBlob, UINT StateNum)
{
	return CGraphicsShader::CompileShaderFromFile(L"GameSceneShader.hlsl", "VS_Position", "vs_5_1", D3D12ShaderBlob);
}

D3D12_SHADER_BYTECODE CDepthWriteShader::CreatePixelShader(ID3DBlob* D3D12ShaderBlob, UINT StateNum)
{
	return CGraphicsShader::CompileShaderFromFile(L"GameSceneShader.hlsl", "PS_DepthWrite", "ps_5_1", D3D12ShaderBlob);
}

DXGI_FORMAT CDepthWriteShader::GetRTVFormat(UINT StateNum, UINT RenderTargetNum)
{
	return DXGI_FORMAT_R32_FLOAT;
}

DXGI_FORMAT CDepthWriteShader::GetDSVFormat(UINT StateNum)
{
	return DXGI_FORMAT_D32_FLOAT;
}

void CDepthWriteShader::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera, const vector<vector<shared_ptr<CGameObject>>>& GameObjects, UINT StateNum)
{
	if (Camera)
	{
		Camera->RSSetViewportsAndScissorRects(D3D12GraphicsCommandList);
		Camera->UpdateShaderVariables(D3D12GraphicsCommandList);
	}

	if (CShaderManager::GetInstance()->SetGlobalShader(TEXT("DepthWriteShader")))
	{
		if (m_D3D12PipelineStates[StateNum])
		{
			D3D12GraphicsCommandList->SetPipelineState(m_D3D12PipelineStates[StateNum].Get());
		}
	}

	for (UINT i = OBJECT_TYPE_PLAYER; i <= OBJECT_TYPE_STRUCTURE; ++i)
	{
		for (const auto& GameObject : GameObjects[i])
		{
			if (GameObject)
			{
				GameObject->UpdateTransform(Matrix4x4::Identity());
				GameObject->Render(D3D12GraphicsCommandList, Camera);
			}
		}
	}

	CShaderManager::GetInstance()->UnSetGlobalShader();
}

void CDepthWriteShader::CreateRtvAndDsvDescriptorHeaps(ID3D12Device* D3D12Device)
{
	D3D12_DESCRIPTOR_HEAP_DESC D3D12DescriptorHeapDesc{};

	D3D12DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	D3D12DescriptorHeapDesc.NumDescriptors = 1;
	D3D12DescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	D3D12DescriptorHeapDesc.NodeMask = 0;
	DX::ThrowIfFailed(D3D12Device->CreateDescriptorHeap(&D3D12DescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), reinterpret_cast<void**>(m_D3D12RtvDescriptorHeap.GetAddressOf())));

	D3D12DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	DX::ThrowIfFailed(D3D12Device->CreateDescriptorHeap(&D3D12DescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), reinterpret_cast<void**>(m_D3D12DsvDescriptorHeap.GetAddressOf())));
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

void CDepthWriteShader::PrepareShadowMap(ID3D12GraphicsCommandList* D3D12GraphicsCommandList,vector<LIGHT>& Lights, const vector<vector<shared_ptr<CGameObject>>>& GameObjects)
{
	if (Lights[0].m_IsActive)
	{
		const float NearPlaneDistance{ 1.0f };
		const float FarPlaneDistance{ Lights[0].m_Range };

		switch (Lights[0].m_Type)
		{
		case LIGHT_TYPE_POINT:
			break;
		case LIGHT_TYPE_SPOT:
			m_LightCamera->GeneratePerspectiveProjectionMatrix(90.0f, static_cast<float>(DEPTH_BUFFER_WIDTH) / static_cast<float>(DEPTH_BUFFER_HEIGHT), NearPlaneDistance, FarPlaneDistance);
			break;
		case LIGHT_TYPE_DIRECTIONAL:
			m_LightCamera->GenerateOrthographicsProjectionMatrix(static_cast<float>(PLANE_WIDTH), static_cast<float>(PLANE_HEIGHT), NearPlaneDistance, FarPlaneDistance);
			break;
		}

		m_LightCamera->GenerateViewMatrix(Lights[0].m_Position, Lights[0].m_Direction);

		XMMATRIX ToTexCoordMatrix{ XMMatrixTranspose(XMLoadFloat4x4(&m_LightCamera->GetViewMatrix()) * XMLoadFloat4x4(&m_LightCamera->GetProjectionMatrix()) * XMLoadFloat4x4(&m_ProjectionMatrixToTexture)) };
		
		XMStoreFloat4x4(&Lights[0].m_ToTexCoordMatrix, ToTexCoordMatrix);

		ID3D12Resource* DepthTexture{ m_DepthTexture->GetResource() };
	
		DX::ResourceTransition(D3D12GraphicsCommandList, DepthTexture, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_RENDER_TARGET);
		
		CD3DX12_CPU_DESCRIPTOR_HANDLE D3D12RtvCPUDescriptorHandle{ m_D3D12RtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };
		CD3DX12_CPU_DESCRIPTOR_HANDLE D3D12DsvCPUDescriptorHandle{ m_D3D12DsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };

		D3D12GraphicsCommandList->ClearRenderTargetView(D3D12RtvCPUDescriptorHandle, Colors::White, 0, nullptr);
		D3D12GraphicsCommandList->ClearDepthStencilView(D3D12DsvCPUDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
		D3D12GraphicsCommandList->OMSetRenderTargets(1, &D3D12RtvCPUDescriptorHandle, TRUE, &D3D12DsvCPUDescriptorHandle);

		Render(D3D12GraphicsCommandList, m_LightCamera.get(), GameObjects);

		DX::ResourceTransition(D3D12GraphicsCommandList, DepthTexture, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COMMON);
	}
}

//=========================================================================================================================

D3D12_INPUT_LAYOUT_DESC CShadowMapShader::CreateInputLayout(UINT StateNum)
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

D3D12_SHADER_BYTECODE CShadowMapShader::CreateVertexShader(ID3DBlob* D3D12ShaderBlob, UINT StateNum)
{
	return CGraphicsShader::CompileShaderFromFile(L"GameSceneShader.hlsl", "VS_Main", "vs_5_1", D3D12ShaderBlob);
}

D3D12_SHADER_BYTECODE CShadowMapShader::CreatePixelShader(ID3DBlob* D3D12ShaderBlob, UINT StateNum)
{
	return CGraphicsShader::CompileShaderFromFile(L"GameSceneShader.hlsl", "PS_Main", "ps_5_1", D3D12ShaderBlob);
}

void CShadowMapShader::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera, UINT StateNum)
{
	if (CShaderManager::GetInstance()->SetShader(TEXT("ShadowMapShader")))
	{
		if (m_D3D12PipelineStates[StateNum])
		{
			D3D12GraphicsCommandList->SetPipelineState(m_D3D12PipelineStates[StateNum].Get());
		}
	}
}
