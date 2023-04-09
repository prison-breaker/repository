#pragma once

class CObject;

class CScene abstract
{
	friend class CSceneManager;

private:
	string			 m_name;

	vector<CObject*> m_objects[(int)GROUP_TYPE::COUNT];

protected:
	// �� ��ü�� ������ ������ CSceneManager�� ���ؼ��� �Ͼ��.
	// ��, �� ��ü�� ��� ���� �ڽ� Ŭ������ �����ڿ��� �� Ŭ������ �����ڸ� ȣ���ؾ��ϹǷ� ���� �����ڸ� protected�� �����Ͽ���.
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
	// �Ҹ����� ��쿡�� SafeDelete �ܺ� �Լ��� �̿��ϱ� ������ ���� �����ڸ� public���� �����Ͽ���.
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
