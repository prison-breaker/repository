#include "stdafx.h"
#include "BilboardMesh.h"

CBilboardMesh::CBilboardMesh(const XMFLOAT3& Position, const XMFLOAT2& Size) :
	m_Position{ Position },
	m_Size{ Size }
{

}

CBilboardMesh::CBilboardMesh(const XMFLOAT3& Position, const XMFLOAT2& Size, const XMUINT2& CellCount, UINT CellIndex) :
	m_Position{ Position },
	m_Size{ Size },
	m_CellCount{ CellCount },
	m_CellIndex{ CellIndex }
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

void CBilboardMesh::SetCellIndex(UINT CellIndex)
{
	m_CellIndex = CellIndex;
}

UINT CBilboardMesh::GetCellIndex() const
{
	return m_CellIndex;
}
