#include "stdafx.h"
#include "Mesh.h"
#include "GameObject.h"

void CMesh::CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{

}

void CMesh::UpdateShaderVariables(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{

}

void CMesh::ReleaseShaderVariables()
{

}

void CMesh::ReleaseUploadBuffers()
{
	if (m_D3D12PositionUploadBuffer)
	{
		m_D3D12PositionUploadBuffer.ReleaseAndGetAddressOf();
	}

	if (m_D3D12NormalUploadBuffer)
	{
		m_D3D12NormalUploadBuffer.ReleaseAndGetAddressOf();
	}

	if (m_D3D12TangentUploadBuffer)
	{
		m_D3D12TangentUploadBuffer.ReleaseAndGetAddressOf();
	}

	if (m_D3D12BiTangentUploadBuffer)
	{
		m_D3D12BiTangentUploadBuffer.ReleaseAndGetAddressOf();
	}

	if (m_D3D12TexCoordUploadBuffer)
	{
		m_D3D12TexCoordUploadBuffer.ReleaseAndGetAddressOf();
	}

	if (!m_D3D12IndexUploadBuffers.empty())
	{
		for (auto& UploadBuffer : m_D3D12IndexUploadBuffers)
		{
			if (UploadBuffer)
			{
				UploadBuffer.ReleaseAndGetAddressOf();
			}
		}

		m_D3D12IndexUploadBuffers.shrink_to_fit();
	}
}

void CMesh::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, UINT SubSetIndex)
{
	D3D12_VERTEX_BUFFER_VIEW VertexBufferViews[] = { m_D3D12PositionBufferView, m_D3D12NormalBufferView, m_D3D12TangentBufferView, m_D3D12BiTangentBufferView, m_D3D12TexCoordBufferView };
	
	D3D12GraphicsCommandList->IASetVertexBuffers(0, _countof(VertexBufferViews), VertexBufferViews);
	D3D12GraphicsCommandList->IASetPrimitiveTopology(m_D3D12PrimitiveTopology);

	UINT BufferSize{ static_cast<UINT>(m_D3D12IndexBuffers.size()) };

	if (BufferSize > 0 && SubSetIndex < BufferSize)
	{
		D3D12GraphicsCommandList->IASetIndexBuffer(&m_D3D12IndexBufferViews[SubSetIndex]);
		D3D12GraphicsCommandList->DrawIndexedInstanced(static_cast<UINT>(m_Indices[SubSetIndex].size()), 1, 0, 0, 0);
	}
	else
	{
		D3D12GraphicsCommandList->DrawInstanced(static_cast<UINT>(m_Positions.size()), 1, 0, 0);
	}
}

