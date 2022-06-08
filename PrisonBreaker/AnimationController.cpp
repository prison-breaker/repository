#include "stdafx.h"
#include "AnimationController.h"
#include "GameObject.h"

void CAnimationClip::LoadAnimationClipInfoFromFile(ifstream& InFile, const shared_ptr<LOADED_MODEL_INFO>& ModelInfo)
{
	tstring Token{};
	UINT SkinnedMeshCount{};

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

			SkinnedMeshCount = static_cast<UINT>(ModelInfo->m_SkinnedMeshCaches.size());
			m_BoneTransformMatrixes.resize(SkinnedMeshCount);

			for (UINT i = 0; i < SkinnedMeshCount; ++i)
			{
				UINT BoneCount{ static_cast<UINT>(ModelInfo->m_BoneFrameCaches[i].size()) };

				m_BoneTransformMatrixes[i].resize(BoneCount);

				for (UINT j = 0; j < BoneCount; ++j)
				{
					m_BoneTransformMatrixes[i][j].reserve(m_KeyFrameCount);
				}
			}
		}
		else if (Token == TEXT("<TransformMatrix>"))
		{
			// Current KeyFrameTime
			File::ReadFloatFromFile(InFile);

			for (UINT i = 0; i < SkinnedMeshCount; ++i)
			{
				UINT BoneCount{ static_cast<UINT>(ModelInfo->m_BoneFrameCaches[i].size()) };

				for (UINT j = 0; j < BoneCount; ++j)
				{
					XMFLOAT4X4 TransformMatrix{};

					InFile.read(reinterpret_cast<TCHAR*>(&TransformMatrix), sizeof(XMFLOAT4X4));

					m_BoneTransformMatrixes[i][j].push_back(TransformMatrix);
				}
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

			SkinnedMeshCount = static_cast<UINT>(ModelInfo->m_SkinnedMeshCaches.size());
			m_BoneTransformMatrixes.resize(SkinnedMeshCount);

			for (UINT i = 0; i < SkinnedMeshCount; ++i)
			{
				UINT BoneCount{ static_cast<UINT>(ModelInfo->m_BoneFrameCaches[i].size()) };

				m_BoneTransformMatrixes[i].resize(BoneCount);

				for (UINT j = 0; j < BoneCount; ++j)
				{
					m_BoneTransformMatrixes[i][j].reserve(m_KeyFrameCount);
				}
			}
		}
		else if (Token == TEXT("<TransformMatrix>"))
		{
			// Current KeyFrameTime
			InFile >> Token;

			for (UINT i = 0; i < SkinnedMeshCount; ++i)
			{
				UINT BoneCount{ static_cast<UINT>(ModelInfo->m_BoneFrameCaches[i].size()) };

				for (UINT j = 0; j < BoneCount; ++j)
				{
					XMFLOAT4X4 TransformMatrix{};

					InFile >> TransformMatrix._11 >> TransformMatrix._12 >> TransformMatrix._13 >> TransformMatrix._14;
					InFile >> TransformMatrix._21 >> TransformMatrix._22 >> TransformMatrix._23 >> TransformMatrix._24;
					InFile >> TransformMatrix._31 >> TransformMatrix._32 >> TransformMatrix._33 >> TransformMatrix._34;
					InFile >> TransformMatrix._41 >> TransformMatrix._42 >> TransformMatrix._43 >> TransformMatrix._44;

					m_BoneTransformMatrixes[i][j].push_back(TransformMatrix);
				}
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

CAnimationController::CAnimationController(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, const shared_ptr<LOADED_MODEL_INFO>& ModelInfo, const shared_ptr<CGameObject>& Owner) :
	m_Owner{ Owner }
{
	m_AnimationClips.assign(ModelInfo->m_AnimationClips.begin(), ModelInfo->m_AnimationClips.end());
	m_BoneFrameCaches.assign(ModelInfo->m_BoneFrameCaches.begin(), ModelInfo->m_BoneFrameCaches.end());
	m_SkinnedMeshCaches.assign(ModelInfo->m_SkinnedMeshCaches.begin(), ModelInfo->m_SkinnedMeshCaches.end());

	UINT Bytes{ ((sizeof(XMFLOAT4X4) * MAX_BONES) + 255) & ~255 };
	UINT SkinnedMeshCount{ static_cast<UINT>(m_SkinnedMeshCaches.size()) };

	m_D3D12BoneTransformMatrixes.resize(SkinnedMeshCount);
	m_MappedBoneTransformMatrixes.resize(SkinnedMeshCount);

	for (UINT i = 0; i < SkinnedMeshCount; ++i)
	{
		m_D3D12BoneTransformMatrixes[i] = DX::CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, nullptr, Bytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr);
		m_D3D12BoneTransformMatrixes[i]->Map(0, nullptr, reinterpret_cast<void**>(&m_MappedBoneTransformMatrixes[i]));
	}
}

void CAnimationController::SetActive(bool IsActive)
{
	m_IsActive = IsActive;
}

bool CAnimationController::IsActive() const
{
	return m_IsActive;
}

void CAnimationController::SetAnimationClipType(ANIMATION_CLIP_TYPE ClipType)
{
	if (ClipType < 0 || ClipType >= m_AnimationClips.size() || m_ClipType == ClipType)
	{
		return;
	}

	m_ClipType = ClipType;
	m_KeyFrameIndex = 0;
	m_ElapsedTime = 0.0f;
}

ANIMATION_CLIP_TYPE CAnimationController::GetAnimationClipType() const
{
	return m_ClipType;
}

void CAnimationController::SetKeyFrameIndex(UINT KeyFrameIndex)
{
	if (KeyFrameIndex < 0 || KeyFrameIndex >= m_AnimationClips[m_ClipType]->m_KeyFrameCount)
	{
		return;
	}

	m_KeyFrameIndex = KeyFrameIndex;
	m_ElapsedTime = 0.0f;
}

UINT CAnimationController::GetKeyFrameIndex() const
{
	return m_KeyFrameIndex;
}

void CAnimationController::UpdateShaderVariables()
{
	UINT SkinnedMeshCount{ static_cast<UINT>(m_SkinnedMeshCaches.size()) };

	// 공유되는 스킨 메쉬에 현재 애니메이션 컨트롤러의 뼈 변환 행렬 리소스를 설정해준다.
	for (UINT i = 0; i < SkinnedMeshCount; ++i)
	{
		m_SkinnedMeshCaches[i]->SetBoneTransformInfo(m_D3D12BoneTransformMatrixes[i], m_MappedBoneTransformMatrixes[i]);
	}

	// 이번 프레임의 애니메이션 변환 행렬을 각 뼈 프레임에 변환 행렬로 설정한다.
	for (UINT i = 0; i < SkinnedMeshCount; ++i)
	{
		UINT BoneFrameCount{ static_cast<UINT>(m_BoneFrameCaches[i].size()) };

		for (UINT j = 0; j < BoneFrameCount; ++j)
		{
			m_BoneFrameCaches[i][j]->SetTransformMatrix(m_AnimationClips[m_ClipType]->m_BoneTransformMatrixes[i][j][m_KeyFrameIndex]);
		}
	}

	if (m_Owner)
	{
		m_Owner->UpdateTransform(Matrix4x4::Identity());
	}
}

bool CAnimationController::UpdateAnimationClip(ANIMATION_TYPE AnimationType)
{
	bool IsFinished{};

	if (IsActive())
	{
		switch (AnimationType)
		{
		case ANIMATION_TYPE_LOOP:
			m_KeyFrameIndex += 1;

			if (m_KeyFrameIndex >= m_AnimationClips[m_ClipType]->m_KeyFrameCount)
			{
				m_KeyFrameIndex = 0;
			}
			break;
		case ANIMATION_TYPE_ONCE:
			m_KeyFrameIndex += 1;

			if (m_KeyFrameIndex >= m_AnimationClips[m_ClipType]->m_KeyFrameCount)
			{
				m_KeyFrameIndex = m_AnimationClips[m_ClipType]->m_KeyFrameCount - 1;
				IsFinished = true;
			}
			break;
		case ANIMATION_TYPE_ONCE_REVERSE:
			m_KeyFrameIndex -= 1;

			if (m_KeyFrameIndex < 0)
			{
				m_KeyFrameIndex = 0;
				IsFinished = true;
			}
			break;
		}
	}

	return IsFinished;
}

bool CAnimationController::UpdateAnimationClip(ANIMATION_TYPE AnimationType, SOUND_TYPE SoundType, float Volume, float PlayTime, float ElapsedTime)
{
	bool IsFinished{ UpdateAnimationClip(AnimationType) };

	if (IsFinished)
	{
		m_ElapsedTime = 0.0f;
	}
	else
	{
		m_ElapsedTime += ElapsedTime;

		if (m_ElapsedTime >= PlayTime)
		{
			m_ElapsedTime = 0.0f;
			CSoundManager::GetInstance()->Play(SoundType, Volume);
		}
	}

	return IsFinished;
}
