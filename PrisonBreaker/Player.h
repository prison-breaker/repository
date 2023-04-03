#pragma once
#include "Character.h"

class CPlayer : public CCharacter
{
private:
	XMFLOAT3 m_rotation;

	bool	 m_isAiming;

	//bool     m_hasKey;

public:
	CPlayer();
	virtual ~CPlayer();

	const XMFLOAT3& GetRotation();

	void SetAiming(bool isAiming);
	bool IsAiming();

	virtual void Init();

	virtual void OnCollisionEnter(CObject* collidedObject);
	virtual void OnCollision(CObject* collidedObject);
	virtual void OnCollisionExit(CObject* collidedObject);

	virtual void Update();

	void Punch();
	void Shoot();

	//void ManagePistol(bool HasPistol);
	//bool HasPistol() const;
	//bool IsEquippedPistol() const;

	//void ManageKey(bool HasKey);
	//bool HasKey() const;

	//bool SwapWeapon(WEAPON_TYPE WeaponType);

	//void Rotate(float Pitch, float Yaw, float Roll, float ElapsedTime, float NearestHitDistance);

	//void IsCollidedByEventTrigger(const XMFLOAT3& NewPosition);
};
