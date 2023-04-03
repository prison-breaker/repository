#include "pch.h"
#include "UIAnimationController.h"
#include "QuadObject.h"

void CUIAnimationClip::LoadAnimationClipInfoFromFile(ifstream& in, UINT VertexCount)
{
	string str{};

	UINT Count{};

	while (true)
	{
		File::ReadStringFromFile(in, str);

		if (str == TEXT("<AnimationClip>"))
		{
			File::ReadStringFromFile(in, m_ClipName);
			in.read(reinterpret_cast<char*>(&m_FramePerSec), sizeof(int));
			in.read(reinterpret_cast<char*>(&m_KeyFrameCount), sizeof(int));
			in.read(reinterpret_cast<char*>(&m_KeyFrameTime), sizeof(float));

			m_TransformData.resize(VertexCount);

			for (UINT i = 0; i < VertexCount; ++i)
			{
				m_TransformData[i].reserve(m_KeyFrameCount);
			}
		}
		else if (str == TEXT("<RectTransform>"))
		{
			// Current KeyFrameTime
			File::ReadFloatFromFile(in);

			for (UINT i = 0; i < VertexCount; ++i)
			{
				XMFLOAT3 Position{};
				XMFLOAT2 Size{};
				float CellIndex{};

				in.read(reinterpret_cast<char*>(&Position), sizeof(XMFLOAT2));
				in.read(reinterpret_cast<char*>(&CellIndex), sizeof(float));
				in.read(reinterpret_cast<char*>(&Size), sizeof(XMFLOAT2));

				QUAD_INFO QuadInfo{};

				QuadInfo.m_Position = Position;
				QuadInfo.m_Size = Size;
				QuadInfo.m_CellIndex = CellIndex;
					
				m_TransformData[i].push_back(QuadInfo);
			}
		}
		else if (str == TEXT("<AlphaColor>"))
		{
			// Current KeyFrameTime
			File::ReadFloatFromFile(in);

			for (UINT i = 0; i < VertexCount; ++i)
			{
				float AlphaColor{};

				in.read(reinterpret_cast<char*>(&AlphaColor), sizeof(float));

				m_TransformData[i][Count].m_AlphaColor = AlphaColor;
			}

			++Count;
		}
		else if (str == TEXT("</AnimationClip>"))
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
