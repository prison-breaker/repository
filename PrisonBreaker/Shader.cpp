#include "stdafx.h"
#include "Shader.h"

D3D12_SHADER_BYTECODE CShader::CompileShaderFromFile(LPCWSTR FileName, LPCSTR ShaderName, LPCSTR ShaderModelName, ID3DBlob* D3D12CodeBlob)
{
	UINT CompileFlags{};

#ifdef DEBUG_MODE
	CompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ComPtr<ID3DBlob> D3D12ErrorBlob{};

	DX::ThrowIfFailed(D3DCompileFromFile(FileName, nullptr, nullptr, ShaderName, ShaderModelName, CompileFlags, 0, &D3D12CodeBlob, D3D12ErrorBlob.GetAddressOf()));

	D3D12_SHADER_BYTECODE D3D12ShaderBytecode{};

	if (D3D12CodeBlob)
	{
		D3D12ShaderBytecode.BytecodeLength = D3D12CodeBlob->GetBufferSize();
		D3D12ShaderBytecode.pShaderBytecode = D3D12CodeBlob->GetBufferPointer();
	}
	else if (D3D12ErrorBlob)
	{
		OutputDebugString(TEXT("쉐이더를 컴파일하지 못했습니다.\n"));
	}

	return D3D12ShaderBytecode;
}

//=========================================================================================================================

D3D12_INPUT_LAYOUT_DESC CGraphicsShader::CreateInputLayout(UINT StateNum)
{
	D3D12_INPUT_LAYOUT_DESC D3D12InputLayoutDesc{};

	return D3D12InputLayoutDesc;
}

D3D12_RASTERIZER_DESC CGraphicsShader::CreateRasterizerState(UINT StateNum)
{
	D3D12_RASTERIZER_DESC D3D12RasterizerDesc{};

	D3D12RasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	D3D12RasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	D3D12RasterizerDesc.FrontCounterClockwise = false;
	D3D12RasterizerDesc.DepthBias = 0;
	D3D12RasterizerDesc.DepthBiasClamp = 0.0f;
	D3D12RasterizerDesc.SlopeScaledDepthBias = 0.0f;
	D3D12RasterizerDesc.DepthClipEnable = true;
	D3D12RasterizerDesc.MultisampleEnable = false;
	D3D12RasterizerDesc.AntialiasedLineEnable = false;
	D3D12RasterizerDesc.ForcedSampleCount = 0;
	D3D12RasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return D3D12RasterizerDesc;
}

D3D12_BLEND_DESC CGraphicsShader::CreateBlendState(UINT StateNum)
{
	D3D12_BLEND_DESC D3D12BlendDesc{};

	D3D12BlendDesc.AlphaToCoverageEnable = false;
	D3D12BlendDesc.IndependentBlendEnable = false;
	D3D12BlendDesc.RenderTarget[0].BlendEnable = false;
	D3D12BlendDesc.RenderTarget[0].LogicOpEnable = false;
	D3D12BlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	D3D12BlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	D3D12BlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	D3D12BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	D3D12BlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	D3D12BlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	D3D12BlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	D3D12BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return D3D12BlendDesc;
}

D3D12_DEPTH_STENCIL_DESC CGraphicsShader::CreateDepthStencilState(UINT StateNum)
{
	D3D12_DEPTH_STENCIL_DESC D3D12DepthStencilDesc{};

	D3D12DepthStencilDesc.DepthEnable = true;
	D3D12DepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	D3D12DepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	D3D12DepthStencilDesc.StencilEnable = false;
	D3D12DepthStencilDesc.StencilReadMask = 0xff;
	D3D12DepthStencilDesc.StencilWriteMask = 0xff;
	D3D12DepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	D3D12DepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	D3D12DepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	D3D12DepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	D3D12DepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	D3D12DepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	D3D12DepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	D3D12DepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return D3D12DepthStencilDesc;
}

D3D12_STREAM_OUTPUT_DESC CGraphicsShader::CreateStreamOutputState(UINT StateNum)
{
	D3D12_STREAM_OUTPUT_DESC D3D12StreamOutputDesc{};

	return D3D12StreamOutputDesc;
}

D3D12_SHADER_BYTECODE CGraphicsShader::CreateVertexShader(ID3DBlob* D3D12ShaderBlob, UINT StateNum)
{
	D3D12_SHADER_BYTECODE D3D12ShaderByteCode{};

	return D3D12ShaderByteCode;
}

D3D12_SHADER_BYTECODE CGraphicsShader::CreateHullShader(ID3DBlob* D3D12ShaderBlob, UINT StateNum)
{
	D3D12_SHADER_BYTECODE D3D12ShaderByteCode{};

	return D3D12ShaderByteCode;
}

D3D12_SHADER_BYTECODE CGraphicsShader::CreateDomainShader(ID3DBlob* D3D12ShaderBlob, UINT StateNum)
{
	D3D12_SHADER_BYTECODE D3D12ShaderByteCode{};

	return D3D12ShaderByteCode;
}

D3D12_SHADER_BYTECODE CGraphicsShader::CreateGeometryShader(ID3DBlob* D3D12ShaderBlob, UINT StateNum)
{
	D3D12_SHADER_BYTECODE D3D12ShaderByteCode{};

	return D3D12ShaderByteCode;
}

D3D12_SHADER_BYTECODE CGraphicsShader::CreatePixelShader(ID3DBlob* D3D12ShaderBlob, UINT StateNum)
{
	D3D12_SHADER_BYTECODE D3D12ShaderByteCode{};

	return D3D12ShaderByteCode;
}

