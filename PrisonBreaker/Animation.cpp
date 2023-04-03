#include "pch.h"
#include "Animation.h"

#include "AssetManager.h"

#include "Object.h"

CAnimation::CAnimation() :
	m_frameRate(),
	m_frameCount(),
	m_duration(),
	m_boneTransformMatrixes()
{
}

CAnimation::~CAnimation()
{
}

int CAnimation::GetFrameRate()
{
	return m_frameRate;
}

int CAnimation::GetFrameCount()
{
	return m_frameCount;
}

float CAnimation::GetDuration()
{
	return m_duration;
}

const vector<vector<vector<XMFLOAT4X4>>>& CAnimation::GetBoneTransformMatrixes()
{
	return m_boneTransformMatrixes;
}

void CAnimation::Load(ifstream& in)
{
	string str;
	int skinnedMeshCount = 0;

	while (true)
	{
		File::ReadStringFromFile(in, str);

		if (str == "<Name>")
		{
			File::ReadStringFromFile(in, m_name);
		}
		else if (str == "<FrameRate>")
		{
			in.read(reinterpret_cast<char*>(&m_frameRate), sizeof(int));
		}
		else if (str == "<FrameCount>")
		{
			in.read(reinterpret_cast<char*>(&m_frameCount), sizeof(int));
		}
		else if (str == "<Duration>")
		{
			in.read(reinterpret_cast<char*>(&m_duration), sizeof(float));
		}
		else if (str == "<SkinnedMeshes>")
		{
			in.read(reinterpret_cast<char*>(&skinnedMeshCount), sizeof(int));
			m_boneTransformMatrixes.resize(skinnedMeshCount);
		}
		else if (str == "<ElapsedTime>")
		{
			float elapsedTime = 0.0f;

			in.read(reinterpret_cast<char*>(&elapsedTime), sizeof(float));

			for (int i = 0; i < skinnedMeshCount; ++i)
			{
				// <BoneTransformMatrix>
				File::ReadStringFromFile(in, str);

				int boneCount = 0;

				in.read(reinterpret_cast<char*>(&boneCount), sizeof(int));

				if (m_boneTransformMatrixes[i].empty())
				{
					m_boneTransformMatrixes[i].resize(boneCount);
				}

				for (int j = 0; j < boneCount; ++j)
				{
					XMFLOAT4X4 transformMatrix = {};

					in.read(reinterpret_cast<char*>(&transformMatrix), sizeof(XMFLOAT4X4));
					m_boneTransformMatrixes[i][j].push_back(transformMatrix);
				}
			}
		}
		else if (str == "</Animation>")
		{
			break;
		}
	}
}
