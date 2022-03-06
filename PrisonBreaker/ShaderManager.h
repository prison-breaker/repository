#pragma once

class CShader;

class CShaderManager
{
private:
	unordered_map<string, shared_ptr<CShader>> m_Shaders{};

	shared_ptr<CShader>                        m_SetGlobalShader{}; // CDepthWriteShader�� ���� CMaterial �� Shader�� ������ ���� �ʰ� ��� ������Ʈ�� ����ϴ� Set�� ���̴�
	shared_ptr<CShader>                        m_SetShader{}; 	    // ���� Set�� ���̴�

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
