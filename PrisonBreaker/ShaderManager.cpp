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

bool CShaderManager::SetGlobalShader(const tstring& ShaderName)
{
	if (m_Shaders.count(ShaderName))
	{
		m_SetGlobalShader = m_Shaders[ShaderName];
		m_SetShader = nullptr;

		return true;
	}

	return false;
}

void CShaderManager::UnSetGlobalShader()
{
	m_SetGlobalShader = nullptr;
}

bool CShaderManager::SetShader(const tstring& ShaderName)
{
	if (m_SetGlobalShader)
	{
		return false;
	}
	else if (m_Shaders.count(ShaderName))
	{
		if (m_SetShader == m_Shaders[ShaderName])
		{
			return false;
		}
		else
		{
			m_SetShader = m_Shaders[ShaderName];

			return true;
		}
	}

	return false;
}