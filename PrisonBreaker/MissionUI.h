#pragma once
#include "BilboardObject.h"

class CMissionUI : public CBilboardObject
{
private:
	shared_ptr<CStateMachine<CMissionUI>> m_StateMachine{};

public:
	CMissionUI() = default;
	virtual ~CMissionUI() = default;

	virtual void Initialize();

	virtual void Animate(float ElapsedTime);

	CStateMachine<CMissionUI>* GetStateMachine() const;
};
