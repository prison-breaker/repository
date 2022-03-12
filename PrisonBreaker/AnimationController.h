#pragma once
#include "GameObject.h"

class CAnimationClip
{
	friend class CAnimationController;

private:
	string                             m_ClipName{};
		                               
	int	                               m_FramePerSec{};
	int	                               m_KeyFrameCount{};
	float                              m_KeyFrameTime{};
	int								   m_KeyFrameIndex{};

	vector<vector<vector<XMFLOAT4X4>>> m_BoneTransformMatrixes{}; // [SkinnedMesh][Bone][KeyFrameTimeIndex]

public:
	CAnimationClip() = default;
	~CAnimationClip() = default;

	void LoadAnimationClipInfoFromFile(ifstream& InFile, const shared_ptr<LOADED_MODEL_INFO>& ModelInfo);
};

//=========================================================================================================================

class CAnimationController
{
private:
	vector<shared_ptr<CAnimationClip>>	    m_AnimationClips{};

	vector<shared_ptr<CSkinnedMesh>>        m_SkinnedMeshCaches{};
	vector<vector<shared_ptr<CGameObject>>> m_BoneFrameCaches{}; // [SkinnedMesh][Bone]

	vector<ComPtr<ID3D12Resource>>	        m_D3D12BoneTransformMatrixes{};
	vector<XMFLOAT4X4*>				        m_MappedBoneTransformMatrixes{};

	UINT									m_ClipNum{};

public:
	CAnimationController(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, const shared_ptr<LOADED_MODEL_INFO>& ModelInfo);
	~CAnimationController() = default;

	void UpdateShaderVariables(ID3D12GraphicsCommandList* D3D12GraphicsCommandList);

	void UpdateAnimationClip(float ElapsedTime, const shared_ptr<CGameObject>& RootObject);
	void ChangeAnimationClip(UINT ClipNum);
};
