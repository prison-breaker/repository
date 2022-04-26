#include "stdafx.h"
#include "HitUI.h"

void CHitUI::Initialize()
{
	m_StateMachine = make_shared<CStateMachine<CHitUI>>(static_pointer_cast<CHitUI>(shared_from_this()));
	m_StateMachine->SetCurrentState(CHitUIActivationState::GetInstance());

	// 일단은 애니메이션이 실행되지 않도록 한다.
	// SetActive() 함수를 호출하는 이유는 State의 Enter에서 매 번 true로 변하기 때문이다.
	SetActive(false);
	m_UIAnimationController->SetActive(false);
}

void CHitUI::Animate(float ElapsedTime)
{
	if (IsActive())
	{
		if (m_StateMachine)
		{
			m_StateMachine->Update(ElapsedTime);
		}
	}
}

CStateMachine<CHitUI>* CHitUI::GetStateMachine() const
{
	return m_StateMachine.get();
}
