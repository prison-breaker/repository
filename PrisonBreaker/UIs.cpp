#include "stdafx.h"
#include "UIs.h"

void CBackgroundUI::Initialize()
{
	m_StateMachine = make_shared<CStateMachine<CBackgroundUI>>(static_pointer_cast<CBackgroundUI>(shared_from_this()));
	m_StateMachine->SetCurrentState(CBackgroundUIFadeState::GetInstance());
}

void CBackgroundUI::Animate(float ElapsedTime)
{
	if (IsActive())
	{
		if (m_StateMachine)
		{
			m_StateMachine->Update(ElapsedTime);
		}
	}
}

shared_ptr<CStateMachine<CBackgroundUI>> CBackgroundUI::GetStateMachine() const
{
	return m_StateMachine;
}

//=========================================================================================================================

void CMissionUI::Initialize()
{
	m_StateMachine = make_shared<CStateMachine<CMissionUI>>(static_pointer_cast<CMissionUI>(shared_from_this()));
	m_StateMachine->SetCurrentState(CMissionUIShowingState::GetInstance());
}

void CMissionUI::Animate(float ElapsedTime)
{
	if (IsActive())
	{
		if (m_StateMachine)
		{
			m_StateMachine->Update(ElapsedTime);
		}
	}
}

shared_ptr<CStateMachine<CMissionUI>> CMissionUI::GetStateMachine() const
{
	return m_StateMachine;
}

//=========================================================================================================================

void CKeyUI::Initialize()
{
	m_StateMachine = make_shared<CStateMachine<CKeyUI>>(static_pointer_cast<CKeyUI>(shared_from_this()));
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

shared_ptr<CStateMachine<CKeyUI>> CKeyUI::GetStateMachine() const
{
	return m_StateMachine;
}

//=========================================================================================================================

void CHitUI::Initialize()
{
	m_StateMachine = make_shared<CStateMachine<CHitUI>>(static_pointer_cast<CHitUI>(shared_from_this()));
	m_StateMachine->SetCurrentState(CHitUIActivationState::GetInstance());

	// 일단은 애니메이션이 실행되지 않도록 한다.
	// SetActive() 함수를 호출하는 이유는 State의 Enter에서 매 번 true로 변하기 때문이다.
	SetActive(false);
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

shared_ptr<CStateMachine<CHitUI>> CHitUI::GetStateMachine() const
{
	return m_StateMachine;
}
