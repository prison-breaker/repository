#pragma once
#include "State.h"

class CBilboardObject;

class CBilboardObjectShowingState : public CState<CBilboardObject>
{
private:
	CBilboardObjectShowingState() = default;
	virtual ~CBilboardObjectShowingState() = default;

public:
	static CBilboardObjectShowingState* GetInstance();

	virtual void Enter(const shared_ptr<CBilboardObject>& Entity);
	virtual void ProcessInput(const shared_ptr<CBilboardObject>& Entity, UINT InputMask, float ElapsedTime);
	virtual void Update(const shared_ptr<CBilboardObject>& Entity);
	virtual void Exit(const shared_ptr<CBilboardObject>& Entity);
};

//=========================================================================================================================

class CBilboardObjectHidingState : public CState<CBilboardObject>
{
private:
	CBilboardObjectHidingState() = default;
	virtual ~CBilboardObjectHidingState() = default;

public:
	static CBilboardObjectHidingState* GetInstance();

	virtual void Enter(const shared_ptr<CBilboardObject>& Entity);
	virtual void ProcessInput(const shared_ptr<CBilboardObject>& Entity, UINT InputMask, float ElapsedTime);
	virtual void Update(const shared_ptr<CBilboardObject>& Entity);
	virtual void Exit(const shared_ptr<CBilboardObject>& Entity);
};
