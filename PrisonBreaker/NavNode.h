#pragma once

struct Triangle
{
	XMFLOAT3 m_vertices[3];    // 정점의 위치
	XMFLOAT3 m_centerSides[3]; // 각 정점을 잇는 변의 중점
	XMFLOAT3 m_centroid;       // 삼각형의 무게중심
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
	// 이 객체의 생성은 오로지 CNavMesh에 의해서만 일어난다.
	CNavNode(const Triangle& triangle);

public:
	// 소멸자의 경우에는 SafeDelete 외부 함수를 이용하기 때문에 접근 지정자를 public으로 설정하였다.
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
