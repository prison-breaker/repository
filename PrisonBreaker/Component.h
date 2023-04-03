#pragma once
#include "Object.h"

class CCamera;

class CComponent
{
	friend class CObject;

protected:
	CObject* m_owner;

protected:
	CComponent();
	virtual ~CComponent();

public:
	CObject* GetOwner();

	virtual void Update() = 0;
};
