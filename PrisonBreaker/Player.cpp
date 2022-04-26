#include "stdafx.h"
#include "Player.h"

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

CCamera* CPlayer::GetCamera() const
{
	return m_Camera.get();
}

CStateMachine<CPlayer>* CPlayer::GetStateMachine() const
{
	return m_StateMachine.get();
}

void CPlayer::AcquirePistol()
{
	m_PistolFrame = FindFrame(TEXT("gun_pr_1"));
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

void CPlayer::ProcessInput(const vector<vector<shared_ptr<CGameObject>>>& GameObjects, const shared_ptr<CNavMesh>& NavMesh, float ElapsedTime, UINT InputMask)
{
	if (m_StateMachine)
	{
		m_StateMachine->ProcessInput(ElapsedTime, InputMask);
	}

	XMFLOAT3 NewPosition{ Vector3::Add(GetPosition(), Vector3::ScalarProduct(m_Speed * ElapsedTime, m_MovingDirection, false)) };

	if (IsInNavMesh(NavMesh, NewPosition))
	{
		bool IsCollision{};

		for (const auto& Guard : GameObjects[OBJECT_TYPE_NPC])
		{
			if (Guard->IsActive())
			{
				if (Math::Distance(Guard->GetPosition(), NewPosition) < 2.0f)
				{
					IsCollision = true;
					break;
				}
			}
		}

		if (!IsCollision)
		{
			SetPosition(NewPosition);
		}
	}
}
