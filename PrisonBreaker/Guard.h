#pragma once
#include "GameObject.h"

class CGuard : public CGameObject
{
private:
	shared_ptr<CCamera>               m_Camera{};

	shared_ptr<CStateMachine<CGuard>> m_StateMachine{};

public:
	CGuard() = default;
	virtual ~CGuard() = default;
};
