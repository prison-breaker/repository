#pragma once

class CShader;

class CShaderManager
{
private:
	unordered_map<string, shared_ptr<CShader>> m_Shaders{};

	shared_ptr<CShader>                        m_SetShader{}; // ���� Set�� ���̴�
	UINT									   m_StateNum{};  // ���� Set�� ���̴��� PipelineState ��ȣ

private:
	CShaderManager() = default;
	~CShaderManager() = default;

public:
	static CShaderManager* GetInstance();

	void RegisterShader(const tstring& ShaderName, const shared_ptr<CShader>& Shader);
	shared_ptr<CShader> GetShader(const tstring& ShaderName);

	bool SetPipelineState(const tstring& ShaderName, UINT StateNum);
	void ResetShaderAndStateNum();
};
