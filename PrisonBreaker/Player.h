#pragma once
#include "GameObject.h"

class CPlayer : public CGameObject
{
private:
	UINT							   m_Health{};
	
	float                              m_Pitch{};
	float                              m_Yaw{};
	float                              m_Roll{};
							           
	shared_ptr<CCamera>                m_Camera{};
							           
	shared_ptr<CGameObject>            m_Weapon{};

	shared_ptr<CStateMachine<CPlayer>> m_StateMachine{};

public:
	CPlayer(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	virtual ~CPlayer() = default;

	void SetHealth(UINT Health);
	UINT GetHealth() const;

	void SetCamera(const shared_ptr<CCamera>& Camera);
	CCamera* GetCamera() const;

	CStateMachine<CPlayer>* GetStateMachine() const;

	void AcquirePistol();
	bool SwapWeapon(WEAPON_TYPE WeaponType);

	void Rotate(float Pitch, float Yaw, float Roll, float ElapsedTime);
};
