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

	bool							  m_RecentTransition{};

	float							  m_ElapsedTime{};
	const float						  m_ToIdleEntryTime{ rand() % 5 + 3.0f };
	const float						  m_UpdateTargetTime{ 4.0f };

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

	void SetRecentTransition(bool RecentTransition);
	bool GetRecentTransition() const;

	void SetElapsedTime(float ElapsedTime);
	float GetElapsedTime() const;

	float GetToIdleEntryTime() const;
	float GetUpdateTargetTime() const;

	void SetTargetPosition(const XMFLOAT3& TargetPosition);
	const XMFLOAT3& GetTargetPosition() const;

	vector<XMFLOAT3>& GetNavPath();
	vector<XMFLOAT3>& GetPatrolNavPath();

	UINT GetPatrolIndex() const;

	bool IsFoundPlayer(const XMFLOAT3& Position);

	void FindNavPath(const shared_ptr<CNavMesh>& NavMesh, const XMFLOAT3& TargetPosition, const vector<vector<shared_ptr<CGameObject>>>& GameObjects);
	void FindRayCastingNavPath(const vector<vector<shared_ptr<CGameObject>>>& GameObjects);

	void FindPatrolNavPath(const shared_ptr<CNavMesh>& NavMesh);

	void MoveToNavPath(float ElapsedTime);
	void Patrol(float ElapsedTime);
};
