#pragma once
#include "State.h"

class CMissionUI;

class CMissionUIShowingState : public CState<CMissionUI>
{
private:
	CMissionUIShowingState() = default;
	virtual ~CMissionUIShowingState() = default;

public:
	static CMissionUIShowingState* GetInstance();

	virtual void Enter(const shared_ptr<CMissionUI>& Entity);
	virtual void ProcessInput(const shared_ptr<CMissionUI>& Entity, const vector<vector<shared_ptr<CGameObject>>>& GameObjects, const shared_ptr<CNavMesh>& NavMesh, float ElapsedTime, UINT InputMask);
	virtual void Update(const shared_ptr<CMissionUI>& Entity, const vector<vector<shared_ptr<CGameObject>>>& GameObjects, const shared_ptr<CNavMesh>& NavMesh, float ElapsedTime);
	virtual void Exit(const shared_ptr<CMissionUI>& Entity);
};

//=========================================================================================================================

class CMissionUIHidingState : public CState<CMissionUI>
{
private:
	CMissionUIHidingState() = default;
	virtual ~CMissionUIHidingState() = default;

public:
	static CMissionUIHidingState* GetInstance();

	virtual void Enter(const shared_ptr<CMissionUI>& Entity);
	virtual void ProcessInput(const shared_ptr<CMissionUI>& Entity, const vector<vector<shared_ptr<CGameObject>>>& GameObjects, const shared_ptr<CNavMesh>& NavMesh, float ElapsedTime, UINT InputMask);
	virtual void Update(const shared_ptr<CMissionUI>& Entity, const vector<vector<shared_ptr<CGameObject>>>& GameObjects, const shared_ptr<CNavMesh>& NavMesh, float ElapsedTime);
	virtual void Exit(const shared_ptr<CMissionUI>& Entity);
};
