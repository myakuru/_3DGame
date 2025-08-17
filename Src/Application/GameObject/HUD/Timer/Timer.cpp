#include "Timer.h"
#include"../../Utility/Time.h"
const uint32_t Timer::TypeID = KdGameObject::GenerateTypeID();

Timer::Timer()
{
	m_typeID = TypeID;
}

void Timer::Init()
{
	m_srcRect = { 0, 0, 128, 256 }; // 1
	//m_srcRect = { 128, 0, 128, 256 }; // 2
	m_texture = KdAssets::Instance().m_textures.GetData("Asset/Textures/Time/Digit01.png");
	m_time = 120.0f; // 初期時間300秒
}

void Timer::Update()
{
	Time::Instance().Update();
	// 残り秒数を取得
	float timeLeft = Time::Instance().GetCountdownTimeLeft();

	if (KeyboardManager::GetInstance().IsKeyJustPressed('4'))
	{
		Time::Instance().StartCountdown(m_time);
	}

	// 秒をフレーム数に変換（1秒=60フレーム）
	m_displayTime = static_cast<int>(timeLeft);
}

void Timer::DrawSprite()
{
	KdShaderManager::Instance().m_spriteShader.SetMatrix(m_mWorld);

	int time = std::min(m_displayTime, 99 * 60 * 60 + 99 * 60 + 99);

	int hours = time / 3600;
	int minutes = (time / 60) % 100;
	int seconds = time % 60;

	int digits[6] = {
		hours / 10,
		hours % 10,
		minutes / 10,
		minutes % 10,
		seconds / 10,
		seconds % 10
	};

	int baseX = static_cast<int>(m_position.x);
	int baseY = static_cast<int>(m_position.y);

	// 数字とコロンの合計表示数（数字6＋コロン2＝8）
	for (int i = 0; i < 8; ++i)
	{
		if (i == 2 || i == 5)
		{
			// コロンの描画（2桁目と4桁目の後）
			Math::Rectangle colonRect = { 128 * 10, 0, 128, 256 };
			KdShaderManager::Instance().m_spriteShader.DrawTex(
				m_texture,
				baseX + 1280 * i,
				baseY,
				&colonRect,
				&m_color
			);
		}
		else
		{
			// 数字の描画
			int digitIdx = i - (i > 2 ? 1 : 0) - (i > 5 ? 1 : 0); // コロン分インデックス調整
			int n = digits[digitIdx];
			int texIndex = (n == 0) ? 9 : n - 1; // 0はインデックス9
			Math::Rectangle srcRect = { 128 * texIndex, 0, 128, 256 };
			KdShaderManager::Instance().m_spriteShader.DrawTex(
				m_texture,
				baseX + 1280 * i,
				baseY,
				&srcRect,
				&m_color
			);
		}
	}

	KdShaderManager::Instance().m_spriteShader.SetMatrix(Math::Matrix::Identity);
}
