#include "pch.h"
#include "Collider.h"

#include "Object.h"

#include "Mesh.h"

#include "Transform.h"

CCollider::CCollider() :
    m_boundingBox()
{
}

CCollider::~CCollider()
{
}

void CCollider::SetBoundingBox(const BoundingBox& boundingBox)
{
    m_boundingBox = boundingBox;
}

const BoundingBox& CCollider::GetBoundingBox()
{
    return m_boundingBox;
}

void CCollider::Update()
{
    if (m_isEnabled)
    {
        CTransform* transform = static_cast<CTransform*>(m_owner->GetComponent(COMPONENT_TYPE::TRANSFORM));

        m_owner->GetMesh()->GetBoundingBox().Transform(m_boundingBox, XMLoadFloat4x4(&transform->GetWorldMatrix()));
    }
}
