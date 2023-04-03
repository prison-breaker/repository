#pragma once
#include "Mesh.h"

class CNavNode;

class CNavMesh : public CMesh
{
	friend class CAssetManager;

private:
	vector<CNavNode*> m_navNodes;

private:
	// 이 객체의 생성은 오로지 CAssetManager에 의해서만 일어난다.
	CNavMesh();

	void InsertNode(CNavNode* newNavNode);

	bool IsInPolygon(XMFLOAT3& position);
	void FixPosition(const XMFLOAT3& position, XMFLOAT3& newPosition);

public:
	// 소멸자의 경우에는 SafeDelete 외부 함수를 이용하기 때문에 접근 지정자를 public으로 설정하였다.
	~CNavMesh();

	const vector<CNavNode*>& GetNavNodes();
	int GetNodeIndex(const XMFLOAT3& position);

	void Load(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList, const string& fileName);

	bool IsInNavMesh(const XMFLOAT3& position, XMFLOAT3& newPosition);
};
