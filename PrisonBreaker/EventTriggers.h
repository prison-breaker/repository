#pragma once
#include "EventTrigger.h"

class COpenDoorEventTrigger : public CEventTrigger
{
private:
	float m_DoorAngle{};

public:
	COpenDoorEventTrigger() = default;
	virtual ~COpenDoorEventTrigger() = default;

	virtual void GenerateEventTrigger(float ElapsedTime);
};

//=========================================================================================================================

class CPowerDownEventTrigger : public CEventTrigger
{
private:
	float m_PanelAngle{};

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

//=========================================================================================================================

class COpenGateEventTrigger : public CEventTrigger
{
private:
	float m_DoorAngle{};

public:
	COpenGateEventTrigger() = default;
	virtual ~COpenGateEventTrigger() = default;

	virtual void GenerateEventTrigger(float ElapsedTime);
};
