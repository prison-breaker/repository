#include "stdafx.h"
#include "HitUI.h"

void CHitUI::Initialize()
{
	m_StateMachine = make_shared<CStateMachine<CHitUI>>(static_pointer_cast<CHitUI>(shared_from_this()));
	m_StateMachine->SetCurrentState(CHitUIActivationState::GetInstance());

	// �ϴ��� �ִϸ��̼��� ������� �ʵ��� �Ѵ�.
	// SetActive() �Լ��� ȣ���ϴ� ������ State�� Enter���� �� �� true�� ���ϱ� �����̴�.
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
