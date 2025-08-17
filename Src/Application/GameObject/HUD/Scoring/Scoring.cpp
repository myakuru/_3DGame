#include "Scoring.h"

const uint32_t Scoring::TypeID = KdGameObject::GenerateTypeID();

void Scoring::Init()
{
	SelectDraw2DTexture::Init();
	m_texture = KdAssets::Instance().m_textures.GetData("Asset/Textures/GameUI/Score.png");

	m_srcRect = { 0,0,50,50 };
}

void Scoring::DrawSprite()
{
	KdShaderManager::Instance().m_spriteShader.SetMatrix(m_mWorld);
		KdShaderManager::Instance().m_spriteShader.DrawTex(
			m_texture,
			static_cast<int>(m_position.x),
			static_cast<int>(m_position.y),
			&m_srcRect,
			&m_color
		);
	KdShaderManager::Instance().m_spriteShader.SetMatrix(Math::Matrix::Identity);
}

void Scoring::Update()
{
	if (KeyboardManager::GetInstance().IsKeyJustPressed('1'))
	{
		m_srcRect = { 0,0,50,50 };
		m_color = { 1.0f, 0.43f, 0.04f, 1.0f };
	}
	if (KeyboardManager::GetInstance().IsKeyJustPressed('2'))
	{
		m_srcRect = { 50,0,50,50 };
		m_color = { 0.71f, 0.24f, 1.0f, 1.0f };
	}
	if (KeyboardManager::GetInstance().IsKeyJustPressed('3'))
	{
		m_srcRect = { 100,0,50,50 };
		m_color = { 0.19f, 0.62f, 1.0f, 1.0f };
	}
}
