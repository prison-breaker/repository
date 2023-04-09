#pragma once
#include "Component.h"

class CCollider : public CComponent
{
private:
	BoundingBox m_boundingBox;

public:
	CCollider();
	virtual ~CCollider();

	void SetBoundingBox(const BoundingBox& boundingBox);
	const BoundingBox& GetBoundingBox();

	virtual void Update();
};
