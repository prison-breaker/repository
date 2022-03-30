#pragma once

struct CB_CAMERA
{
	XMFLOAT4X4 m_ViewMatrix{};
	XMFLOAT4X4 m_ProjectionMatrix{};
			 
	XMFLOAT3   m_Position{};
};

class CCamera
{
private:
	D3D12_VIEWPORT              m_D3D12Viewport{ 0, 0, CLIENT_WIDTH, CLIENT_HEIGHT, 0.0f, 1.0f };
	D3D12_RECT	                m_D3D12ScissorRect{ 0, 0, CLIENT_WIDTH, CLIENT_HEIGHT };
				                
	XMFLOAT4X4	                m_ViewMatrix{ Matrix4x4::Identity() };
	XMFLOAT4X4	                m_ProjectionMatrix{ Matrix4x4::Identity() };
				                   
	XMFLOAT3	                m_Right{ 1.0f, 0.0f, 0.0f };
	XMFLOAT3	                m_Up{ 0.0f, 1.0f, 0.0f };
	XMFLOAT3	                m_Look{ 0.0f, 0.0f, 1.0f };
	XMFLOAT3	                m_Position{};
				                   
	float		                m_Pitch{};
	float		                m_Roll{};
	float		                m_Yaw{};
				                
	XMFLOAT3	                m_Offset{ 0.0f, 0.0, -4.0f };
	float		                m_TimeLag{ 0.1f };
	
	BoundingFrustum             m_BoundingFrustum{};

	ComPtr<ID3D12Resource>      m_D3D12Camera{};
	CB_CAMERA*			        m_MappedCamera{};

public:
	CCamera() = default;
	~CCamera() = default;

	void CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	void UpdateShaderVariables(ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	void ReleaseShaderVariables();

	void SetViewport(int TopLeftX, int TopLeftY, UINT Width, UINT Height, float MinDepth, float MaxDepth);
	void SetScissorRect(LONG Left, LONG Top, LONG Right, LONG Bottom);
	void RSSetViewportsAndScissorRects(ID3D12GraphicsCommandList* D3D12GraphicsCommandList) const;

	void GenerateViewMatrix(const XMFLOAT3& Position, const XMFLOAT3& Look);
	void RegenerateViewMatrix();
	const XMFLOAT4X4& GetViewMatrix() const;

	void GenerateOrthographicsProjectionMatrix(float ViewWidth, float ViewHeight, float NearZ, float FarZ);
	void GeneratePerspectiveProjectionMatrix(float FOVAngleY, float AspectRatio, float NearZ, float FarZ);
	const XMFLOAT4X4& GetProjectionMatrix() const;

	void GenerateBoundingFrustum();
	bool IsInBoundingFrustum(const BoundingBox& BoundingBox) const;

	const XMFLOAT3& GetRight() const;
	const XMFLOAT3& GetUp() const;
	const XMFLOAT3& GetLook() const;

	void SetPosition(const XMFLOAT3& Position);
	const XMFLOAT3& GetPosition() const;

	void Move(const XMFLOAT3& Shift);
	void Rotate(float Pitch, float Yaw, float Roll);
	void Rotate(const XMFLOAT4X4& WorldMatrix, float ElapsedTime);
};
