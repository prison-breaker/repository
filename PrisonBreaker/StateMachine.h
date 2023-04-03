#pragma once
#include "Component.h"

class CState;

class CStateMachine : public CComponent
{
	friend class CObject;

private:
	CState* m_previousState;
	CState* m_currentState;

private:
	CStateMachine();
	virtual ~CStateMachine();

public:
	CState* GetPreviousState();

	void SetCurrentState(CState* state);
	CState* GetCurrentState();

	void ChangeState(CState* state);

	virtual void Update();
};
