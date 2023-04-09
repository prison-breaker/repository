#pragma once

class CAsset
{
	friend class CAssetManager;

protected:
	string m_name;

protected:
	// �� ��ü�� ������ ������ CAssetManager�� ���ؼ��� �Ͼ��.
	// ��, �� ��ü�� ��� ���� �ڽ� Ŭ������ �����ڿ��� �� Ŭ������ �����ڸ� ȣ���ؾ��ϹǷ� ���� �����ڸ� protected�� �����Ͽ���.
	CAsset();

public:
	// �Ҹ����� ��쿡�� SafeDelete �ܺ� �Լ��� �̿��ϱ� ������ ���� �����ڸ� public���� �����Ͽ���.
	virtual ~CAsset();

	void SetName(const string& name);
	const string& GetName();

	virtual void CreateShaderVariables(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* d3d12GraphicsCommandList);
	virtual void ReleaseShaderVariables();
};
