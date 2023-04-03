#include "pch.h"
#include "Player.h"

#include "Core.h"

#include "TimeManager.h"
#include "InputManager.h"
#include "CameraManager.h"
#include "SceneManager.h"

#include "Scene.h"

#include "Guard.h"

#include "Camera.h"

#include "StateMachine.h"

#include "PlayerStates.h"
#include "GuardStates.h"

CPlayer::CPlayer() :
	m_rotation(),
	m_isAiming()
{
}

CPlayer::~CPlayer()
{
}

const XMFLOAT3& CPlayer::GetRotation()
{
	return m_rotation;
}

void CPlayer::SetAiming(bool isAiming)
{
	m_isAiming = isAiming;
}

bool CPlayer::IsAiming()
{
	return m_isAiming;
}

void CPlayer::Init()
{
	// ���� ��, ����ִ� ������ ��Ȱ��ȭ ��Ų��.
	CObject* weapon = FindFrame("gun_pr_1");

	SetWeapon(weapon);
	//weapon->SetActive(false);

	GetStateMachine()->SetCurrentState(CPlayerIdleState::GetInstance());
}

void CPlayer::OnCollisionEnter(CObject* collidedObject)
{
}

void CPlayer::OnCollision(CObject* collidedObject)
{
}

void CPlayer::OnCollisionExit(CObject* collidedObject)
{
}

void CPlayer::Update()
{
	// ���� �����찡 ��Ŀ�� �������� �˾Ƴ���.
	HWND hWnd = GetFocus();

	if (hWnd != nullptr)
	{
		// ������ ���� ���
		RECT rect = {};

		GetWindowRect(hWnd, &rect);

		// ���콺 Ŀ�� ��ġ ���
		POINT oldCursor = { (LONG)(rect.right / 2), (LONG)(rect.bottom / 2) };
		POINT cursor = {};

		// �� �Լ��� ������ ��ü ������ �������� Ŀ���� ��ġ�� ����Ѵ�.
		GetCursorPos(&cursor);

		XMFLOAT2 delta = {};

		delta.x = (cursor.x - oldCursor.x) * 20.0f * DT;
		delta.y = (cursor.y - oldCursor.y) * 20.0f * DT;

		SetCursorPos(oldCursor.x, oldCursor.y);

		// X�� ȸ��(ī�޶�)
		if (!Math::IsZero(delta.y))
		{
			m_rotation.x += delta.y;

			if (m_rotation.x < -15.0f)
			{
				m_rotation.x = -15.0f;
			}
			else if (m_rotation.x > 15.0f)
			{
				m_rotation.x = 15.0f;
			}
		}

		// Y�� ȸ��(�÷��̾�� ī�޶� ���)
		if (!Math::IsZero(delta.x))
		{
			m_rotation.y += delta.x;

			if (m_rotation.y < 0.0f)
			{
				m_rotation.y += 360.0f;
			}
			else if (m_rotation.y > 360.0f)
			{
				m_rotation.y -= 360.0f;
			}

			XMFLOAT4X4 rotationMatrix = Matrix4x4::RotationAxis(GetUp(), delta.x);

			SetRight(Vector3::TransformNormal(GetRight(), rotationMatrix));
			SetForward(Vector3::TransformNormal(GetForward(), rotationMatrix));
		}
	}

	if (KEY_TAP(KEY::NUM1))
	{
		if (IsEquippedWeapon())
		{
			SwapWeapon(WEAPON_TYPE::PUNCH);
		}
	}

	if (KEY_TAP(KEY::NUM2))
	{
		if (!IsEquippedWeapon())
		{
			SwapWeapon(WEAPON_TYPE::PISTOL);
		}
	}

	CObject::Update();
}

void CPlayer::Punch()
{
	const vector<CObject*>& guards = CSceneManager::GetInstance()->GetCurrentScene()->GetGroupObject(GROUP_TYPE::ENEMY);

	for (const auto& object : guards)
	{
		CGuard* guard = (CGuard*)object;

		if (guard->GetHealth() > 0)
		{
			XMFLOAT3 toGuard = Vector3::Subtract(guard->GetPosition(), GetPosition());
			float dist = Vector3::Length(toGuard);

			if (dist <= 3.0f)
			{
				toGuard = Vector3::Normalize(toGuard);

				XMFLOAT3 forward = GetForward();
				float angle = Vector3::Angle(forward, toGuard);

				// forward ���Ϳ� toGaurd ���Ͱ� �̷�� ���� 80�� ������ �� �����Ѵ�.
				if (angle <= 80.0f)
				{
					// �÷��̾��� forward ���Ϳ� �������� forward ���� ���� ���� 40�� ������ �� �ĸ� Ÿ��(���)���� ó���Ѵ�.
					angle = Vector3::Angle(forward, guard->GetForward());

					if (angle <= 40.0f)
					{
						guard->SetHealth(0);
						guard->GetStateMachine()->ChangeState(CGuardDieState::GetInstance());
					}
					else
					{
						guard->SetHealth(guard->GetHealth() - 40);
						guard->SetTarget(this);
						guard->GetStateMachine()->ChangeState(CGuardHitState::GetInstance());
					}
				}
			}
		}
	}
}

void CPlayer::Shoot()
{
	CObject* nearestIntersectedRootObject = nullptr;
	CObject* nearestIntersectedObject = nullptr;
	float nearestHitDist = FLT_MAX;

	CCamera* camera = CCameraManager::GetInstance()->GetMainCamera();
	const XMFLOAT3& rayOrigin = camera->GetPosition();
	const XMFLOAT3& rayDirection = camera->GetFoward();

	for (int i = (int)GROUP_TYPE::STRUCTURE; i <= (int)GROUP_TYPE::ENEMY; ++i)
	{
		const vector<CObject*>& objects = CSceneManager::GetInstance()->GetCurrentScene()->GetGroupObject((GROUP_TYPE)i);

		for (const auto& object : objects)
		{
			if (object->IsActive())
			{
				if (i == (int)GROUP_TYPE::ENEMY)
				{
					CGuard* guard = (CGuard*)object;

					if (guard->GetHealth() <= 0)
					{
						continue;
					}
				}

				float hitDist = 0.0f, maxDist = 400.0f;
				CObject* intersectedObject = object->CheckRayIntersection(rayOrigin, rayDirection, hitDist, maxDist);

				if ((intersectedObject != nullptr) && (hitDist < nearestHitDist))
				{
					nearestIntersectedRootObject = object;
					nearestIntersectedObject = intersectedObject;
					nearestHitDist = hitDist;
				}
			}
		}

		if ((nearestIntersectedRootObject != nullptr) && (typeid(*nearestIntersectedRootObject) == typeid(CGuard)))
		{
			CGuard* guard = (CGuard*)nearestIntersectedRootObject;
			const string& hitFrameName = nearestIntersectedObject->GetName();

			// �Ӹ��� ���� ���, ����Ų��.
			if (hitFrameName == "hat" || hitFrameName == "head_1" || hitFrameName == "head_2")
			{
				guard->SetHealth(0);
				guard->GetStateMachine()->ChangeState(CGuardDieState::GetInstance());
			}
			else
			{
				guard->SetHealth(guard->GetHealth() - 70);
				guard->SetTarget(this);
				guard->GetStateMachine()->ChangeState(CGuardHitState::GetInstance());
			}
		}
	}
}
