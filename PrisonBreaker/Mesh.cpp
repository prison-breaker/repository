#include "stdafx.h"
#include "Mesh.h"

void CMesh::LoadMeshFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, tifstream& InFile)
{
	tstring Token{};

#ifdef BINARY_MODE
	while (true)
	{
		File::ReadStringFromFile(InFile, Token);

		if (Token == TEXT("<Positions>"))
		{
			m_VertexCount = File::ReadIntegerFromFile(InFile);

			if (m_VertexCount > 0)
			{
				tcout << TEXT(" 정점 수 : ") << m_VertexCount << endl;

				vector<XMFLOAT3> Positions{ m_VertexCount };

				InFile.read(reinterpret_cast<TCHAR*>(Positions.data()), sizeof(XMFLOAT3) * m_VertexCount);

				m_D3D12PositionBuffer = DX::CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, Positions.data(), sizeof(XMFLOAT3) * m_VertexCount, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_D3D12PositionUploadBuffer.GetAddressOf());
				m_D3D12PositionBufferView.BufferLocation = m_D3D12PositionBuffer->GetGPUVirtualAddress();
				m_D3D12PositionBufferView.StrideInBytes = sizeof(XMFLOAT3);
				m_D3D12PositionBufferView.SizeInBytes = sizeof(XMFLOAT3) * m_VertexCount;
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
			}
		}
		else if (Token == TEXT("<SubMeshes>"))
		{
			UINT SubMeshCount{ File::ReadIntegerFromFile(InFile) };

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
					File::ReadStringFromFile(InFile, Token);
					m_IndexCounts[i] = File::ReadIntegerFromFile(InFile);

					if (m_IndexCounts[i] > 0)
					{
						vector<UINT> Indices(m_IndexCounts[i]);

						InFile.read(reinterpret_cast<TCHAR*>(Indices.data()), sizeof(UINT) * m_IndexCounts[i]);

						m_D3D12IndexBuffers[i] = DX::CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, Indices.data(), sizeof(UINT) * m_IndexCounts[i], D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, m_D3D12IndexUploadBuffers[i].GetAddressOf());
						m_D3D12IndexBufferViews[i].BufferLocation = m_D3D12IndexBuffers[i]->GetGPUVirtualAddress();
						m_D3D12IndexBufferViews[i].Format = DXGI_FORMAT_R32_UINT;
						m_D3D12IndexBufferViews[i].SizeInBytes = sizeof(UINT) * m_IndexCounts[i];
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

			const UINT IndexCount{ 36 };
			UINT Indices[IndexCount]{ 0, 1, 2, 1, 3, 2, 4, 6, 5, 5, 6, 7, 0, 4, 1, 1, 4, 5, 2, 3, 6, 3, 7, 6, 0, 2, 6, 0, 6, 4, 1, 5, 7, 1, 7, 3 };

			m_IndexCounts.back() = IndexCount;
			m_D3D12IndexBuffers.back() = DX::CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, Indices, sizeof(UINT) * IndexCount, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, m_D3D12IndexUploadBuffers.back().GetAddressOf());
			m_D3D12IndexBufferViews.back().BufferLocation = m_D3D12IndexBuffers.back()->GetGPUVirtualAddress();
			m_D3D12IndexBufferViews.back().Format = DXGI_FORMAT_R32_UINT;
			m_D3D12IndexBufferViews.back().SizeInBytes = sizeof(UINT) * IndexCount;
		}
		else if (Token == TEXT("</Mesh>"))
		{
			break;
		}
	}