void CMesh::LoadMeshInfoFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, tifstream& InFile)
{
	tstring Token{};

#ifdef READ_BINARY_FILE
	File::ReadStringFromFile(InFile, m_Name);

	while (true)
	{
		File::ReadStringFromFile(InFile, Token);

		if (Token == TEXT("<Positions>"))
		{
			UINT VertexCount{ File::ReadIntegerFromFile(InFile) };

			if (VertexCount > 0)
			{
				m_Positions.resize(VertexCount);

				InFile.read(reinterpret_cast<TCHAR*>(m_Positions.data()), sizeof(XMFLOAT3) * VertexCount);

				m_D3D12PositionBuffer = DX::CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, m_Positions.data(), sizeof(XMFLOAT3) * VertexCount, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_D3D12PositionUploadBuffer.GetAddressOf());
				m_D3D12PositionBufferView.BufferLocation = m_D3D12PositionBuffer->GetGPUVirtualAddress();
				m_D3D12PositionBufferView.StrideInBytes = sizeof(XMFLOAT3);
				m_D3D12PositionBufferView.SizeInBytes = sizeof(XMFLOAT3) * VertexCount;

				// 메쉬 캐시는 지역변수이므로 지역을 벗어나면 소멸하기 리소스가 소멸하기 때문에 카운트를 미리 1 증가시켜 놓는다.
				m_D3D12PositionBuffer->AddRef();
				m_D3D12PositionUploadBuffer->AddRef();

				//tcout << TEXT(" 정점 수 : ") << VertexCount << endl;
			}
		}
		else if (Token == TEXT("<Normals>"))
		{
			UINT VertexCount{ File::ReadIntegerFromFile(InFile) };

			if (VertexCount > 0)
			{
				vector<XMFLOAT3> Normals{ VertexCount };

				InFile.read(reinterpret_cast<TCHAR*>(Normals.data()), sizeof(XMFLOAT3) * VertexCount);

				m_D3D12NormalBuffer = DX::CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, Normals.data(), sizeof(XMFLOAT3) * VertexCount, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_D3D12NormalUploadBuffer.GetAddressOf());
				m_D3D12NormalBufferView.BufferLocation = m_D3D12NormalBuffer->GetGPUVirtualAddress();
				m_D3D12NormalBufferView.StrideInBytes = sizeof(XMFLOAT3);
				m_D3D12NormalBufferView.SizeInBytes = sizeof(XMFLOAT3) * VertexCount;

				// 메쉬 캐시는 지역변수이므로 지역을 벗어나면 소멸하기 리소스가 소멸하기 때문에 카운트를 미리 1 증가시켜 놓는다.
				m_D3D12NormalBuffer->AddRef();
				m_D3D12NormalUploadBuffer->AddRef();
			}
		}
		else if (Token == TEXT("<Tangents>"))
		{
			UINT VertexCount{ File::ReadIntegerFromFile(InFile) };

			if (VertexCount > 0)
			{
				vector<XMFLOAT3> Tangents{ VertexCount };

				InFile.read(reinterpret_cast<TCHAR*>(Tangents.data()), sizeof(XMFLOAT3) * VertexCount);

				m_D3D12TangentBuffer = DX::CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, Tangents.data(), sizeof(XMFLOAT3) * VertexCount, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_D3D12TangentUploadBuffer.GetAddressOf());
				m_D3D12TangentBufferView.BufferLocation = m_D3D12TangentBuffer->GetGPUVirtualAddress();
				m_D3D12TangentBufferView.StrideInBytes = sizeof(XMFLOAT3);
				m_D3D12TangentBufferView.SizeInBytes = sizeof(XMFLOAT3) * VertexCount;

				// 메쉬 캐시는 지역변수이므로 지역을 벗어나면 소멸하기 리소스가 소멸하기 때문에 카운트를 미리 1 증가시켜 놓는다.
				m_D3D12TangentBuffer->AddRef();
				m_D3D12TangentUploadBuffer->AddRef();
			}
		}
		else if (Token == TEXT("<BiTangents>"))
		{
			UINT VertexCount{ File::ReadIntegerFromFile(InFile) };

			if (VertexCount > 0)
			{
				vector<XMFLOAT3> BiTangents{ VertexCount };

				InFile.read(reinterpret_cast<TCHAR*>(BiTangents.data()), sizeof(XMFLOAT3) * VertexCount);

				m_D3D12BiTangentBuffer = DX::CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, BiTangents.data(), sizeof(XMFLOAT3) * VertexCount, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_D3D12BiTangentUploadBuffer.GetAddressOf());
				m_D3D12BiTangentBufferView.BufferLocation = m_D3D12BiTangentBuffer->GetGPUVirtualAddress();
				m_D3D12BiTangentBufferView.StrideInBytes = sizeof(XMFLOAT3);
				m_D3D12BiTangentBufferView.SizeInBytes = sizeof(XMFLOAT3) * VertexCount;

				// 메쉬 캐시는 지역변수이므로 지역을 벗어나면 소멸하기 리소스가 소멸하기 때문에 카운트를 미리 1 증가시켜 놓는다.
				m_D3D12BiTangentBuffer->AddRef();
				m_D3D12BiTangentUploadBuffer->AddRef();
			}
		}
		else if (Token == TEXT("<TexCoords>"))
		{
			UINT VertexCount{ File::ReadIntegerFromFile(InFile) };

			if (VertexCount > 0)
			{
				vector<XMFLOAT2> TexCoords{ VertexCount };

				InFile.read(reinterpret_cast<TCHAR*>(TexCoords.data()), sizeof(XMFLOAT2) * VertexCount);

				m_D3D12TexCoordBuffer = DX::CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, TexCoords.data(), sizeof(XMFLOAT2) * VertexCount, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_D3D12TexCoordUploadBuffer.GetAddressOf());
				m_D3D12TexCoordBufferView.BufferLocation = m_D3D12TexCoordBuffer->GetGPUVirtualAddress();
				m_D3D12TexCoordBufferView.StrideInBytes = sizeof(XMFLOAT2);
				m_D3D12TexCoordBufferView.SizeInBytes = sizeof(XMFLOAT2) * VertexCount;

				// 메쉬 캐시는 지역변수이므로 지역을 벗어나면 소멸하기 리소스가 소멸하기 때문에 카운트를 미리 1 증가시켜 놓는다.
				m_D3D12TexCoordBuffer->AddRef();
				m_D3D12TexCoordUploadBuffer->AddRef();
			}
		}
		else if (Token == TEXT("<SubMeshes>"))
		{
			UINT SubMeshCount{ File::ReadIntegerFromFile(InFile) };

			if (SubMeshCount > 0)
			{
				// +1: BoundingBox
				m_Indices.resize(SubMeshCount + 1);
				m_D3D12IndexBuffers.resize(SubMeshCount + 1);
				m_D3D12IndexUploadBuffers.resize(SubMeshCount + 1);
				m_D3D12IndexBufferViews.resize(SubMeshCount + 1);

				for (UINT i = 0; i < SubMeshCount; ++i)
				{
					// <Indices>
					File::ReadStringFromFile(InFile, Token);

					UINT IndexCount{ File::ReadIntegerFromFile(InFile) };

					if (IndexCount > 0)
					{
						m_Indices[i].resize(IndexCount);

						InFile.read(reinterpret_cast<TCHAR*>(m_Indices[i].data()), sizeof(UINT) * m_Indices[i].size());

						m_D3D12IndexBuffers[i] = DX::CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, m_Indices[i].data(), sizeof(UINT) * IndexCount, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, m_D3D12IndexUploadBuffers[i].GetAddressOf());
						m_D3D12IndexBufferViews[i].BufferLocation = m_D3D12IndexBuffers[i]->GetGPUVirtualAddress();
						m_D3D12IndexBufferViews[i].Format = DXGI_FORMAT_R32_UINT;
						m_D3D12IndexBufferViews[i].SizeInBytes = sizeof(UINT) * IndexCount;

						// 메쉬 캐시는 지역변수이므로 지역을 벗어나면 소멸하기 리소스가 소멸하기 때문에 카운트를 미리 1 증가시켜 놓는다.
						m_D3D12IndexBuffers[i]->AddRef();
						m_D3D12IndexUploadBuffers[i]->AddRef();
					}
				}
			}
		}
		else if (Token == TEXT("<Bounds>"))
		{
			InFile.read(reinterpret_cast<TCHAR*>(&m_BoundingBox.Center), sizeof(XMFLOAT3));
			InFile.read(reinterpret_cast<TCHAR*>(&m_BoundingBox.Extents), sizeof(XMFLOAT3));

			const UINT VertexCount{ 8 };
			XMFLOAT3 Positions[VertexCount]{
				{ m_BoundingBox.Center.x - m_BoundingBox.Extents.x, m_BoundingBox.Center.y + m_BoundingBox.Extents.y, m_BoundingBox.Center.z + m_BoundingBox.Extents.z },
				{ m_BoundingBox.Center.x + m_BoundingBox.Extents.x, m_BoundingBox.Center.y + m_BoundingBox.Extents.y, m_BoundingBox.Center.z + m_BoundingBox.Extents.z },
				{ m_BoundingBox.Center.x - m_BoundingBox.Extents.x, m_BoundingBox.Center.y + m_BoundingBox.Extents.y, m_BoundingBox.Center.z - m_BoundingBox.Extents.z },
				{ m_BoundingBox.Center.x + m_BoundingBox.Extents.x, m_BoundingBox.Center.y + m_BoundingBox.Extents.y, m_BoundingBox.Center.z - m_BoundingBox.Extents.z },
				{ m_BoundingBox.Center.x - m_BoundingBox.Extents.x, m_BoundingBox.Center.y - m_BoundingBox.Extents.y, m_BoundingBox.Center.z + m_BoundingBox.Extents.z },
				{ m_BoundingBox.Center.x + m_BoundingBox.Extents.x, m_BoundingBox.Center.y - m_BoundingBox.Extents.y, m_BoundingBox.Center.z + m_BoundingBox.Extents.z },
				{ m_BoundingBox.Center.x - m_BoundingBox.Extents.x, m_BoundingBox.Center.y - m_BoundingBox.Extents.y, m_BoundingBox.Center.z - m_BoundingBox.Extents.z },
				{ m_BoundingBox.Center.x + m_BoundingBox.Extents.x, m_BoundingBox.Center.y - m_BoundingBox.Extents.y, m_BoundingBox.Center.z - m_BoundingBox.Extents.z }
			};

			m_D3D12BoundingBoxPositionBuffer = DX::CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, Positions, sizeof(XMFLOAT3) * VertexCount, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_D3D12BoundingBoxPositionUploadBuffer.GetAddressOf());
			m_D3D12BoundingBoxPositionBufferView.BufferLocation = m_D3D12BoundingBoxPositionBuffer->GetGPUVirtualAddress();
			m_D3D12BoundingBoxPositionBufferView.StrideInBytes = sizeof(XMFLOAT3);
			m_D3D12BoundingBoxPositionBufferView.SizeInBytes = sizeof(XMFLOAT3) * VertexCount;

			// 메쉬 캐시는 지역변수이므로 지역을 벗어나면 소멸하기 리소스가 소멸하기 때문에 카운트를 미리 1 증가시켜 놓는다.
			m_D3D12BoundingBoxPositionBuffer->AddRef();
			m_D3D12BoundingBoxPositionUploadBuffer->AddRef();

			const UINT IndexCount{ 36 };
			UINT Indices[IndexCount]{ 0, 1, 2, 1, 3, 2, 4, 6, 5, 5, 6, 7, 0, 4, 1, 1, 4, 5, 2, 3, 6, 3, 7, 6, 0, 2, 6, 0, 6, 4, 1, 5, 7, 1, 7, 3 };

			m_Indices.back().reserve(IndexCount);

			for (UINT i = 0; i < IndexCount; ++i)
			{
				m_Indices.back().push_back(Indices[i]);
			}

			m_D3D12IndexBuffers.back() = DX::CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, Indices, sizeof(UINT) * IndexCount, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, m_D3D12IndexUploadBuffers.back().GetAddressOf());
			m_D3D12IndexBufferViews.back().BufferLocation = m_D3D12IndexBuffers.back()->GetGPUVirtualAddress();
			m_D3D12IndexBufferViews.back().Format = DXGI_FORMAT_R32_UINT;
			m_D3D12IndexBufferViews.back().SizeInBytes = sizeof(UINT) * IndexCount;

			// 메쉬 캐시는 지역변수이므로 지역을 벗어나면 소멸하기 리소스가 소멸하기 때문에 카운트를 미리 1 증가시켜 놓는다.
			m_D3D12IndexBuffers.back()->AddRef();
			m_D3D12IndexUploadBuffers.back()->AddRef();
		}
		else if (Token == TEXT("</Mesh>") || Token == TEXT("</SkinnedMesh>"))
		{
			break;
		}
	}
