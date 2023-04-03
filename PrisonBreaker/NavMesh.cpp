#include "pch.h"
#include "NavMesh.h"

#include "AssetManager.h"

#include "NavNode.h"

CNavMesh::CNavMesh() :
	m_navNodes()
{
}

CNavMesh::~CNavMesh()
{
	Utility::SafeDelete(m_navNodes);
}

const vector<CNavNode*>& CNavMesh::GetNavNodes()
{
	return m_navNodes;
}

int CNavMesh::GetNodeIndex(const XMFLOAT3& position)
{
	for (int i = 0; i < m_navNodes.size(); ++i)
	{
		if (Math::IsInTriangle(m_navNodes[i]->m_triangle.m_vertices[0], m_navNodes[i]->m_triangle.m_vertices[1], m_navNodes[i]->m_triangle.m_vertices[2], position))
		{
			return i;
		}
	}

	// position�� navMesh ���� ���� ��쿡�� ���� ������ navNode�� �ε����� ��ȯ�Ѵ�.
	float nearestDist = FLT_MAX, dist = 0.0f;
	int index = 0;

	for (int i = 0; i < m_navNodes.size(); ++i)
	{
		dist = Math::Distance(m_navNodes[i]->GetTriangle().m_centroid, position);

		if (dist < nearestDist)
		{
			nearestDist = dist;
			index = i;
		}
	}

	return index;
}

void CNavMesh::Load(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList, const string& fileName)
{
	string filePath = CAssetManager::GetInstance()->GetAssetPath() + "Mesh\\" + fileName;
	ifstream in(filePath, ios::binary);
	string str;

	while (true)
	{
		File::ReadStringFromFile(in, str);

		if (str == "<NavMesh>")
		{
			cout << fileName << " �ε� ����..." << endl;
			m_name = "NavMesh";
		}
		else if (str == "<Positions>")
		{
			int vertexCount = 0;
			
			in.read(reinterpret_cast<char*>(&vertexCount), sizeof(int));

			if (vertexCount > 0)
			{
				m_positions.resize(vertexCount);
				in.read(reinterpret_cast<char*>(m_positions.data()), sizeof(XMFLOAT3) * vertexCount);

				m_d3d12PositionBuffer = DX::CreateBufferResource(d3d12Device, d3d12GraphicsCommandList, m_positions.data(), sizeof(XMFLOAT3) * vertexCount, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_d3d12PositionUploadBuffer.GetAddressOf());
				m_d3d12PositionBufferView.BufferLocation = m_d3d12PositionBuffer->GetGPUVirtualAddress();
				m_d3d12PositionBufferView.StrideInBytes = sizeof(XMFLOAT3);
				m_d3d12PositionBufferView.SizeInBytes = sizeof(XMFLOAT3) * vertexCount;
			}
		}
		else if (str == "<Indices>")
		{
			int indexCount = 0;

			in.read(reinterpret_cast<char*>(&indexCount), sizeof(int));

			if (indexCount > 0)
			{
				m_indices.resize(1);
				m_indices[0].resize(indexCount);
				m_d3d12IndexBuffers.resize(1);
				m_d3d12IndexUploadBuffers.resize(1);
				m_d3d12IndexBufferViews.resize(1);

				in.read(reinterpret_cast<char*>(m_indices[0].data()), sizeof(int) * indexCount);
				transform(m_indices[0].begin(), m_indices[0].end(), m_indices[0].begin(), [](UINT i) { return i - 1; });

				m_d3d12IndexBuffers[0] = DX::CreateBufferResource(d3d12Device, d3d12GraphicsCommandList, m_indices[0].data(), sizeof(int) * indexCount, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, m_d3d12IndexUploadBuffers[0].GetAddressOf());
				m_d3d12IndexBufferViews[0].BufferLocation = m_d3d12IndexBuffers[0]->GetGPUVirtualAddress();
				m_d3d12IndexBufferViews[0].Format = DXGI_FORMAT_R32_UINT;
				m_d3d12IndexBufferViews[0].SizeInBytes = sizeof(int) * indexCount;

				for (int i = 0; i < indexCount; i += 3)
				{
					Triangle triangle = {};

					triangle.m_vertices[0] = m_positions[m_indices[0][i]];
					triangle.m_vertices[1] = m_positions[m_indices[0][i + 1]];
					triangle.m_vertices[2] = m_positions[m_indices[0][i + 2]];

					CNavNode* navNode = new CNavNode(triangle);

					InsertNode(navNode);
				}
			}
		}
		else if (str == "</NavMesh>")
		{
			break;
		}
	}

	cout << fileName << " �ε� �Ϸ�...(���� ��: " << m_navNodes.size() << ")\n\n";
}

