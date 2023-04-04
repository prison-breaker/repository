#pragma once
#include "Object.h"

class CCharacter : public CObject
{
private:
	int      m_health;

	CObject* m_weapon;

public:
	CCharacter();
	virtual ~CCharacter();

	void SetHealth(int health);
	int GetHealth();

	void SetWeapon(CObject* object);
	CObject* GetWeapon();

	bool IsEquippedWeapon();
	void SwapWeapon(WEAPON_TYPE weaponType);

	virtual void OnCollisionEnter(CObject* collidedObject);
	virtual void OnCollision(CObject* collidedObject);
	virtual void OnCollisionExit(CObject* collidedObject);
};