#else
	InFile >> m_Name;

	while (InFile >> Token)
	{
		if (Token == TEXT("<Positions>"))
		{
			InFile >> VertexCount;

			if (VertexCount > 0)
			{
				tcout << TEXT(" 정점 수 : ") << VertexCount << endl;

				vector<XMFLOAT3> Positions{ VertexCount };

				for (UINT i = 0; i < VertexCount; ++i)
				{
					InFile >> Positions[i].x >> Positions[i].y >> Positions[i].z;
				}

				m_D3D12PositionBuffer = DX::CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, Positions.data(), sizeof(XMFLOAT3) * VertexCount, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_D3D12PositionUploadBuffer.GetAddressOf());
				m_D3D12PositionBufferView.BufferLocation = m_D3D12PositionBuffer->GetGPUVirtualAddress();
				m_D3D12PositionBufferView.StrideInBytes = sizeof(XMFLOAT3);
				m_D3D12PositionBufferView.SizeInBytes = sizeof(XMFLOAT3) * VertexCount;

				// 메쉬 캐시는 지역변수이므로 지역을 벗어나면 소멸하기 리소스가 소멸하기 때문에 카운트를 미리 1 증가시켜 놓는다.
				m_D3D12PositionBuffer->AddRef();
				m_D3D12PositionUploadBuffer->AddRef();
			}
		}
		else if (Token == TEXT("<Normals>"))
		{
			InFile >> VertexCount;

			if (VertexCount > 0)
			{
				vector<XMFLOAT3> Normals{ VertexCount };

				for (UINT i = 0; i < VertexCount; ++i)
				{
					InFile >> Normals[i].x >> Normals[i].y >> Normals[i].z;
				}

				m_D3D12NormalBuffer = DX::CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, Normals.data(), sizeof(XMFLOAT3) * VertexCount, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_D3D12NormalUploadBuffer.GetAddressOf());
				m_D3D12NormalBufferView.BufferLocation = m_D3D12NormalBuffer->GetGPUVirtualAddress();
				m_D3D12NormalBufferView.StrideInBytes = sizeof(XMFLOAT3);
				m_D3D12NormalBufferView.SizeInBytes = sizeof(XMFLOAT3) * VertexCount;

				// 메쉬 캐시는 지역변수이므로 지역을 벗어나면 소멸하기 리소스가 소멸하기 때문에 카운트를 미리 1 증가시켜 놓는다.
				m_D3D12NormalBuffer->AddRef();
				m_D3D12NormalUploadBuffer->AddRef();
			}
		}
		else if (Token == TEXT("<Tangents>"))
		{
			InFile >> VertexCount;

			if (VertexCount > 0)
			{
				vector<XMFLOAT3> Tangents{ VertexCount };

				for (UINT i = 0; i < VertexCount; ++i)
				{
					InFile >> Tangents[i].x >> Tangents[i].y >> Tangents[i].z;
				}

				m_D3D12TangentBuffer = DX::CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, Tangents.data(), sizeof(XMFLOAT3) * VertexCount, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_D3D12TangentUploadBuffer.GetAddressOf());
				m_D3D12TangentBufferView.BufferLocation = m_D3D12TangentBuffer->GetGPUVirtualAddress();
				m_D3D12TangentBufferView.StrideInBytes = sizeof(XMFLOAT3);
				m_D3D12TangentBufferView.SizeInBytes = sizeof(XMFLOAT3) * VertexCount;

				// 메쉬 캐시는 지역변수이므로 지역을 벗어나면 소멸하기 리소스가 소멸하기 때문에 카운트를 미리 1 증가시켜 놓는다.
				m_D3D12TangentBuffer->AddRef();
				m_D3D12TangentUploadBuffer->AddRef();
			}
		}
		else if (Token == TEXT("<BiTangents>"))
		{
			InFile >> VertexCount;

			if (VertexCount > 0)
			{
				vector<XMFLOAT3> BiTangents{ VertexCount };

				for (UINT i = 0; i < VertexCount; ++i)
				{
					InFile >> BiTangents[i].x >> BiTangents[i].y >> BiTangents[i].z;
				}

				m_D3D12BiTangentBuffer = DX::CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, BiTangents.data(), sizeof(XMFLOAT3) * VertexCount, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_D3D12BiTangentUploadBuffer.GetAddressOf());
				m_D3D12BiTangentBufferView.BufferLocation = m_D3D12BiTangentBuffer->GetGPUVirtualAddress();
				m_D3D12BiTangentBufferView.StrideInBytes = sizeof(XMFLOAT3);
				m_D3D12BiTangentBufferView.SizeInBytes = sizeof(XMFLOAT3) * VertexCount;

				// 메쉬 캐시는 지역변수이므로 지역을 벗어나면 소멸하기 리소스가 소멸하기 때문에 카운트를 미리 1 증가시켜 놓는다.
				m_D3D12BiTangentBuffer->AddRef();
				m_D3D12BiTangentUploadBuffer->AddRef();
			}
		}
		else if (Token == TEXT("<TexCoords>"))
		{
			InFile >> VertexCount;

			if (VertexCount > 0)
			{
				vector<XMFLOAT2> TexCoords{ VertexCount };

				for (UINT i = 0; i < VertexCount; ++i)
				{
					InFile >> TexCoords[i].x >> TexCoords[i].y;
				}

				m_D3D12TexCoordBuffer = DX::CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, TexCoords.data(), sizeof(XMFLOAT2) * VertexCount, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_D3D12TexCoordUploadBuffer.GetAddressOf());
				m_D3D12TexCoordBufferView.BufferLocation = m_D3D12TexCoordBuffer->GetGPUVirtualAddress();
				m_D3D12TexCoordBufferView.StrideInBytes = sizeof(XMFLOAT2);
				m_D3D12TexCoordBufferView.SizeInBytes = sizeof(XMFLOAT2) * VertexCount;

				// 메쉬 캐시는 지역변수이므로 지역을 벗어나면 소멸하기 리소스가 소멸하기 때문에 카운트를 미리 1 증가시켜 놓는다.
				m_D3D12TexCoordBuffer->AddRef();
				m_D3D12TexCoordUploadBuffer->AddRef();
			}
		}
		else if (Token == TEXT("<SubMeshes>"))
		{
			UINT SubMeshCount{};

			InFile >> SubMeshCount;

			if (SubMeshCount > 0)
			{
				// +1: BoundingBox
				m_IndexCounts.resize(SubMeshCount + 1);
				m_D3D12IndexBuffers.resize(SubMeshCount + 1);
				m_D3D12IndexUploadBuffers.resize(SubMeshCount + 1);
				m_D3D12IndexBufferViews.resize(SubMeshCount + 1);

				for (UINT i = 0; i < SubMeshCount; ++i)
				{
					// <Indices>
					InFile >> Token;
					InFile >> m_IndexCounts[i];

					if (m_IndexCounts[i] > 0)
					{
						vector<UINT> Indices(m_IndexCounts[i]);

						for (UINT j = 0; j < m_IndexCounts[i]; ++j)
						{
							InFile >> Indices[j];
						}

						m_D3D12IndexBuffers[i] = DX::CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, Indices.data(), sizeof(UINT) * m_IndexCounts[i], D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, m_D3D12IndexUploadBuffers[i].GetAddressOf());
						m_D3D12IndexBufferViews[i].BufferLocation = m_D3D12IndexBuffers[i]->GetGPUVirtualAddress();
						m_D3D12IndexBufferViews[i].Format = DXGI_FORMAT_R32_UINT;
						m_D3D12IndexBufferViews[i].SizeInBytes = sizeof(UINT) * m_IndexCounts[i];

						// 메쉬 캐시는 지역변수이므로 지역을 벗어나면 소멸하기 리소스가 소멸하기 때문에 카운트를 미리 1 증가시켜 놓는다.
						m_D3D12IndexBuffers[i]->AddRef();
						m_D3D12IndexUploadBuffers[i]->AddRef();
					}
				}
			}
		}
		else if (Token == TEXT("<Bounds>"))
		{
			XMFLOAT3 Center{};
			XMFLOAT3 Extents{};

			InFile >> Center.x >> Center.y >> Center.z;
			InFile >> Extents.x >> Extents.y >> Extents.z;

			SetBoundingBox(BoundingBox{ Center, Extents });

			const UINT VertexCount{ 8 };
			XMFLOAT3 Positions[VertexCount]{
				{ Center.x - Extents.x, Center.y + Extents.y, Center.z + Extents.z },
				{ Center.x + Extents.x, Center.y + Extents.y, Center.z + Extents.z },
				{ Center.x - Extents.x, Center.y + Extents.y, Center.z - Extents.z },
				{ Center.x + Extents.x, Center.y + Extents.y, Center.z - Extents.z },
				{ Center.x - Extents.x, Center.y - Extents.y, Center.z + Extents.z },
				{ Center.x + Extents.x, Center.y - Extents.y, Center.z + Extents.z },
				{ Center.x - Extents.x, Center.y - Extents.y, Center.z - Extents.z },
				{ Center.x + Extents.x, Center.y - Extents.y, Center.z - Extents.z }
			};

			m_D3D12BoundingBoxPositionBuffer = DX::CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, Positions, sizeof(XMFLOAT3) * VertexCount, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_D3D12BoundingBoxPositionUploadBuffer.GetAddressOf());
			m_D3D12BoundingBoxPositionBufferView.BufferLocation = m_D3D12BoundingBoxPositionBuffer->GetGPUVirtualAddress();
			m_D3D12BoundingBoxPositionBufferView.StrideInBytes = sizeof(XMFLOAT3);
			m_D3D12BoundingBoxPositionBufferView.SizeInBytes = sizeof(XMFLOAT3) * VertexCount;

			// 메쉬 캐시는 지역변수이므로 지역을 벗어나면 소멸하기 리소스가 소멸하기 때문에 카운트를 미리 1 증가시켜 놓는다.
			m_D3D12BoundingBoxPositionBuffer->AddRef();
			m_D3D12BoundingBoxPositionUploadBuffer->AddRef();

			const UINT IndexCount{ 36 };
			UINT Indices[IndexCount]{ 0, 1, 2, 1, 3, 2, 4, 6, 5, 5, 6, 7, 0, 4, 1, 1, 4, 5, 2, 3, 6, 3, 7, 6, 0, 2, 6, 0, 6, 4, 1, 5, 7, 1, 7, 3 };

			m_IndexCounts.back() = IndexCount;
			m_D3D12IndexBuffers.back() = DX::CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, Indices, sizeof(UINT) * IndexCount, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, m_D3D12IndexUploadBuffers.back().GetAddressOf());
			m_D3D12IndexBufferViews.back().BufferLocation = m_D3D12IndexBuffers.back()->GetGPUVirtualAddress();
			m_D3D12IndexBufferViews.back().Format = DXGI_FORMAT_R32_UINT;
			m_D3D12IndexBufferViews.back().SizeInBytes = sizeof(UINT) * IndexCount;

			// 메쉬 캐시는 지역변수이므로 지역을 벗어나면 소멸하기 리소스가 소멸하기 때문에 카운트를 미리 1 증가시켜 놓는다.
			m_D3D12IndexBuffers.back()->AddRef();
			m_D3D12IndexUploadBuffers.back()->AddRef();
		}
		else if (Token == TEXT("</Mesh>") || Token == TEXT("</SkinnedMesh>"))
		{
			break;
		}
	}
