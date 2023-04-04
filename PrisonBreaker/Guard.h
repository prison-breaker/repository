#pragma once
#include "Character.h"

class CGuard : public CCharacter
{
private:
	const float		 m_idleEntryTime;
	const float		 m_targetUpdateTime;
	float			 m_elapsedTime;

	vector<XMFLOAT3> m_movePath;
	vector<XMFLOAT3> m_patrolPath;
	int			     m_patrolIndex;

	CCharacter*      m_target;
	//CEventTrigger* m_eventTrigger;

public:
	CGuard();
	virtual ~CGuard();

	float GetIdleEntryTime();
	float GetTargetUpdateTime();

	void SetElapsedTime(float elapsedTime);
	float GetElapsedTime();

	bool IsFinishedMovePath();

	const XMFLOAT3& GetNextPatrolPosition();

	void SetTarget(CCharacter* target);
	CCharacter* GetTarget();

	virtual void Init();

	virtual void OnCollisionEnter(CObject* collidedObject);

	CCharacter* FindTarget(float maxDist, float fov);

	void CreateMovePath(const XMFLOAT3& targetPosition);
	void CreatePatrolPath(const XMFLOAT3& targetPosition);

	void FollowMovePath(float force);
	void FollowPatrolPath(float force);

private:
	void CreatePath(vector<XMFLOAT3>& path, const XMFLOAT3& targetPosition);
	void OptimizePath(vector<XMFLOAT3>& path);

	//void FindNavPath(const shared_ptr<CNavMesh>& NavMesh, const XMFLOAT3& TargetPosition, const vector<vector<shared_ptr<CObject>>>& GameObjects);
	//void FindRayCastingNavPath(const vector<vector<shared_ptr<CObject>>>& GameObjects);

	//void MoveToNavPath(float ElapsedTime);
	//void GenerateTrigger();
};
