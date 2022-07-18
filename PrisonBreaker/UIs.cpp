#include "stdafx.h"
#include "UIs.h"
#include "TitleScene.h"
#include "Framework.h"

void CBackgroundUI::Initialize()
{
	m_StateMachine = make_shared<CStateMachine<CBackgroundUI>>(static_pointer_cast<CBackgroundUI>(shared_from_this()));
	m_StateMachine->SetCurrentState(CBackgroundUIFadeState::GetInstance());
}

void CBackgroundUI::Animate(float ElapsedTime)
{
	if (IsActive())
	{
		if (m_StateMachine)
		{
			m_StateMachine->Update(ElapsedTime);
		}
	}
}

shared_ptr<CStateMachine<CBackgroundUI>> CBackgroundUI::GetStateMachine() const
{
	return m_StateMachine;
}

//=========================================================================================================================

void CMissionUI::Initialize()
{
	m_StateMachine = make_shared<CStateMachine<CMissionUI>>(static_pointer_cast<CMissionUI>(shared_from_this()));
	m_StateMachine->SetCurrentState(CMissionUIShowingState::GetInstance());
}

void CMissionUI::Reset()
{
	m_StateMachine->SetCurrentState(CMissionUIShowingState::GetInstance());
	m_UIAnimationController->SetKeyFrameIndex(0);
}

void CMissionUI::Animate(float ElapsedTime)
{
	if (IsActive())
	{
		if (m_StateMachine)
		{
			m_StateMachine->Update(ElapsedTime);
		}
	}
}

shared_ptr<CStateMachine<CMissionUI>> CMissionUI::GetStateMachine() const
{
	return m_StateMachine;
}

//=========================================================================================================================

void CKeyUI::Initialize()
{
	m_StateMachine = make_shared<CStateMachine<CKeyUI>>(static_pointer_cast<CKeyUI>(shared_from_this()));
}

void CKeyUI::Reset()
{
	m_StateMachine->SetCurrentState(nullptr);
	m_UIAnimationController->SetKeyFrameIndex(0);
}

void CKeyUI::Animate(float ElapsedTime)
{
	if (IsActive())
	{
		if (m_StateMachine)
		{
			m_StateMachine->Update(ElapsedTime);
		}
	}
}

shared_ptr<CStateMachine<CKeyUI>> CKeyUI::GetStateMachine() const
{
	return m_StateMachine;
}

//=========================================================================================================================

void CHitUI::Initialize()
{
	m_StateMachine = make_shared<CStateMachine<CHitUI>>(static_pointer_cast<CHitUI>(shared_from_this()));
}

void CHitUI::Reset()
{
	m_IsActive = false;
}

void CHitUI::Animate(float ElapsedTime)
{
	if (IsActive())
	{
		if (m_StateMachine)
		{
			m_StateMachine->Update(ElapsedTime);
		}
	}
}

shared_ptr<CStateMachine<CHitUI>> CHitUI::GetStateMachine() const
{
	return m_StateMachine;
}

//=========================================================================================================================

void CButtonUI::ProcessMouseMessage(UINT Message, const XMINT2& ScreenPosition, UINT RootFrameIndex)
{

}

void CButtonUI::SetButtonArea(UINT Index, const XMFLOAT4& Area)
{
	if (Index < 0 || Index >= 2)
	{
		return;
	}
	
	m_ButtonAreas[Index] = Area;
}

const XMFLOAT4& CButtonUI::GetButtonArea(UINT Index) const
{
	return m_ButtonAreas[Index];
}

//=========================================================================================================================

void CMainButtonUI::ProcessMouseMessage(UINT Message, const XMINT2& ScreenPosition, UINT RootFrameIndex)
{
	for (UINT i = 0; i < m_VertexCount; ++i)
	{
		if ((m_ButtonAreas[i].x <= ScreenPosition.x) && (ScreenPosition.x <= m_ButtonAreas[i].y) &&
			(m_ButtonAreas[i].z <= ScreenPosition.y) && (ScreenPosition.y <= m_ButtonAreas[i].w))
		{
			SetCellIndex(i, static_cast<float>(2 * i + 1));

			if (!m_IsMouseOver)
			{
				m_IsMouseOver = true;

				CSoundManager::GetInstance()->Play(SOUND_TYPE_BUTTON_OVER, 0.7f);
			}

			if (Message == WM_LBUTTONUP)
			{
				switch (i)
				{
				case 0:
				{
					vector<shared_ptr<CBilboardObject>>& BilboardObjects{ static_pointer_cast<CTitleScene>(CSceneManager::GetInstance()->GetScene("TitleScene"))->GetBilboardObjects() };

					BilboardObjects[3]->SetActive(true);
				}
					break;
				case 1:
					PostQuitMessage(0);
					break;
				}
			}
			break;
		}
		else
		{
			SetCellIndex(i, static_cast<float>(2 * i));

			if (i == m_VertexCount - 1)
			{
				m_IsMouseOver = false;
			}
		}
	}
}

//=========================================================================================================================

void CPanelButtonUI::ProcessMouseMessage(UINT Message, const XMINT2& ScreenPosition, UINT RootFrameIndex)
{
	for (UINT i = 0; i < m_VertexCount; ++i)
	{
		if ((m_ButtonAreas[i].x <= ScreenPosition.x) && (ScreenPosition.x <= m_ButtonAreas[i].y) &&
			(m_ButtonAreas[i].z <= ScreenPosition.y) && (ScreenPosition.y <= m_ButtonAreas[i].w))
		{
			SetCellIndex(i, static_cast<float>(3 - 2 * i));

			if (!m_IsMouseOver)
			{
				m_IsMouseOver = true;

				CSoundManager::GetInstance()->Play(SOUND_TYPE_BUTTON_OVER, 0.7f);
			}

			if (Message == WM_LBUTTONUP)
			{
				vector<shared_ptr<CBilboardObject>>& BilboardObjects{ static_pointer_cast<CTitleScene>(CSceneManager::GetInstance()->GetScene("TitleScene"))->GetBilboardObjects() };

				BilboardObjects[RootFrameIndex]->SetActive(false);

				switch (i)
				{
				case 1: // Connect Button
					BilboardObjects[RootFrameIndex + 1]->SetActive(true);

					CFramework::GetInstance()->ConnectServer();
					break;
				}
			}
			return;
		}
		else
		{
			SetCellIndex(i, static_cast<float>(2 - 2 * i));

			if (i == m_VertexCount - 1)
			{
				m_IsMouseOver = false;
			}
		}
	}
}

//=========================================================================================================================

void CLoadingIconUI::Animate(float ElapsedTime)
{
	if (IsActive())
	{
		SetCellIndex(0, GetCellIndex(0) + 12.0f * ElapsedTime);
	}
}
