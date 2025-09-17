#include "EnemyHp.h"
#include"../../../Scene/SceneManager.h"
#include"../../../main.h"
#include"../../../GameObject/Character/Enemy/Enemy.h"
#include"../../Camera/PlayerCamera/PlayerCamera.h"

void EnemyHp::Init()
{
	m_displayTime = 10000;
	m_scale = Math::Vector3(0.5f, 5.0f, 1.0f);
	m_texture = KdAssets::Instance().m_textures.GetData("Asset/Textures/EnemyUI/BlackBar.png");
	m_timer = 0.0f; // 2秒で消える
}

void EnemyHp::Update()
{
	SceneManager::Instance().GetObjectWeakPtr(m_enemy);
	SceneManager::Instance().GetObjectWeakPtr(m_camera);

	auto camera = m_camera.lock()->GetCamera();
	auto enemy = m_enemy.lock();

	if (!enemy) return;
	if (!camera) return;

	// 敵のワールド座標を取得
	Math::Vector3 enemyPos = enemy->GetPos();

	// Initで生成したm_offsetを使う
	Math::Vector3 worldPos = enemyPos + m_offset;

	// ワールド座標→スクリーン座標へ変換
	camera->ConvertWorldToScreenDetail(worldPos, m_screenPos);
}

void EnemyHp::DrawSprite()
{
	KdShaderManager::Instance().m_spriteShader.SetMatrix(m_mWorld);

	std::string numStr = std::to_string(m_displayTime);

	// screenPosを使用
	int baseX = static_cast<int>(m_screenPos.x);
	int baseY = static_cast<int>(m_screenPos.y);

	const int digitWidth = 320;
	const int totalWidth = static_cast<int>(numStr.size()) * digitWidth;
	int startX = baseX - totalWidth / 2;

	for (size_t i = 0; i < numStr.size(); ++i)
	{
		int n = numStr[i] - '0';
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
