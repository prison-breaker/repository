#include "pch.h"
#include "PlayerStates.h"

#include "TimeManager.h"
#include "InputManager.h"
#include "CameraManager.h"

#include "Player.h"

#include "StateMachine.h"
#include "RigidBody.h"
#include "Animator.h"

#include "Camera.h"

CPlayerIdleState::CPlayerIdleState()
{
}

CPlayerIdleState::~CPlayerIdleState()
{
}

void CPlayerIdleState::Enter(CObject* object)
{
	CAnimator* animator = static_cast<CAnimator*>(object->GetComponent(COMPONENT_TYPE::ANIMATOR));

	animator->Play("Idle", true);
}

void CPlayerIdleState::Exit(CObject* object)
{
}

void CPlayerIdleState::Update(CObject* object)
{
	CPlayer* player = static_cast<CPlayer*>(object);
	CStateMachine* stateMachine = static_cast<CStateMachine*>(player->GetComponent(COMPONENT_TYPE::STATE_MACHINE));

	if (KEY_TAP(KEY::LBUTTON))
	{
		if (!player->IsEquippedWeapon())
		{
			stateMachine->ChangeState(CPlayerPunchState::GetInstance());
		}
	}
	else if (KEY_HOLD(KEY::RBUTTON))
	{
		if (player->IsEquippedWeapon())
		{
			stateMachine->ChangeState(CPlayerShootState::GetInstance());
		}
	}
	else if (KEY_HOLD(KEY::W) || KEY_HOLD(KEY::S) || KEY_HOLD(KEY::A) || KEY_HOLD(KEY::D))
	{
		if (KEY_HOLD(KEY::SHIFT))
		{
			stateMachine->ChangeState(CPlayerRunState::GetInstance());
		}
		else
		{
			stateMachine->ChangeState(CPlayerWalkState::GetInstance());
		}
	}
}

//=========================================================================================================================

CPlayerWalkState::CPlayerWalkState()
{
}

CPlayerWalkState::~CPlayerWalkState()
{
}

void CPlayerWalkState::Enter(CObject* object)
{
	CRigidBody* rigidBody = static_cast<CRigidBody*>(object->GetComponent(COMPONENT_TYPE::RIGIDBODY));

	if (KEY_HOLD(KEY::W))
	{
		rigidBody->SetMaxSpeedXZ(400.0f);
		rigidBody->AddVelocity(Vector3::ScalarProduct(400.0f * DT, object->GetForward(), false));
	}

	if (KEY_HOLD(KEY::S))
	{
		rigidBody->SetMaxSpeedXZ(400.0f);
		rigidBody->AddVelocity(Vector3::ScalarProduct(400.0f * DT, Vector3::Inverse(object->GetForward()), false));
	}

	if (KEY_HOLD(KEY::A))
	{
		rigidBody->SetMaxSpeedXZ(400.0f);
		rigidBody->AddVelocity(Vector3::ScalarProduct(400.0f * DT, Vector3::Inverse(object->GetRight()), false));
	}

	if (KEY_HOLD(KEY::D))
	{
		rigidBody->SetMaxSpeedXZ(400.0f);
		rigidBody->AddVelocity(Vector3::ScalarProduct(400.0f * DT, object->GetRight(), false));
	}
}

void CPlayerWalkState::Exit(CObject* object)
{
}

