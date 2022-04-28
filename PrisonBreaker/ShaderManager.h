#pragma once

class CShader;

class CShaderManager
{
private:
	unordered_map<string, shared_ptr<CShader>> m_Shaders{};

	shared_ptr<CShader>                        m_SetShader{}; // 현재 Set된 셰이더
	UINT									   m_StateNum{};  // 현재 Set된 셰이더의 PipelineState 번호

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
