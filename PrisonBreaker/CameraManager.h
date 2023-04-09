#pragma once

class CCamera;

class CCameraManager : public CSingleton<CCameraManager>
{
	friend class CSingleton;

private:
	vector<CCamera*> m_cameras;

private:
	CCameraManager();
	virtual ~CCameraManager();

public:
	CCamera* GetMainCamera();
	CCamera* GetUICamera();

	const vector<CCamera*>& GetCameras();

	void Init(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList);

	void Update();
};
