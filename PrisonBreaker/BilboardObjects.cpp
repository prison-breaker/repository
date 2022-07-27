#include "stdafx.h"
#include "BilboardObjects.h"
#include "Material.h"
#include "Texture.h"

CSkyBox::CSkyBox(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	const UINT Face{ 6 };
	vector<QUAD_INFO> Vertices{};

	Vertices.reserve(Face);

	QUAD_INFO QuadInfo{};

	QuadInfo.m_Position = { 0.0f, 0.0f, -10.0f };
	QuadInfo.m_Size = { 20.0f, 20.0f };
	Vertices.push_back(QuadInfo);

	QuadInfo.m_Position = { 0.0f, 0.0f, +10.0f };
	Vertices.push_back(QuadInfo);

	QuadInfo.m_Position = { +10.0f, 0.0f, 0.0f };
	Vertices.push_back(QuadInfo);

	QuadInfo.m_Position = { -10.0f, 0.0f, 0.0f };
	Vertices.push_back(QuadInfo);

	QuadInfo.m_Position = { 0.0f, +10.0f, 0.0f };
	Vertices.push_back(QuadInfo);

	QuadInfo.m_Position = { 0.0f, -10.0f, 0.0f };
	Vertices.push_back(QuadInfo);

	m_D3D12VertexBuffer = DX::CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, Vertices.data(), sizeof(QUAD_INFO) * Face, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_D3D12VertexUploadBuffer.GetAddressOf());
	m_D3D12VertexBufferView.BufferLocation = m_D3D12VertexBuffer->GetGPUVirtualAddress();
	m_D3D12VertexBufferView.StrideInBytes = sizeof(QUAD_INFO);
	m_D3D12VertexBufferView.SizeInBytes = sizeof(QUAD_INFO) * Face;

	shared_ptr<CMaterial> Material{ make_shared<CMaterial>() };
	shared_ptr<CTexture> Texture{ make_shared<CTexture>() };
	shared_ptr<CShader> Shader{ CShaderManager::GetInstance()->GetShader(TEXT("QuadShader")) };

	Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, TEXTURE_TYPE_ALBEDO_MAP, TEXT("Textures/SkyBox_Back.dds"));
	Material->RegisterTexture(Texture);
	Material->RegisterShader(Shader);
	Material->SetStateNum(1);
	CTextureManager::GetInstance()->RegisterTexture(TEXT("SkyBox_Back"), Texture);
	m_Materials.push_back(Material);

	Material = make_shared<CMaterial>();
	Texture = make_shared<CTexture>();
	Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, TEXTURE_TYPE_ALBEDO_MAP, TEXT("Textures/SkyBox_Front.dds"));
	Material->RegisterTexture(Texture);
	Material->RegisterShader(Shader);
	Material->SetStateNum(1);
	CTextureManager::GetInstance()->RegisterTexture(TEXT("SkyBox_Front"), Texture);
	m_Materials.push_back(Material);

	Material = make_shared<CMaterial>();
	Texture = make_shared<CTexture>();
	Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, TEXTURE_TYPE_ALBEDO_MAP, TEXT("Textures/SkyBox_Right.dds"));
	Material->RegisterTexture(Texture);
	Material->RegisterShader(Shader);
	Material->SetStateNum(1);
	CTextureManager::GetInstance()->RegisterTexture(TEXT("SkyBox_Right"), Texture);
	m_Materials.push_back(Material);

	Material = make_shared<CMaterial>();
	Texture = make_shared<CTexture>();
	Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, TEXTURE_TYPE_ALBEDO_MAP, TEXT("Textures/SkyBox_Left.dds"));
	Material->RegisterTexture(Texture);
	Material->RegisterShader(Shader);
	Material->SetStateNum(1);
	CTextureManager::GetInstance()->RegisterTexture(TEXT("SkyBox_Left"), Texture);
	m_Materials.push_back(Material);

	Material = make_shared<CMaterial>();
	Texture = make_shared<CTexture>();
	Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, TEXTURE_TYPE_ALBEDO_MAP, TEXT("Textures/SkyBox_Top.dds"));
	Material->RegisterTexture(Texture);
	Material->RegisterShader(Shader);
	Material->SetStateNum(1);
	CTextureManager::GetInstance()->RegisterTexture(TEXT("SkyBox_Top"), Texture);
	m_Materials.push_back(Material);

	Material = make_shared<CMaterial>();
	Texture = make_shared<CTexture>();
	Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, TEXTURE_TYPE_ALBEDO_MAP, TEXT("Textures/SkyBox_Bottom.dds"));
	Material->RegisterTexture(Texture);
	Material->RegisterShader(Shader);
	Material->SetStateNum(1);
	CTextureManager::GetInstance()->RegisterTexture(TEXT("SkyBox_Bottom"), Texture);
	m_Materials.push_back(Material);
}

