#include "stdafx.h"
#include "Player.h"

void CPlayer::SetCamera(const shared_ptr<CCamera>& Camera)
{
	m_Camera = Camera;
}

CCamera* CPlayer::GetCamera() const
{
	return m_Camera.get();
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
