#pragma once
#include "Component.h"

class CCollider : public CComponent
{
	friend class CObject;

private:
	BoundingBox m_boundingBox;

private:
	CCollider();
	virtual ~CCollider();

	void SetBoundingBox(const BoundingBox& boundingBox);
	const BoundingBox& GetBoundingBox();

	virtual void Update();
};
