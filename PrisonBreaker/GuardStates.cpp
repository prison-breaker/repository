#include "pch.h"
#include "GuardStates.h"

#include "TimeManager.h"
#include "SceneManager.h"

#include "Scene.h"

#include "Guard.h"

#include "RigidBody.h"
#include "StateMachine.h"
#include "Animator.h"

CGuardIdleState::CGuardIdleState()
{
}

CGuardIdleState::~CGuardIdleState()
{
}

void CGuardIdleState::Enter(CObject* object)
{
	CGuard* guard = (CGuard*)object;

	guard->SetTarget(nullptr);
	guard->GetAnimator()->Play("Holding_Idle", false);
}

void CGuardIdleState::Exit(CObject* object)
{
}

void CGuardIdleState::Update(CObject* object)
{
	CGuard* guard = (CGuard*)object;
	CCharacter* target = guard->FindTarget(30.0f, 75.0f);

	if (target != nullptr)
	{
		// 타겟이 설정되면, ChaseState로 전이한다.
		guard->SetTarget(target);
		guard->GetStateMachine()->ChangeState(CGuardChaseState::GetInstance());
	}
	else
	{
		// Idle 애니메이션이 종료되면, PatrolState로 전이한다.
		if (object->GetAnimator()->IsFinished())
		{
			object->GetStateMachine()->ChangeState(CGuardPatrolState::GetInstance());
		}
	}
}

//=========================================================================================================================

CGuardPatrolState::CGuardPatrolState()
{
}

CGuardPatrolState::~CGuardPatrolState()
{
}

void CGuardPatrolState::Enter(CObject* object)
{
	CGuard* guard = (CGuard*)object;

	guard->SetElapsedTime(0.0f);
	guard->GetRigidBody()->SetMaxSpeedXZ(300.0f);
	guard->GetRigidBody()->AddVelocity(Vector3::ScalarProduct(300.0f * DT, guard->GetForward(), false));
	guard->GetAnimator()->Play("Walking", true);
}

void CGuardPatrolState::Exit(CObject* object)
{
}

void CGuardPatrolState::Update(CObject* object)
{
	CGuard* guard = (CGuard*)object;
	CCharacter* target = guard->FindTarget(30.0f, 75.0f);

	if (target != nullptr)
	{
		// 타겟이 설정되면, ChaseState로 전이한다.
		guard->SetTarget(target);
		guard->GetStateMachine()->ChangeState(CGuardChaseState::GetInstance());
	}
	else
	{
		// toIdleEntryTime에 도달하면, IdleState로 전이한다.
		guard->SetElapsedTime(guard->GetElapsedTime() + DT);

		if (guard->GetElapsedTime() >= guard->GetIdleEntryTime())
		{
			guard->SetElapsedTime(0.0f);
			guard->GetStateMachine()->ChangeState(CGuardIdleState::GetInstance());
		}
		else
		{
			guard->FollowPatrolPath(10000.0f * DT);
		}
	}
}

//=========================================================================================================================

CGuardChaseState::CGuardChaseState()
{
}

CGuardChaseState::~CGuardChaseState()
{
}

void CGuardChaseState::Enter(CObject* object)
{
	CGuard* guard = (CGuard*)object;

	guard->SetElapsedTime(0.0f);
	guard->CreateMovePath(guard->GetTarget()->GetPosition());
	guard->GetRigidBody()->SetMaxSpeedXZ(500.0f);
	guard->GetRigidBody()->AddVelocity(Vector3::ScalarProduct(500.0f * DT, guard->GetForward(), false));
	guard->GetAnimator()->Play("Pistol_Run", true);
}

void CGuardChaseState::Exit(CObject* object)
{
}

