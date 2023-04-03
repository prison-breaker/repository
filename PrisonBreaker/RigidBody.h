#pragma once
#include "Component.h"

class CRigidBody : public CComponent
{
	friend class CObject;

private:
	float    m_mass;           // 질량
	XMFLOAT3 m_force;		   // 한 프레임동안 누적된 힘
	XMFLOAT3 m_velocity;	   // 속도
	float    m_maxSpeedXZ;	   // XZ축 최대 속력
	float    m_maxSpeedY;      // Y축 최대 속력
	XMFLOAT3 m_accel;		   // 가속도
	float    m_frictionCoeffX; // X축 마찰계수
	float    m_frictionCoeffZ; // Z축 마찰계수

private:
	CRigidBody();
	virtual ~CRigidBody();

	void MovePosition();

public:
	void SetMass(float mass);
	float GetMass();

	const XMFLOAT3& GetVelocity();

	void SetMaxSpeedXZ(float maxSpeedXZ);
	float GetMaxSpeedXZ();

	void SetMaxSpeedY(float maxSpeedY);
	float GetMaxSpeedY();

	float GetSpeedXZ();
	float GetSpeedY();

	void SetFrictionCoeffX(float frictionCoeffX);
	float GetFrictionCoeffX();

	void SetFrictionCoeffZ(float frictionCoeffZ);
	float GetFrictionCoeffZ();

	void AddForce(const XMFLOAT3& force);
	void AddVelocity(const XMFLOAT3& velocity);

	virtual void Update();
};
