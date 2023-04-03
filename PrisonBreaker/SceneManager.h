#pragma once

class CScene;

class CSceneManager : public CSingleton<CSceneManager>
{
	friend class CSingleton;

private:
	vector<CScene*> m_scenes;
	CScene*			m_currentScene;

private:
	CSceneManager();
	virtual ~CSceneManager();

	void ChangeScene(SCENE_TYPE sceneType);

public:
	CScene* GetCurrentScene();

	void Init(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList);

	void ReleaseUploadBuffers();

	void Update();

	void PreRender(ID3D12GraphicsCommandList* d3d12GraphicsCommandList);
	void Render(ID3D12GraphicsCommandList* d3d12GraphicsCommandList);
	void PostRender(ID3D12GraphicsCommandList* d3d12GraphicsCommandList);

	//void ProcessPacket();
};
