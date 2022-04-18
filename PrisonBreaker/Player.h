#pragma once
#include "GameObject.h"
#include "State_Player.h"

class CPlayer : public CGameObject
{
private:
	XMFLOAT3						   m_Rotation{};

	UINT							   m_Health{};

	float							   m_Speed{};
	XMFLOAT3						   m_MovingDirection{};

	shared_ptr<CGameObject>            m_PistolFrame{};
									   
	shared_ptr<CCamera>                m_Camera{};

	shared_ptr<CStateMachine<CPlayer>> m_StateMachine{};

public:
	CPlayer(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	virtual ~CPlayer() = default;

	virtual void Initialize();

	virtual void Animate(float ElapsedTime);

	void SetHealth(UINT Health);
	UINT GetHealth() const;

	void SetSpeed(float Speed);
	float GetSpeed() const;

	void SetMovingDirection(const XMFLOAT3& MovingDirection);
	const XMFLOAT3& GetMovingDirection() const;

	CCamera* GetCamera() const;

	CStateMachine<CPlayer>* GetStateMachine() const;

	void AcquirePistol();
	bool SwapWeapon(WEAPON_TYPE WeaponType);

	void Rotate(float Pitch, float Yaw, float Roll, float ElapsedTime, float NearestHitDistance);

	void ProcessInput(UINT InputMask, float ElapsedTime, const shared_ptr<CNavMesh>& NavMesh);
};
