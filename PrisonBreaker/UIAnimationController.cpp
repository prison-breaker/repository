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

CUIAnimationController::CUIAnimationController(vector<shared_ptr<CUIAnimationClip>>& UIAnimationClips) :
	m_AnimationClips{ move(UIAnimationClips) }
{

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

		m_AnimationClips[ClipNum]->m_KeyFrameIndex = KeyFrameIndex;
	}
}

void CUIAnimationController::UpdateAnimationClip(float ElapsedTime, const shared_ptr<CBilboardObject>& BilboardObject)
{
	UINT VertexCount{ BilboardObject->GetVertexCount() };

	for (UINT i = 0; i < VertexCount; ++i)
	{
		// CBilboardMesh의 Get 함수는 유일하게 GPU의 가상주소로 사용되지 않는 곳에서만 사용해야한다!!
		BilboardObject->SetPosition(m_AnimationClips[m_ClipNum]->m_TransformData[i][m_AnimationClips[m_ClipNum]->m_KeyFrameIndex].GetPosition(), i);
		BilboardObject->SetSize(m_AnimationClips[m_ClipNum]->m_TransformData[i][m_AnimationClips[m_ClipNum]->m_KeyFrameIndex].GetSize(), i);
	}

	switch (m_AnimationClips[m_ClipNum]->m_AnimationType)
	{
	case ANIMATION_TYPE_LOOP:
		m_AnimationClips[m_ClipNum]->m_KeyFrameIndex += 1;

		if (m_AnimationClips[m_ClipNum]->m_KeyFrameIndex >= m_AnimationClips[m_ClipNum]->m_KeyFrameCount)
		{
			m_AnimationClips[m_ClipNum]->m_KeyFrameIndex -= 1;
		}
		break;
	case ANIMATION_TYPE_ONCE:
		break;
	case ANIMATION_TYPE_PINGPONG:
		break;
	case ANIMATION_TYPE_ONCE_PINGPONG:
		break;
	}
}
