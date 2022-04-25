#pragma once
#include "State.h"

class CKeyUI;

class CKeyUIActivationState : public CState<CKeyUI>
{
public:
	CKeyUIActivationState() = default;
	virtual ~CKeyUIActivationState() = default;

	static CKeyUIActivationState* GetInstance();

	virtual void Enter(const shared_ptr<CKeyUI>& Entity);
	virtual void ProcessInput(const shared_ptr<CKeyUI>& Entity, float ElapsedTime, UINT InputMask);
	virtual void Update(const shared_ptr<CKeyUI>& Entity, float ElapsedTime);
	virtual void Exit(const shared_ptr<CKeyUI>& Entity);
};
