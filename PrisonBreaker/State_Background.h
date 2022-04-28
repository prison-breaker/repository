#pragma once
#include "State.h"

class CBackgroundUI;

class CBackgroundUIFadeState : public CState<CBackgroundUI>
{
public:
	CBackgroundUIFadeState() = default;
	virtual ~CBackgroundUIFadeState() = default;

	static CBackgroundUIFadeState* GetInstance();

	virtual void Enter(const shared_ptr<CBackgroundUI>&Entity);
	virtual void ProcessInput(const shared_ptr<CBackgroundUI>&Entity, float ElapsedTime, UINT InputMask);
	virtual void Update(const shared_ptr<CBackgroundUI>&Entity, float ElapsedTime);
	virtual void Exit(const shared_ptr<CBackgroundUI>&Entity);
};
