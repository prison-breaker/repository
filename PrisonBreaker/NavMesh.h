#pragma once
#include "Mesh.h"

class CNavNode;

class CNavMesh : public CMesh
{
	friend class CAssetManager;

private:
	vector<CNavNode*> m_navNodes;

private:
	// �� ��ü�� ������ ������ CAssetManager�� ���ؼ��� �Ͼ��.
	CNavMesh();

	void InsertNode(CNavNode* newNavNode);

	bool IsInPolygon(XMFLOAT3& position);
	void FixPosition(const XMFLOAT3& position, XMFLOAT3& newPosition);

public:
	// �Ҹ����� ��쿡�� SafeDelete �ܺ� �Լ��� �̿��ϱ� ������ ���� �����ڸ� public���� �����Ͽ���.
	~CNavMesh();

	const vector<CNavNode*>& GetNavNodes();
	int GetNodeIndex(const XMFLOAT3& position);

	void Load(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList, const string& fileName);

	bool IsInNavMesh(const XMFLOAT3& position, XMFLOAT3& newPosition);
};
