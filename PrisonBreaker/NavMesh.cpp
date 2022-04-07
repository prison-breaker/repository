#include "stdafx.h"
#include "NavMesh.h"

void CNavMesh::LoadNavNodeFromFile(const tstring& FileName)
{
	tifstream InFile{ FileName };
	tstring Token{};

	vector<XMFLOAT3> Vertices{};

	while (InFile >> Token)
	{
		shared_ptr<CNavNode> NewNavNode{ make_shared<CNavNode>() };

		if (Token == TEXT("v"))
		{
			XMFLOAT3 Vertex{};

			InFile >> Vertex.x >> Vertex.y >> Vertex.z;

			Vertices.push_back(Vertex);
		}
		else if (Token == TEXT("f"))
		{
			TRIANGLE Triangle{};

			for (UINT i = 0; i < 3; ++i)
			{
				InFile >> Token;

				size_t Location{ Token.find('/') };

				Token = Token.substr(0, Location);

				UINT Index{ static_cast<UINT>(stoi(Token) - 1) };

				Triangle.m_Vertices[i] = Vertices[Index];
			}

			NewNavNode->SetTriangle(Triangle);
			InsertNode(NewNavNode);
		}
	}

	tcout << TEXT("성공적으로 ") << m_NavNodes.size() << TEXT("개의 삼각형을 읽어 왔습니다!") << endl;
}

vector<shared_ptr<CNavNode>>& CNavMesh::GetNavNodes()
{
	return m_NavNodes;
}

void CNavMesh::InsertNode(const shared_ptr<CNavNode>& NewNavNode)
{
	if (m_NavNodes.empty())
	{
		m_NavNodes.push_back(NewNavNode);
	}
	else
	{
		// 기존의 노드와 이웃한 꼭짓점이 있다면 추가한다.
		for (const auto& NavNode : m_NavNodes)
		{
			UINT NeighborVertexCount{};

			for (UINT i = 0; i < 3; ++i)
			{
				XMFLOAT3 Vertex1{ NavNode->GetTriangle().m_Vertices[i] };

				for (UINT j = 0; j < 3; ++j)
				{
					XMFLOAT3 Vertex2{ NewNavNode->GetTriangle().m_Vertices[j] };

					if (Vector3::IsEqual(Vertex1, Vertex2))
					{
						NeighborVertexCount += 1;
						continue;
					}
				}
			}

			if (NeighborVertexCount == 2)
			{
				NavNode->m_NeighborNavNodes.push_back(NewNavNode);
				NewNavNode->m_NeighborNavNodes.push_back(NavNode);
			}
		}

		m_NavNodes.push_back(NewNavNode);
	}
}

UINT CNavMesh::GetNodeIndex(const XMFLOAT3& Position)
{
	UINT NavNodeSize{ static_cast<UINT>(m_NavNodes.size()) };

	for (UINT i = 0; i < NavNodeSize; ++i)
	{
		if (Math::IsInTriangle(m_NavNodes[i]->m_Triangle.m_Vertices[0], m_NavNodes[i]->m_Triangle.m_Vertices[1], m_NavNodes[i]->m_Triangle.m_Vertices[2], Position))
		{
			return i;
		}
	}

	return UINT_MAX;
}
