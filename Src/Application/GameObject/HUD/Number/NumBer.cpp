#include "NumBer.h"
const uint32_t NumBer::TypeID = KdGameObject::GenerateTypeID();

void NumBer::Init()
{
	m_displayTime = 0;
	m_isIncreasing = false;
	m_texture = KdAssets::Instance().m_textures.GetData("Asset/Textures/Time/Hp.png");

	m_displayTime = 10000;
}

void NumBer::Update()
{
	if (KeyboardManager::GetInstance().IsKeyPressed('7'))
	{
		m_displayTime = 0; // 0から開始
		m_isIncreasing = true;
	}

	// 増加中なら徐々に値を増やす
	if (m_isIncreasing)
	{
		m_displayTime += 1; // 1フレームごとに1増加（速度調整はお好みで）
		m_isIncreasing = (m_displayTime < 99999); // 最大値9999まで増加
	}
}

void NumBer::DrawSprite()
{
	KdShaderManager::Instance().m_spriteShader.SetMatrix(m_mWorld);

	// m_displayTimeの値を文字列化
	std::string numStr = std::to_string(m_displayTime);

	int baseX = static_cast<int>(m_position.x);
	int baseY = static_cast<int>(m_position.y);

	// 桁ごとの幅
	const int digitWidth = 250;
	const int totalWidth = static_cast<int>(numStr.size()) * digitWidth;

	// 中心寄せのため、描画開始位置を調整
	int startX = baseX - totalWidth / 2;

	// 各桁を左から順に描画
	for (size_t i = 0; i < numStr.size(); ++i)
	{
		int n = numStr[i] - '0'; // 文字→数字
		int texIndex = n;
		Math::Rectangle srcRect = { 50 * texIndex, 0, 50, 50 };
		KdShaderManager::Instance().m_spriteShader.DrawTex(
			m_texture,
			startX + digitWidth * static_cast<int>(i),
			baseY,
			&srcRect,
			&m_color
		);
	}

	KdShaderManager::Instance().m_spriteShader.SetMatrix(Math::Matrix::Identity);
}
