#include "stdafx.h"
#include "KeyUI.h"

void CKeyUI::Initialize()
{
	m_StateMachine = make_shared<CStateMachine<CKeyUI>>(static_pointer_cast<CKeyUI>(shared_from_this()));
	m_StateMachine->SetCurrentState(CKeyUIActivationState::GetInstance());

	// 일단은 애니메이션이 실행되지 않도록 한다.
	m_UIAnimationController->SetActive(false);
}

void CKeyUI::Animate(float ElapsedTime)
{
	if (IsActive())
	{
		if (m_StateMachine)
		{
			m_StateMachine->Update(ElapsedTime);
		}
	}
}

CStateMachine<CKeyUI>* CKeyUI::GetStateMachine() const
{
	return m_StateMachine.get();
}
