#pragma once
#include "Asset.h"

class CObject;
class CSkinnedMesh;

class CAnimation : public CAsset
{
	friend class CAssetManager;

private:                              
	int	                               m_frameRate;
	int	                               m_frameCount;
	float                              m_duration;

	vector<vector<vector<XMFLOAT4X4>>> m_boneTransformMatrixes; // [skinnedMesh][bone][frameIndex]

private:
	// �� ��ü�� ������ ������ CAssetManager�� ���ؼ��� �Ͼ��.
	CAnimation();

public:
	// �Ҹ����� ��쿡�� SafeDelete �ܺ� �Լ��� �̿��ϱ� ������ ���� �����ڸ� public���� �����Ͽ���.
	virtual ~CAnimation();

	int GetFrameRate();
	int GetFrameCount();
	float GetDuration();
	const vector<vector<vector<XMFLOAT4X4>>>& GetBoneTransformMatrixes();

	void Load(ifstream& in);
};
