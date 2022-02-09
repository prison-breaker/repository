#pragma once
#include "Mesh.h"
#include "Camera.h"

class CGameObject
{
protected:
	bool				    m_IsAlive{};

	tstring					m_FrameName{};

	XMFLOAT4X4		        m_WorldMatrix{ Matrix4x4::Identity() };
	XMFLOAT4X4				m_TransformMatrix{ Matrix4x4::Identity() };

	shared_ptr<CMesh>	    m_Mesh{};
	shared_ptr<CTexture>    m_Texture{};
	BoundingOrientedBox     m_BoundingBox{};

	shared_ptr<CGameObject> m_SiblingObject{};
	shared_ptr<CGameObject> m_ChildObject{};

public:
	CGameObject() = default;
	virtual ~CGameObject() = default;

	static shared_ptr<CGameObject> LoadObjectFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, const tstring& FileName);
	static shared_ptr<CGameObject> LoadObjectInfoFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, tifstream& InFile);

	virtual void CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	virtual void ReleaseShaderVariables();

	virtual void ReleaseUploadBuffers();

	virtual void Move(const XMFLOAT3& Direction, float Distance);

	virtual void Animate(float ElapsedTime);
	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);

	bool IsAlive() const;
	void SetAlive(bool IsAlive);

	void SetWorldMatrix(const XMFLOAT4X4 WorldMatrix);
	const XMFLOAT4X4& GetWorldMatrix() const;

	void SetTransformMatrix(const XMFLOAT4X4& TransformMatrix);
	const XMFLOAT4X4& GetTransformMatrix() const;

	void SetRight(const XMFLOAT3& Right);
	XMFLOAT3 GetRight() const;

	void SetUp(const XMFLOAT3& Up);
	XMFLOAT3 GetUp() const;

	void SetLook(const XMFLOAT3& Look);
	XMFLOAT3 GetLook() const;

	void SetPosition(const XMFLOAT3& Position);
	XMFLOAT3 GetPosition() const;

	void SetMesh(const shared_ptr<CMesh>& Mesh);
	void SetTexture(const shared_ptr<CTexture>& Texture);

	void SetBoundingBox(const BoundingOrientedBox& BoundingBox);
	const BoundingOrientedBox& GetBoundingBox() const;

	void SetChild(const shared_ptr<CGameObject>& ChildObject);

	bool IsVisible(CCamera* Camera) const;

	void UpdateBoundingBox();
	void UpdateTransform(const XMFLOAT4X4& ParentMatrix);

	void Scale(float Pitch, float Yaw, float Roll);
	void Rotate(float Pitch, float Yaw, float Roll);
	void Rotate(const XMFLOAT3& Axis, float Angle);
};