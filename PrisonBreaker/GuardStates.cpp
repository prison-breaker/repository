#include "pch.h"
#include "GuardStates.h"

#include "TimeManager.h"
#include "SceneManager.h"

#include "Scene.h"

#include "Guard.h"

#include "StateMachine.h"
#include "RigidBody.h"
#include "Animator.h"
#include "Transform.h"

#include "UIStates.h"

CGuardIdleState::CGuardIdleState()
{
}

CGuardIdleState::~CGuardIdleState()
{
}

void CGuardIdleState::Enter(CObject* object)
{
	CGuard* guard = static_cast<CGuard*>(object);
	CAnimator* animator = static_cast<CAnimator*>(guard->GetComponent(COMPONENT_TYPE::ANIMATOR));

	guard->SetTarget(nullptr);
	animator->Play("Holding_Idle", false);
}

void CGuardIdleState::Exit(CObject* object)
{
}

void CGuardIdleState::Update(CObject* object)
{
	CGuard* guard = static_cast<CGuard*>(object);
	CStateMachine* stateMachine = static_cast<CStateMachine*>(guard->GetComponent(COMPONENT_TYPE::STATE_MACHINE));

	CCharacter* target = guard->FindTarget(30.0f, 75.0f);

	if (target != nullptr)
	{
		// Ÿ���� �����Ǹ�, ChaseState�� �����Ѵ�.
		guard->SetTarget(target);
		stateMachine->ChangeState(CGuardChaseState::GetInstance());
	}
	else
	{
		CAnimator* animator = static_cast<CAnimator*>(guard->GetComponent(COMPONENT_TYPE::ANIMATOR));

		// Idle �ִϸ��̼��� ����Ǹ�, PatrolState�� �����Ѵ�.
		if (animator->IsFinished())
		{
			stateMachine->ChangeState(CGuardPatrolState::GetInstance());
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
	CGuard* guard = static_cast<CGuard*>(object);
	CRigidBody* rigidBody = static_cast<CRigidBody*>(guard->GetComponent(COMPONENT_TYPE::RIGIDBODY));
	CAnimator* animator = static_cast<CAnimator*>(guard->GetComponent(COMPONENT_TYPE::ANIMATOR));
	CTransform* transform = static_cast<CTransform*>(guard->GetComponent(COMPONENT_TYPE::TRANSFORM));

	guard->SetElapsedTime(0.0f);
	rigidBody->SetMaxSpeedXZ(300.0f);
	rigidBody->AddVelocity(Vector3::ScalarProduct(transform->GetForward(), 300.0f * DT));
	animator->Play("Walking", true);
}

void CGuardPatrolState::Exit(CObject* object)
{
}

void CGuardPatrolState::Update(CObject* object)
{
	CGuard* guard = static_cast<CGuard*>(object);
	CStateMachine* stateMachine = static_cast<CStateMachine*>(guard->GetComponent(COMPONENT_TYPE::STATE_MACHINE));
	CCharacter* target = guard->FindTarget(30.0f, 75.0f);

	if (target != nullptr)
	{
		// Ÿ���� �����Ǹ�, ChaseState�� �����Ѵ�.
		guard->SetTarget(target);
		stateMachine->ChangeState(CGuardChaseState::GetInstance());
	}
	else
	{
		// toIdleEntryTime�� �����ϸ�, IdleState�� �����Ѵ�.
		guard->SetElapsedTime(guard->GetElapsedTime() + DT);

		if (guard->GetElapsedTime() >= guard->GetIdleEntryTime())
		{
			guard->SetElapsedTime(0.0f);
			stateMachine->ChangeState(CGuardIdleState::GetInstance());
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
	CGuard* guard = static_cast<CGuard*>(object);
	CRigidBody* rigidBody = static_cast<CRigidBody*>(guard->GetComponent(COMPONENT_TYPE::RIGIDBODY));
	CAnimator* animator = static_cast<CAnimator*>(guard->GetComponent(COMPONENT_TYPE::ANIMATOR));
	CTransform* transform = static_cast<CTransform*>(guard->GetComponent(COMPONENT_TYPE::TRANSFORM));

	CCharacter* target = guard->GetTarget();
	CTransform* targetTransform = static_cast<CTransform*>(target->GetComponent(COMPONENT_TYPE::TRANSFORM));

	guard->SetElapsedTime(0.0f);
	guard->CreateMovePath(targetTransform->GetPosition());
	rigidBody->SetMaxSpeedXZ(500.0f);
	rigidBody->AddVelocity(Vector3::ScalarProduct(transform->GetForward(), 500.0f * DT));
	animator->Play("Pistol_Run", true);
}

void CGuardChaseState::Exit(CObject* object)
{
}

void CGuardChaseState::Update(CObject* object)
{
	CGuard* guard = static_cast<CGuard*>(object);
	CStateMachine* stateMachine = static_cast<CStateMachine*>(guard->GetComponent(COMPONENT_TYPE::STATE_MACHINE));
	CTransform* transform = static_cast<CTransform*>(guard->GetComponent(COMPONENT_TYPE::TRANSFORM));

	CCharacter* target = guard->GetTarget();
	CTransform* targetTransform = static_cast<CTransform*>(target->GetComponent(COMPONENT_TYPE::TRANSFORM));

	guard->SetElapsedTime(guard->GetElapsedTime() + DT);

	// ���� Ÿ���� ü���� 0���ϰ� �Ǿ��ų�, ���Žð��� �ƴٸ� Ÿ���� �����Ѵ�.
	if ((target->GetHealth() <= 0) || (guard->GetElapsedTime() >= guard->GetTargetUpdateTime()))
	{
		guard->SetElapsedTime(0.0f);
		target = guard->FindTarget(40.0f, 200.0f);

		if (target != nullptr)
		{
			guard->SetTarget(target);
			guard->CreateMovePath(targetTransform->GetPosition());
		}
		else
		{
			// �÷��̾ �þ߰����� ������ �ʴ´ٸ� ReturnState�� �����Ͽ� ���� Patrol Node�� ��ġ�� �̵��Ѵ�.
			stateMachine->ChangeState(CGuardReturnState::GetInstance());
		}
	}
	else
	{
		XMFLOAT3 position = transform->GetPosition();
		XMFLOAT3 toTarget = Vector3::Subtract(targetTransform->GetPosition(), position);
		float dist = Vector3::Length(toTarget);

		// Ÿ�ٰ��� �Ÿ��� 10.0f���϶��, ������ ��� ����.
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

			// ��� �������� ������ ���� �ʾҴٸ�, Ÿ���� ���ϵ��� ȸ���� �� ���� ��� ShootState�� �����Ѵ�.
			if (!isHit)
			{
				XMFLOAT3 forward = transform->GetForward();
				float angle = Vector3::Angle(forward, toTarget);

				// �� ���͸� �����Ͽ� ȸ������ ���� �ּ� ȸ�� �������� ȸ����Ų��.
				float axis = Vector3::CrossProduct(forward, toTarget).y;

				if (axis < 0.0f)
				{
					angle = -angle;
				}

				transform->Rotate(XMFLOAT3(0.0f, angle, 0.0f));
				stateMachine->ChangeState(CGuardShootState::GetInstance());
			}
		}
		else
		{
			guard->FollowMovePath(15000.0f * DT);

			// targetPosition�� ��ġ�ߴٸ�, �ٽ� Ÿ���� ��ġ�� ��θ� �����.
			if (guard->IsFinishedMovePath())
			{
				guard->CreateMovePath(targetTransform->GetPosition());
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
	CGuard* guard = static_cast<CGuard*>(object);
	CRigidBody* rigidBody = static_cast<CRigidBody*>(guard->GetComponent(COMPONENT_TYPE::RIGIDBODY));
	CAnimator* animator = static_cast<CAnimator*>(guard->GetComponent(COMPONENT_TYPE::ANIMATOR));
	CTransform* transform = static_cast<CTransform*>(guard->GetComponent(COMPONENT_TYPE::TRANSFORM));

	guard->SetTarget(nullptr);
	guard->CreateMovePath(guard->GetNextPatrolPosition());
	rigidBody->SetMaxSpeedXZ(300.0f);
	rigidBody->AddVelocity(Vector3::ScalarProduct(transform->GetForward(), 300.0f * DT));
	animator->Play("Walking", true);
}

void CGuardReturnState::Exit(CObject* object)
{
}

void CGuardReturnState::Update(CObject* object)
{
	CGuard* guard = static_cast<CGuard*>(object);
	CStateMachine* stateMachine = static_cast<CStateMachine*>(guard->GetComponent(COMPONENT_TYPE::STATE_MACHINE));

	CCharacter* target = guard->FindTarget(30.0f, 75.0f);

	if (target != nullptr)
	{
		// Ÿ���� �����Ǹ�, ChaseState�� �����Ѵ�.
		guard->SetTarget(target);
		stateMachine->ChangeState(CGuardChaseState::GetInstance());
	}
	else
	{
		guard->FollowMovePath(10000.0f * DT);

		// PatrolPath�� ��ġ�ߴٸ�, PatrolState�� �����Ѵ�.
		if (guard->IsFinishedMovePath())
		{
			stateMachine->ChangeState(CGuardPatrolState::GetInstance());
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
	CRigidBody* rigidBody = static_cast<CRigidBody*>(object->GetComponent(COMPONENT_TYPE::RIGIDBODY));
	CAnimator* animator = static_cast<CAnimator*>(object->GetComponent(COMPONENT_TYPE::ANIMATOR));
	CTransform* transform = static_cast<CTransform*>(object->GetComponent(COMPONENT_TYPE::TRANSFORM));

	rigidBody->SetMaxSpeedXZ(500.0f);
	rigidBody->AddVelocity(Vector3::ScalarProduct(transform->GetForward(), 500.0f * DT));
	animator->Play("Pistol_Run", true);
}

void CGuardAssembleState::Exit(CObject* object)
{
}

void CGuardAssembleState::Update(CObject* object)
{
	CGuard* guard = static_cast<CGuard*>(object);
	CStateMachine* stateMachine = static_cast<CStateMachine*>(guard->GetComponent(COMPONENT_TYPE::STATE_MACHINE));

	CCharacter* target = guard->FindTarget(30.0f, 75.0f);

	if (target != nullptr)
	{
		// Ÿ���� �����Ǹ�, ChaseState�� �����Ѵ�.
		guard->SetTarget(target);
		stateMachine->ChangeState(CGuardChaseState::GetInstance());
	}
	else
	{
		guard->FollowMovePath(15000.0f * DT);

		// PatrolPath�� ��ġ�ߴٸ�, PatrolState�� �����Ѵ�.
		if (guard->IsFinishedMovePath())
		{
			stateMachine->ChangeState(CGuardPatrolState::GetInstance());
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
	CAnimator* animator = static_cast<CAnimator*>(object->GetComponent(COMPONENT_TYPE::ANIMATOR));

	animator->Play("Shooting", false, true);

	// ShootState�� �����ߴٸ�, �ǰ� UI�� �ִϸ��̼��� �����Ų��.
	const vector<CObject*>& uis = CSceneManager::GetInstance()->GetCurrentScene()->GetGroupObject(GROUP_TYPE::UI);
	CObject* hitUI = uis[0]->FindFrame("HitUI");
	CStateMachine* stateMachine = static_cast<CStateMachine*>(hitUI->GetComponent(COMPONENT_TYPE::STATE_MACHINE));

	stateMachine->SetCurrentState(CHitUIFadeState::GetInstance());
}

void CGuardShootState::Exit(CObject* object)
{
}

void CGuardShootState::Update(CObject* object)
{
	CGuard* guard = static_cast<CGuard*>(object);
	CAnimator* animator = static_cast<CAnimator*>(guard->GetComponent(COMPONENT_TYPE::ANIMATOR));

	if (animator->IsFinished())
	{
		CStateMachine* stateMachine = static_cast<CStateMachine*>(guard->GetComponent(COMPONENT_TYPE::STATE_MACHINE));
		CTransform* transform = static_cast<CTransform*>(guard->GetComponent(COMPONENT_TYPE::TRANSFORM));

		CCharacter* target = guard->FindTarget(30.0f, 200.0f);

		if (target != nullptr)
		{
			guard->SetTarget(target);

			CTransform* targetTransform = static_cast<CTransform*>(target->GetComponent(COMPONENT_TYPE::TRANSFORM));
			XMFLOAT3 toTarget = Vector3::Subtract(targetTransform->GetPosition(), transform->GetPosition());
			float dist = Vector3::Length(toTarget);

			// Ÿ�ٰ��� �Ÿ��� 10.0f���϶��, �ٽ� Ÿ���� ���� ȸ���� �� ���� ��� �ƴ� ��� ChaseState�� �����Ѵ�.
			if (dist <= 10.0f)
			{
				toTarget = Vector3::Normalize(toTarget);

				XMFLOAT3 forward = transform->GetForward();
				float angle = Vector3::Angle(forward, toTarget);

				// �� ���͸� �����Ͽ� ȸ������ ���� �ּ� ȸ�� �������� ȸ����Ų��.
				float axis = Vector3::CrossProduct(forward, toTarget).y;

				if (axis < 0.0f)
				{
					angle = -angle;
				}

				transform->Rotate(XMFLOAT3(0.0f, angle, 0.0f));
				stateMachine->GetCurrentState()->Enter(guard);
			}
			else
			{
				stateMachine->ChangeState(CGuardChaseState::GetInstance());
			}
		}
		else
		{
			// Ÿ���� ���ٸ�, ReturnState�� �����Ѵ�.
			stateMachine->ChangeState(CGuardReturnState::GetInstance());
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
	CAnimator* animator = static_cast<CAnimator*>(object->GetComponent(COMPONENT_TYPE::ANIMATOR));

	animator->Play("Head_Hit", false);
}

void CGuardHitState::Exit(CObject* object)
{
}

void CGuardHitState::Update(CObject* object)
{
	CGuard* guard = static_cast<CGuard*>(object);
	CStateMachine* stateMachine = static_cast<CStateMachine*>(guard->GetComponent(COMPONENT_TYPE::STATE_MACHINE));
	CAnimator* animator = static_cast<CAnimator*>(guard->GetComponent(COMPONENT_TYPE::ANIMATOR));

	if (guard->GetHealth() <= 0)
	{
		stateMachine->ChangeState(CGuardDieState::GetInstance());
	}
	else if (animator->IsFinished())
	{
		stateMachine->ChangeState(CGuardChaseState::GetInstance());
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
	CAnimator* animator = static_cast<CAnimator*>(object->GetComponent(COMPONENT_TYPE::ANIMATOR));

	animator->Play("Standing_React_Death_Backward", false);
}

void CGuardDieState::Exit(CObject* object)
{
}

void CGuardDieState::Update(CObject* object)
{
}