void CNavMesh::InsertNode(CNavNode* newNavNode)
{
	// ���� ���� �̿��� ������ �ִٸ� �߰��Ѵ�.
	for (const auto& navNode : m_navNodes)
	{
		int neighborVertexCount = 0;

		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				if (Vector3::IsEqual(navNode->m_triangle.m_vertices[i], newNavNode->m_triangle.m_vertices[j]))
				{
					++neighborVertexCount;
					break;
				}
			}
		}

		if (neighborVertexCount == 2)
		{
			navNode->m_nearNodes.push_back(newNavNode);
			newNavNode->m_nearNodes.push_back(navNode);
		}
	}

	m_navNodes.push_back(newNavNode);
}

bool CNavMesh::IsInNavMesh(const XMFLOAT3& position, XMFLOAT3& newPosition)
{
	if (IsInPolygon(newPosition))
	{
		return true;
	}

	// position�� ������ ���� ���ٸ�, �����̵� ���͸� ���� ���� ���� �����Ѵ�.
	FixPosition(position, newPosition);

	if (IsInPolygon(newPosition))
	{
		return true;
	}

	// ������ ���� ���� ������ ���� ���ٸ�, false�� ��ȯ�Ѵ�.
	return false;
}

bool CNavMesh::IsInPolygon(XMFLOAT3& position)
{
	for (const auto& navNode : m_navNodes)
	{
		const Triangle& triangle = navNode->GetTriangle();

		if (Math::IsInTriangle(triangle.m_vertices[0], triangle.m_vertices[1], triangle.m_vertices[2], position))
		{
			// ������ ���� �ִٸ�, position�� y���� �ﰢ�� �����߽��� y������ �������ش�.
			position.y = triangle.m_centroid.y;

			return true;
		}
	}

	return false;
}

void CNavMesh::FixPosition(const XMFLOAT3& position, XMFLOAT3& newPosition)
{
	CNavNode* navNode = m_navNodes[GetNodeIndex(position)];

	XMFLOAT3 slidingVector = {};
	XMFLOAT3 shift = Vector3::Subtract(newPosition, position);
	XMFLOAT3 vertices[3] = { navNode->GetTriangle().m_vertices[0], navNode->GetTriangle().m_vertices[1], navNode->GetTriangle().m_vertices[2] };
	
	// �ﰢ���� ���� ��, �����ϴ� ��
	XMFLOAT3 crossedEdge = {};

	// �ش� ���� ������ ��������
	XMFLOAT3 contactNormal = {};

	XMFLOAT3 worldUp = XMFLOAT3(0.0f, 1.0f, 0.0f);

	for (int i = 0; i < 3; ++i)
	{
		int index1 = i;
		int index2 = (i + 1) % 3;

		if (Math::LineIntersection(vertices[index1], vertices[index2], position, newPosition))
		{
			crossedEdge = Vector3::Subtract(vertices[index1], vertices[index2]);
			contactNormal = Vector3::Normalize(Vector3::TransformNormal(crossedEdge, Matrix4x4::RotationAxis(worldUp, 90.0f)));
			slidingVector = Vector3::Subtract(shift, Vector3::ScalarProduct(Vector3::DotProduct(shift, contactNormal), contactNormal, false));
			newPosition = Vector3::Add(position, slidingVector);
			return;
		}
	}
}
