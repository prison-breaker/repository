#include "pch.h"
#include "Collider.h"

#include "Object.h"

#include "Mesh.h"

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
    if (!m_isEnabled)
    {
        return;
    }

    m_owner->GetMesh()->GetBoundingBox().Transform(m_boundingBox, XMLoadFloat4x4(&m_owner->GetWorldMatrix()));
}
