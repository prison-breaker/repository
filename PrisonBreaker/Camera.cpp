#include "stdafx.h"
#include "Camera.h"

void CCamera::CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	UINT Bytes{ (sizeof(CB_CAMERA) + 255) & ~255 };

	m_D3D12Camera = DX::CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, nullptr, Bytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr);
	DX::ThrowIfFailed(m_D3D12Camera->Map(0, nullptr, (void**)&m_MappedCamera));
}

void CCamera::UpdateShaderVariables(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	XMStoreFloat4x4(&m_MappedCamera->m_ViewMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_ViewMatrix)));
	XMStoreFloat4x4(&m_MappedCamera->m_ProjectionMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_ProjectionMatrix)));
	memcpy(&m_MappedCamera->m_Position, &m_Position, sizeof(XMFLOAT3));

	D3D12GraphicsCommandList->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_TYPE_CAMERA, m_D3D12Camera->GetGPUVirtualAddress());
}

void CCamera::ReleaseShaderVariables()
{
	m_D3D12Camera->Unmap(0, nullptr);
}

void CCamera::SetViewport(int TopLeftX, int TopLeftY, UINT Width, UINT Height, float MinDepth, float MaxDepth)
{
	m_D3D12Viewport.TopLeftX = (float)TopLeftX;
	m_D3D12Viewport.TopLeftY = (float)TopLeftY;
	m_D3D12Viewport.Width = (float)Width;
	m_D3D12Viewport.Height = (float)Height;
	m_D3D12Viewport.MinDepth = MinDepth;
	m_D3D12Viewport.MaxDepth = MaxDepth;
}

void CCamera::SetScissorRect(LONG Left, LONG Top, LONG Right, LONG Bottom)
{
	m_D3D12ScissorRect.left = Left;
	m_D3D12ScissorRect.top = Top;
	m_D3D12ScissorRect.right = Right;
	m_D3D12ScissorRect.bottom = Bottom;
}

void CCamera::RSSetViewportsAndScissorRects(ID3D12GraphicsCommandList* D3D12GraphicsCommandList) const
{
	D3D12GraphicsCommandList->RSSetViewports(1, &m_D3D12Viewport);
	D3D12GraphicsCommandList->RSSetScissorRects(1, &m_D3D12ScissorRect);
}

void CCamera::GenerateViewMatrix(const XMFLOAT3& Position, const XMFLOAT3& Look)
{
	m_Position = Position;
	m_Look = Look;

	RegenerateViewMatrix();
}

void CCamera::RegenerateViewMatrix()
{
	const XMFLOAT3 WorldUp{ 0.0f, 1.0f, 0.0f };

	m_Look = Vector3::Normalize(m_Look);
	m_Right = Vector3::CrossProduct(WorldUp, m_Look, false);
	m_Up = Vector3::CrossProduct(m_Look, m_Right, false);
	m_ViewMatrix = Matrix4x4::LookToLH(m_Position, m_Look, WorldUp);

	GenerateBoundingFrustum();
}

const XMFLOAT4X4& CCamera::GetViewMatrix() const
{
	return m_ViewMatrix;
}

void CCamera::GenerateOrthographicsProjectionMatrix(float ViewWidth, float ViewHeight, float NearZ, float FarZ)
{
	m_ProjectionMatrix = Matrix4x4::OrthographicFovLH(ViewWidth, ViewHeight, NearZ, FarZ);
}

void CCamera::GeneratePerspectiveProjectionMatrix(float FOVAngleY, float AspectRatio, float NearZ, float FarZ)
{
	m_ProjectionMatrix = Matrix4x4::PerspectiveFovLH(XMConvertToRadians(FOVAngleY), AspectRatio, NearZ, FarZ);
}

const XMFLOAT4X4& CCamera::GetProjectionMatrix() const
{
	return m_ProjectionMatrix;
}

void CCamera::GenerateBoundingFrustum()
{
	// 원근 투영 변환 행렬에서 절두체를 생성한다(절두체는 카메라 좌표계로 표현된다).
	m_BoundingFrustum.CreateFromMatrix(m_BoundingFrustum, XMLoadFloat4x4(&m_ProjectionMatrix));

	// 카메라 변환 행렬의 역행렬을 구한다.
	XMMATRIX InverseViewMatrix{ XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_ViewMatrix)) };

	// 절두체를 카메라 변환 행렬의 역행렬로 변환한다(이제 절두체는 월드 좌표계로 표현된다).
	m_BoundingFrustum.Transform(m_BoundingFrustum, InverseViewMatrix);
}

bool CCamera::IsInBoundingFrustum(const BoundingBox& BoundingBox) const
{
	return m_BoundingFrustum.Intersects(BoundingBox);
}

const XMFLOAT3& CCamera::GetRight() const
{
	return m_Right;
}

const XMFLOAT3& CCamera::GetUp() const
{
	return m_Up;
}

const XMFLOAT3& CCamera::GetLook() const
{
	return m_Look;
}

void CCamera::SetPosition(const XMFLOAT3& Position)
{
	m_Position = Position;
}

const XMFLOAT3& CCamera::GetPosition() const
{
	return m_Position;
}

void CCamera::Move(const XMFLOAT3& Shift)
{
	m_Position = Vector3::Add(m_Position, Shift);

	RegenerateViewMatrix();
}

void CCamera::Rotate(float Pitch, float Yaw, float Roll)
{
	if (Pitch)
	{
		XMFLOAT4X4 RotationMatrix{ Matrix4x4::RotationAxis(m_Right, Pitch) };
		m_Look = Vector3::TransformNormal(m_Look, RotationMatrix);
	}

	if (Yaw)
	{
		XMFLOAT4X4 RotationMatrix{ Matrix4x4::RotationAxis(m_Up, Yaw) };
		m_Look = Vector3::TransformNormal(m_Look, RotationMatrix);
	}

	if (Roll)
	{
		XMFLOAT4X4 RotationMatrix{ Matrix4x4::RotationAxis(m_Look, Roll) };
		m_Look = Vector3::TransformNormal(m_Look, RotationMatrix);
	}

	RegenerateViewMatrix();
}

void CCamera::Rotate(const XMFLOAT4X4& WorldMatrix, float ElapsedTime)
{
	XMFLOAT4X4 RotationMatrix{
		WorldMatrix._11, WorldMatrix._12, WorldMatrix._13, 0.0f,
		WorldMatrix._21, WorldMatrix._22, WorldMatrix._23, 0.0f,
		WorldMatrix._31, WorldMatrix._32, WorldMatrix._33, 0.0f,
				   0.0f,			0.0f,			 0.0f, 1.0f
	};
	XMFLOAT3 Position{ WorldMatrix._41, WorldMatrix._42 + 5.0f, WorldMatrix._43 };
	XMFLOAT3 NewOffset{ Vector3::TransformNormal(m_Offset, RotationMatrix) };
	XMFLOAT3 NewPosition{ Vector3::Add(Position, NewOffset) };
	XMFLOAT3 Direction{ Vector3::Subtract(NewPosition, m_Position) };
	float Distance{ Vector3::Length(Direction) };
	float TimeLagScale{ (m_TimeLag) ? ElapsedTime * (1.0f / m_TimeLag) : 1.0f };
	float Shift{ Distance * TimeLagScale };

	if (Shift > 0.0f)
	{
		Direction = Vector3::Normalize(Direction);
		m_Position = Vector3::Add(m_Position, Vector3::ScalarProduct(Shift, Direction, false));
		m_Look = Vector3::Subtract(Position, m_Position);
	}

	RegenerateViewMatrix();
}
