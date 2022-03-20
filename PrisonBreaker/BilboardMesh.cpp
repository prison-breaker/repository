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
