#include "HitDamage.h"
#include"../../Scene/SceneManager.h"
#include"../../main.h"
#include"../../GameObject/Character/Enemy/Enemy.h"
#include"../Camera/PlayerCamera/PlayerCamera.h"

void HitDamage::Init()
{
	m_displayTime = 10000;
	m_scale = Math::Vector3(0.5f, 5.0f, 1.0f);
	m_texture = KdAssets::Instance().m_textures.GetData("Asset/Textures/Time/Damage.png");
	m_timer = 1.0f; // 2秒で消える

	m_color = Math::Vector4(0.26f, 0.79f, 0.93f, 1.f);

	// ここで一度だけランダムオフセット生成
	float angle = KdRandom::GetFloat(0.0f, DirectX::XM_2PI);
	float radiusY = KdRandom::GetFloat(-0.2f, 0.1f);
	m_offset = Math::Vector3(std::cos(angle) * radiusY, 0.0f, std::sin(angle) * radiusY);
}

void HitDamage::Update()
{
	SceneManager::Instance().GetObjectWeakPtr(m_enemy);
	SceneManager::Instance().GetObjectWeakPtr(m_camera);

	auto camera = m_camera.lock()->GetCamera();
	auto enemy = m_enemy.lock();

	if (!enemy) return;
	if (!camera) return;

	// 1. 敵のワールド座標を取得
	Math::Vector3 enemyPos = enemy->GetPos();

	// 2. Initで生成したm_offsetを使う
	Math::Vector3 worldPos = enemyPos + m_offset;

	// 3. ワールド座標→スクリーン座標へ変換
	camera->ConvertWorldToScreenDetail(worldPos, m_screenPos);

	// m_scale.xを0.1まで徐々に減少
	if (m_scale.x > 0.1f) {
		m_scale.x -= 0.01f; // 減少速度は調整してください
		if (m_scale.x < 0.1f) m_scale.x = 0.1f;
	}

	// m_scale.yをバウンドさせて1.0fに収束
	static float bounceTime = 0.0f;
	bounceTime += 0.2f; // バウンド速度

	float amplitude = (m_scale.x - 0.1f) * 2.0f; // xが小さくなるほどバウンド幅も小さく
	m_scale.y = 1.0f + amplitude * std::sin(bounceTime);

	m_mWorld = Math::Matrix::CreateScale(m_scale);
	m_mWorld *= Math::Matrix::CreateFromYawPitchRoll(
		DirectX::XMConvertToRadians(m_degree.y),
		DirectX::XMConvertToRadians(m_degree.x),
		DirectX::XMConvertToRadians(m_degree.z)
	);
	m_mWorld.Translation(m_position);

	float deltaTime = Application::Instance().GetUnscaledDeltaTime();
	m_timer -= deltaTime;

	if (m_timer <= 0.0f) {
		m_isExpired = true;
	}
}

void HitDamage::DrawSprite()
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
