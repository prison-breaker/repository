#pragma once
#include "Mesh.h"
#include "Material.h"
#include "Camera.h"
#include "AnimationController.h"
#include "StateMachine.h"
#include "NavMesh.h"

class CGameObject;

struct LOADED_MODEL_INFO
{
	shared_ptr<CGameObject>			        m_Model{};
									        
	vector<shared_ptr<CAnimationClip>>      m_AnimationClips{};

	vector<vector<shared_ptr<CGameObject>>> m_BoneFrameCaches{}; // [SkinnedMesh][Bone]
	vector<shared_ptr<CSkinnedMesh>>	    m_SkinnedMeshCaches{};
};

class CGameObject : public enable_shared_from_this<CGameObject>
{
protected:
	bool				             m_IsActive{};
									 
	tstring					         m_FrameName{};
									 
	XMFLOAT4X4		                 m_WorldMatrix{ Matrix4x4::Identity() };
	XMFLOAT4X4				         m_TransformMatrix{ Matrix4x4::Identity() };
							     	 
	shared_ptr<CMesh>	             m_Mesh{};
	vector<shared_ptr<CMaterial>>    m_Materials{};
	shared_ptr<CAnimationController> m_AnimationController{};
									 
	shared_ptr<BoundingBox>			 m_BoundingBox{};
									 
	vector<shared_ptr<CGameObject>>  m_ChildObjects{};

public:
	CGameObject() = default;
	virtual ~CGameObject() = default;

	static shared_ptr<LOADED_MODEL_INFO> LoadObjectFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, const tstring& FileName, unordered_map<tstring, shared_ptr<CMesh>>& MeshCaches, unordered_map<tstring, shared_ptr<CMaterial>>& MaterialCaches);
	static shared_ptr<CGameObject> LoadModelInfoFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, tifstream& InFile, unordered_map<tstring, shared_ptr<CMesh>>& MeshCaches, unordered_map<tstring, shared_ptr<CMaterial>>& MaterialCaches);
	static void LoadAnimationInfoFromFile(tifstream& InFile, const shared_ptr<LOADED_MODEL_INFO>& ModelInfo);

	virtual void Initialize();

	virtual void CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	virtual void ReleaseShaderVariables();

	virtual void ReleaseUploadBuffers();

	virtual void Animate(float ElapsedTime);

	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera, RENDER_TYPE RenderType);
	virtual void RenderBoundingBox(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);

	shared_ptr<CGameObject> FindFrame(const tstring& FrameName);
	shared_ptr<CSkinnedMesh> FindSkinnedMesh(const tstring& SkinnedMeshName);

	shared_ptr<CGameObject> PickObjectByRayIntersection(const XMFLOAT3& RayOrigin, const XMFLOAT3& RayDirection, float& HitDistance, float MaxDistance);

	void SetActive(bool IsActive);
	bool IsActive() const;

	const tstring& GetName() const;

	const XMFLOAT4X4& GetWorldMatrix() const;

	void SetTransformMatrix(const XMFLOAT4X4& TransformMatrix);
	const XMFLOAT4X4& GetTransformMatrix() const;

	void SetRight(const XMFLOAT3& Right);
	XMFLOAT3 GetRight() const;

	void SetUp(const XMFLOAT3& Up);
	XMFLOAT3 GetUp() const;

	void SetLook(const XMFLOAT3& Look);
	XMFLOAT3 GetLook() const;

	void UpdateLocalCoord(const XMFLOAT3& LookDirection);

	void SetPosition(const XMFLOAT3& Position);
	XMFLOAT3 GetPosition() const;

	void SetMesh(const shared_ptr<CMesh>& Mesh);

	void SetMaterial(const shared_ptr<CMaterial>& Material);

	void SetAnimationController(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, const shared_ptr<LOADED_MODEL_INFO>& ModelInfo);
	shared_ptr<CAnimationController> GetAnimationController() const;

	void SetAnimationClip(UINT ClipNum);
	UINT GetAnimationClip() const;

	void SetBoundingBox(const shared_ptr<BoundingBox>& BoundingBox);
	shared_ptr<BoundingBox> GetBoundingBox() const;

	void SetChild(const shared_ptr<CGameObject>& ChildObject);

	bool IsVisible(CCamera* Camera) const;

	bool IsInNavMesh(const shared_ptr<CNavMesh>& NavMesh, XMFLOAT3& NewPosition);

	void UpdateBoundingBox();
	void UpdateTransform(const XMFLOAT4X4& ParentMatrix);

	void Move(const XMFLOAT3& Direction, float Distance);
	void Scale(float Pitch, float Yaw, float Roll);
	void Rotate(float Pitch, float Yaw, float Roll);
	void Rotate(const XMFLOAT3& Axis, float Angle);
};
