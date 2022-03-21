#include "stdafx.h"
#include "BilboardObject.h"
#include "Material.h"

shared_ptr<CBilboardObject> CBilboardObject::LoadObjectInfoFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, tifstream& InFile)
{
	tstring Token{};

	shared_ptr<CBilboardObject> NewObject{ make_shared<CBilboardObject>() };
	XMUINT2 CellCount{};

#ifdef READ_BINARY_FILE
	while (true)
	{
		File::ReadStringFromFile(InFile, Token);

		if (Token == TEXT("<IsActive>"))
		{
			bool IsActive{ static_cast<bool>(File::ReadIntegerFromFile(InFile)) };

			NewObject->SetActive(IsActive);
		}
		else if (Token == TEXT("<TextureName>"))
		{
			File::ReadStringFromFile(InFile, Token);

			shared_ptr<CMaterial> Material{ make_shared<CMaterial>() };
			shared_ptr<CTexture> Texture{ make_shared<CTexture>() };
			shared_ptr<CShader> Shader{ CShaderManager::GetInstance()->GetShader("UIShader") };

			Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, TEXTURE_TYPE_ALBEDO_MAP, Token);
			Material->RegisterTexture(Texture);
			Material->RegisterShader(Shader);
			CTextureManager::GetInstance()->RegisterTexture(Token, Texture);

			NewObject->SetMaterial(Material);
		}
		else if (Token == TEXT("<CellInfo>"))
		{
			CellCount = { File::ReadIntegerFromFile(InFile), File::ReadIntegerFromFile(InFile) };
		}
		else if (Token == TEXT("<RectTransform>"))
		{
			NewObject->m_VertexCount = File::ReadIntegerFromFile(InFile);

			NewObject->m_D3D12VertexBuffer = DX::CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, nullptr, sizeof(CBilboardMesh) * NewObject->m_VertexCount, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr);
			DX::ThrowIfFailed(NewObject->m_D3D12VertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&NewObject->m_MappedImageInfo)));

			NewObject->m_D3D12VertexBufferView.BufferLocation = NewObject->m_D3D12VertexBuffer->GetGPUVirtualAddress();
			NewObject->m_D3D12VertexBufferView.StrideInBytes = sizeof(CBilboardMesh);
			NewObject->m_D3D12VertexBufferView.SizeInBytes = sizeof(CBilboardMesh) * NewObject->m_VertexCount;

			for (UINT i = 0; i < NewObject->m_VertexCount; ++i)
			{
				XMFLOAT2 Size{};

				InFile.read(reinterpret_cast<TCHAR*>(&NewObject->m_Position), sizeof(XMFLOAT2));
				InFile.read(reinterpret_cast<TCHAR*>(&NewObject->m_IndexTime), sizeof(float));
				InFile.read(reinterpret_cast<TCHAR*>(&Size), sizeof(XMFLOAT2));

				*(NewObject->m_MappedImageInfo + i) = CBilboardMesh{ NewObject->m_Position, Size, CellCount, static_cast<UINT>(NewObject->m_IndexTime) };
			}
		}
		else if (Token == TEXT("</UIObject>"))
		{
			break;
		}
	}
#else
	while (InFile >> Token)
	{
		if (Token == TEXT("<IsActive>"))
		{
			InFile >> Token;

			NewObject->SetActive(stoi(Token));
		}
		else if (Token == TEXT("<TextureName>"))
		{
			InFile >> Token;

			shared_ptr<CMaterial> Material{ make_shared<CMaterial>() };
			shared_ptr<CTexture> Texture{ make_shared<CTexture>() };
			shared_ptr<CShader> Shader{ CShaderManager::GetInstance()->GetShader("UIShader") };

			Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, TEXTURE_TYPE_ALBEDO_MAP, Token);
			Material->RegisterTexture(Texture);
			Material->RegisterShader(Shader);
			CTextureManager::GetInstance()->RegisterTexture(Token, Texture);

			NewObject->SetMaterial(Material);
		}
		else if (Token == TEXT("<RectTransform>"))
		{
			InFile >> NewObject->m_VertexCount;

			NewObject->m_D3D12VertexBuffer = DX::CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, nullptr, sizeof(CBilboardMesh) * NewObject->m_VertexCount, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr);
			DX::ThrowIfFailed(NewObject->m_D3D12VertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&NewObject->m_MappedImageInfo)));

			NewObject->m_D3D12VertexBufferView.BufferLocation = NewObject->m_D3D12VertexBuffer->GetGPUVirtualAddress();
			NewObject->m_D3D12VertexBufferView.StrideInBytes = sizeof(CBilboardMesh);
			NewObject->m_D3D12VertexBufferView.SizeInBytes = sizeof(CBilboardMesh) * NewObject->m_VertexCount;

			for (UINT i = 0; i < NewObject->m_VertexCount; ++i)
			{
				XMFLOAT3 Position{};
				XMFLOAT2 Size{};

				InFile >> Position.x >> Position.y;
				InFile >> Size.x >> Size.y;

				*(NewObject->m_MappedImageInfo + i) = CBilboardMesh{ Position, Size };
			}
		}
		else if (Token == TEXT("</UIObject>"))
		{
			break;
		}
	}
#endif

	return NewObject;
}

void CBilboardObject::UpdateShaderVariables(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{

}

void CBilboardObject::Animate(float ElapsedTime)
{
	//m_IndexTime += ElapsedTime;

	//if (m_IndexTime > 5.0f)
	//{
	//	m_IndexTime = 0.0f;
	//}

	//(m_MappedImageInfo)->SetCellIndex(static_cast<UINT>(m_IndexTime));
	//(m_MappedImageInfo + 1)->SetCellIndex(static_cast<UINT>(m_IndexTime));
}

void CBilboardObject::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera, RENDER_TYPE RenderType)
{
	if (IsActive())
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

			D3D12GraphicsCommandList->DrawInstanced(m_VertexCount, 1, 0, 0);
		}
	}
}

void CBilboardObject::ReleaseUploadBuffers()
{
	if (m_D3D12VertexUploadBuffer)
	{
		m_D3D12VertexUploadBuffer.ReleaseAndGetAddressOf();
	}
}

bool CBilboardObject::IsActive() const
{
	return m_IsActive;
}

void CBilboardObject::SetActive(bool IsActive)
{
	m_IsActive = IsActive;
}

void CBilboardObject::SetPosition(const XMFLOAT3& Position)
{
	m_Position = Position;
}

const XMFLOAT3& CBilboardObject::GetPosition() const
{
	return m_Position;
}

void CBilboardObject::SetIndexTime(float IndexTime)
{
	m_IndexTime = IndexTime;
}

float CBilboardObject::GetIndexTime() const
{
	return m_IndexTime;
}

void CBilboardObject::SetMaterial(const shared_ptr<CMaterial>& Material)
{
	if (Material)
	{
		m_Materials.push_back(Material);
	}
}
