#pragma once

class CBilboardMesh
{
private:
	XMFLOAT3 m_Position{};
	XMFLOAT2 m_Size{};

public:
	CBilboardMesh(const XMFLOAT3& Position, const XMFLOAT2& Size);
	~CBilboardMesh() = default;

	// 이 클래스는 GPU의 가상주소로 사용되기 때문에 값을 읽는 연산은 매우 느리므로 사용을 자제하자!
	void SetPosition(const XMFLOAT3& Position);
};
