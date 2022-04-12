#include "stdafx.h"
#include "UIAnimationController.h"
#include "BilboardObject.h"

void CUIAnimationClip::LoadAnimationClipInfoFromFile(ifstream& InFile, UINT VertexCount)
{
	tstring Token{};

#ifdef READ_BINARY_FILE
	while (true)
	{
		File::ReadStringFromFile(InFile, Token);

		if (Token == TEXT("<AnimationClip>"))
		{
			File::ReadStringFromFile(InFile, m_ClipName);
			m_FramePerSec = File::ReadIntegerFromFile(InFile);
			m_KeyFrameCount = File::ReadIntegerFromFile(InFile);
			m_KeyFrameTime = File::ReadFloatFromFile(InFile);

			m_TransformData.resize(VertexCount);
		}
		else if (Token == TEXT("<RectTransform>"))
		{
			// Current KeyFrameTime
			File::ReadFloatFromFile(InFile);

			for (UINT i = 0; i < VertexCount; ++i)
			{
				XMFLOAT3 Position{};
				XMFLOAT2 Size{};
				XMUINT2 CellCount{};
				float CellIndex{};

				InFile.read(reinterpret_cast<TCHAR*>(&Position), sizeof(XMFLOAT2));
				InFile.read(reinterpret_cast<TCHAR*>(&CellIndex), sizeof(float));
				InFile.read(reinterpret_cast<TCHAR*>(&Size), sizeof(XMFLOAT2));

				m_TransformData[i].emplace_back(Position, Size, CellCount, static_cast<UINT>(CellIndex));
			}
		}
		else if (Token == TEXT("</AnimationClip>"))
		{
			break;
		}
	}
#else
	while (InFile >> Token)
	{
		if (Token == TEXT("<AnimationClip>"))
		{
			InFile >> m_ClipName;
			InFile >> m_FramePerSec;
			InFile >> m_KeyFrameCount;
			InFile >> m_KeyFrameTime;

			m_TransformData.resize(VertexCount);
		}
		else if (Token == TEXT("<RectTransform>"))
		{
			// Current KeyFrameTime
			InFile >> Token;

			for (UINT i = 0; i < VertexCount; ++i)
			{
				XMFLOAT3 Position{};
				UINT CellIndex{};
				XMFLOAT2 Size{};

				InFile >> Position.x >> Position.y;
				InFile >> CellIndex;
				InFile >> Size.x >> Size.y;

				m_TransformData[i].emplace_back(Position, Size, XMUINT2(), CellIndex);
			}
		}
		else if (Token == TEXT("</AnimationClip>"))
		{
			break;
		}
	}
#endif
}

//=========================================================================================================================

CUIAnimationController::CUIAnimationController(const shared_ptr<CBilboardObject>& Owner, vector<shared_ptr<CUIAnimationClip>>& UIAnimationClips) :
	m_Owner{ Owner },
	m_AnimationClips{ move(UIAnimationClips) }
{

}

void CUIAnimationController::SetActive(bool IsActive)
{
	m_IsActive = IsActive;
}

bool CUIAnimationController::IsActive() const
{
	return m_IsActive;
}

void CUIAnimationController::SetAnimationClip(UINT ClipNum)
{
	m_ClipNum = ClipNum;
}

void CUIAnimationController::SetKeyFrameIndex(UINT ClipNum, UINT KeyFrameIndex)
{
	if (m_AnimationClips[ClipNum])
	{
		if (KeyFrameIndex < 0 || KeyFrameIndex >= m_AnimationClips[ClipNum]->m_KeyFrameCount)
		{
			return;
		}

		m_KeyFrameIndex = KeyFrameIndex;
	}
}

bool CUIAnimationController::UpdateAnimationClip(ANIMATION_TYPE AnimationType)
{
	UINT VertexCount{ m_Owner->GetVertexCount() };

	for (UINT i = 0; i < VertexCount; ++i)
	{
		// CBilboardMesh�� Get �Լ��� �����ϰ� GPU�� �����ּҷ� ������ �ʴ� �������� ����ؾ��Ѵ�!!
		m_Owner->SetPosition(i, m_AnimationClips[m_ClipNum]->m_TransformData[i][m_KeyFrameIndex].GetPosition());
		m_Owner->SetSize(i, m_AnimationClips[m_ClipNum]->m_TransformData[i][m_KeyFrameIndex].GetSize());
	}

	bool IsFinished{};

	if (IsActive())
	{
		switch (AnimationType)
		{
		case ANIMATION_TYPE_LOOP:
			m_KeyFrameIndex += 1;

			if (m_KeyFrameIndex >= m_AnimationClips[m_ClipNum]->m_KeyFrameCount)
			{
				m_KeyFrameIndex = 0;
			}
			break;
		case ANIMATION_TYPE_ONCE:
			m_KeyFrameIndex += 1;

			if (m_KeyFrameIndex >= m_AnimationClips[m_ClipNum]->m_KeyFrameCount)
			{
				m_KeyFrameIndex = m_AnimationClips[m_ClipNum]->m_KeyFrameCount - 1;
				IsFinished = true;
			}
			break;
		case ANIMATION_TYPE_ONCE_REVERSE:
			m_KeyFrameIndex -= 1;

			if (m_KeyFrameIndex < 1)
			{
				m_KeyFrameIndex = 1;
				IsFinished = true;
			}
			break;
		}
	}

	return IsFinished;
}