#endif
}

void CMesh::SetName(const tstring& Name)
{
	m_Name = Name;
}

const tstring& CMesh::GetName() const
{
	return m_Name;
}

void CMesh::SetBoundingBox(const BoundingBox& BoundingBox)
{
	m_BoundingBox = BoundingBox;
}

const BoundingBox& CMesh::GetBoundingBox() const
{
	return m_BoundingBox;
}

void CMesh::RenderBoundingBox(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	D3D12_VERTEX_BUFFER_VIEW VertexBufferViews[] = { m_D3D12BoundingBoxPositionBufferView };

	D3D12GraphicsCommandList->IASetVertexBuffers(0, _countof(VertexBufferViews), VertexBufferViews);
	D3D12GraphicsCommandList->IASetPrimitiveTopology(m_D3D12PrimitiveTopology);
	D3D12GraphicsCommandList->IASetIndexBuffer(&m_D3D12IndexBufferViews.back());
	D3D12GraphicsCommandList->DrawIndexedInstanced(static_cast<UINT>(m_Indices.back().size()), 1, 0, 0, 0);
}

bool CMesh::CheckRayIntersection(const XMFLOAT3& RayOrigin, const XMFLOAT3& RayDirection, const XMMATRIX& WorldMatrix, float& HitDistance)
{
	bool Intersected{};

	// -1: BoundBox
	UINT SubMeshCount{ static_cast<UINT>(m_Indices.size() - 1) };
	float NearestHitDistance{ FLT_MAX };

	for (UINT i = 0; i < SubMeshCount; ++i)
	{
		UINT PrimitiveCount{ static_cast<UINT>(m_Indices[i].size()) };

		for (UINT j = 0; j < PrimitiveCount; j += 3)
		{
			XMVECTOR Vertex1{ XMVector3TransformCoord(XMLoadFloat3(&m_Positions[m_Indices[i][j]]), WorldMatrix) };
			XMVECTOR Vertex2{ XMVector3TransformCoord(XMLoadFloat3(&m_Positions[m_Indices[i][j + 1]]), WorldMatrix) };
			XMVECTOR Vertex3{ XMVector3TransformCoord(XMLoadFloat3(&m_Positions[m_Indices[i][j + 2]]), WorldMatrix) };

			// 메쉬의 모든 프리미티브(삼각형)들에 대하여 픽킹 광선과의 충돌을 검사한다.
			if (TriangleTests::Intersects(XMLoadFloat3(&RayOrigin), XMLoadFloat3(&RayDirection), Vertex1, Vertex2, Vertex3, HitDistance))
			{
				if (HitDistance < NearestHitDistance)
				{
					NearestHitDistance = HitDistance;
					Intersected = true;
				}
			}
		}
	}

	HitDistance = NearestHitDistance;

	return Intersected;
}

