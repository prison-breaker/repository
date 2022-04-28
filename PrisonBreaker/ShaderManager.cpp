#include "stdafx.h"
#include "ShaderManager.h"
#include "Shader.h"

CShaderManager* CShaderManager::GetInstance()
{
	static CShaderManager Instance{};

	return &Instance;
}

void CShaderManager::RegisterShader(const tstring& ShaderName, const shared_ptr<CShader>& Shader)
{
	if (Shader)
	{
		m_Shaders.emplace(ShaderName, Shader);
	}
}

shared_ptr<CShader> CShaderManager::GetShader(const tstring& ShaderName)
{
	if (m_Shaders.count(ShaderName))
	{
		return m_Shaders[ShaderName];
	}

	return nullptr;
}

bool CShaderManager::SetPipelineState(const tstring& ShaderName, UINT StateNum)
{
	if (m_Shaders.count(ShaderName))
	{
		if (m_SetShader != m_Shaders[ShaderName])
		{
			m_SetShader = m_Shaders[ShaderName];
			m_StateNum = StateNum;

			return true;
		}
		else if (m_StateNum != StateNum)
		{
			m_StateNum = StateNum;

			return true;
		}
	}

	return false;
}

void CShaderManager::ResetShaderAndStateNum()
{
	m_SetShader = nullptr;
	m_StateNum = 0;
}
