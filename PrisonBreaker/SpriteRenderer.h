#pragma once
#include "Component.h"

class CSpriteRenderer : public CComponent
{
private:
	bool	 m_isLoop;

	XMFLOAT2 m_spriteSize;
	int      m_startFrameIndex;
	int	     m_endFrameIndex;
	int		 m_frameIndex;

	float	 m_duration;
	float	 m_elapsedTime;

public:
	CSpriteRenderer();
	virtual ~CSpriteRenderer();

	void SetSpriteSize(const XMFLOAT2& spriteSize);
	const XMFLOAT2& GetSpriteSize();

	void SetFrameIndex(int frameIndex);
	int GetFrameIndex();

	void SetDuration(float duration);
	float GetDuration();

	virtual void CreateShaderVariables(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* d3d12GraphicsCommandList);

	void Play(bool isLoop, int startFrameIndex, int endFrameIndex);

	virtual void Update();
};
