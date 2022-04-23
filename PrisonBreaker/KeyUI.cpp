#include "stdafx.h"
#include "KeyUI.h"

void CKeyUI::Initialize()
{
	m_StateMachine = make_shared<CStateMachine<CKeyUI>>(static_pointer_cast<CKeyUI>(shared_from_this()));
}

void CKeyUI::Animate(const vector<vector<shared_ptr<CGameObject>>>& GameObjects, const shared_ptr<CNavMesh>& NavMesh, float ElapsedTime)
{
	if (m_StateMachine)
	{
		m_StateMachine->Update(GameObjects, NavMesh, ElapsedTime);
	}
}

CStateMachine<CKeyUI>* CKeyUI::GetStateMachine() const
{
	return m_StateMachine.get();
}
