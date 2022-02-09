#include "stdafx.h"
#include "Mesh.h"

void CMesh::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	D3D12_VERTEX_BUFFER_VIEW VertexBufferViews[] = { m_D3D12PositionBufferView, m_D3D12NormalBufferView, m_D3D12TangentBufferView, m_D3D12BiTangentBufferView, m_D3D12TexCoordBufferView };

	D3D12GraphicsCommandList->IASetVertexBuffers(0, _countof(VertexBufferViews), VertexBufferViews);
	D3D12GraphicsCommandList->IASetPrimitiveTopology(m_D3D12PrimitiveTopology);

	if (m_D3D12IndexBuffer)
	{
		D3D12GraphicsCommandList->IASetIndexBuffer(&m_D3D12IndexBufferView);
		D3D12GraphicsCommandList->DrawIndexedInstanced(m_IndexCount, 1, 0, 0, 0);
	}
	else
	{
		D3D12GraphicsCommandList->DrawInstanced(m_VertexCount, 1, 0, 0);
	}
}

void CMesh::LoadMeshFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, tifstream& InFile)
{
	tstring Token{};
	vector<XMFLOAT3> Positions{}, Normals{}, Tangents{}, BiTangents{};
	vector<XMFLOAT2> TexCoords{};
	vector<UINT> Indices{};

	while (InFile >> Token)
	{
		if (!Token.compare(TEXT("<Bounds>")))
		{
			XMFLOAT3 Center{};
			XMFLOAT3 Extents{};
			XMFLOAT4 Orientation{ 0.0f, 0.0f, 0.0f, 1.0f };

			InFile >> Center.x >> Center.y >> Center.z;
			InFile >> Extents.x >> Extents.y >> Extents.z;

			BoundingOrientedBox BoundingBox{ Center, Extents, Orientation };

			SetBoundingBox(BoundingBox);
		}
		else if (!Token.compare(TEXT("<Positions>")))
		{
			InFile >> m_VertexCount;
			Positions.reserve(m_VertexCount);

			if (m_VertexCount > 0)
			{
				XMFLOAT3 Position{};

				for (UINT i = 0; i < m_VertexCount; ++i)
				{
					InFile >> Position.x >> Position.y >> Position.z;
					Positions.emplace_back(Position.x, Position.y, Position.z);
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
			Normals.reserve(VertexCount);

			if (VertexCount > 0)
			{
				XMFLOAT3 Normal{};

				for (UINT i = 0; i < VertexCount; ++i)
				{
					InFile >> Normal.x >> Normal.y >> Normal.z;
					Normals.emplace_back(Normal.x, Normal.y, Normal.z);
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
			Tangents.reserve(VertexCount);

			if (VertexCount > 0)
			{
				XMFLOAT4 Tangent{};

				for (UINT i = 0; i < VertexCount; ++i)
				{
					InFile >> Tangent.x >> Tangent.y >> Tangent.z >> Tangent.w;
					Tangents.emplace_back(Tangent.x, Tangent.y, Tangent.z);
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
			BiTangents.reserve(VertexCount);

			if (VertexCount > 0)
			{
				XMFLOAT3 BiTangent{};

				for (UINT i = 0; i < VertexCount; ++i)
				{
					InFile >> BiTangent.x >> BiTangent.y >> BiTangent.z;
					BiTangents.emplace_back(BiTangent.x, BiTangent.y, BiTangent.z);
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
			TexCoords.reserve(VertexCount);

			if (VertexCount > 0)
			{
				XMFLOAT2 TexCoord{};

				for (UINT i = 0; i < VertexCount; ++i)
				{
					InFile >> TexCoord.x >> TexCoord.y;
					TexCoords.emplace_back(TexCoord.x, TexCoord.y);
				}

				UINT Stride{ sizeof(XMFLOAT2) };

				m_D3D12TexCoordBuffer = DX::CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, TexCoords.data(), Stride * VertexCount, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_D3D12TexCoordUploadBuffer.GetAddressOf());
				m_D3D12TexCoordBufferView.BufferLocation = m_D3D12TexCoordBuffer->GetGPUVirtualAddress();
				m_D3D12TexCoordBufferView.StrideInBytes = Stride;
				m_D3D12TexCoordBufferView.SizeInBytes = Stride * VertexCount;
			}
		}
		else if (!Token.compare(TEXT("<Indices>")))
		{
			InFile >> m_IndexCount;
			Indices.reserve(m_IndexCount);

			if (m_IndexCount > 0)
			{
				UINT Index{};

				for (UINT i = 0; i < m_IndexCount; ++i)
				{
					InFile >> Index;
					Indices.emplace_back(Index);
				}

				UINT Stride{ sizeof(UINT) };

				m_D3D12IndexBuffer = DX::CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, Indices.data(), Stride * m_IndexCount, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, m_D3D12IndexUploadBuffer.GetAddressOf());
				m_D3D12IndexBufferView.BufferLocation = m_D3D12IndexBuffer->GetGPUVirtualAddress();
				m_D3D12IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
				m_D3D12IndexBufferView.SizeInBytes = Stride * m_IndexCount;
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

	if (m_D3D12IndexUploadBuffer)
	{
		m_D3D12IndexUploadBuffer.ReleaseAndGetAddressOf();
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