void CPlayerWalkState::Update(CObject* object)
{
	CPlayer* player = (CPlayer*)object;
	CStateMachine* stateMachine = static_cast<CStateMachine*>(player->GetComponent(COMPONENT_TYPE::STATE_MACHINE));
	CRigidBody* rigidBody = static_cast<CRigidBody*>(player->GetComponent(COMPONENT_TYPE::RIGIDBODY));
	CAnimator* animator = static_cast<CAnimator*>(player->GetComponent(COMPONENT_TYPE::ANIMATOR));

	if ((KEY_NONE(KEY::W) && KEY_NONE(KEY::S) && KEY_NONE(KEY::A) && KEY_NONE(KEY::D)) && Math::IsZero(rigidBody->GetSpeedXZ()))
	{
		stateMachine->ChangeState(CPlayerIdleState::GetInstance());
	}
	else if ((KEY_HOLD(KEY::SHIFT)) && (KEY_HOLD(KEY::W) || KEY_HOLD(KEY::S) || KEY_HOLD(KEY::A) || KEY_HOLD(KEY::D)))
	{
		stateMachine->ChangeState(CPlayerRunState::GetInstance());
	}
	else if (KEY_TAP(KEY::LBUTTON))
	{
		if (!player->IsEquippedWeapon())
		{
			stateMachine->ChangeState(CPlayerPunchState::GetInstance());
		}
	}
	else if (KEY_HOLD(KEY::RBUTTON))
	{
		if (player->IsEquippedWeapon())
		{
			stateMachine->ChangeState(CPlayerShootState::GetInstance());
		}
	}
	else
	{
		if (KEY_HOLD(KEY::W))
		{
			rigidBody->AddForce(Vector3::ScalarProduct(10000.0f * DT, player->GetForward(), false));
		}

		if (KEY_HOLD(KEY::S))
		{
			rigidBody->AddForce(Vector3::ScalarProduct(10000.0f * DT, Vector3::Inverse(player->GetForward()), false));
		}

		if (KEY_HOLD(KEY::A))
		{
			rigidBody->AddForce(Vector3::ScalarProduct(10000.0f * DT, Vector3::Inverse(player->GetRight()), false));
		}

		if (KEY_HOLD(KEY::D))
		{
			rigidBody->AddForce(Vector3::ScalarProduct(10000.0f * DT, player->GetRight(), false));
		}

		if (Math::IsZero(rigidBody->GetSpeedXZ()))
		{
			stateMachine->ChangeState(CPlayerIdleState::GetInstance());
		}
		else
		{
			XMFLOAT3 direction = Vector3::Normalize(rigidBody->GetVelocity());

			if (Vector3::IsEqual(direction, player->GetRight()))
			{
				animator->Play("Right_Strafe_Walking", true);
			}
			else if (Vector3::IsEqual(direction, Vector3::Inverse(player->GetRight())))
			{
				animator->Play("Left_Strafe_Walking", true);
			}
			else
			{
				animator->Play("Crouched_Walking", true);
			}
		}
	}
}

//=========================================================================================================================

CPlayerRunState::CPlayerRunState()
{
}

CPlayerRunState::~CPlayerRunState()
{
}

void CPlayerRunState::Enter(CObject* object)
{
	CRigidBody* rigidBody = static_cast<CRigidBody*>(object->GetComponent(COMPONENT_TYPE::RIGIDBODY));

	if (KEY_HOLD(KEY::W))
	{
		rigidBody->SetMaxSpeedXZ(700.0f);
		rigidBody->AddVelocity(Vector3::ScalarProduct(700.0f * DT, object->GetForward(), false));
	}

	if (KEY_HOLD(KEY::S))
	{
		rigidBody->SetMaxSpeedXZ(400.0f);
		rigidBody->AddVelocity(Vector3::ScalarProduct(400.0f * DT, Vector3::Inverse(object->GetForward()), false));
	}

	if (KEY_HOLD(KEY::A))
	{
		rigidBody->SetMaxSpeedXZ(700.0f);
		rigidBody->AddVelocity(Vector3::ScalarProduct(700.0f * DT, Vector3::Inverse(object->GetRight()), false));
	}

	if (KEY_HOLD(KEY::D))
	{
		rigidBody->SetMaxSpeedXZ(700.0f);
		rigidBody->AddVelocity(Vector3::ScalarProduct(700.0f * DT, object->GetRight(), false));
	}
}

void CPlayerRunState::Exit(CObject* object)
{
}

