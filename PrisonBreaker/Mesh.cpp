#include "stdafx.h"
#include "Mesh.h"

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

void CMesh::LoadMeshFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, tifstream& InFile)
{
	tstring Token{};

	while (InFile >> Token)
	{
		if (!Token.compare(TEXT("<Bounds>")))
		{
			XMFLOAT3 Center{};
			XMFLOAT3 Extents{};
			XMFLOAT4 Orientation{ 0.0f, 0.0f, 0.0f, 1.0f };

			InFile >> Center.x >> Center.y >> Center.z;
			InFile >> Extents.x >> Extents.y >> Extents.z;

			SetBoundingBox(BoundingOrientedBox{ Center, Extents, Orientation });
		}
		else if (!Token.compare(TEXT("<Positions>")))
		{
			InFile >> m_VertexCount;

			if (m_VertexCount > 0)
			{
				tcout << TEXT(" Á¤Á¡ ¼ö : ") << m_VertexCount << endl;

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
		else if (!Token.compare(TEXT("<Normals>")))
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
		else if (!Token.compare(TEXT("<Tangents>")))
		{
			UINT VertexCount{};

			InFile >> VertexCount;

			if (VertexCount > 0)
			{
				vector<XMFLOAT4> Tangents{ VertexCount };

				for (UINT i = 0; i < VertexCount; ++i)
				{
					InFile >> Tangents[i].x >> Tangents[i].y >> Tangents[i].z >> Tangents[i].w;
				}

				UINT Stride{ sizeof(XMFLOAT3) };

				m_D3D12TangentBuffer = DX::CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, Tangents.data(), Stride * VertexCount, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_D3D12TangentUploadBuffer.GetAddressOf());
				m_D3D12TangentBufferView.BufferLocation = m_D3D12TangentBuffer->GetGPUVirtualAddress();
				m_D3D12TangentBufferView.StrideInBytes = Stride;
				m_D3D12TangentBufferView.SizeInBytes = Stride * VertexCount;
			}
		}
		else if (!Token.compare(TEXT("<BiTangents>")))
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
		else if (!Token.compare(TEXT("<TexCoords>")))
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
		else if (!Token.compare(TEXT("<SubMeshes>")))
		{
			UINT SubMeshCount{};

			InFile >> SubMeshCount;

			if (SubMeshCount > 0)
			{
				m_IndexCounts.resize(SubMeshCount);
				m_D3D12IndexBuffers.resize(SubMeshCount);
				m_D3D12IndexUploadBuffers.resize(SubMeshCount);
				m_D3D12IndexBufferViews.resize(SubMeshCount);

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
		else if (!Token.compare(TEXT("</Mesh>")))
		{
			break;
		}
	}
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

void CMesh::SetBoundingBox(const BoundingOrientedBox& BoundingBox)
{
	m_BoundingBox = BoundingBox;
}

const BoundingOrientedBox& CMesh::GetBoundingBox() const
{
	return m_BoundingBox;
}
