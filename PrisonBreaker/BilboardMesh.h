#pragma once

class CBilboardMesh
{
private:
	XMFLOAT3 m_Position{};
	XMFLOAT2 m_Size{};

public:
	CBilboardMesh(const XMFLOAT3& Position, const XMFLOAT2& Size);
	~CBilboardMesh() = default;

	// �� Ŭ������ GPU�� �����ּҷ� ���Ǳ� ������ ���� �д� ������ �ſ� �����Ƿ� ����� ��������!
	void SetPosition(const XMFLOAT3& Position);
};
