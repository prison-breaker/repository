#include "stdafx.h"
#include "State_HitUI.h"
#include "UIObjects.h"
#include "UIAnimationController.h"

CHitUIActivationState* CHitUIActivationState::GetInstance()
{
	static CHitUIActivationState Instance{};

	return &Instance;
}

void CHitUIActivationState::Enter(const shared_ptr<CHitUI>& Entity)
{
	Entity->SetActive(true);
	Entity->GetAnimationController()->SetKeyFrameIndex(0);
}

void CHitUIActivationState::ProcessInput(const shared_ptr<CHitUI>& Entity, float ElapsedTime, UINT InputMask)
{

}

void CHitUIActivationState::Update(const shared_ptr<CHitUI>& Entity, float ElapsedTime)
{
	if (Entity->GetAnimationController()->UpdateAnimationClip(ANIMATION_TYPE_ONCE))
	{
		Entity->SetActive(false);
	}
}

void CHitUIActivationState::Exit(const shared_ptr<CHitUI>& Entity)
{

}
