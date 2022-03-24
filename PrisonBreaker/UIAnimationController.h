#pragma once

class CBilboardMesh;
class CBilboardObject;

class CUIAnimationClip
{
	friend class CUIAnimationController;

private:
	string                        m_ClipName{};

	ANIMATION_TYPE				  m_AnimationType{};
								
	UINT	                      m_FramePerSec{};
	UINT	                      m_KeyFrameCount{};
	float                         m_KeyFrameTime{};
	UINT						  m_KeyFrameIndex{};

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
	UINT								 m_ClipNum{};
	vector<shared_ptr<CUIAnimationClip>> m_AnimationClips{};

public:
	CUIAnimationController(vector<shared_ptr<CUIAnimationClip>>& UIAnimationClips);
	~CUIAnimationController() = default;

	void SetAnimationClip(UINT ClipNum);
	void SetKeyFrameIndex(UINT ClipNum, UINT KeyFrameIndex);

	void UpdateAnimationClip(float ElapsedTime, const shared_ptr<CBilboardObject>& BilboardObject);
};
