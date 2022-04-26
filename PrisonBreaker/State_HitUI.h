#pragma once
#include "State.h"

class CHitUI;

class CHitUIActivationState : public CState<CHitUI>
{
public:
	CHitUIActivationState() = default;
	virtual ~CHitUIActivationState() = default;

	static CHitUIActivationState* GetInstance();

	virtual void Enter(const shared_ptr<CHitUI>& Entity);
	virtual void ProcessInput(const shared_ptr<CHitUI>& Entity, float ElapsedTime, UINT InputMask);
	virtual void Update(const shared_ptr<CHitUI>& Entity, float ElapsedTime);
	virtual void Exit(const shared_ptr<CHitUI>& Entity);
};