//=========================================================================================================================

CSkinnedMesh::CSkinnedMesh(const CSkinnedMesh& Rhs)
{
	m_Name = Rhs.m_Name;

	m_D3D12PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_Positions = Rhs.m_Positions;
	m_Indices = Rhs.m_Indices;

	m_D3D12PositionBuffer = Rhs.m_D3D12PositionBuffer;
	m_D3D12PositionUploadBuffer = Rhs.m_D3D12PositionUploadBuffer;
	m_D3D12PositionBufferView = Rhs.m_D3D12PositionBufferView;

	m_D3D12NormalBuffer = Rhs.m_D3D12NormalBuffer;
	m_D3D12NormalUploadBuffer = Rhs.m_D3D12NormalUploadBuffer;
	m_D3D12NormalBufferView = Rhs.m_D3D12NormalBufferView;

	m_D3D12TangentBuffer = Rhs.m_D3D12TangentBuffer;
	m_D3D12TangentUploadBuffer = Rhs.m_D3D12TangentUploadBuffer;
	m_D3D12TangentBufferView = Rhs.m_D3D12TangentBufferView;

	m_D3D12BiTangentBuffer = Rhs.m_D3D12BiTangentBuffer;
	m_D3D12BiTangentUploadBuffer = Rhs.m_D3D12BiTangentUploadBuffer;
	m_D3D12BiTangentBufferView = Rhs.m_D3D12BiTangentBufferView;

	m_D3D12TexCoordBuffer = Rhs.m_D3D12TexCoordBuffer;
	m_D3D12TexCoordUploadBuffer = Rhs.m_D3D12TexCoordUploadBuffer;
	m_D3D12TexCoordBufferView = Rhs.m_D3D12TexCoordBufferView;

	m_D3D12BoundingBoxPositionBuffer = Rhs.m_D3D12BoundingBoxPositionBuffer;
	m_D3D12BoundingBoxPositionUploadBuffer = Rhs.m_D3D12BoundingBoxPositionUploadBuffer;
	m_D3D12BoundingBoxPositionBufferView = Rhs.m_D3D12BoundingBoxPositionBufferView;

	m_D3D12PositionBuffer->AddRef();
	m_D3D12PositionUploadBuffer->AddRef();

	m_D3D12NormalBuffer->AddRef();
	m_D3D12NormalUploadBuffer->AddRef();

	m_D3D12TangentBuffer->AddRef();
	m_D3D12TangentUploadBuffer->AddRef();

	m_D3D12BiTangentBuffer->AddRef();
	m_D3D12BiTangentUploadBuffer->AddRef();

	m_D3D12TexCoordBuffer->AddRef();
	m_D3D12TexCoordUploadBuffer->AddRef();

	m_D3D12BoundingBoxPositionBuffer->AddRef();
	m_D3D12BoundingBoxPositionUploadBuffer->AddRef();

	UINT IndexBufferSize{ static_cast<UINT>(Rhs.m_D3D12IndexBuffers.size()) };

	m_D3D12IndexBuffers.reserve(IndexBufferSize);
	m_D3D12IndexUploadBuffers.reserve(IndexBufferSize);
	m_D3D12IndexBufferViews.reserve(IndexBufferSize);

	for (UINT i = 0; i < IndexBufferSize; ++i)
	{
		m_D3D12IndexBuffers.push_back(Rhs.m_D3D12IndexBuffers[i]);
		m_D3D12IndexUploadBuffers.push_back(Rhs.m_D3D12IndexUploadBuffers[i]);
		m_D3D12IndexBufferViews.push_back(Rhs.m_D3D12IndexBufferViews[i]);

		m_D3D12IndexBuffers[i]->AddRef();
		m_D3D12IndexUploadBuffers[i]->AddRef();
	}

	m_BoundingBox = Rhs.m_BoundingBox;
}

