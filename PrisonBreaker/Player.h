#pragma once
#include "GameObject.h"

class CPlayer : public CGameObject
{
private:
	float                   m_Pitch{};
	float                   m_Yaw{};
	float                   m_Roll{};

	shared_ptr<CCamera>     m_Camera{};

	shared_ptr<CGameObject> m_Weapon{};

public:
	CPlayer() = default;
	virtual ~CPlayer() = default;

	void SetCamera(const shared_ptr<CCamera>& Camera);
	CCamera* GetCamera() const;

	void AcquirePistol();
	bool SwapWeapon(WEAPON_TYPE WeaponType);

	void Rotate(float Pitch, float Yaw, float Roll, float ElapsedTime);
};
