#pragma once

class CGameObject;
class CSkinnedMesh;

class CAnimationClip
{
private:
	string                             m_ClipName{};
		                               
	int	                               m_FramePerSec{};
	int	                               m_KeyFrameCount{};
	float                              m_KeyFrameTime{};

	vector<vector<vector<XMFLOAT4X4>>> m_BoneTransformMatrixes{}; // [SkinnedMesh][Bone][KeyFrameTimeIndex]

public:
	CAnimationClip() = default;
	~CAnimationClip() = default;
};

//=========================================================================================================================

class CAnimationController
{
private:
	vector<shared_ptr<CAnimationClip>>	    m_AnimationClips{};

	vector<vector<shared_ptr<CGameObject>>> m_BoneFrameCaches{}; // [SkinnedMesh][Bone]
	vector<shared_ptr<CSkinnedMesh>>        m_SkinnedMeshCaches{};
									        
	vector<ComPtr<ID3D12Resource>>	        m_D3D12BoneTransformMatrixes{};
	vector<XMFLOAT4X4*>				        m_MappedBoneTransformMatrixes{};

public:
	CAnimationController() = default;
	~CAnimationController() = default;
};
