#pragma once

class CGameObject;
class CNavMesh;

template<typename EntityType>
class CState
{
public:
	CState() = default;
	virtual ~CState() = default;

	virtual void Enter(const shared_ptr<EntityType>& Entity) = 0;
	virtual void ProcessInput(const shared_ptr<EntityType>& Entity, const vector<vector<shared_ptr<CGameObject>>>& GameObjects, const shared_ptr<CNavMesh>& NavMesh, float ElapsedTime, UINT InputMask) = 0;
	virtual void Update(const shared_ptr<EntityType>& Entity, const vector<vector<shared_ptr<CGameObject>>>& GameObjects, const shared_ptr<CNavMesh>& NavMesh, float ElapsedTime) = 0;
	virtual void Exit(const shared_ptr<EntityType>& Entity) = 0;
};
