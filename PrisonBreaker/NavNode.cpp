#include "pch.h"
#include "NavNode.h"

CNavNode::CNavNode(const Triangle& triangle) :
	m_isVisited(),
	m_triangle(triangle),
	m_f(),
	m_h(),
	m_g(),
	m_nearNodes(),
	m_parent()
{
	m_triangle.m_centerSides[0] = { 0.5f * (m_triangle.m_vertices[0].x + m_triangle.m_vertices[1].x), 0.5f * (m_triangle.m_vertices[0].y + m_triangle.m_vertices[1].y), 0.5f * (m_triangle.m_vertices[0].z + m_triangle.m_vertices[1].z) };
	m_triangle.m_centerSides[1] = { 0.5f * (m_triangle.m_vertices[1].x + m_triangle.m_vertices[2].x), 0.5f * (m_triangle.m_vertices[1].y + m_triangle.m_vertices[2].y), 0.5f * (m_triangle.m_vertices[1].z + m_triangle.m_vertices[2].z) };
	m_triangle.m_centerSides[2] = { 0.5f * (m_triangle.m_vertices[2].x + m_triangle.m_vertices[0].x), 0.5f * (m_triangle.m_vertices[2].y + m_triangle.m_vertices[0].y), 0.5f * (m_triangle.m_vertices[2].z + m_triangle.m_vertices[0].z) };
	m_triangle.m_centroid = { (m_triangle.m_vertices[0].x + m_triangle.m_vertices[1].x + m_triangle.m_vertices[2].x) / 3, (m_triangle.m_vertices[0].y + m_triangle.m_vertices[1].y + m_triangle.m_vertices[2].y) / 3, (m_triangle.m_vertices[0].z + m_triangle.m_vertices[1].z + m_triangle.m_vertices[2].z) / 3 };
}

CNavNode::~CNavNode()
{
}

void CNavNode::SetVisited(bool isVisited)
{
	m_isVisited = isVisited;
}

bool CNavNode::IsVisited()
{
	return m_isVisited;
}

const Triangle& CNavNode::GetTriangle()
{
	return m_triangle;
}

void CNavNode::CalculateH(CNavNode* targetNode)
{
	m_h = Math::Distance(m_triangle.m_centroid, targetNode->m_triangle.m_centroid);
}

float CNavNode::GetH()
{
	return m_h;
}

void CNavNode::CalculateG(CNavNode* parentNode)
{
	m_g = parentNode->GetG() + Math::Distance(m_triangle.m_centroid, parentNode->m_triangle.m_centroid);
}

float CNavNode::GetG()
{
	return m_g;
}

void CNavNode::CalculateF(CNavNode* targetNode, CNavNode* parentNode)
{
	CalculateH(targetNode);
	CalculateG(parentNode);

	m_f = m_h + m_g;
}

float CNavNode::GetF()
{
	return m_f;
}

const vector<CNavNode*>& CNavNode::GetNearNodes()
{
	return m_nearNodes;
}

void CNavNode::SetParent(CNavNode* node)
{
	if (node != nullptr)
	{
		m_parent = node;
	}
}

CNavNode* CNavNode::GetParent()
{
	return m_parent;
}

int CNavNode::CalculateNearSideIndex(CNavNode* node)
{
	int totalNearSideIndex = 0;

	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			if (Vector3::IsEqual(m_triangle.m_vertices[i], node->m_triangle.m_vertices[j]))
			{
				totalNearSideIndex += i;
				break;
			}
		}
	}

	switch (totalNearSideIndex)
	{
	case 1:
		return 0;
	case 2:
		return 2;
	case 3:
		return 1;
	}

	return 4;
}

void CNavNode::Reset()
{
	m_isVisited = false;
	m_f = m_h = m_g = 0.0f;
	m_parent = nullptr;
}
