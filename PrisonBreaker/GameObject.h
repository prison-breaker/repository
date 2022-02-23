#pragma once
#include "Mesh.h"
#include "Material.h"
#include "Camera.h"

class CGameObject
{
protected:
	bool				          m_IsAlive{};
							      
	tstring					      m_FrameName{};
							      
	XMFLOAT4X4		              m_WorldMatrix{ Matrix4x4::Identity() };
	XMFLOAT4X4				      m_TransformMatrix{ Matrix4x4::Identity() };
							      
	shared_ptr<CMesh>	          m_Mesh{};
	vector<shared_ptr<CMaterial>> m_Materials{};
	BoundingBox					  m_BoundingBox{};

	shared_ptr<CGameObject>       m_SiblingObject{};
	shared_ptr<CGameObject>       m_ChildObject{};

public:
	CGameObject() = default;
	virtual ~CGameObject() = default;

	static shared_ptr<CGameObject> LoadObjectFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, const tstring& FileName);
	static shared_ptr<CGameObject> LoadObjectInfoFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, tifstream& InFile, unordered_map<tstring, shared_ptr<CMesh>>& MeshCache, unordered_map<tstring, vector<shared_ptr<CMaterial>>>& MaterialCache);

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
	void SetMaterial(const shared_ptr<CMaterial>& Material);

	void SetBoundingBox(const BoundingBox& BoundingBox);
	const BoundingBox& GetBoundingBox() const;

	void SetChild(const shared_ptr<CGameObject>& ChildObject);

	bool IsVisible(CCamera* Camera) const;

	void UpdateBoundingBox();
	void UpdateTransform(const XMFLOAT4X4& ParentMatrix);

	void Scale(float Pitch, float Yaw, float Roll);
	void Rotate(float Pitch, float Yaw, float Roll);
	void Rotate(const XMFLOAT3& Axis, float Angle);

	void RenderBoundingBox(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);
};
