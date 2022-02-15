#include "stdafx.h"
#include "SkyBox.h"

CSkyBox::CSkyBox(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	const UINT Face{ 6 };
	vector<CBilboardMesh> Vertices{};
	XMFLOAT3 Position{};
	XMFLOAT2 Size{ 20.0f, 20.0f };

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

	UINT Stride{ sizeof(CBilboardMesh) };

	m_D3D12PositionBuffer = DX::CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, Vertices.data(), Stride * (UINT)Vertices.size(),
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_D3D12PositionUploadBuffer.GetAddressOf());

	m_D3D12PositionBufferView.BufferLocation = m_D3D12PositionBuffer->GetGPUVirtualAddress();
	m_D3D12PositionBufferView.StrideInBytes = Stride;
	m_D3D12PositionBufferView.SizeInBytes = Stride * (UINT)Vertices.size();

	shared_ptr<CMaterial> Material{ make_shared<CMaterial>() };
	shared_ptr<CTexture> Texture{ make_shared<CTexture>() };
	Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, TEXTURE_TYPE_ALBEDO_MAP, TEXT("Texture/SkyBox_Back.dds"));
	Material->RegisterTexture(Texture);
	CTextureManager::GetInstance()->RegisterTexture(TEXT("SkyBox_Back"), Texture);
	m_Materials.push_back(Material);

	Material = make_shared<CMaterial>();
	Texture = make_shared<CTexture>();
	Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, TEXTURE_TYPE_ALBEDO_MAP, TEXT("Texture/SkyBox_Front.dds"));
	Material->RegisterTexture(Texture);
	CTextureManager::GetInstance()->RegisterTexture(TEXT("SkyBox_Front"), Texture);
	m_Materials.push_back(Material);

	Material = make_shared<CMaterial>();
	Texture = make_shared<CTexture>();
	Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, TEXTURE_TYPE_ALBEDO_MAP, TEXT("Texture/SkyBox_Right.dds"));
	Material->RegisterTexture(Texture);
	CTextureManager::GetInstance()->RegisterTexture(TEXT("SkyBox_Right"), Texture);
	m_Materials.push_back(Material);

	Material = make_shared<CMaterial>();
	Texture = make_shared<CTexture>();
	Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, TEXTURE_TYPE_ALBEDO_MAP, TEXT("Texture/SkyBox_Left.dds"));
	Material->RegisterTexture(Texture);
	CTextureManager::GetInstance()->RegisterTexture(TEXT("SkyBox_Left"), Texture);
	m_Materials.push_back(Material);

	Material = make_shared<CMaterial>();
	Texture = make_shared<CTexture>();
	Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, TEXTURE_TYPE_ALBEDO_MAP, TEXT("Texture/SkyBox_Top.dds"));
	Material->RegisterTexture(Texture);
	CTextureManager::GetInstance()->RegisterTexture(TEXT("SkyBox_Top"), Texture);
	m_Materials.push_back(Material);

	Material = make_shared<CMaterial>();
	Texture = make_shared<CTexture>();
	Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, TEXTURE_TYPE_ALBEDO_MAP, TEXT("Texture/SkyBox_Bottom.dds"));
	Material->RegisterTexture(Texture);
	CTextureManager::GetInstance()->RegisterTexture(TEXT("SkyBox_Bottom"), Texture);
	m_Materials.push_back(Material);
}

void CSkyBox::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera)
{
	D3D12_VERTEX_BUFFER_VIEW VertexBufferViews[] = { m_D3D12PositionBufferView };

	D3D12GraphicsCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	D3D12GraphicsCommandList->IASetVertexBuffers(0, 1, VertexBufferViews);

	UINT MaterialCount{ (UINT)m_Materials.size() };
	for (UINT i = 0; i < MaterialCount; ++i)
	{
		if (m_Materials[i])
		{
			m_Materials[i]->UpdateShaderVariables(D3D12GraphicsCommandList);
		}

		D3D12GraphicsCommandList->DrawInstanced(1, 1, i, 0);
	}
}
