#include "stdafx.h"
#include "Player.h"

CPlayer::CPlayer(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	// 카메라 객체를 생성한다.
	m_Camera = make_shared<CCamera>();
	m_Camera->CreateShaderVariables(D3D12Device, D3D12GraphicsCommandList);
	m_Camera->GeneratePerspectiveProjectionMatrix(60.0f, static_cast<float>(CLIENT_WIDTH) / static_cast<float>(CLIENT_HEIGHT), 1.0f, 500.0f);
	m_Camera->GenerateViewMatrix(XMFLOAT3(0.0f, 5.0f, -150.0f), XMFLOAT3(0.0f, 0.0f, 1.0f));
}

void CPlayer::SetHealth(UINT Health)
{
	m_Health = Health;
}

UINT CPlayer::GetHealth() const
{
	return m_Health;
}

void CPlayer::SetCamera(const shared_ptr<CCamera>& Camera)
{
	m_Camera = Camera;
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
	shared_ptr<CGameObject> PistolFrame{ FindFrame(TEXT("gun_pr_1")) };

	if (PistolFrame)
	{
		m_Weapon = PistolFrame;
	}
}

bool CPlayer::SwapWeapon(WEAPON_TYPE WeaponType)
{
	if (m_Weapon)
	{
		switch (WeaponType)
		{
		case WEAPON_TYPE_PUNCH:
			m_Weapon->SetActive(false);
			return true;
		case WEAPON_TYPE_PISTOL:
			m_Weapon->SetActive(true);
			return true;
		}
	}

	return false;
}

void CPlayer::Rotate(float Pitch, float Yaw, float Roll, float ElapsedTime)
{
	if (!Math::IsZero(Pitch))
	{
		m_Pitch += Pitch;

		if (m_Pitch > 10.0f)
		{
			Pitch -= (m_Pitch - 10.0f);
			m_Pitch = 10.0f;
		}

		if (m_Pitch < -5.0f)
		{
			Pitch -= (m_Pitch + 10.0f);
			m_Pitch = -10.0f;
		}
	}

	if (!Math::IsZero(Yaw))
	{
		m_Yaw += Yaw;

		if (m_Yaw > 360.0f)
		{
			m_Yaw -= 360.0f;
		}

		if (m_Yaw < 0.0f)
		{
			m_Yaw += 360.0f;
		}
	}

	m_Camera->Rotate(GetWorldMatrix(), ElapsedTime);

	//if (!Math::IsZero(Pitch))
	//{
	//	XMFLOAT4X4 RotationMatrix = Matrix4x4::RotationAxis(GetRight(), Pitch);

	//	SetUp(Vector3::TransformNormal(GetUp(), RotationMatrix));
	//	SetLook(Vector3::TransformNormal(GetLook(), RotationMatrix));
	//}

	if (!Math::IsZero(Yaw))
	{
		XMFLOAT4X4 RotationMatrix{ Matrix4x4::RotationAxis(GetUp(), Yaw) };

		SetRight(Vector3::TransformNormal(GetRight(), RotationMatrix));
		SetLook(Vector3::TransformNormal(GetLook(), RotationMatrix));
	}

	if (!Math::IsZero(Roll))
	{
		XMFLOAT4X4 RotationMatrix{ Matrix4x4::RotationAxis(GetLook(), Roll) };

		SetRight(Vector3::TransformNormal(GetRight(), RotationMatrix));
		SetUp(Vector3::TransformNormal(GetUp(), RotationMatrix));
	}

	const XMFLOAT3 WorldUp{ 0.0f, 1.0f, 0.0f };

	SetLook(Vector3::Normalize(GetLook()));
	SetRight(Vector3::CrossProduct(WorldUp, GetLook(), false));
	SetUp(Vector3::CrossProduct(GetLook(), GetRight(), false));
}
