#pragma once

class CScene;

class CSceneManager
{
private:
	unordered_map<tstring, shared_ptr<CScene>> m_Scenes{};
	shared_ptr<CScene>						   m_CurrentScene{};
	shared_ptr<CScene>						   m_ReservedScene{};

private:
	CSceneManager() = default;
	~CSceneManager() = default;

public:
	static CSceneManager* GetInstance();

	void RegisterScene(const tstring& SceneName, const shared_ptr<CScene>& Scene);
	void ReserveChangeScene(const tstring& SceneName);
	void ChangeScene();

	void ProcessMouseMessage(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
	void ProcessKeyboardMessage(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
	void ProcessInput(HWND hWnd, float ElapsedTime);

	void Animate(float ElapsedTime);
	void PreRender(ID3D12GraphicsCommandList* D3D12GraphicsCommandList) const;
	void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList) const;
};
