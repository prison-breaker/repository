#pragma once

class CScene;

class CSceneManager
{
private:
	unordered_map<tstring, shared_ptr<CScene>> m_Scenes{};

	shared_ptr<CScene>						   m_CurrentScene{};
	shared_ptr<CScene>						   m_ReservedScene{};
	MSG_TYPE								   m_ReservedMsgType{};

private:
	CSceneManager() = default;
	~CSceneManager() = default;

public:
	static CSceneManager* GetInstance();

	void ReleaseUploadBuffers();

	shared_ptr<CScene> GetScene(const tstring& SceneName);

	void SetCurrentScene(const tstring& SceneName);
	shared_ptr<CScene> GetCurrentScene() const;

	shared_ptr<CScene> GetReservedScene() const;

	void RegisterScene(const tstring& SceneName, const shared_ptr<CScene>& Scene);
	void ReserveScene(const tstring& SceneName, MSG_TYPE MsgType = MSG_TYPE_NONE);

	void ChangeScene(const tstring& SceneName, MSG_TYPE MsgType = MSG_TYPE_NONE);
	void ChangeToReservedScene();

	void ProcessMouseMessage(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
	void ProcessKeyboardMessage(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
	void ProcessInput(HWND hWnd, float ElapsedTime);

	void Animate(float ElapsedTime);

	void PreRender(ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	void PostRender(ID3D12GraphicsCommandList* D3D12GraphicsCommandList);

	// Server Function
	void ProcessPacket();
};