void CSkinnedMesh::CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{

}

void CSkinnedMesh::UpdateShaderVariables(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	if (m_D3D12BoneOffsetMatrixes)
	{
		D3D12GraphicsCommandList->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_TYPE_BONE_OFFSET, m_D3D12BoneOffsetMatrixes->GetGPUVirtualAddress());
	}

	if (m_D3D12BoneTransformMatrixes)
	{
		UINT BoneFrameCount{ static_cast<UINT>(m_BoneFrameCaches.size()) };

		for (UINT i = 0; i < BoneFrameCount; ++i)
		{
			XMStoreFloat4x4(&m_MappedBoneTransformMatrixes[i], XMMatrixTranspose(XMLoadFloat4x4(&m_BoneFrameCaches[i]->GetWorldMatrix())));
		}

		D3D12GraphicsCommandList->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_TYPE_BONE_TRANSFORM, m_D3D12BoneTransformMatrixes->GetGPUVirtualAddress());
	}
}

void CSkinnedMesh::ReleaseShaderVariables()
{

}

void CSkinnedMesh::ReleaseUploadBuffers()
{
	CMesh::ReleaseUploadBuffers();

	if (m_D3D12BoneIndexUploadBuffer)
	{
		m_D3D12BoneIndexUploadBuffer.ReleaseAndGetAddressOf();
	}

	if (m_D3D12BoneWeightUploadBuffer)
	{
		m_D3D12BoneWeightUploadBuffer.ReleaseAndGetAddressOf();
	}
}

