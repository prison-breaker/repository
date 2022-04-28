#include "stdafx.h"
#include "UIAnimationController.h"
#include "BilboardObject.h"

void CUIAnimationClip::LoadAnimationClipInfoFromFile(ifstream& InFile, UINT VertexCount)
{
	tstring Token{};

	UINT Count{};

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

			for (UINT i = 0; i < VertexCount; ++i)
			{
				m_TransformData[i].reserve(m_KeyFrameCount);
			}
		}
		else if (Token == TEXT("<RectTransform>"))
		{
			// Current KeyFrameTime
			File::ReadFloatFromFile(InFile);

			for (UINT i = 0; i < VertexCount; ++i)
			{
				XMFLOAT3 Position{};
				XMFLOAT2 Size{};
				float CellIndex{};

				InFile.read(reinterpret_cast<TCHAR*>(&Position), sizeof(XMFLOAT2));
				InFile.read(reinterpret_cast<TCHAR*>(&CellIndex), sizeof(float));
				InFile.read(reinterpret_cast<TCHAR*>(&Size), sizeof(XMFLOAT2));

				CBilboardMesh BilboardMesh{};

				BilboardMesh.SetPosition(Position);
				BilboardMesh.SetSize(Size);
				BilboardMesh.SetCellIndex(static_cast<UINT>(CellIndex));

				m_TransformData[i].push_back(BilboardMesh);
			}
		}
		else if (Token == TEXT("<AlphaColor>"))
		{
			// Current KeyFrameTime
			File::ReadFloatFromFile(InFile);

			for (UINT i = 0; i < VertexCount; ++i)
			{
				float AlphaColor{};

				InFile.read(reinterpret_cast<TCHAR*>(&AlphaColor), sizeof(float));

				m_TransformData[i][Count].SetAlphaColor(AlphaColor);
			}

			++Count;
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

void CUIAnimationController::SetKeyFrameIndex(UINT KeyFrameIndex)
{
	if (KeyFrameIndex < 0 || KeyFrameIndex >= m_AnimationClips[m_ClipNum]->m_KeyFrameCount)
	{
		return;
	}

	m_KeyFrameIndex = KeyFrameIndex;
}

bool CUIAnimationController::UpdateAnimationClip(ANIMATION_TYPE AnimationType)
{
	bool IsFinished{};

	if (IsActive())
	{
		UINT VertexCount{ m_Owner->GetVertexCount() };

		for (UINT i = 0; i < VertexCount; ++i)
		{
			// CBilboardMesh의 Get 함수는 유일하게 GPU의 가상주소로 사용되지 않는 곳에서만 사용해야한다!!
			m_Owner->SetPosition(i, m_AnimationClips[m_ClipNum]->m_TransformData[i][m_KeyFrameIndex].GetPosition());
			m_Owner->SetSize(i, m_AnimationClips[m_ClipNum]->m_TransformData[i][m_KeyFrameIndex].GetSize());
			m_Owner->SetAlphaColor(i, m_AnimationClips[m_ClipNum]->m_TransformData[i][m_KeyFrameIndex].GetAlphaColor());
		}

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

			// UINT UnderFlow
			if (m_KeyFrameIndex >= m_AnimationClips[m_ClipNum]->m_KeyFrameCount)
			{
				m_KeyFrameIndex = 0;
				IsFinished = true;
			}
			break;
		}
	}

	return IsFinished;
}
