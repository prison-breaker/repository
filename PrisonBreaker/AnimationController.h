#pragma once

struct LOADED_MODEL_INFO;
class CGameObject;
class CSkinnedMesh;

class CAnimationClip
{
	friend class CAnimationController;

private:
	string                             m_ClipName{};
                             
	UINT	                           m_FramePerSec{};
	UINT	                           m_KeyFrameCount{};
	float                              m_KeyFrameTime{};

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
	bool									m_IsActive{ true };

	float									m_ElapsedTime{};

	shared_ptr<CGameObject>					m_Owner{};

	UINT									m_ClipNum{};
	vector<shared_ptr<CAnimationClip>>	    m_AnimationClips{};

	UINT							        m_KeyFrameIndex{};

	vector<shared_ptr<CSkinnedMesh>>        m_SkinnedMeshCaches{};
	vector<vector<shared_ptr<CGameObject>>> m_BoneFrameCaches{}; // [SkinnedMesh][Bone]

	vector<ComPtr<ID3D12Resource>>	        m_D3D12BoneTransformMatrixes{};
	vector<XMFLOAT4X4*>				        m_MappedBoneTransformMatrixes{};

public:
	CAnimationController(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, const shared_ptr<LOADED_MODEL_INFO>& ModelInfo, const shared_ptr<CGameObject>& Owner);
	~CAnimationController() = default;
	
	void SetActive(bool IsActive);
	bool IsActive() const;

	void SetAnimationClip(UINT ClipNum);
	UINT GetAnimationClip() const;

	void SetKeyFrameIndex(UINT KeyFrameIndex);
	UINT GetKeyFrameIndex() const;

	void UpdateShaderVariables();

	bool UpdateAnimationClip(ANIMATION_TYPE AnimationType);
	bool UpdateAnimationClip(ANIMATION_TYPE AnimationType, SOUND_TYPE SoundType, float Volume, float PlayTime, float ElapsedTime);
};
