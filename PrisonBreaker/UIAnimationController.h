#pragma once

class CBilboardMesh;
class CBilboardObject;

class CUIAnimationClip
{
	friend class CUIAnimationController;

private:
	string                        m_ClipName{};
				
	UINT	                      m_FramePerSec{};
	UINT	                      m_KeyFrameCount{};
	float                         m_KeyFrameTime{};

	vector<vector<CBilboardMesh>> m_TransformData{}; // [VertexIndex][KeyFrameIndex]

public:
	CUIAnimationClip() = default;
	~CUIAnimationClip() = default;

	void LoadAnimationClipInfoFromFile(ifstream& InFile, UINT VertexCount);
};

//=========================================================================================================================

class CUIAnimationController
{
private:
	bool								 m_IsActive{ true };

	shared_ptr<CBilboardObject>			 m_Owner{};

	UINT								 m_ClipNum{};
	vector<shared_ptr<CUIAnimationClip>> m_AnimationClips{};

	UINT						         m_KeyFrameIndex{};

public:
	CUIAnimationController(const shared_ptr<CBilboardObject>& Owner, vector<shared_ptr<CUIAnimationClip>>& UIAnimationClips);
	~CUIAnimationController() = default;

	void SetActive(bool IsActive);
	bool IsActive() const;

	void SetAnimationClip(UINT ClipNum);
	void SetKeyFrameIndex(UINT ClipNum, UINT KeyFrameIndex);

	bool UpdateAnimationClip(ANIMATION_TYPE AnimationType);
};