D3D12_PRIMITIVE_TOPOLOGY_TYPE CGraphicsShader::GetPrimitiveType(UINT StateNum)
{
	return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
}

DXGI_FORMAT CGraphicsShader::GetRTVFormat(UINT RenderTargetNum, UINT StateNum)
{
	return DXGI_FORMAT_R8G8B8A8_UNORM;
}

DXGI_FORMAT CGraphicsShader::GetDSVFormat(UINT StateNum)
{
	return DXGI_FORMAT_D24_UNORM_S8_UINT;
}

void CGraphicsShader::CreatePipelineState(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature, UINT StateNum)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC D3D12GraphicsPipelineState{};
	ComPtr<ID3DBlob> D3D12VertexShaderBlob{}, D3D12HullShaderBlob{}, D3D12DomainShaderBlob{}, D3D12GeometryShaderBlob{}, D3D12PixelShaderBlob{};

	D3D12GraphicsPipelineState.pRootSignature = D3D12RootSignature;
	D3D12GraphicsPipelineState.VS = CreateVertexShader(D3D12VertexShaderBlob.Get(), StateNum);
	D3D12GraphicsPipelineState.HS = CreateHullShader(D3D12HullShaderBlob.Get(), StateNum);
	D3D12GraphicsPipelineState.DS = CreateDomainShader(D3D12DomainShaderBlob.Get(), StateNum);
	D3D12GraphicsPipelineState.GS = CreateGeometryShader(D3D12GeometryShaderBlob.Get(), StateNum);
	D3D12GraphicsPipelineState.PS = CreatePixelShader(D3D12PixelShaderBlob.Get(), StateNum);
	D3D12GraphicsPipelineState.RasterizerState = CreateRasterizerState(StateNum);
	D3D12GraphicsPipelineState.BlendState = CreateBlendState(StateNum);
	D3D12GraphicsPipelineState.DepthStencilState = CreateDepthStencilState(StateNum);
	D3D12GraphicsPipelineState.StreamOutput = CreateStreamOutputState(StateNum);
	D3D12GraphicsPipelineState.InputLayout = CreateInputLayout(StateNum);
	D3D12GraphicsPipelineState.SampleMask = UINT_MAX;
	D3D12GraphicsPipelineState.PrimitiveTopologyType = GetPrimitiveType(StateNum);
	D3D12GraphicsPipelineState.NumRenderTargets = 1;
	D3D12GraphicsPipelineState.RTVFormats[0] = GetRTVFormat(0, StateNum);
	D3D12GraphicsPipelineState.DSVFormat = GetDSVFormat(StateNum);
	D3D12GraphicsPipelineState.SampleDesc.Count = 1;
	D3D12GraphicsPipelineState.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	m_D3D12PipelineStates.emplace_back();
	DX::ThrowIfFailed(D3D12Device->CreateGraphicsPipelineState(&D3D12GraphicsPipelineState, __uuidof(ID3D12PipelineState), reinterpret_cast<void**>(m_D3D12PipelineStates.back().GetAddressOf())));

	if (D3D12GraphicsPipelineState.InputLayout.pInputElementDescs)
	{
		delete[] D3D12GraphicsPipelineState.InputLayout.pInputElementDescs;
	}
}

void CGraphicsShader::SetPipelineState(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, UINT StateNum)
{

}

void CGraphicsShader::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera)
{

}

//=========================================================================================================================

D3D12_SHADER_BYTECODE CComputeShader::CreateComputeShader(ID3DBlob* D3D12ShaderBlob, UINT StateNum)
{
	D3D12_SHADER_BYTECODE D3D12ShaderByteCode{};

	return D3D12ShaderByteCode;
}

void CComputeShader::CreatePipelineStates(ID3D12Device* D3D12Device, ID3D12RootSignature* D3D12RootSignature, const XMUINT3& ThreadGroups, UINT StateNum)
{
	D3D12_CACHED_PIPELINE_STATE D3D12CachedPipelineState{};
	D3D12_COMPUTE_PIPELINE_STATE_DESC D3D12ComputePipelineStateDesc{};
	ComPtr<ID3DBlob> D3D12ComputeShaderBlob{};

	D3D12ComputePipelineStateDesc.pRootSignature = D3D12RootSignature;
	D3D12ComputePipelineStateDesc.CS = CreateComputeShader(D3D12ComputeShaderBlob.Get(), StateNum);
	D3D12ComputePipelineStateDesc.NodeMask = 0;
	D3D12ComputePipelineStateDesc.CachedPSO = D3D12CachedPipelineState;
	D3D12ComputePipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	m_ThreadGroups = ThreadGroups;
	m_D3D12PipelineStates.emplace_back();
	DX::ThrowIfFailed(D3D12Device->CreateComputePipelineState(&D3D12ComputePipelineStateDesc, __uuidof(ID3D12PipelineState), reinterpret_cast<void**>(m_D3D12PipelineStates.back().GetAddressOf())));
}

void CComputeShader::Dispatch(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, UINT StateNum)
{
	if (m_D3D12PipelineStates[StateNum])
	{
		D3D12GraphicsCommandList->SetPipelineState(m_D3D12PipelineStates[StateNum].Get());
		D3D12GraphicsCommandList->Dispatch(m_ThreadGroups.x, m_ThreadGroups.y, m_ThreadGroups.z);
	}
}

void CComputeShader::Dispatch(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, const XMUINT3& ThreadGroups, UINT StateNum)
{
	if (m_D3D12PipelineStates[StateNum])
	{
		D3D12GraphicsCommandList->SetPipelineState(m_D3D12PipelineStates[StateNum].Get());
		D3D12GraphicsCommandList->Dispatch(ThreadGroups.x, ThreadGroups.y, ThreadGroups.z);
	}
}
