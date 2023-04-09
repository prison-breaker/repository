#include "pch.h"
#include "SpriteRenderer.h"

#include "TimeManager.h"

CSpriteRenderer::CSpriteRenderer() :
    m_isLoop(),
    m_spriteSize(),
    m_startFrameIndex(),
    m_endFrameIndex(),
    m_frameIndex(),
    m_duration(),
    m_elapsedTime()
{
}

CSpriteRenderer::~CSpriteRenderer()
{
}

void CSpriteRenderer::SetSpriteSize(const XMFLOAT2& spriteSize)
{
    m_spriteSize = spriteSize;
}

const XMFLOAT2& CSpriteRenderer::GetSpriteSize()
{
    return m_spriteSize;
}

void CSpriteRenderer::SetFrameIndex(int frameIndex)
{
    m_frameIndex = frameIndex;
}

int CSpriteRenderer::GetFrameIndex()
{
    return m_frameIndex;
}

void CSpriteRenderer::SetDuration(float duration)
{
    m_duration = duration;
}

float CSpriteRenderer::GetDuration()
{
    return m_duration;
}

void CSpriteRenderer::CreateShaderVariables(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList)
{
}

void CSpriteRenderer::UpdateShaderVariables(ID3D12GraphicsCommandList* d3d12GraphicsCommandList)
{
    d3d12GraphicsCommandList->SetGraphicsRoot32BitConstants(static_cast<UINT>(ROOT_PARAMETER_TYPE::SPRITE), 2, &m_spriteSize, 23);
    d3d12GraphicsCommandList->SetGraphicsRoot32BitConstants(static_cast<UINT>(ROOT_PARAMETER_TYPE::SPRITE), 1, &m_frameIndex, 25);
}

void CSpriteRenderer::Play(bool isLoop, int startFrameIndex, int endFrameIndex)
{
    int validIndex = static_cast<int>(m_spriteSize.x * m_spriteSize.y - 1);

    if ((startFrameIndex < 0) || (endFrameIndex < 0) || (startFrameIndex >= validIndex) || (endFrameIndex > validIndex) || (startFrameIndex >= endFrameIndex))
    {
        return;
    }

    m_isLoop = isLoop;
    m_startFrameIndex = startFrameIndex;
    m_endFrameIndex = endFrameIndex;
}

void CSpriteRenderer::Update()
{
    if (!m_isEnabled || !m_isLoop)
    {
        return;
    }

    int validIndex = (m_endFrameIndex > 0) ? m_endFrameIndex : static_cast<int>(m_spriteSize.x * m_spriteSize.y - 1);

    m_elapsedTime += DT;

    while (m_elapsedTime >= m_duration)
    {
        m_elapsedTime -= m_duration;
        ++m_frameIndex;

        if (m_frameIndex > validIndex)
        {
            if (m_isLoop)
            {
                m_frameIndex = m_startFrameIndex;
            }
            else
            {
                --m_frameIndex;
                break;
            }
        }
    }
}
