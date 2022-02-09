#pragma once

class CBilboardMesh
{
private:
	XMFLOAT3 m_Position{};
	XMFLOAT2 m_Size{};

public:
	CBilboardMesh(const XMFLOAT3& Position, const XMFLOAT2& Size);
	~CBilboardMesh() = default;
};