void CSkinnedMesh::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, UINT SubSetIndex)
{
	UpdateShaderVariables(D3D12GraphicsCommandList);

	D3D12_VERTEX_BUFFER_VIEW VertexBufferViews[] = { m_D3D12PositionBufferView, m_D3D12NormalBufferView, m_D3D12TangentBufferView, m_D3D12BiTangentBufferView, m_D3D12TexCoordBufferView, m_D3D12BoneIndexBufferView, m_D3D12BoneWeightBufferView };

	D3D12GraphicsCommandList->IASetVertexBuffers(0, _countof(VertexBufferViews), VertexBufferViews);
	D3D12GraphicsCommandList->IASetPrimitiveTopology(m_D3D12PrimitiveTopology);

	UINT BufferSize{ static_cast<UINT>(m_D3D12IndexBuffers.size()) };

	if (BufferSize > 0 && SubSetIndex < BufferSize)
	{
		D3D12GraphicsCommandList->IASetIndexBuffer(&m_D3D12IndexBufferViews[SubSetIndex]);
		D3D12GraphicsCommandList->DrawIndexedInstanced(static_cast<UINT>(m_Indices[SubSetIndex].size()), 1, 0, 0, 0);
	}
	else
	{
		D3D12GraphicsCommandList->DrawInstanced(static_cast<UINT>(m_Positions.size()), 1, 0, 0);
	}
}

void CSkinnedMesh::LoadSkinInfoFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, tifstream& InFile)
{
	tstring Token{};

#ifdef READ_BINARY_FILE
	while (true)
	{
		File::ReadStringFromFile(InFile, Token);

		if (Token == TEXT("<BoneOffsetMatrixes>"))
		{
			m_BoneCount = File::ReadIntegerFromFile(InFile);

			if (m_BoneCount > 0)
			{
				m_BoneOffsetMatrixes.reserve(m_BoneCount);

				for (UINT i = 0; i < m_BoneCount; ++i)
				{
					XMFLOAT4X4 BoneOffsetMatrix{};

					InFile.read(reinterpret_cast<TCHAR*>(&BoneOffsetMatrix), sizeof(XMFLOAT4X4));

					m_BoneOffsetMatrixes.push_back(BoneOffsetMatrix);
				}

				UINT Byte{ ((sizeof(XMFLOAT4X4) * MAX_BONES) + 255) & ~255 };

				m_D3D12BoneOffsetMatrixes = DX::CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, nullptr, Byte, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr);
				m_D3D12BoneOffsetMatrixes->Map(0, nullptr, reinterpret_cast<void**>(&m_MappedBoneOffsetMatrixes));

				for (UINT i = 0; i < m_BoneCount; ++i)
				{
					XMStoreFloat4x4(&m_MappedBoneOffsetMatrixes[i], XMMatrixTranspose(XMLoadFloat4x4(&m_BoneOffsetMatrixes[i])));
				}
			}
		}
		else if (Token == TEXT("<BoneIndices>"))
		{
			UINT VertexCount{ File::ReadIntegerFromFile(InFile) };

			if (VertexCount > 0)
			{
				vector<XMUINT4> BoneIndices{ VertexCount };

				for (UINT i = 0; i < VertexCount; ++i)
				{
					InFile.read(reinterpret_cast<TCHAR*>(&BoneIndices[i]), sizeof(XMUINT4));
				}

				m_D3D12BoneIndexBuffer = DX::CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, BoneIndices.data(), sizeof(XMUINT4) * VertexCount, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_D3D12BoneIndexUploadBuffer.GetAddressOf());
				m_D3D12BoneIndexBufferView.BufferLocation = m_D3D12BoneIndexBuffer->GetGPUVirtualAddress();
				m_D3D12BoneIndexBufferView.StrideInBytes = sizeof(XMUINT4);
				m_D3D12BoneIndexBufferView.SizeInBytes = sizeof(XMUINT4) * VertexCount;
			}
		}
		else if (Token == TEXT("<BoneWeights>"))
		{
			UINT VertexCount{ File::ReadIntegerFromFile(InFile) };

			if (VertexCount > 0)
			{
				vector<XMFLOAT4> BoneWeights{ VertexCount };

				for (UINT i = 0; i < VertexCount; ++i)
				{
					InFile.read(reinterpret_cast<TCHAR*>(&BoneWeights[i]), sizeof(XMFLOAT4));
				}

				m_D3D12BoneWeightBuffer = DX::CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, BoneWeights.data(), sizeof(XMFLOAT4) * VertexCount, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_D3D12BoneWeightUploadBuffer.GetAddressOf());
				m_D3D12BoneWeightBufferView.BufferLocation = m_D3D12BoneWeightBuffer->GetGPUVirtualAddress();
				m_D3D12BoneWeightBufferView.StrideInBytes = sizeof(XMFLOAT4);
				m_D3D12BoneWeightBufferView.SizeInBytes = sizeof(XMFLOAT4) * VertexCount;
			}
		}
		else if (Token == TEXT("</SkinInfo>"))
		{
			break;
		}
	}
