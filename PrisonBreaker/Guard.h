#pragma once
#include "GameObject.h"

class CGuard : public CGameObject
{
private:
	UINT							  m_Health{};

	float							  m_Speed{};

	shared_ptr<CCamera>               m_Camera{};

	shared_ptr<CStateMachine<CGuard>> m_StateMachine{};

	vector<XMFLOAT3>                  m_NavPath{};

public:
	CGuard() = default;
	virtual ~CGuard() = default;

	virtual void Initialize();

	virtual void Animate(float ElapsedTime);

	void SetHealth(UINT Health);
	UINT GetHealth() const;

	void SetSpeed(float Speed);
	float GetSpeed() const;

	CCamera* GetCamera() const;

	CStateMachine<CGuard>* GetStateMachine() const;

	void FindPath(const shared_ptr<CNavMesh>& NavMesh, const XMFLOAT3& TargetPosition);
	void MoveToNavPath(float ElapsedTime);
};
