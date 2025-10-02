#include "Scoring.h"
#include"../../Utility/Time.h"
#include"../../../Scene/SceneManager.h"

const uint32_t Scoring::TypeID = KdGameObject::GenerateTypeID();

void Scoring::Init()
{
	SelectDraw2DTexture::Init();
	m_texture = KdAssets::Instance().m_textures.GetData("Asset/Textures/GameUI/Score.png");

	m_srcRect = { 0,0,50,50 };
}

void Scoring::DrawSprite()
{
	SelectDraw2DTexture::DrawSprite();
}

void Scoring::Update()
{
	// 残り秒数を取得
	float timeLeft = Time::Instance().GetCountdownTimeLeft();

	m_displayTime = static_cast<int>(timeLeft);

	if (m_displayTime >= 60)
	{
		SceneManager::Instance().SetScore(0); //S
		m_srcRect = { 0,0,50,50 };
		m_color = { 1.0f, 0.43f, 0.04f, 1.0f };
	}
	else if (m_displayTime >= 30)
	{
		SceneManager::Instance().SetScore(1); //A
		m_srcRect = { 50,0,50,50 };
		m_color = { 0.71f, 0.24f, 1.0f, 1.0f };
	}
	else if(m_displayTime >= 10)
	{
		SceneManager::Instance().SetScore(2); //B
		m_srcRect = { 100,0,50,50 };
		m_color = { 0.19f, 0.62f, 1.0f, 1.0f };
	}
	else
	{
		SceneManager::Instance().SetScore(3); //X
	}
}