#else
	while (InFile >> Token)
	{
		if (Token == TEXT("<BoneOffsetMatrixes>"))
		{
			InFile >> m_BoneCount;

			if (m_BoneCount > 0)
			{
				m_BoneOffsetMatrixes.reserve(m_BoneCount);

				for (UINT i = 0; i < m_BoneCount; ++i)
				{
					XMFLOAT4X4 BoneOffsetMatrix{};

					InFile >> BoneOffsetMatrix._11 >> BoneOffsetMatrix._12 >> BoneOffsetMatrix._13 >> BoneOffsetMatrix._14;
					InFile >> BoneOffsetMatrix._21 >> BoneOffsetMatrix._22 >> BoneOffsetMatrix._23 >> BoneOffsetMatrix._24;
					InFile >> BoneOffsetMatrix._31 >> BoneOffsetMatrix._32 >> BoneOffsetMatrix._33 >> BoneOffsetMatrix._34;
					InFile >> BoneOffsetMatrix._41 >> BoneOffsetMatrix._42 >> BoneOffsetMatrix._43 >> BoneOffsetMatrix._44;

					m_BoneOffsetMatrixes.push_back(BoneOffsetMatrix);
				}

				UINT Byte{ ((sizeof(XMFLOAT4X4) * MAX_BONES) + 255) & ~255 };

				m_D3D12BoneOffsetMatrixes = DX::CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, nullptr, Byte, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr);
				m_D3D12BoneOffsetMatrixes->Map(0, nullptr, reinterpret_cast<void**>(&m_MappedBoneOffsetMatrixes));

				for (UINT i = 0; i < m_BoneCount; ++i)
				{
					XMStoreFloat4x4(&m_MappedBoneOffsetMatrixes[i], XMMatrixTranspose(XMLoadFloat4x4(&m_BoneOffsetMatrixes[i])));
				}
			}
		}
		else if (Token == TEXT("<BoneIndices>"))
		{
			InFile >> VertexCount;

			if (VertexCount > 0)
			{
				vector<XMUINT4> BoneIndices{ VertexCount };

				for (UINT i = 0; i < VertexCount; ++i)
				{
					InFile >> BoneIndices[i].x >> BoneIndices[i].y >> BoneIndices[i].z >> BoneIndices[i].w;
				}

				m_D3D12BoneIndexBuffer = DX::CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, BoneIndices.data(), sizeof(XMUINT4) * VertexCount, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_D3D12BoneIndexUploadBuffer.GetAddressOf());
				m_D3D12BoneIndexBufferView.BufferLocation = m_D3D12BoneIndexBuffer->GetGPUVirtualAddress();
				m_D3D12BoneIndexBufferView.StrideInBytes = sizeof(XMUINT4);
				m_D3D12BoneIndexBufferView.SizeInBytes = sizeof(XMUINT4) * VertexCount;
			}
		}
		else if (Token == TEXT("<BoneWeights>"))
		{
			InFile >> VertexCount;

			if (VertexCount > 0)
			{
				vector<XMFLOAT4> BoneWeights{ VertexCount };

				for (UINT i = 0; i < VertexCount; ++i)
				{
					InFile >> BoneWeights[i].x >> BoneWeights[i].y >> BoneWeights[i].z >> BoneWeights[i].w;
				}

				m_D3D12BoneWeightBuffer = DX::CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, BoneWeights.data(), sizeof(XMFLOAT4) * VertexCount, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_D3D12BoneWeightUploadBuffer.GetAddressOf());
				m_D3D12BoneWeightBufferView.BufferLocation = m_D3D12BoneWeightBuffer->GetGPUVirtualAddress();
				m_D3D12BoneWeightBufferView.StrideInBytes = sizeof(XMFLOAT4);
				m_D3D12BoneWeightBufferView.SizeInBytes = sizeof(XMFLOAT4) * VertexCount;
			}
		}
		else if (Token == TEXT("</SkinInfo>"))
		{
			break;
		}
	}
#endif
}

void CSkinnedMesh::SetBoneFrameCaches(const vector<shared_ptr<CGameObject>>& BoneFrames)
{
	m_BoneFrameCaches.assign(BoneFrames.begin(), BoneFrames.end());
}

void CSkinnedMesh::SetBoneTransformInfo(const ComPtr<ID3D12Resource>& D3D12BoneTransformMatrixes, XMFLOAT4X4* MappedBoneTransformMatrixes)
{
	if (D3D12BoneTransformMatrixes)
	{
		m_D3D12BoneTransformMatrixes = D3D12BoneTransformMatrixes;
	}

	if (MappedBoneTransformMatrixes)
	{
		m_MappedBoneTransformMatrixes = MappedBoneTransformMatrixes;
	}
}
