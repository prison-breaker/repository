#pragma once
#include "State.h"

template<typename EntityType>
class CStateMachine
{
private:
	shared_ptr<EntityType> m_Owner{};
							
	CState<EntityType>*	   m_CurrentState{};
	CState<EntityType>*	   m_PreviousState{};

public:
	CStateMachine(const shared_ptr<EntityType>& Owner) :
		m_Owner{ Owner }
	{

	}

	~CStateMachine() = default;

	void SetOwner(shared_ptr<EntityType> Owner)
	{
		m_Owner = Owner;
	}

	void SetCurrentState(CState<EntityType>* State)
	{
		if (State)
		{
			m_CurrentState = State;
			m_CurrentState->Enter(m_Owner);
		}
	}

	const CState<EntityType>* GetCurrentState() const
	{
		return m_CurrentState;
	}

	void ChangeState(CState<EntityType>* NewState)
	{
		if (NewState && m_CurrentState != NewState)
		{
			m_CurrentState->Exit(m_Owner);
			m_PreviousState = m_CurrentState;
			m_CurrentState = NewState;
			m_CurrentState->Enter(m_Owner);
		}
	}

	bool IsInState(CState<EntityType>* State) const
	{
		return typeid(*m_CurrentState) == typeid(*State);
	}

	void ProcessInput(UINT InputMask, float ElapsedTime)
	{
		if (m_CurrentState)
		{
			m_CurrentState->ProcessInput(m_Owner, InputMask, ElapsedTime);
		}
	}

	void Update(float ElapsedTime)
	{
		if (m_CurrentState)
		{
			m_CurrentState->Update(m_Owner, ElapsedTime);
		}
	}
};
