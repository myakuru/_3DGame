#include "InvestigationComplete.h"
#include"../../Utility/Time.h"
#include"../../../main.h"

const uint32_t InvestigationComplete::TypeID = KdGameObject::GenerateTypeID();

void InvestigationComplete::Init()
{
	SelectDraw2DTexture::Init();
	m_isComplete = false;
}

void InvestigationComplete::DrawSprite()
{
	if (m_isComplete) return;

	KdShaderManager::Instance().m_spriteShader.SetMatrix(m_mWorld);
	SelectDraw2DTexture::DrawSprite();
	KdShaderManager::Instance().m_spriteShader.SetMatrix(Math::Matrix::Identity);
}

void InvestigationComplete::Update()
{
	// 3秒で消える
	float time = Time::Instance().GetElapsedTime();
	float deltaTime = Application::Instance().GetDeltaTime();

	if (time >= 4.6f)
	{
		if (m_scale.y >= 0.0f)
		{
			m_scale.y -= 50.0f * deltaTime;
		}
		else
		{
			m_scale.y = 0.0f;
			m_isComplete = true;
		}
	}

	m_mWorld = Math::Matrix::CreateScale(m_scale);
	m_mWorld *= Math::Matrix::CreateFromYawPitchRoll(
		DirectX::XMConvertToRadians(m_degree.y),
		DirectX::XMConvertToRadians(m_degree.x),
		DirectX::XMConvertToRadians(m_degree.z)
	);
	m_mWorld.Translation(m_position);

}