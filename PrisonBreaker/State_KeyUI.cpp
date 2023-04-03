#include "pch.h"
#include "State_KeyUI.h"
#include "UIObjects.h"
#include "UIAnimationController.h"

CKeyUIActivationState* CKeyUIActivationState::GetInstance()
{
	static CKeyUIActivationState Instance{};

	return &Instance;
}

void CKeyUIActivationState::Enter(const shared_ptr<CKeyUI>& Entity)
{

}

void CKeyUIActivationState::ProcessInput(const shared_ptr<CKeyUI>& Entity, float ElapsedTime, UINT InputMask)
{

}

void CKeyUIActivationState::Update(const shared_ptr<CKeyUI>& Entity, float ElapsedTime)
{
	Entity->GetAnimationController()->UpdateAnimationClip(ANIMATION_TYPE_ONCE);
}

void CKeyUIActivationState::Exit(const shared_ptr<CKeyUI>& Entity)
{

}
