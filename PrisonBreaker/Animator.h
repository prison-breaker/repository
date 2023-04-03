#pragma once
#include "Component.h"

class CSkinnedMesh;
class CAnimation;

class CAnimator : public CComponent
{
	friend class CObject;

private:
	bool							   m_isActive;
	bool							   m_isLoop;
	bool							   m_isFinished;

	unordered_map<string, CAnimation*> m_animations;
	CAnimation*						   m_playingAnimation;
	int							       m_frameIndex;
	float							   m_elapsedTime;

	vector<CSkinnedMesh*>			   m_skinnedMeshCache;
	vector<vector<CObject*>>           m_boneFrameCaches; // [skinnedMesh][boneFrame]

	vector<ComPtr<ID3D12Resource>>	   m_d3d12BoneTransformMatrixes;
	vector<XMFLOAT4X4*>				   m_mappedBoneTransformMatrixes;

private:
	CAnimator();
	~CAnimator();
	
public:
	void SetActive(bool isActive);
	bool IsActive();

	bool IsFinished();

	void SetFrameIndex(int frameIndex);
	int GetFrameIndex();

	void Load(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList, ifstream& in);

	void Play(const string& key, bool isLoop, bool duplicatable = false);

	void UpdateShaderVariables();

	virtual void Update();
};
