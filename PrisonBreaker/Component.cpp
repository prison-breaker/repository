#include "pch.h"
#include "Component.h"

CComponent::CComponent() :
	m_owner()
{
}

CComponent::~CComponent()
{
}

CObject* CComponent::GetOwner()
{
	return m_owner;
}