void CGuardChaseState::Update(CObject* object)
{
	CGuard* guard = (CGuard*)object;
	CCharacter* target = guard->GetTarget();

	guard->SetElapsedTime(guard->GetElapsedTime() + DT);

	// 기존 타겟의 체력이 0이하가 되었거나, 갱신시간이 됐다면 타겟을 갱신한다.
	if ((target->GetHealth() <= 0) || (guard->GetElapsedTime() >= guard->GetTargetUpdateTime()))
	{
		guard->SetElapsedTime(0.0f);
		target = guard->FindTarget(40.0f, 200.0f);

		if (target != nullptr)
		{
			guard->SetTarget(target);
			guard->CreateMovePath(target->GetPosition());
		}
		else
		{
			// 플레이어가 시야각에서 보이지 않는다면 ReturnState로 전이하여 다음 Patrol Node의 위치로 이동한다.
			guard->GetStateMachine()->ChangeState(CGuardReturnState::GetInstance());
		}
	}
	else
	{
		XMFLOAT3 position = guard->GetPosition();
		XMFLOAT3 toTarget = Vector3::Subtract(target->GetPosition(), position);
		float dist = Vector3::Length(toTarget);

		// 타겟과의 거리가 10.0f이하라면, 광선을 쏘아 본다.
		if (dist <= 10.0f)
		{
			toTarget = Vector3::Normalize(toTarget);

			float nearestHitDist = FLT_MAX;
			bool isHit = false;
			XMFLOAT3 rayOrigin = position;

			rayOrigin.y = 5.0f;

			const vector<CObject*>& structures = CSceneManager::GetInstance()->GetCurrentScene()->GetGroupObject(GROUP_TYPE::STRUCTURE);

			for (const auto& structure : structures)
			{
				float hitDist = 0.0f, maxDist = 10.0f;
				CObject* intersectedObject = structure->CheckRayIntersection(rayOrigin, toTarget, hitDist, maxDist);

				if ((intersectedObject != nullptr) && (hitDist <= maxDist))
				{
					isHit = true;
					break;
				}
			}

			// 어떠한 구조물도 광선에 맞지 않았다면, 타겟을 향하도록 회전한 후 총을 쏘도록 ShootState로 전이한다.
			if (!isHit)
			{
				XMFLOAT3 forward = guard->GetForward();
				float angle = Vector3::Angle(forward, toTarget);

				// 두 벡터를 외적하여 회전축을 구해 최소 회전 방향으로 회전시킨다.
				float axis = Vector3::CrossProduct(forward, toTarget, false).y;

				if (axis < 0.0f)
				{
					angle = -angle;
				}

				guard->Rotate(guard->GetUp(), angle);
				guard->GetStateMachine()->ChangeState(CGuardShootState::GetInstance());
			}
		}
		else
		{
			guard->FollowMovePath(15000.0f * DT);

			// targetPosition에 위치했다면, 다시 타겟의 위치로 경로를 만든다.
			if (guard->IsFinishedMovePath())
			{
				guard->CreateMovePath(target->GetPosition());
			}
		}
	}
}

//=========================================================================================================================

CGuardReturnState::CGuardReturnState()
{
}

CGuardReturnState::~CGuardReturnState()
{
}

void CGuardReturnState::Enter(CObject* object)
{
	CGuard* guard = (CGuard*)object;

	guard->SetTarget(nullptr);
	guard->CreateMovePath(guard->GetNextPatrolPosition());
	guard->GetRigidBody()->SetMaxSpeedXZ(300.0f);
	guard->GetRigidBody()->AddVelocity(Vector3::ScalarProduct(300.0f * DT, guard->GetForward(), false));
	guard->GetAnimator()->Play("Walking", true);
}

void CGuardReturnState::Exit(CObject* object)
{
}

void CGuardReturnState::Update(CObject* object)
{
	CGuard* guard = (CGuard*)object;
	CCharacter* target = guard->FindTarget(30.0f, 75.0f);

	if (target != nullptr)
	{
		// 타겟이 설정되면, ChaseState로 전이한다.
		guard->SetTarget(target);
		guard->GetStateMachine()->ChangeState(CGuardChaseState::GetInstance());
	}
	else
	{
		guard->FollowMovePath(10000.0f * DT);

		// PatrolPath에 위치했다면, PatrolState로 전이한다.
		if (guard->IsFinishedMovePath())
		{
			guard->GetStateMachine()->ChangeState(CGuardPatrolState::GetInstance());
		}
	}
}

//=========================================================================================================================

CGuardAssembleState::CGuardAssembleState()
{
}

CGuardAssembleState::~CGuardAssembleState()
{
}

