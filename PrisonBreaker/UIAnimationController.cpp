#include "stdafx.h"
#include "UIAnimationController.h"
#include "QuadObject.h"

void CUIAnimationClip::LoadAnimationClipInfoFromFile(ifstream& InFile, UINT VertexCount)
{
	tstring Token{};

	UINT Count{};

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

				QUAD_INFO QuadInfo{};

				QuadInfo.m_Position = Position;
				QuadInfo.m_Size = Size;
				QuadInfo.m_CellIndex = CellIndex;
					
				m_TransformData[i].push_back(QuadInfo);
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

				m_TransformData[i][Count].m_AlphaColor = AlphaColor;
			}

			++Count;
		}
		else if (Token == TEXT("</AnimationClip>"))
		{
			break;
		}
	}
}

//=========================================================================================================================

CUIAnimationController::CUIAnimationController(const shared_ptr<CQuadObject>& Owner, vector<shared_ptr<CUIAnimationClip>>& UIAnimationClips) :
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
			m_Owner->SetPosition(i, m_AnimationClips[m_ClipNum]->m_TransformData[i][m_KeyFrameIndex].m_Position);
			m_Owner->SetSize(i, m_AnimationClips[m_ClipNum]->m_TransformData[i][m_KeyFrameIndex].m_Size);
			m_Owner->SetAlphaColor(i, m_AnimationClips[m_ClipNum]->m_TransformData[i][m_KeyFrameIndex].m_AlphaColor);
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