#else
	while (InFile >> Token)
	{
		if (Token == TEXT("<Positions>"))
		{
			InFile >> m_VertexCount;

			if (m_VertexCount > 0)
			{
				tcout << TEXT(" 정점 수 : ") << m_VertexCount << endl;

				vector<XMFLOAT3> Positions{ m_VertexCount };

				for (UINT i = 0; i < m_VertexCount; ++i)
				{
					InFile >> Positions[i].x >> Positions[i].y >> Positions[i].z;
				}

				UINT Stride{ sizeof(XMFLOAT3) };

				m_D3D12PositionBuffer = DX::CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, Positions.data(), Stride * m_VertexCount, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_D3D12PositionUploadBuffer.GetAddressOf());
				m_D3D12PositionBufferView.BufferLocation = m_D3D12PositionBuffer->GetGPUVirtualAddress();
				m_D3D12PositionBufferView.StrideInBytes = Stride;
				m_D3D12PositionBufferView.SizeInBytes = Stride * m_VertexCount;
			}
		}
		else if (Token == TEXT("<Normals>"))
		{
			UINT VertexCount{};

			InFile >> VertexCount;

			if (VertexCount > 0)
			{
				vector<XMFLOAT3> Normals{ VertexCount };

				for (UINT i = 0; i < VertexCount; ++i)
				{
					InFile >> Normals[i].x >> Normals[i].y >> Normals[i].z;
				}

				UINT Stride{ sizeof(XMFLOAT3) };

				m_D3D12NormalBuffer = DX::CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, Normals.data(), Stride * VertexCount, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_D3D12NormalUploadBuffer.GetAddressOf());
				m_D3D12NormalBufferView.BufferLocation = m_D3D12NormalBuffer->GetGPUVirtualAddress();
				m_D3D12NormalBufferView.StrideInBytes = Stride;
				m_D3D12NormalBufferView.SizeInBytes = Stride * VertexCount;
			}
		}
		else if (Token == TEXT("<Tangents>"))
		{
			UINT VertexCount{};

			InFile >> VertexCount;

			if (VertexCount > 0)
			{
				vector<XMFLOAT3> Tangents{ VertexCount };

				for (UINT i = 0; i < VertexCount; ++i)
				{
					InFile >> Tangents[i].x >> Tangents[i].y >> Tangents[i].z;
				}

				UINT Stride{ sizeof(XMFLOAT3) };

				m_D3D12TangentBuffer = DX::CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, Tangents.data(), Stride * VertexCount, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_D3D12TangentUploadBuffer.GetAddressOf());
				m_D3D12TangentBufferView.BufferLocation = m_D3D12TangentBuffer->GetGPUVirtualAddress();
				m_D3D12TangentBufferView.StrideInBytes = Stride;
				m_D3D12TangentBufferView.SizeInBytes = Stride * VertexCount;
			}
		}
		else if (Token == TEXT("<BiTangents>"))
		{
			UINT VertexCount{};

			InFile >> VertexCount;

			if (VertexCount > 0)
			{
				vector<XMFLOAT3> BiTangents{ VertexCount };

				for (UINT i = 0; i < VertexCount; ++i)
				{
					InFile >> BiTangents[i].x >> BiTangents[i].y >> BiTangents[i].z;
				}

				UINT Stride{ sizeof(XMFLOAT3) };

				m_D3D12BiTangentBuffer = DX::CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, BiTangents.data(), Stride * VertexCount, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_D3D12BiTangentUploadBuffer.GetAddressOf());
				m_D3D12BiTangentBufferView.BufferLocation = m_D3D12BiTangentBuffer->GetGPUVirtualAddress();
				m_D3D12BiTangentBufferView.StrideInBytes = Stride;
				m_D3D12BiTangentBufferView.SizeInBytes = Stride * VertexCount;
			}
		}
		else if (Token == TEXT("<TexCoords>"))
		{
			UINT VertexCount{};

			InFile >> VertexCount;

			if (VertexCount > 0)
			{
				vector<XMFLOAT2> TexCoords{ VertexCount };

				for (UINT i = 0; i < VertexCount; ++i)
				{
					InFile >> TexCoords[i].x >> TexCoords[i].y;
				}

				UINT Stride{ sizeof(XMFLOAT2) };

				m_D3D12TexCoordBuffer = DX::CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, TexCoords.data(), Stride * VertexCount, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_D3D12TexCoordUploadBuffer.GetAddressOf());
				m_D3D12TexCoordBufferView.BufferLocation = m_D3D12TexCoordBuffer->GetGPUVirtualAddress();
				m_D3D12TexCoordBufferView.StrideInBytes = Stride;
				m_D3D12TexCoordBufferView.SizeInBytes = Stride * VertexCount;
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

						UINT Stride{ sizeof(UINT) };

						m_D3D12IndexBuffers[i] = DX::CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, Indices.data(), Stride * m_IndexCounts[i], D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, m_D3D12IndexUploadBuffers[i].GetAddressOf());
						m_D3D12IndexBufferViews[i].BufferLocation = m_D3D12IndexBuffers[i]->GetGPUVirtualAddress();
						m_D3D12IndexBufferViews[i].Format = DXGI_FORMAT_R32_UINT;
						m_D3D12IndexBufferViews[i].SizeInBytes = Stride * m_IndexCounts[i];
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

			UINT Stride{ sizeof(XMFLOAT3) };

			m_D3D12BoundingBoxPositionBuffer = DX::CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, Positions, Stride * VertexCount, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_D3D12BoundingBoxPositionUploadBuffer.GetAddressOf());
			m_D3D12BoundingBoxPositionBufferView.BufferLocation = m_D3D12BoundingBoxPositionBuffer->GetGPUVirtualAddress();
			m_D3D12BoundingBoxPositionBufferView.StrideInBytes = Stride;
			m_D3D12BoundingBoxPositionBufferView.SizeInBytes = Stride * VertexCount;

			const UINT IndexCount{ 36 };
			UINT Indices[IndexCount]{ 0, 1, 2, 1, 3, 2, 4, 6, 5, 5, 6, 7, 0, 4, 1, 1, 4, 5, 2, 3, 6, 3, 7, 6, 0, 2, 6, 0, 6, 4, 1, 5, 7, 1, 7, 3 };

			Stride = sizeof(UINT);

			m_IndexCounts.back() = IndexCount;
			m_D3D12IndexBuffers.back() = DX::CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, Indices, Stride * IndexCount, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, m_D3D12IndexUploadBuffers.back().GetAddressOf());
			m_D3D12IndexBufferViews.back().BufferLocation = m_D3D12IndexBuffers.back()->GetGPUVirtualAddress();
			m_D3D12IndexBufferViews.back().Format = DXGI_FORMAT_R32_UINT;
			m_D3D12IndexBufferViews.back().SizeInBytes = Stride * IndexCount;
		}
		else if (Token == TEXT("</Mesh>"))
		{
			break;
		}
	}
