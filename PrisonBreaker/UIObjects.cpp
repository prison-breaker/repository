#include "stdafx.h"
#include "UIObjects.h"
#include "TitleScene.h"
#include "Framework.h"
#include "StateMachine.h"
#include "State_Background.h"
#include "State_MissionUI.h"
#include "UIAnimationController.h"

void CBackgroundUI::Initialize()
{
	m_StateMachine = make_shared<CStateMachine<CBackgroundUI>>(static_pointer_cast<CBackgroundUI>(shared_from_this()));
	m_StateMachine->SetCurrentState(CBackgroundUIFadeState::GetInstance());
}

void CBackgroundUI::Animate(float ElapsedTime)
{
	if (m_IsActive)
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
	m_AnimationController->SetKeyFrameIndex(0);

	for (const auto& ChildObjects : m_ChildObjects)
	{
		if (ChildObjects)
		{
			ChildObjects->Reset();
		}
	}
}

void CMissionUI::Animate(float ElapsedTime)
{
	if (m_IsActive)
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
	m_AnimationController->SetKeyFrameIndex(0);
	m_AnimationController->UpdateAnimationClip(ANIMATION_TYPE_NONE); // Only Initialize Mapped Data

	for (const auto& ChildObjects : m_ChildObjects)
	{
		if (ChildObjects)
		{
			ChildObjects->Reset();
		}
	}
}

void CKeyUI::Animate(float ElapsedTime)
{
	if (m_IsActive)
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

	for (const auto& ChildObjects : m_ChildObjects)
	{
		if (ChildObjects)
		{
			ChildObjects->Reset();
		}
	}
}

void CHitUI::Animate(float ElapsedTime)
{
	if (m_IsActive)
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

void CButtonUI::SetButtonArea(UINT Index, const XMFLOAT4& Area)
{
	if (Index < 0 || Index >= m_MaxVertexCount)
	{
		return;
	}
	
	if (m_ButtonAreas.empty())
	{
		m_ButtonAreas.resize(m_MaxVertexCount);
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
	vector<shared_ptr<CQuadObject>>& QuadObjects{ static_pointer_cast<CTitleScene>(CSceneManager::GetInstance()->GetScene("TitleScene"))->GetQuadObjects() };

	// 3 ~ 5 : Panel
	if (!QuadObjects[3]->IsActive() && !QuadObjects[4]->IsActive() && !QuadObjects[5]->IsActive())
	{
		for (UINT i = 0; i < m_MaxVertexCount; ++i)
		{
			if ((m_ButtonAreas[i].x <= ScreenPosition.x) && (ScreenPosition.x <= m_ButtonAreas[i].y) &&
				(m_ButtonAreas[i].z <= ScreenPosition.y) && (ScreenPosition.y <= m_ButtonAreas[i].w))
			{
				SetCellIndex(i, static_cast<float>(2 * i + 1));

				if (!m_IsMouseOver)
				{
					m_IsMouseOver = true;

					CSoundManager::GetInstance()->Play(SOUND_TYPE_BUTTON_OVER, 0.7f, false);
				}

				if (Message == WM_LBUTTONUP)
				{
					switch (i)
					{
					case 0: // GameStart Button
					{
						QuadObjects[3]->SetActive(true);
					}
					break;
					case 1: // Exit Button
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
}

//=========================================================================================================================

void CPanelButtonUI::ProcessMouseMessage(UINT Message, const XMINT2& ScreenPosition, UINT RootFrameIndex)
{
	for (UINT i = 0; i < m_MaxVertexCount; ++i)
	{
		if ((m_ButtonAreas[i].x <= ScreenPosition.x) && (ScreenPosition.x <= m_ButtonAreas[i].y) &&
			(m_ButtonAreas[i].z <= ScreenPosition.y) && (ScreenPosition.y <= m_ButtonAreas[i].w))
		{
			SetCellIndex(i, static_cast<float>(3 - 2 * i));

			if (!m_IsMouseOver)
			{
				m_IsMouseOver = true;

				CSoundManager::GetInstance()->Play(SOUND_TYPE_BUTTON_OVER, 0.7f, false);
			}

			if (Message == WM_LBUTTONUP)
			{
				vector<shared_ptr<CQuadObject>>& QuadObjects{ static_pointer_cast<CTitleScene>(CSceneManager::GetInstance()->GetScene("TitleScene"))->GetQuadObjects() };

				QuadObjects[RootFrameIndex]->SetActive(false);

				switch (i)
				{
				case 0: // Close Button
					CFramework::GetInstance()->DisconnectServer();
					break;
				case 1: // Connect Button
					QuadObjects[RootFrameIndex + 1]->SetActive(true); // Open Waiting Popup

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

void CLoadingIconUI::Reset()
{
	SetCellIndex(0, 0);

	for (const auto& ChildObjects : m_ChildObjects)
	{
		if (ChildObjects)
		{
			ChildObjects->Reset();
		}
	}
}

void CLoadingIconUI::Animate(float ElapsedTime)
{
	if (m_IsActive)
	{
		SetCellIndex(0, GetCellIndex(0) + 15.0f * ElapsedTime);
	}
}

//=========================================================================================================================

void CGameOverUI::Reset()
{
	SetCellIndex(0, 0);

	for (const auto& ChildObjects : m_ChildObjects)
	{
		if (ChildObjects)
		{
			ChildObjects->Reset();
		}
	}
}

void CGameOverUI::Animate(float ElapsedTime)
{
	if (m_IsActive)
	{
		SetCellIndex(0, GetCellIndex(0) + ElapsedTime);
	}
}

//=========================================================================================================================

void CEndingCreditUI::Reset()
{
	for (UINT i = 0; i < m_MaxVertexCount; ++i)
	{
		m_MappedQuadInfo[i].m_Position = m_InitPosition[i];
	}
}

void CEndingCreditUI::Animate(float ElapsedTime)
{
	if (m_IsActive)
	{
		for (UINT i = 0; i < m_MaxVertexCount; ++i)
		{
			XMFLOAT3 PrevPosition{ m_MappedQuadInfo[i].m_Position };
			XMFLOAT3 NewPosition{ PrevPosition.x, PrevPosition.y - 50.0f * ElapsedTime, 0.0f };

			m_MappedQuadInfo[i].m_Position = NewPosition;
		}
	}
}

void CEndingCreditUI::SetInitPosition(UINT Index, const XMFLOAT3& InitPosition)
{
	if (Index < 0 || Index >= m_MaxVertexCount)
	{
		return;
	}

	if (m_InitPosition.empty())
	{
		m_InitPosition.resize(m_MaxVertexCount);
	}

	m_InitPosition[Index] = InitPosition;
}

const XMFLOAT3& CEndingCreditUI::GetInitPositio(UINT Index) const
{
	return m_InitPosition[Index];
}
