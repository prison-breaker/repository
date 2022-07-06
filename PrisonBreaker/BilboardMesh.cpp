#include "stdafx.h"
#include "BilboardMesh.h"

CBilboardMesh::CBilboardMesh(const XMFLOAT3& Position, const XMFLOAT2& Size) :
	m_Position{ Position },
	m_Size{ Size }
{

}

void CBilboardMesh::SetPosition(const XMFLOAT3& Position)
{
	m_Position = Position;
}

const XMFLOAT3& CBilboardMesh::GetPosition() const
{
	return m_Position;
}

void CBilboardMesh::SetSize(const XMFLOAT2& Size)
{
	m_Size = Size;
}

const XMFLOAT2& CBilboardMesh::GetSize() const
{
	return m_Size;
}

void CBilboardMesh::SetAlphaColor(float AlphaColor)
{
	m_AlphaColor = AlphaColor;
}

float CBilboardMesh::GetAlphaColor() const
{
	return m_AlphaColor;
}

void CBilboardMesh::SetCellCount(const XMUINT2& CellCount)
{
	m_CellCount = CellCount;
}

const XMUINT2& CBilboardMesh::GetCellCount() const
{
	return m_CellCount;
}

void CBilboardMesh::SetCellIndex(float CellIndex)
{
	m_CellIndex = CellIndex;
}

float CBilboardMesh::GetCellIndex() const
{
	return m_CellIndex;
}
