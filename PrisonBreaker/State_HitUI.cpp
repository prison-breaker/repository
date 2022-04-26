#include "stdafx.h"
#include "State_HitUI.h"
#include "HitUI.h"

CHitUIActivationState* CHitUIActivationState::GetInstance()
{
	static CHitUIActivationState Instance{};

	return &Instance;
}

void CHitUIActivationState::Enter(const shared_ptr<CHitUI>& Entity)
{
	Entity->SetActive(true);
	Entity->GetUIAnimationController()->SetActive(true);
	Entity->GetUIAnimationController()->SetKeyFrameIndex(0);
}

void CHitUIActivationState::ProcessInput(const shared_ptr<CHitUI>& Entity, float ElapsedTime, UINT InputMask)
{

}

void CHitUIActivationState::Update(const shared_ptr<CHitUI>& Entity, float ElapsedTime)
{
	if (Entity->GetUIAnimationController()->UpdateAnimationClip(ANIMATION_TYPE_ONCE))
	{
		Entity->SetActive(false);
	}
}

void CHitUIActivationState::Exit(const shared_ptr<CHitUI>& Entity)
{

}
