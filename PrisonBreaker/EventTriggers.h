#pragma once
#include "EventTrigger.h"

class COpenDoorEventTrigger : public CEventTrigger
{
private:
	float m_MaxRotationAngle{};

public:
	COpenDoorEventTrigger() = default;
	virtual ~COpenDoorEventTrigger() = default;

	virtual void GenerateEventTrigger(float ElapsedTime);
};

//=========================================================================================================================

class CPowerDownEventTrigger : public CEventTrigger
{
public:
	CPowerDownEventTrigger() = default;
	virtual ~CPowerDownEventTrigger() = default;

	virtual void GenerateEventTrigger(float ElapsedTime);
};

//=========================================================================================================================

class CSirenEventTrigger : public CEventTrigger
{
public:
	CSirenEventTrigger() = default;
	virtual ~CSirenEventTrigger() = default;

	virtual void GenerateEventTrigger(float ElapsedTime);
};
