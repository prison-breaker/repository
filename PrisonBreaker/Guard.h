#pragma once
#include "GameObject.h"
#include "State_Guard.h"

class CGuard : public CGameObject
{
private:
	UINT							  m_Health{};

	float							  m_Speed{};
	XMFLOAT3						  m_MovingDirection{};

	shared_ptr<CStateMachine<CGuard>> m_StateMachine{};

	XMFLOAT3					      m_TargetPosition{};
	vector<XMFLOAT3>                  m_NavPath{};
	vector<XMFLOAT3>				  m_PatrolNavPath{};
	UINT							  m_PatrolIndex{};

public:
	CGuard() = default;
	virtual ~CGuard() = default;

	virtual void Initialize();

	virtual void Animate(float ElapsedTime);

	void SetHealth(UINT Health);
	UINT GetHealth() const;

	void SetSpeed(float Speed);
	float GetSpeed() const;

	void SetMovingDirection(const XMFLOAT3& MovingDirection);
	const XMFLOAT3& GetMovingDirection() const;

	CStateMachine<CGuard>* GetStateMachine() const;

	void SetTargetPosition(const XMFLOAT3& TargetPosition);
	const XMFLOAT3& GetTargetPosition() const;

	bool IsFoundPlayer(const XMFLOAT3& Position);

	void FindNavPath(const shared_ptr<CNavMesh>& NavMesh, const vector<vector<shared_ptr<CGameObject>>>& GameObjects);
	void FindRayCastingNavPath(const vector<vector<shared_ptr<CGameObject>>>& GameObjects);

	void FindPatrolNavPath(const shared_ptr<CNavMesh>& NavMesh);

	void MoveToNavPath(float ElapsedTime);
	void Patrol(float ElapsedTime);
};