void CPlayerRunState::Update(CObject* object)
{
	CPlayer* player = (CPlayer*)object;
	CStateMachine* stateMachine = static_cast<CStateMachine*>(player->GetComponent(COMPONENT_TYPE::STATE_MACHINE));
	CRigidBody* rigidBody = static_cast<CRigidBody*>(player->GetComponent(COMPONENT_TYPE::RIGIDBODY));
	CAnimator* animator = static_cast<CAnimator*>(player->GetComponent(COMPONENT_TYPE::ANIMATOR));

	if ((KEY_NONE(KEY::W) && KEY_NONE(KEY::S) && KEY_NONE(KEY::A) && KEY_NONE(KEY::D)) && Math::IsZero(rigidBody->GetSpeedXZ()))
	{
		stateMachine->ChangeState(CPlayerIdleState::GetInstance());
	}
	else if ((KEY_NONE(KEY::SHIFT)) && (KEY_HOLD(KEY::W) || KEY_HOLD(KEY::S) || KEY_HOLD(KEY::A) || KEY_HOLD(KEY::D)))
	{
		stateMachine->ChangeState(CPlayerWalkState::GetInstance());
	}
	else if (KEY_TAP(KEY::LBUTTON))
	{
		if (!player->IsEquippedWeapon())
		{
			stateMachine->ChangeState(CPlayerPunchState::GetInstance());
		}
	}
	else if (KEY_HOLD(KEY::RBUTTON))
	{
		if (player->IsEquippedWeapon())
		{
			stateMachine->ChangeState(CPlayerShootState::GetInstance());
		}
	}
	else
	{
		if (KEY_HOLD(KEY::W))
		{
			rigidBody->AddForce(Vector3::ScalarProduct(15000.0f * DT, player->GetForward(), false));
		}

		if (KEY_HOLD(KEY::S))
		{
			rigidBody->AddForce(Vector3::ScalarProduct(10000.0f * DT, Vector3::Inverse(player->GetForward()), false));
		}

		if (KEY_HOLD(KEY::A))
		{
			rigidBody->AddForce(Vector3::ScalarProduct(15000.0f * DT, Vector3::Inverse(player->GetRight()), false));
		}

		if (KEY_HOLD(KEY::D))
		{
			rigidBody->AddForce(Vector3::ScalarProduct(15000.0f * DT, player->GetRight(), false));
		}

		if (Math::IsZero(rigidBody->GetSpeedXZ()))
		{
			stateMachine->ChangeState(CPlayerIdleState::GetInstance());
		}
		else
		{
			XMFLOAT3 direction = Vector3::Normalize(rigidBody->GetVelocity());

			if (Vector3::IsEqual(direction, player->GetRight()))
			{
				rigidBody->SetMaxSpeedXZ(700.0f);
				animator->Play("Right_Strafe", true);
			}
			else if (Vector3::IsEqual(direction, Vector3::Inverse(player->GetRight())))
			{
				rigidBody->SetMaxSpeedXZ(700.0f);
				animator->Play("Left_Strafe", true);
			}
			else if (Vector3::Angle(direction, player->GetForward()) < 90.0f)
			{
				rigidBody->SetMaxSpeedXZ(700.0f);
				animator->Play("Running", true);
			}
			else
			{
				rigidBody->SetMaxSpeedXZ(400.0f);
				animator->Play("Crouched_Walking", true);
			}
		}
	}
}

//=========================================================================================================================

CPlayerPunchState::CPlayerPunchState()
{
}

CPlayerPunchState::~CPlayerPunchState()
{
}

void CPlayerPunchState::Enter(CObject* object)
{
	CPlayer* player = (CPlayer*)object;
	CAnimator* animator = static_cast<CAnimator*>(object->GetComponent(COMPONENT_TYPE::ANIMATOR));

	player->Punch();
	animator->Play("Punching", false);
}

void CPlayerPunchState::Exit(CObject* object)
{
}

void CPlayerPunchState::Update(CObject* object)
{
	CStateMachine* stateMachine = static_cast<CStateMachine*>(object->GetComponent(COMPONENT_TYPE::STATE_MACHINE));
	CAnimator* animator = static_cast<CAnimator*>(object->GetComponent(COMPONENT_TYPE::ANIMATOR));

	if (animator->IsFinished())
	{
		if ((KEY_NONE(KEY::W) && KEY_NONE(KEY::S) && KEY_NONE(KEY::A) && KEY_NONE(KEY::D)) && Math::IsZero(((CRigidBody*)object->GetComponent(COMPONENT_TYPE::RIGIDBODY))->GetSpeedXZ()))
		{
			stateMachine->ChangeState(CPlayerIdleState::GetInstance());
		}
		else if (KEY_HOLD(KEY::W) || KEY_HOLD(KEY::S) || KEY_HOLD(KEY::A) || KEY_HOLD(KEY::D))
		{
			if (KEY_NONE(KEY::SHIFT))
			{
				stateMachine->ChangeState(CPlayerWalkState::GetInstance());
			}
			else
			{
				stateMachine->ChangeState(CPlayerRunState::GetInstance());
			}
		}
	}
}

