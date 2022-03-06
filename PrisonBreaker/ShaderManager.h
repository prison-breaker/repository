#pragma once

class CShader;

class CShaderManager
{
private:
	unordered_map<string, shared_ptr<CShader>> m_Shaders{};

	shared_ptr<CShader>                        m_SetGlobalShader{}; // CDepthWriteShader와 같이 CMaterial 내 Shader의 영향을 받지 않고 모든 오브젝트가 사용하는 Set된 셰이더
	shared_ptr<CShader>                        m_SetShader{}; 	    // 현재 Set된 셰이더

public:
	CShaderManager() = default;
	~CShaderManager() = default;

	static CShaderManager* GetInstance();

	void RegisterShader(const tstring& ShaderName, const shared_ptr<CShader>& Shader);
	shared_ptr<CShader> GetShader(const tstring& ShaderName);

	bool SetGlobalShader(const tstring& ShaderName);
	void UnSetGlobalShader();

	bool SetShader(const tstring& ShaderName);
};
