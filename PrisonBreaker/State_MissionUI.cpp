#include "stdafx.h"
#include "State_MissionUI.h"
#include "BilboardObject.h"

CBilboardObjectShowingState* CBilboardObjectShowingState::GetInstance()
{
	static CBilboardObjectShowingState Instance{};

	return &Instance;
}

void CBilboardObjectShowingState::Enter(const shared_ptr<CBilboardObject>& Entity)
{

}

void CBilboardObjectShowingState::ProcessInput(const shared_ptr<CBilboardObject>& Entity, UINT InputMask, float ElapsedTime)
{
	if (InputMask & INPUT_MASK_TAB)
	{
		Entity->GetStateMachine()->ChangeState(CBilboardObjectHidingState::GetInstance());
	}
}

void CBilboardObjectShowingState::Update(const shared_ptr<CBilboardObject>& Entity, float ElapsedTime)
{
	Entity->GetUIAnimationController()->UpdateAnimationClip(ANIMATION_TYPE_ONCE);
}

void CBilboardObjectShowingState::Exit(const shared_ptr<CBilboardObject>& Entity)
{

}

//=========================================================================================================================

CBilboardObjectHidingState* CBilboardObjectHidingState::GetInstance()
{
	static CBilboardObjectHidingState Instance{};

	return &Instance;
}

void CBilboardObjectHidingState::Enter(const shared_ptr<CBilboardObject>& Entity)
{

}

void CBilboardObjectHidingState::ProcessInput(const shared_ptr<CBilboardObject>& Entity, UINT InputMask, float ElapsedTime)
{
	if (InputMask & INPUT_MASK_TAB)
	{
		Entity->GetStateMachine()->ChangeState(CBilboardObjectShowingState::GetInstance());
	}
}

void CBilboardObjectHidingState::Update(const shared_ptr<CBilboardObject>& Entity, float ElapsedTime)
{
	Entity->GetUIAnimationController()->UpdateAnimationClip(ANIMATION_TYPE_ONCE_REVERSE);
}

void CBilboardObjectHidingState::Exit(const shared_ptr<CBilboardObject>& Entity)
{

}