void CGuardAssembleState::Enter(CObject* object)
{
	CGuard* guard = (CGuard*)object;

	guard->GetRigidBody()->SetMaxSpeedXZ(500.0f);
	guard->GetRigidBody()->AddVelocity(Vector3::ScalarProduct(500.0f * DT, guard->GetForward(), false));
	guard->GetAnimator()->Play("Pistol_Run", true);
}

void CGuardAssembleState::Exit(CObject* object)
{
}

void CGuardAssembleState::Update(CObject* object)
{
	CGuard* guard = (CGuard*)object;
	CCharacter* target = guard->FindTarget(30.0f, 75.0f);

	if (target != nullptr)
	{
		// 타겟이 설정되면, ChaseState로 전이한다.
		guard->SetTarget(target);
		guard->GetStateMachine()->ChangeState(CGuardChaseState::GetInstance());
	}
	else
	{
		guard->FollowMovePath(15000.0f * DT);

		// PatrolPath에 위치했다면, PatrolState로 전이한다.
		if (guard->IsFinishedMovePath())
		{
			guard->GetStateMachine()->ChangeState(CGuardPatrolState::GetInstance());
		}
	}
}

//=========================================================================================================================

CGuardShootState::CGuardShootState()
{
}

CGuardShootState::~CGuardShootState()
{
}

void CGuardShootState::Enter(CObject* object)
{
	object->GetAnimator()->Play("Shooting", false, true);
}

void CGuardShootState::Exit(CObject* object)
{
}

void CGuardShootState::Update(CObject* object)
{
	CGuard* guard = (CGuard*)object;

	if (guard->GetAnimator()->IsFinished())
	{
		CCharacter* target = guard->FindTarget(30.0f, 200.0f);

		if (target != nullptr)
		{
			guard->SetTarget(target);

			XMFLOAT3 position = guard->GetPosition();
			XMFLOAT3 toTarget = Vector3::Subtract(target->GetPosition(), position);
			float dist = Vector3::Length(toTarget);

			// 타겟과의 거리가 10.0f이하라면, 다시 타겟을 향해 회전한 후 총을 쏘고 아닌 경우 ChaseState로 전이한다.
			if (dist <= 10.0f)
			{
				toTarget = Vector3::Normalize(toTarget);

				XMFLOAT3 forward = guard->GetForward();
				float angle = Vector3::Angle(forward, toTarget);

				// 두 벡터를 외적하여 회전축을 구해 최소 회전 방향으로 회전시킨다.
				float axis = Vector3::CrossProduct(forward, toTarget, false).y;

				if (axis < 0.0f)
				{
					angle = -angle;
				}

				guard->Rotate(guard->GetUp(), angle);
				guard->GetStateMachine()->GetCurrentState()->Enter(guard);
			}
			else
			{
				guard->GetStateMachine()->ChangeState(CGuardChaseState::GetInstance());
			}
		}
		else
		{
			// 타겟이 없다면, ReturnState로 전이한다.
			guard->GetStateMachine()->ChangeState(CGuardReturnState::GetInstance());
		}
	}
}

//=========================================================================================================================

CGuardHitState::CGuardHitState()
{
}

CGuardHitState::~CGuardHitState()
{
}

void CGuardHitState::Enter(CObject* object)
{
	object->GetAnimator()->Play("Head_Hit", false);
}

void CGuardHitState::Exit(CObject* object)
{
}

void CGuardHitState::Update(CObject* object)
{
	CGuard* guard = (CGuard*)object;

	if (guard->GetHealth() <= 0)
	{
		guard->GetStateMachine()->ChangeState(CGuardDieState::GetInstance());
	}
	else if (guard->GetAnimator()->IsFinished())
	{
		guard->GetStateMachine()->ChangeState(CGuardChaseState::GetInstance());
	}
}

//=========================================================================================================================

CGuardDieState::CGuardDieState()
{
}

CGuardDieState::~CGuardDieState()
{
}

void CGuardDieState::Enter(CObject* object)
{
	object->GetAnimator()->Play("Standing_React_Death_Backward", false);
}

void CGuardDieState::Exit(CObject* object)
{
}

void CGuardDieState::Update(CObject* object)
{
}
