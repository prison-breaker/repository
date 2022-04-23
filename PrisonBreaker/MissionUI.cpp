#include "stdafx.h"
#include "MissionUI.h"
#include "State_MissionUI.h"

void CMissionUI::Initialize()
{
	m_StateMachine = make_shared<CStateMachine<CMissionUI>>(static_pointer_cast<CMissionUI>(shared_from_this()));
	m_StateMachine->SetCurrentState(CMissionUIShowingState::GetInstance());
}

void CMissionUI::Animate(const vector<vector<shared_ptr<CGameObject>>>& GameObjects, const shared_ptr<CNavMesh>& NavMesh, float ElapsedTime)
{
	if (m_StateMachine)
	{
		m_StateMachine->Update(GameObjects, NavMesh, ElapsedTime);
	}
}

CStateMachine<CMissionUI>* CMissionUI::GetStateMachine() const
{
	return m_StateMachine.get();
}
