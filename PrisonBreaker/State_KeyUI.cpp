#include "stdafx.h"
#include "State_KeyUI.h"
#include "KeyUI.h"

CKeyUIActivationState* CKeyUIActivationState::GetInstance()
{
	static CKeyUIActivationState Instance{};

	return &Instance;
}

void CKeyUIActivationState::Enter(const shared_ptr<CKeyUI>& Entity)
{

}

void CKeyUIActivationState::ProcessInput(const shared_ptr<CKeyUI>& Entity, const vector<vector<shared_ptr<CGameObject>>>& GameObjects, const shared_ptr<CNavMesh>& NavMesh, float ElapsedTime, UINT InputMask)
{

}

void CKeyUIActivationState::Update(const shared_ptr<CKeyUI>& Entity, const vector<vector<shared_ptr<CGameObject>>>& GameObjects, const shared_ptr<CNavMesh>& NavMesh, float ElapsedTime)
{
	Entity->GetUIAnimationController()->UpdateAnimationClip(ANIMATION_TYPE_ONCE);
}

void CKeyUIActivationState::Exit(const shared_ptr<CKeyUI>& Entity)
{

}
