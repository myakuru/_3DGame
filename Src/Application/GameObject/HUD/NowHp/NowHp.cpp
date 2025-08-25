#include "NowHp.h"
#include"../../../Scene/SceneManager.h"
#include"../../Character/Player/Player.h"

const uint32_t NowHp::TypeID = KdGameObject::GenerateTypeID();

void NowHp::Init()
{
	m_displayTime = 10000;
	m_texture = KdAssets::Instance().m_textures.GetData("Asset/Textures/Time/Hp.png");
}

void NowHp::Update()
{
	SceneManager::Instance().GetObjectWeakPtr(m_player);
	auto player = m_player.lock();
	if (!player) return;
	int hp = player->GetPlayerStatus().hp;
	m_displayTime = hp;
}

void NowHp::DrawSprite()
{
	KdShaderManager::Instance().m_spriteShader.SetMatrix(m_mWorld);

	// m_displayTimeの値を文字列化
	std::string numStr = std::to_string(m_displayTime);

	int baseX = static_cast<int>(m_position.x);
	int baseY = static_cast<int>(m_position.y);

	// 桁ごとの幅（1桁50px、間隔500pxで描画しているので、1桁分の描画幅は500px）
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
