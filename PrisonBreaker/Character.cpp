#include "pch.h"
#include "Character.h"

#include "TimeManager.h"

#include "RigidBody.h"

CCharacter::CCharacter() :
    m_health(100),
    m_weapon()
{
    CreateComponent(COMPONENT_TYPE::STATE_MACHINE);
    CreateComponent(COMPONENT_TYPE::RIGIDBODY);
}

CCharacter::~CCharacter()
{
}

void CCharacter::SetHealth(int health)
{
    m_health = clamp(health, 0, 100);
}

int CCharacter::GetHealth()
{
    return m_health;
}

void CCharacter::SetWeapon(CObject* object)
{
    m_weapon = object;
}

CObject* CCharacter::GetWeapon()
{
    return m_weapon;
}

bool CCharacter::IsEquippedWeapon()
{
    // 무기를 소유하고 있다면, 활성화 여부에 따라 장착 여부가 결정된다.
    if (m_weapon != nullptr)
    {
        return m_weapon->IsActive();
    }

    // 무기가 없다면 false를 반환한다.
    return false;
}

void CCharacter::SwapWeapon(WEAPON_TYPE weaponType)
{
    if (m_weapon != nullptr)
    {
        switch (weaponType)
        {
        case WEAPON_TYPE::PUNCH:
            m_weapon->SetActive(false);
            break;
        case WEAPON_TYPE::PISTOL:
            m_weapon->SetActive(true);
            break;
        }
    }
}

void CCharacter::OnCollisionEnter(CObject* collidedObject)
{
}

void CCharacter::OnCollision(CObject* collidedObject)
{
    CRigidBody* rigidBody = static_cast<CRigidBody*>(GetComponent(COMPONENT_TYPE::RIGIDBODY));
    float speedXZ = rigidBody->GetSpeedXZ();

    if (!Math::IsZero(speedXZ))
    {
        float angle = Vector3::Angle(GetForward(), collidedObject->GetForward());
        XMFLOAT3 shift = {};

        if (angle < 90.0f)
        {
            // 이 객체가 충돌한 객체보다 앞에 있는 객체인지 판별한다.
            XMFLOAT3 toCollidedObject = Vector3::Normalize(Vector3::Subtract(collidedObject->GetPosition(), GetPosition()));

            // 두 벡터의 각이 둔각이면, 앞에 있는 객체이다.
            angle = Vector3::Angle(GetForward(), toCollidedObject);

            // 뒤에 있는 객체일 경우에만, 뒤로 밀어준다.
            if (angle <= 90.0f)
            {
                shift = Vector3::ScalarProduct(speedXZ * DT, Vector3::Inverse(collidedObject->GetForward()), false);
            }
        }
        else if (angle > 90.0f)
        {
            shift = Vector3::ScalarProduct(speedXZ * DT, collidedObject->GetForward(), false);
        }

        XMFLOAT3 newPosition = Vector3::Add(GetPosition(), shift);

        SetPosition(newPosition);
    }
}

void CCharacter::OnCollisionExit(CObject* collidedObject)
{
}