#endif
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

void CMesh::SetBoundingBox(const BoundingBox& BoundingBox)
{
	m_BoundingBox = BoundingBox;
}

const BoundingBox& CMesh::GetBoundingBox() const
{
	return m_BoundingBox;
}

void CMesh::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, UINT SubSetIndex)
{
	D3D12_VERTEX_BUFFER_VIEW VertexBufferViews[] = { m_D3D12PositionBufferView, m_D3D12NormalBufferView, m_D3D12TangentBufferView, m_D3D12BiTangentBufferView, m_D3D12TexCoordBufferView };

	D3D12GraphicsCommandList->IASetVertexBuffers(0, _countof(VertexBufferViews), VertexBufferViews);
	D3D12GraphicsCommandList->IASetPrimitiveTopology(m_D3D12PrimitiveTopology);

	UINT BufferSize{ (UINT)m_D3D12IndexBuffers.size() };

	if (BufferSize > 0 && SubSetIndex < BufferSize)
	{
		D3D12GraphicsCommandList->IASetIndexBuffer(&m_D3D12IndexBufferViews[SubSetIndex]);
		D3D12GraphicsCommandList->DrawIndexedInstanced(m_IndexCounts[SubSetIndex], 1, 0, 0, 0);
	}
	else
	{
		D3D12GraphicsCommandList->DrawInstanced(m_VertexCount, 1, 0, 0);
	}
}

void CMesh::RenderBoundingBox(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	D3D12_VERTEX_BUFFER_VIEW VertexBufferViews[] = { m_D3D12BoundingBoxPositionBufferView };

	D3D12GraphicsCommandList->IASetVertexBuffers(0, _countof(VertexBufferViews), VertexBufferViews);
	D3D12GraphicsCommandList->IASetPrimitiveTopology(m_D3D12PrimitiveTopology);
	D3D12GraphicsCommandList->IASetIndexBuffer(&m_D3D12IndexBufferViews.back());
	D3D12GraphicsCommandList->DrawIndexedInstanced(m_IndexCounts.back(), 1, 0, 0, 0);
}
