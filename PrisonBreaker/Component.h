#pragma once

class CObject;

class CComponent abstract
{
protected:
	bool     m_isEnabled;

	CObject* m_owner;

public:
	CComponent();
	CComponent(const CComponent& rhs) = delete;
	virtual ~CComponent();

	CComponent& operator =(const CComponent& rhs) = delete;

	void SetEnabled(bool isEnabled);
	bool IsEnabled();

	void SetOwner(CObject* owner);
	CObject* GetOwner();

	virtual void CreateShaderVariables(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* d3d12GraphicsCommandList);
	virtual void ReleaseShaderVariables();

	virtual void Update() = 0;
};
