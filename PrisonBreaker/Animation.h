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
	// 이 객체의 생성은 오로지 CAssetManager에 의해서만 일어난다.
	CAnimation();

public:
	// 소멸자의 경우에는 SafeDelete 외부 함수를 이용하기 때문에 접근 지정자를 public으로 설정하였다.
	virtual ~CAnimation();

	int GetFrameRate();
	int GetFrameCount();
	float GetDuration();
	const vector<vector<vector<XMFLOAT4X4>>>& GetBoneTransformMatrixes();

	void Load(ifstream& in);
};
