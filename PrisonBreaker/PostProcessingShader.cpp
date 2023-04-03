#include "pch.h"
#include "PostProcessingShader.h"
#include "Texture.h"

D3D12_SHADER_BYTECODE CPostProcessingShader::CreateVertexShader(ID3DBlob* D3D12ShaderBlob, UINT StateNum)
{
	return CGraphicsShader::CompileShaderFromFile(L"main.hlsl", "VS_PostProcessing", "vs_5_1", D3D12ShaderBlob);
}

D3D12_SHADER_BYTECODE CPostProcessingShader::CreatePixelShader(ID3DBlob* D3D12ShaderBlob, UINT StateNum)
{
	return CGraphicsShader::CompileShaderFromFile(L"main.hlsl", "PS_PostProcessing", "ps_5_1", D3D12ShaderBlob);
}

void CPostProcessingShader::SetPipelineState(ID3D12GraphicsCommandList* d3d12GraphicsCommandList, UINT StateNum)
{
	if (CShaderManager::GetInstance()->SetPipelineState(TEXT("PostProcessingShader"), StateNum))
	{
		if (m_d3d12PipelineStates[StateNum])
		{
			d3d12GraphicsCommandList->SetPipelineState(m_d3d12PipelineStates[StateNum].Get());
		}
	}
}

void CPostProcessingShader::Render(ID3D12GraphicsCommandList* d3d12GraphicsCommandList, CCamera* Camera)
{
	SetPipelineState(d3d12GraphicsCommandList, 0);

	CTextureManager::GetInstance()->GetTexture(TEXT("RenderingResult"))->UpdateShaderVariable(d3d12GraphicsCommandList);

	d3d12GraphicsCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	d3d12GraphicsCommandList->DrawInstanced(6, 1, 0, 0);
}

void CPostProcessingShader::SetPostProcessingType(POST_PROCESSING_TYPE PostProcessingType)
{
	if (m_PostProcessingType == POST_PROCESSING_TYPE_NONE)
	{
		m_PostProcessingType = PostProcessingType;
	}
}

POST_PROCESSING_TYPE CPostProcessingShader::GetPostProcessingType() const
{
	return m_PostProcessingType;
}

void CPostProcessingShader::UpdateFadeAmount(float ElapsedTime)
{
	switch (m_PostProcessingType)
	{
	case POST_PROCESSING_TYPE_FADE_IN:
		m_FadeAmount += ElapsedTime;

		if (m_FadeAmount >= 1.0f)
		{
			m_FadeAmount = 1.0f;
			m_PostProcessingType = POST_PROCESSING_TYPE_NONE;
		}
		break;
	case POST_PROCESSING_TYPE_FADE_OUT:
		m_FadeAmount -= ElapsedTime;

		if (m_FadeAmount <= 0.0f)
		{
			m_FadeAmount = 0.0f;
			m_PostProcessingType = POST_PROCESSING_TYPE_NONE;
		}
		break;
	}
}

float CPostProcessingShader::GetFadeAmount() const
{
	return m_FadeAmount;
}

void CPostProcessingShader::SetLetterboxAmount(float LetterboxAmount)
{
	m_LetterboxAmount = LetterboxAmount;

	if (m_LetterboxAmount > 0.15f)
	{
		m_LetterboxAmount = 0.15f;
	}
}

float CPostProcessingShader::GetLetterboxAmount() const
{
	return m_LetterboxAmount;
}
