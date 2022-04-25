#pragma once
#include "BilboardObject.h"

class CKeyUI : public CBilboardObject
{
private:
	shared_ptr<CStateMachine<CKeyUI>> m_StateMachine{};

public:
	CKeyUI() = default;
	virtual ~CKeyUI() = default;

	virtual void Initialize();

	virtual void Animate(float ElapsedTime);

	CStateMachine<CKeyUI>* GetStateMachine() const;
};
