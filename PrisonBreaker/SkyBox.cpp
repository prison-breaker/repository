#include "stdafx.h"
#include "SkyBox.h"
#include "Material.h"
#include "SkyBoxShader.h"

CSkyBox::CSkyBox(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	const UINT Face{ 6 };
	XMFLOAT3 Position{};
	XMFLOAT2 Size{ 20.0f, 20.0f };
	vector<CBilboardMesh> Vertices{};

	Vertices.reserve(Face);

	Position = { 0.0f, 0.0f, -10.0f };
	Vertices.emplace_back(Position, Size);

	Position = { 0.0f, 0.0f, +10.0f };
	Vertices.emplace_back(Position, Size);

	Position = { +10.0f, 0.0f, 0.0f };
	Vertices.emplace_back(Position, Size);

	Position = { -10.0f, 0.0f, 0.0f };
	Vertices.emplace_back(Position, Size);

	Position = { 0.0f, +10.0f, 0.0f };
	Vertices.emplace_back(Position, Size);

	Position = { 0.0f, -10.0f, 0.0f };
	Vertices.emplace_back(Position, Size);

	m_D3D12VertexBuffer = DX::CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, Vertices.data(), sizeof(CBilboardMesh) * Face, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_D3D12VertexUploadBuffer.GetAddressOf());
	m_D3D12VertexBufferView.BufferLocation = m_D3D12VertexBuffer->GetGPUVirtualAddress();
	m_D3D12VertexBufferView.StrideInBytes = sizeof(CBilboardMesh);
	m_D3D12VertexBufferView.SizeInBytes = sizeof(CBilboardMesh) * Face;

	shared_ptr<CMaterial> Material{ make_shared<CMaterial>() };
	shared_ptr<CTexture> Texture{ make_shared<CTexture>() };
	shared_ptr<CShader> Shader{ CShaderManager::GetInstance()->GetShader(TEXT("SkyBoxShader")) };

	Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, TEXTURE_TYPE_ALBEDO_MAP, TEXT("Textures/SkyBox_Back.dds"));
	Material->RegisterTexture(Texture);
	Material->RegisterShader(Shader);
	Material->SetStateNum(SHADER_TYPE_STANDARD);
	CTextureManager::GetInstance()->RegisterTexture(TEXT("SkyBox_Back"), Texture);
	m_Materials.push_back(Material);

	Material = make_shared<CMaterial>();
	Texture = make_shared<CTexture>();
	Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, TEXTURE_TYPE_ALBEDO_MAP, TEXT("Textures/SkyBox_Front.dds"));
	Material->RegisterTexture(Texture);
	Material->RegisterShader(Shader);
	Material->SetStateNum(SHADER_TYPE_STANDARD);
	CTextureManager::GetInstance()->RegisterTexture(TEXT("SkyBox_Front"), Texture);
	m_Materials.push_back(Material);

	Material = make_shared<CMaterial>();
	Texture = make_shared<CTexture>();
	Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, TEXTURE_TYPE_ALBEDO_MAP, TEXT("Textures/SkyBox_Right.dds"));
	Material->RegisterTexture(Texture);
	Material->RegisterShader(Shader);
	Material->SetStateNum(SHADER_TYPE_STANDARD);
	CTextureManager::GetInstance()->RegisterTexture(TEXT("SkyBox_Right"), Texture);
	m_Materials.push_back(Material);

	Material = make_shared<CMaterial>();
	Texture = make_shared<CTexture>();
	Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, TEXTURE_TYPE_ALBEDO_MAP, TEXT("Textures/SkyBox_Left.dds"));
	Material->RegisterTexture(Texture);
	Material->RegisterShader(Shader);
	Material->SetStateNum(SHADER_TYPE_STANDARD);
	CTextureManager::GetInstance()->RegisterTexture(TEXT("SkyBox_Left"), Texture);
	m_Materials.push_back(Material);

	Material = make_shared<CMaterial>();
	Texture = make_shared<CTexture>();
	Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, TEXTURE_TYPE_ALBEDO_MAP, TEXT("Textures/SkyBox_Top.dds"));
	Material->RegisterTexture(Texture);
	Material->RegisterShader(Shader);
	Material->SetStateNum(SHADER_TYPE_STANDARD);
	CTextureManager::GetInstance()->RegisterTexture(TEXT("SkyBox_Top"), Texture);
	m_Materials.push_back(Material);

	Material = make_shared<CMaterial>();
	Texture = make_shared<CTexture>();
	Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, TEXTURE_TYPE_ALBEDO_MAP, TEXT("Textures/SkyBox_Bottom.dds"));
	Material->RegisterTexture(Texture);
	Material->RegisterShader(Shader);
	Material->SetStateNum(SHADER_TYPE_STANDARD);
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
