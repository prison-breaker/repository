#pragma once
#include "Shader.h"

class CPostProcessingShader : public CShader
{
private:
	POST_PROCESSING_TYPE m_PostProcessingType;// { POST_PROCESSING_TYPE_FADE_IN };

	float				 m_FadeAmount{};
	float				 m_LetterboxAmount{};

public:
	CPostProcessingShader() = default;
	virtual ~CPostProcessingShader() = default;

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob* D3D12ShaderBlob, UINT StateNum);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob* D3D12ShaderBlob, UINT StateNum);

	virtual void SetPipelineState(UINT StateNum);

	virtual void Render(CCamera* Camera);

	void SetPostProcessingType(POST_PROCESSING_TYPE PostProcessingType);
	POST_PROCESSING_TYPE GetPostProcessingType() const;

	void UpdateFadeAmount(float ElapsedTime);
	float GetFadeAmount() const;

	void SetLetterboxAmount(float LetterboxAmount);
	float GetLetterboxAmount() const;
};
