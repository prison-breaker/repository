#pragma once

struct Triangle
{
	XMFLOAT3 m_vertices[3];    // ������ ��ġ
	XMFLOAT3 m_centerSides[3]; // �� ������ �մ� ���� ����
	XMFLOAT3 m_centroid;       // �ﰢ���� �����߽�
};

class CNavNode
{
	friend class CNavMesh;

private:
	bool			  m_isVisited;
					  
	Triangle		  m_triangle;
					  
	float			  m_f;
	float             m_h;
	float             m_g;

	vector<CNavNode*> m_nearNodes;
	CNavNode*		  m_parent;

private:
	// �� ��ü�� ������ ������ CNavMesh�� ���ؼ��� �Ͼ��.
	CNavNode(const Triangle& triangle);

public:
	// �Ҹ����� ��쿡�� SafeDelete �ܺ� �Լ��� �̿��ϱ� ������ ���� �����ڸ� public���� �����Ͽ���.
	~CNavNode();

	void SetVisited(bool isVisited);
	bool IsVisited();

	const Triangle& GetTriangle();

	void CalculateH(CNavNode* targetNode);
	float GetH();

	void CalculateG(CNavNode* parentNode);
	float GetG();

	void CalculateF(CNavNode* targetNode, CNavNode* parentNode);
	float GetF();

	const vector<CNavNode*>& GetNearNodes();

	void SetParent(CNavNode* node);
	CNavNode* GetParent();

	int CalculateNearSideIndex(CNavNode* node);

	void Reset();
};

struct compare
{
	bool operator ()(CNavNode* navNode1, CNavNode* navNode2)
	{
		return navNode1->GetF() > navNode2->GetF();
	}
};
