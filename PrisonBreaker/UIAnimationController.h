#pragma once

struct QUAD_INFO;
class CQuadObject;

class CUIAnimationClip
{
	friend class CUIAnimationController;

private:
	string                    m_ClipName{};
				
	UINT	                  m_FramePerSec{};
	UINT	                  m_KeyFrameCount{};
	float                     m_KeyFrameTime{};

	vector<vector<QUAD_INFO>> m_TransformData{}; // [VertexIndex][KeyFrameIndex]

public:
	CUIAnimationClip() = default;
	~CUIAnimationClip() = default;

	void LoadAnimationClipInfoFromFile(ifstream& in, UINT VertexCount);
};

//=========================================================================================================================

class CUIAnimationController
{
private:
	bool								 m_IsActive{ true };

	shared_ptr<CQuadObject>			     m_Owner{};

	UINT								 m_ClipNum{};
	vector<shared_ptr<CUIAnimationClip>> m_AnimationClips{};

	UINT						         m_KeyFrameIndex{};

public:
	CUIAnimationController(const shared_ptr<CQuadObject>& Owner, vector<shared_ptr<CUIAnimationClip>>& UIAnimationClips);
	~CUIAnimationController() = default;

	void SetActive(bool IsActive);
	bool IsActive() const;

	void SetAnimationClip(UINT ClipNum);
	void SetKeyFrameIndex(UINT KeyFrameIndex);

	bool UpdateAnimationClip(ANIMATION_TYPE AnimationType);
};
