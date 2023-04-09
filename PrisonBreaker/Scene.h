#pragma once

class CObject;

class CScene abstract
{
	friend class CSceneManager;

private:
	string			 m_name;

	vector<CObject*> m_objects[(int)GROUP_TYPE::COUNT];

protected:
	// 이 객체의 생성은 오로지 CSceneManager에 의해서만 일어난다.
	// 단, 이 객체를 상속 받은 자식 클래스의 생성자에서 이 클래스의 생성자를 호출해야하므로 접근 지정자를 protected로 설정하였다.
	CScene();

	void Load(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList, const string& fileName);
	void LoadUI(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList, const string& fileName);

private:
	virtual void Enter() = 0;
	virtual void Exit() = 0;

	virtual void CreateShaderVariables(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* d3d12GraphicsCommandList);
	virtual void ReleaseShaderVariables();

public:
	// 소멸자의 경우에는 SafeDelete 외부 함수를 이용하기 때문에 접근 지정자를 public으로 설정하였다.
	virtual ~CScene();

	void SetName(const string& name);
	const string& GetName();

	void AddObject(GROUP_TYPE group, CObject* object);
	const vector<CObject*>& GetGroupObject(GROUP_TYPE group);
	void DeleteGroupObject(GROUP_TYPE group);

	virtual void Init(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList) = 0;

	void ReleaseUploadBuffers();

	virtual void Update();
	virtual void LateUpdate();

	virtual void PreRender(ID3D12GraphicsCommandList* d3d12GraphicsCommandList);
	virtual void Render(ID3D12GraphicsCommandList* d3d12GraphicsCommandList);
	virtual void PostRender(ID3D12GraphicsCommandList* d3d12GraphicsCommandList);
};
