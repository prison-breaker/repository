#include "pch.h"
#include "PlayerStates.h"

#include "TimeManager.h"
#include "InputManager.h"
#include "CameraManager.h"

#include "Player.h"

#include "RigidBody.h"
#include "StateMachine.h"
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
	object->GetAnimator()->Play("Idle", true);
}

void CPlayerIdleState::Exit(CObject* object)
{
}

void CPlayerIdleState::Update(CObject* object)
{
	CPlayer* player = (CPlayer*)object;

	if (KEY_TAP(KEY::LBUTTON))
	{
		if (!player->IsEquippedWeapon())
		{
			player->GetStateMachine()->ChangeState(CPlayerPunchState::GetInstance());
		}
	}
	else if (KEY_HOLD(KEY::RBUTTON))
	{
		if (player->IsEquippedWeapon())
		{
			player->GetStateMachine()->ChangeState(CPlayerShootState::GetInstance());
		}
	}
	else if (KEY_HOLD(KEY::W) || KEY_HOLD(KEY::S) || KEY_HOLD(KEY::A) || KEY_HOLD(KEY::D))
	{
		if (KEY_HOLD(KEY::SHIFT))
		{
			player->GetStateMachine()->ChangeState(CPlayerRunState::GetInstance());
		}
		else
		{
			player->GetStateMachine()->ChangeState(CPlayerWalkState::GetInstance());
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
	if (KEY_HOLD(KEY::W))
	{
		object->GetRigidBody()->SetMaxSpeedXZ(400.0f);
		object->GetRigidBody()->AddVelocity(Vector3::ScalarProduct(400.0f * DT, object->GetForward(), false));
	}

	if (KEY_HOLD(KEY::S))
	{
		object->GetRigidBody()->SetMaxSpeedXZ(400.0f);
		object->GetRigidBody()->AddVelocity(Vector3::ScalarProduct(400.0f * DT, Vector3::Inverse(object->GetForward()), false));
	}

	if (KEY_HOLD(KEY::A))
	{
		object->GetRigidBody()->SetMaxSpeedXZ(400.0f);
		object->GetRigidBody()->AddVelocity(Vector3::ScalarProduct(400.0f * DT, Vector3::Inverse(object->GetRight()), false));
	}

	if (KEY_HOLD(KEY::D))
	{
		object->GetRigidBody()->SetMaxSpeedXZ(400.0f);
		object->GetRigidBody()->AddVelocity(Vector3::ScalarProduct(400.0f * DT, object->GetRight(), false));
	}
}

void CPlayerWalkState::Exit(CObject* object)
{
}

void CPlayerWalkState::Update(CObject* object)
{
	CPlayer* player = (CPlayer*)object;

	if ((KEY_NONE(KEY::W) && KEY_NONE(KEY::S) && KEY_NONE(KEY::A) && KEY_NONE(KEY::D)) && Math::IsZero(player->GetRigidBody()->GetSpeedXZ()))
	{
		player->GetStateMachine()->ChangeState(CPlayerIdleState::GetInstance());
	}
	else if ((KEY_HOLD(KEY::SHIFT)) && (KEY_HOLD(KEY::W) || KEY_HOLD(KEY::S) || KEY_HOLD(KEY::A) || KEY_HOLD(KEY::D)))
	{
		player->GetStateMachine()->ChangeState(CPlayerRunState::GetInstance());
	}
	else if (KEY_TAP(KEY::LBUTTON))
	{
		if (!player->IsEquippedWeapon())
		{
			player->GetStateMachine()->ChangeState(CPlayerPunchState::GetInstance());
		}
	}
	else if (KEY_HOLD(KEY::RBUTTON))
	{
		if (player->IsEquippedWeapon())
		{
			player->GetStateMachine()->ChangeState(CPlayerShootState::GetInstance());
		}
	}
	else
	{
		if (KEY_HOLD(KEY::W))
		{
			player->GetRigidBody()->AddForce(Vector3::ScalarProduct(10000.0f * DT, player->GetForward(), false));
		}

		if (KEY_HOLD(KEY::S))
		{
			player->GetRigidBody()->AddForce(Vector3::ScalarProduct(10000.0f * DT, Vector3::Inverse(player->GetForward()), false));
		}

		if (KEY_HOLD(KEY::A))
		{
			player->GetRigidBody()->AddForce(Vector3::ScalarProduct(10000.0f * DT, Vector3::Inverse(player->GetRight()), false));
		}

		if (KEY_HOLD(KEY::D))
		{
			player->GetRigidBody()->AddForce(Vector3::ScalarProduct(10000.0f * DT, player->GetRight(), false));
		}

		if (Math::IsZero(player->GetRigidBody()->GetSpeedXZ()))
		{
			player->GetStateMachine()->ChangeState(CPlayerIdleState::GetInstance());
		}
		else
		{
			XMFLOAT3 direction = Vector3::Normalize(player->GetRigidBody()->GetVelocity());

			if (Vector3::IsEqual(direction, player->GetRight()))
			{
				player->GetAnimator()->Play("Right_Strafe_Walking", true);
			}
			else if (Vector3::IsEqual(direction, Vector3::Inverse(player->GetRight())))
			{
				player->GetAnimator()->Play("Left_Strafe_Walking", true);
			}
			else
			{
				player->GetAnimator()->Play("Crouched_Walking", true);
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
	if (KEY_HOLD(KEY::W))
	{
		object->GetRigidBody()->SetMaxSpeedXZ(700.0f);
		object->GetRigidBody()->AddVelocity(Vector3::ScalarProduct(700.0f * DT, object->GetForward(), false));
	}

	if (KEY_HOLD(KEY::S))
	{
		object->GetRigidBody()->SetMaxSpeedXZ(400.0f);
		object->GetRigidBody()->AddVelocity(Vector3::ScalarProduct(400.0f * DT, Vector3::Inverse(object->GetForward()), false));
	}

	if (KEY_HOLD(KEY::A))
	{
		object->GetRigidBody()->SetMaxSpeedXZ(700.0f);
		object->GetRigidBody()->AddVelocity(Vector3::ScalarProduct(700.0f * DT, Vector3::Inverse(object->GetRight()), false));
	}

	if (KEY_HOLD(KEY::D))
	{
		object->GetRigidBody()->SetMaxSpeedXZ(700.0f);
		object->GetRigidBody()->AddVelocity(Vector3::ScalarProduct(700.0f * DT, object->GetRight(), false));
	}
}

void CPlayerRunState::Exit(CObject* object)
{
}

void CPlayerRunState::Update(CObject* object)
{
	CPlayer* player = (CPlayer*)object;

	if ((KEY_NONE(KEY::W) && KEY_NONE(KEY::S) && KEY_NONE(KEY::A) && KEY_NONE(KEY::D)) && Math::IsZero(player->GetRigidBody()->GetSpeedXZ()))
	{
		player->GetStateMachine()->ChangeState(CPlayerIdleState::GetInstance());
	}
	else if ((KEY_NONE(KEY::SHIFT)) && (KEY_HOLD(KEY::W) || KEY_HOLD(KEY::S) || KEY_HOLD(KEY::A) || KEY_HOLD(KEY::D)))
	{
		player->GetStateMachine()->ChangeState(CPlayerWalkState::GetInstance());
	}
	else if (KEY_TAP(KEY::LBUTTON))
	{
		if (!player->IsEquippedWeapon())
		{
			player->GetStateMachine()->ChangeState(CPlayerPunchState::GetInstance());
		}
	}
	else if (KEY_HOLD(KEY::RBUTTON))
	{
		if (player->IsEquippedWeapon())
		{
			player->GetStateMachine()->ChangeState(CPlayerShootState::GetInstance());
		}
	}
	else
	{
		if (KEY_HOLD(KEY::W))
		{
			player->GetRigidBody()->AddForce(Vector3::ScalarProduct(15000.0f * DT, player->GetForward(), false));
		}

		if (KEY_HOLD(KEY::S))
		{
			player->GetRigidBody()->AddForce(Vector3::ScalarProduct(10000.0f * DT, Vector3::Inverse(player->GetForward()), false));
		}

		if (KEY_HOLD(KEY::A))
		{
			player->GetRigidBody()->AddForce(Vector3::ScalarProduct(15000.0f * DT, Vector3::Inverse(player->GetRight()), false));
		}

		if (KEY_HOLD(KEY::D))
		{
			player->GetRigidBody()->AddForce(Vector3::ScalarProduct(15000.0f * DT, player->GetRight(), false));
		}

		if (Math::IsZero(player->GetRigidBody()->GetSpeedXZ()))
		{
			player->GetStateMachine()->ChangeState(CPlayerIdleState::GetInstance());
		}
		else
		{
			XMFLOAT3 direction = Vector3::Normalize(player->GetRigidBody()->GetVelocity());

			if (Vector3::IsEqual(direction, player->GetRight()))
			{
				player->GetRigidBody()->SetMaxSpeedXZ(700.0f);
				player->GetAnimator()->Play("Right_Strafe", true);
			}
			else if (Vector3::IsEqual(direction, Vector3::Inverse(player->GetRight())))
			{
				player->GetRigidBody()->SetMaxSpeedXZ(700.0f);
				player->GetAnimator()->Play("Left_Strafe", true);
			}
			else if (Vector3::Angle(direction, player->GetForward()) < 90.0f)
			{
				player->GetRigidBody()->SetMaxSpeedXZ(700.0f);
				player->GetAnimator()->Play("Running", true);
			}
			else
			{
				player->GetRigidBody()->SetMaxSpeedXZ(400.0f);
				player->GetAnimator()->Play("Crouched_Walking", true);
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

	player->Punch();
	player->GetAnimator()->Play("Punching", false);
}

void CPlayerPunchState::Exit(CObject* object)
{
}

void CPlayerPunchState::Update(CObject* object)
{
	if (object->GetAnimator()->IsFinished())
	{
		if ((KEY_NONE(KEY::W) && KEY_NONE(KEY::S) && KEY_NONE(KEY::A) && KEY_NONE(KEY::D)) && Math::IsZero(object->GetRigidBody()->GetSpeedXZ()))
		{
			object->GetStateMachine()->ChangeState(CPlayerIdleState::GetInstance());
		}
		else if (KEY_HOLD(KEY::W) || KEY_HOLD(KEY::S) || KEY_HOLD(KEY::A) || KEY_HOLD(KEY::D))
		{
			if (KEY_NONE(KEY::SHIFT))
			{
				object->GetStateMachine()->ChangeState(CPlayerWalkState::GetInstance());
			}
			else
			{
				object->GetStateMachine()->ChangeState(CPlayerRunState::GetInstance());
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

	player->SetAiming(true);
	player->GetAnimator()->Play("Pistol_Idle", true);

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

	if (player->IsAiming())
	{
		if (KEY_HOLD(KEY::RBUTTON))
		{
			CCamera* camera = CCameraManager::GetInstance()->GetMainCamera();
			
			camera->SetMagnification(camera->GetMagnification() + DT);

			// 우클릭으로 줌을 하고 있는 상태에서, 좌클릭이 입력된다면 총을 발사한다.
			if (KEY_TAP(KEY::LBUTTON))
			{
				player->SetAiming(false);
				player->Shoot();
				player->GetAnimator()->Play("Shooting", false);
			}
		}
		else
		{ 
			if ((KEY_NONE(KEY::W) && KEY_NONE(KEY::S) && KEY_NONE(KEY::A) && KEY_NONE(KEY::D)) && Math::IsZero(player->GetRigidBody()->GetSpeedXZ()))
			{
				player->SetAiming(false);
				player->GetStateMachine()->ChangeState(CPlayerIdleState::GetInstance());
			}
			else if (KEY_HOLD(KEY::W) || KEY_HOLD(KEY::S) || KEY_HOLD(KEY::A) || KEY_HOLD(KEY::D))
			{
				player->SetAiming(false);

				if (KEY_NONE(KEY::SHIFT))
				{
					player->GetStateMachine()->ChangeState(CPlayerWalkState::GetInstance());
				}
				else
				{
					player->GetStateMachine()->ChangeState(CPlayerRunState::GetInstance());
				}
			}
		}
	}
	else
	{
		if (player->GetAnimator()->IsFinished())
		{
			player->SetAiming(true);
			player->GetAnimator()->Play("Pistol_Idle", true);
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
	object->GetAnimator()->Play("Standing_React_Death_Backward", false);
}

void CPlayerDieState::Exit(CObject* object)
{
}

void CPlayerDieState::Update(CObject* object)
{
}