//=========================================================================================================================

CPlayerShootState::CPlayerShootState()
{
}

CPlayerShootState::~CPlayerShootState()
{
}

void CPlayerShootState::Enter(CObject* object)
{
	CPlayer* player = (CPlayer*)object;
	CAnimator* animator = static_cast<CAnimator*>(player->GetComponent(COMPONENT_TYPE::ANIMATOR));

	player->SetAiming(true);
	animator->Play("Pistol_Idle", true);

	CCameraManager::GetInstance()->GetMainCamera()->SetZoomIn(true);
}

void CPlayerShootState::Exit(CObject* object)
{
	CCamera* camera = CCameraManager::GetInstance()->GetMainCamera();

	camera->SetZoomIn(false);
	camera->SetMagnification(1.0f);
}

void CPlayerShootState::Update(CObject* object)
{
	CPlayer* player = (CPlayer*)object;
	CRigidBody* rigidBody = static_cast<CRigidBody*>(player->GetComponent(COMPONENT_TYPE::RIGIDBODY));
	CAnimator* animator = static_cast<CAnimator*>(player->GetComponent(COMPONENT_TYPE::ANIMATOR));

	if (player->IsAiming())
	{
		if (KEY_HOLD(KEY::RBUTTON))
		{
			CCamera* camera = CCameraManager::GetInstance()->GetMainCamera();
			
			camera->SetMagnification(camera->GetMagnification() + 1.5f * DT);

			// 우클릭으로 줌을 하고 있는 상태에서, 좌클릭이 입력된다면 총을 발사한다.
			if (KEY_TAP(KEY::LBUTTON) && player->HasBullet())
			{
				player->SetAiming(false);
				player->Shoot();
				animator->Play("Shooting", false);
			}
		}
		else
		{ 
			if ((KEY_NONE(KEY::W) && KEY_NONE(KEY::S) && KEY_NONE(KEY::A) && KEY_NONE(KEY::D)) && Math::IsZero(rigidBody->GetSpeedXZ()))
			{
				player->SetAiming(false);
				((CStateMachine*)player->GetComponent(COMPONENT_TYPE::STATE_MACHINE))->ChangeState(CPlayerIdleState::GetInstance());
			}
			else if (KEY_HOLD(KEY::W) || KEY_HOLD(KEY::S) || KEY_HOLD(KEY::A) || KEY_HOLD(KEY::D))
			{
				player->SetAiming(false);

				if (KEY_NONE(KEY::SHIFT))
				{
					((CStateMachine*)player->GetComponent(COMPONENT_TYPE::STATE_MACHINE))->ChangeState(CPlayerWalkState::GetInstance());
				}
				else
				{
					((CStateMachine*)player->GetComponent(COMPONENT_TYPE::STATE_MACHINE))->ChangeState(CPlayerRunState::GetInstance());
				}
			}
		}
	}
	else
	{
		if (animator->IsFinished())
		{
			player->SetAiming(true);
			animator->Play("Pistol_Idle", true);
		}
	}
}

//=========================================================================================================================

CPlayerDieState::CPlayerDieState()
{
}

CPlayerDieState::~CPlayerDieState()
{
}

void CPlayerDieState::Enter(CObject* object)
{
	CAnimator* animator = static_cast<CAnimator*>(object->GetComponent(COMPONENT_TYPE::ANIMATOR));

	animator->Play("Standing_React_Death_Backward", false);
}

void CPlayerDieState::Exit(CObject* object)
{
}

void CPlayerDieState::Update(CObject* object)
{
}
