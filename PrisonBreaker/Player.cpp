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
	m_Health = Health;
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

void CPlayer::Rotate(float Pitch, float Yaw, float Roll, float ElapsedTime)
{
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

	m_Camera->Rotate(GetWorldMatrix(), ElapsedTime);

	if (!Math::IsZero(Yaw))
	{
		XMFLOAT4X4 RotationMatrix{ Matrix4x4::RotationAxis(GetUp(), Yaw) };

		SetRight(Vector3::TransformNormal(GetRight(), RotationMatrix));
		SetLook(Vector3::TransformNormal(GetLook(), RotationMatrix));
	}

	CGameObject::UpdateLocalCoord(Vector3::Normalize(GetLook()));
}

void CPlayer::ProcessInput(UINT InputMask, float ElapsedTime, const shared_ptr<CNavMesh>& NavMesh)
{
	if (m_StateMachine)
	{
		m_StateMachine->ProcessInput(InputMask, ElapsedTime);
	}

	XMFLOAT3 NewPosition{ Vector3::Add(GetPosition(), Vector3::ScalarProduct(m_Speed * ElapsedTime, m_MovingDirection, false)) };

	if (IsInNavMesh(NavMesh, NewPosition))
	{
		SetPosition(NewPosition);
	}
}