void CSkyBox::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera, RENDER_TYPE RenderType)
{
	D3D12_VERTEX_BUFFER_VIEW VertexBufferViews[] = { m_D3D12VertexBufferView };

	D3D12GraphicsCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	D3D12GraphicsCommandList->IASetVertexBuffers(0, 1, VertexBufferViews);

	UINT MaterialCount{ static_cast<UINT>(m_Materials.size()) };

	for (UINT i = 0; i < MaterialCount; ++i)
	{
		if (m_Materials[i])
		{
			m_Materials[i]->SetPipelineState(D3D12GraphicsCommandList, RenderType);
			m_Materials[i]->UpdateShaderVariables(D3D12GraphicsCommandList);
		}

		D3D12GraphicsCommandList->DrawInstanced(1, 1, i, 0);
	}
}

//=========================================================================================================================

CTree::CTree(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	m_IsActive = true;
	m_VertexCount = m_MaxVertexCount = 40;

	vector<QUAD_INFO> Vertices{};

	Vertices.reserve(m_MaxVertexCount);

	QUAD_INFO QuadInfo{};

	for (UINT i = 0; i < m_MaxVertexCount / 2; ++i)
	{
		float RandomValue{ Random::Range(5.0f, 7.0f) };

		QuadInfo.m_Position = { -15.0f, 1.05f + 0.5f * RandomValue, -25.0f + 2.0f * i };
		QuadInfo.m_Size = { RandomValue, RandomValue };
		Vertices.push_back(QuadInfo);

		RandomValue = Random::Range(5.0f, 8.0f);

		QuadInfo.m_Position = { 15.0f, 1.05f + 0.5f * RandomValue, -25.0f + 2.0f * i };
		QuadInfo.m_Size = { RandomValue, RandomValue };
		Vertices.push_back(QuadInfo);
	}

	m_D3D12VertexBuffer = DX::CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, Vertices.data(), sizeof(QUAD_INFO) * m_MaxVertexCount, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_D3D12VertexUploadBuffer.GetAddressOf());
	m_D3D12VertexBufferView.BufferLocation = m_D3D12VertexBuffer->GetGPUVirtualAddress();
	m_D3D12VertexBufferView.StrideInBytes = sizeof(QUAD_INFO);
	m_D3D12VertexBufferView.SizeInBytes = sizeof(QUAD_INFO) * m_MaxVertexCount;

	shared_ptr<CMaterial> Material{ make_shared<CMaterial>() };
	shared_ptr<CTexture> Texture{ make_shared<CTexture>() };
	shared_ptr<CShader> Shader{ CShaderManager::GetInstance()->GetShader(TEXT("QuadShader")) };

	Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, TEXTURE_TYPE_ALBEDO_MAP, TEXT("Textures/Tree.dds"));
	Material->RegisterTexture(Texture);
	Material->RegisterShader(Shader);
	Material->SetStateNum(0);
	CTextureManager::GetInstance()->RegisterTexture(TEXT("Tree"), Texture);
	m_Materials.push_back(Material);
}
