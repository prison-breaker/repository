#pragma once
#include "BilboardObject.h"
#include "State_HitUI.h"

class CHitUI : public CBilboardObject
{
private:
	shared_ptr<CStateMachine<CHitUI>> m_StateMachine{};

public:
	CHitUI() = default;
	virtual ~CHitUI() = default;

	virtual void Initialize();

	virtual void Animate(float ElapsedTime);

	CStateMachine<CHitUI>* GetStateMachine() const;
};
