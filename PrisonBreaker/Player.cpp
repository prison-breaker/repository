#include "stdafx.h"
#include "Player.h"
#include "State_Player.h"
#include "GameScene.h"
#include "Guard.h"
#include "StateMachine.h"
#include "EventTrigger.h"
#include "Camera.h"
#include "NavMesh.h"
#include "NavNode.h"

CPlayer::CPlayer(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	// 카메라 객체를 생성한다.
	m_Camera = make_shared<CCamera>();
	m_Camera->CreateShaderVariables(D3D12Device, D3D12GraphicsCommandList);
	m_Camera->GeneratePerspectiveProjectionMatrix(90.0f, static_cast<float>(CLIENT_WIDTH) / static_cast<float>(CLIENT_HEIGHT), 1.0f, 200.0f);
	m_Camera->GenerateViewMatrix(XMFLOAT3(0.0f, 5.0f, -150.0f), XMFLOAT3(0.0f, 0.0f, 1.0f));
}

void CPlayer::Initialize()
{
	CGameObject::Initialize();

	// 상태머신 객체를 생성한다.
	m_StateMachine = make_shared<CStateMachine<CPlayer>>(static_pointer_cast<CPlayer>(shared_from_this()));
	m_StateMachine->SetCurrentState(CPlayerIdleState::GetInstance());

	FindFrame(TEXT("gun_pr_1"))->SetActive(false);
}

void CPlayer::Reset(const XMFLOAT4X4& TransformMatrix)
{
	m_Health = 100;

	SwapWeapon(WEAPON_TYPE_PUNCH);
	ManagePistol(false);
	ManageKey(false);

	SetTransformMatrix(TransformMatrix);
	UpdateTransform(Matrix4x4::Identity());

	m_StateMachine->SetCurrentState(CPlayerIdleState::GetInstance());
	m_Camera->SetZoomIn(false);
}

void CPlayer::Animate(float ElapsedTime)
{
	if (IsActive())
	{
		if (m_StateMachine)
		{
			m_StateMachine->Update(ElapsedTime);
		}
	}
}

void CPlayer::SetID(UINT ID)
{
	m_ID = ID;
}

UINT CPlayer::GetID() const
{
	return m_ID;
}

void CPlayer::SetHealth(UINT Health)
{
	// UINT UnderFlow
	if (Health > 100)
	{
		m_Health = 0;
	}
	else
	{
		m_Health = Health;
	}
}

UINT CPlayer::GetHealth() const
{
	return m_Health;
}

void CPlayer::SetSpeed(float Speed)
{
	m_Speed = Speed;
}

float CPlayer::GetSpeed() const
{
	return m_Speed;
}

void CPlayer::SetMovingDirection(const XMFLOAT3& MovingDirection)
{
	m_MovingDirection = MovingDirection;
}

const XMFLOAT3& CPlayer::GetMovingDirection() const
{
	return m_MovingDirection;
}

shared_ptr<CCamera> CPlayer::GetCamera() const
{
	return m_Camera;
}

shared_ptr<CStateMachine<CPlayer>> CPlayer::GetStateMachine() const
{
	return m_StateMachine;
}

void CPlayer::ManagePistol(bool HasPistol)
{
	if (HasPistol)
	{
		m_PistolFrame = FindFrame(TEXT("gun_pr_1"));
	}
	else
	{
		m_PistolFrame = nullptr;
	}
}

bool CPlayer::HasPistol() const
{
	return (m_PistolFrame) ? true : false;
}

bool CPlayer::IsEquippedPistol() const
{
	if (m_PistolFrame)
	{
		return m_PistolFrame->IsActive();
	}

	return false;
}

void CPlayer::ManageKey(bool HasKey)
{
	m_HasKey = HasKey;
}

bool CPlayer::HasKey() const
{
	return m_HasKey;
}

bool CPlayer::SwapWeapon(WEAPON_TYPE WeaponType)
{
	bool IsSwapped{};

	if (m_PistolFrame)
	{
		switch (WeaponType)
		{
		case WEAPON_TYPE_PUNCH:
			m_PistolFrame->SetActive(false);
			IsSwapped = true;
			break;
		case WEAPON_TYPE_PISTOL:
			m_PistolFrame->SetActive(true);
			IsSwapped = true;
			break;
		}
	}

	return IsSwapped;
}

void CPlayer::Rotate(float Pitch, float Yaw, float Roll, float ElapsedTime, float NearestHitDistance)
{
	if (!Math::IsZero(Pitch))
	{
		m_Rotation.x += Pitch;

		if (m_Rotation.x > 15.0f)
		{
			Pitch -= (Pitch - 15.0f);
			m_Rotation.x = 15.0f;

		}

		if (m_Rotation.x < -15.0f)
		{
			Pitch -= (Pitch + 15.0f);
			m_Rotation.x = -15.0f;
		}
	}

	if (!Math::IsZero(Yaw))
	{
		m_Rotation.y += Yaw;

		if (m_Rotation.y > 360.0f)
		{
			m_Rotation.y -= 360.0f;
		}

		if (m_Rotation.y < 0.0f)
		{
			m_Rotation.y += 360.0f;
		}
	}

	if (!Math::IsZero(Yaw))
	{
		XMFLOAT4X4 RotationMatrix{ Matrix4x4::RotationAxis(GetUp(), Yaw) };

		SetRight(Vector3::TransformNormal(GetRight(), RotationMatrix));
		SetLook(Vector3::TransformNormal(GetLook(), RotationMatrix));
	}

	CGameObject::UpdateLocalCoord(Vector3::Normalize(GetLook()));

	XMFLOAT4X4 RotationMatrix{ Matrix4x4::Multiply(Matrix4x4::RotationYawPitchRoll(m_Rotation.x, 0.0f, 0.0f), GetWorldMatrix()) };
	
	m_Camera->Rotate(RotationMatrix, ElapsedTime, NearestHitDistance);
}

void CPlayer::IsCollidedByEventTrigger(const XMFLOAT3& NewPosition)
{
	vector<shared_ptr<CEventTrigger>>& EventTriggers{ static_pointer_cast<CGameScene>(CSceneManager::GetInstance()->GetScene("GameScene"))->GetEventTriggers() };
	UINT TriggerCount{ static_cast<UINT>(EventTriggers.size()) };

	for (UINT i = 0; i < TriggerCount; ++i)
	{
		if (EventTriggers[i])
		{
			// 0 ~ 1: Key EventTriggers
			// 열쇠를 가지고 있는 경우, 해당 트리거는 건너뛴다.
			if (i <= 1)
			{
				if (m_HasKey)
				{
					continue;
				}
			}

			if (EventTriggers[i]->IsInTriggerArea(GetPosition(), GetLook()))
			{
				return;
			}
		}
	}

	// 반복문을 모두 돌았다면, 플레이어는 트리거 영역안에 없는 것이므로 상호작용 UI를 렌더링되지 않도록 만든다.
	// 이때, 모든 트리거는 상호작용 UI(m_InteractionUI)를 공유하여 사용하므로 0번 트리거를 이용하여 함수를 호출하였다.
	EventTriggers[0]->HideInteractionUI();
}
