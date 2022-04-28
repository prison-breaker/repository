#include "stdafx.h"
#include "State_MissionUI.h"
#include "UIs.h"

CMissionUIShowingState* CMissionUIShowingState::GetInstance()
{
	static CMissionUIShowingState Instance{};

	return &Instance;
}

void CMissionUIShowingState::Enter(const shared_ptr<CMissionUI>& Entity)
{

}

void CMissionUIShowingState::ProcessInput(const shared_ptr<CMissionUI>& Entity, float ElapsedTime, UINT InputMask)
{
	if (InputMask & INPUT_MASK_TAB)
	{
		Entity->GetStateMachine()->ChangeState(CMissionUIHidingState::GetInstance());
	}
}

void CMissionUIShowingState::Update(const shared_ptr<CMissionUI>& Entity, float ElapsedTime)
{
	Entity->GetUIAnimationController()->UpdateAnimationClip(ANIMATION_TYPE_ONCE);
}

void CMissionUIShowingState::Exit(const shared_ptr<CMissionUI>& Entity)
{

}

//=========================================================================================================================

CMissionUIHidingState* CMissionUIHidingState::GetInstance()
{
	static CMissionUIHidingState Instance{};

	return &Instance;
}

void CMissionUIHidingState::Enter(const shared_ptr<CMissionUI>& Entity)
{

}

void CMissionUIHidingState::ProcessInput(const shared_ptr<CMissionUI>& Entity, float ElapsedTime, UINT InputMask)
{
	if (InputMask & INPUT_MASK_TAB)
	{
		Entity->GetStateMachine()->ChangeState(CMissionUIShowingState::GetInstance());
	}
}

void CMissionUIHidingState::Update(const shared_ptr<CMissionUI>& Entity, float ElapsedTime)
{
	Entity->GetUIAnimationController()->UpdateAnimationClip(ANIMATION_TYPE_ONCE_REVERSE);
}

void CMissionUIHidingState::Exit(const shared_ptr<CMissionUI>& Entity)
{

}
