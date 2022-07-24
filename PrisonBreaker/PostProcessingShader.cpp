#include "stdafx.h"
#include "PostProcessingShader.h"

D3D12_SHADER_BYTECODE CPostProcessingShader::CreateVertexShader(ID3DBlob* D3D12ShaderBlob, UINT StateNum)
{
	return CGraphicsShader::CompileShaderFromFile(L"GameSceneShader.hlsl", "VS_PostProcessing", "vs_5_1", D3D12ShaderBlob);
}

D3D12_SHADER_BYTECODE CPostProcessingShader::CreatePixelShader(ID3DBlob* D3D12ShaderBlob, UINT StateNum)
{
	return CGraphicsShader::CompileShaderFromFile(L"GameSceneShader.hlsl", "PS_PostProcessing", "ps_5_1", D3D12ShaderBlob);
}

void CPostProcessingShader::SetPipelineState(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, UINT StateNum)
{
	if (CShaderManager::GetInstance()->SetPipelineState(TEXT("PostProcessingShader"), StateNum))
	{
		if (m_D3D12PipelineStates[StateNum])
		{
			D3D12GraphicsCommandList->SetPipelineState(m_D3D12PipelineStates[StateNum].Get());
		}
	}
}

void CPostProcessingShader::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera)
{
	SetPipelineState(D3D12GraphicsCommandList, 0);

	CTextureManager::GetInstance()->GetTexture(TEXT("RenderingResult"))->UpdateShaderVariable(D3D12GraphicsCommandList);

	D3D12GraphicsCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	D3D12GraphicsCommandList->DrawInstanced(6, 1, 0, 0);
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

		if (m_FadeAmount > 1.0f)
		{
			m_FadeAmount = 1.0f;
			m_PostProcessingType = POST_PROCESSING_TYPE_NONE;
		}
		break;
	case POST_PROCESSING_TYPE_FADE_OUT:
		m_FadeAmount -= ElapsedTime;

		if (m_FadeAmount < 0.0f)
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
