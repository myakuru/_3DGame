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
	m_timer = 1.0f; // 1秒で消える

	m_color = Math::Vector4(0.26f, 0.79f, 0.93f, 1.f);

	// ここで一度だけランダムオフセット生成
	float radiusY = KdRandom::GetFloat(-0.5f, 0.5f);
	m_offset = Math::Vector3(radiusY, 1.0f + radiusY, 0);

	// 幅は0から開始してアニメで伸ばす
	m_textureSizeX = 0.0f;
	m_textureSizeY = 0.0f;
}

void HitDamage::Update()
{
	SceneManager::Instance().GetObjectWeakPtr(m_camera);

	// カメラが無ければ終了
	auto camera = m_camera.lock();
	if (!camera) { m_isExpired = true; return; }

	bool updated = false;

	// すでに追従対象がある場合はそれだけを追う
	if (auto tracked = m_enemy.lock())
	{
		// 対象が消滅していたら自分も消す
		if (tracked->IsExpired())
		{
			m_isExpired = true;
			return;
		}

		if (auto cam = camera->GetCamera())
		{
			m_enemyPos = tracked->GetPos();
			cam->ConvertWorldToScreenDetail(m_enemyPos + m_offset, m_screenPos);
			updated = true;
		}
	}
	else
	{
		// まだ未固定なら、このフレームでヒット中の敵を一体だけ見つけて固定
		SceneManager::Instance().GetObjectWeakPtrList(m_enemies);

		for (const auto& e : m_enemies)
		{
			if (auto ep = e.lock())
			{
				if (!ep->EnemyHit()) { continue; } // ヒット中のみ対象

				// 以降この敵だけを追従
				m_enemy = ep;

				if (auto cam = camera->GetCamera())
				{
					m_enemyPos = ep->GetPos();
					cam->ConvertWorldToScreenDetail(m_enemyPos + m_offset, m_screenPos);
					updated = true;
				}
				break; // 最初に見つけた一体で打ち切り
			}
		}
	}

	// 表示対象が見つからなければ終了
	if (!updated)
	{
		m_isExpired = true;
		return;
	}

	// ここから下はアニメ/寿命処理
	float deltaTime = Application::Instance().GetUnscaledDeltaTime();

	// 生成からの経過時間（m_timerは1.0→0.0へ減るので反転）
	float animTime = 1.0f - m_timer;
	if (animTime < 0.0f) animTime = 0.0f;

	// 0→50の成長 + 減衰付きバウンド（幅は最大50に制限して隣桁の食い込みを防止）
	constexpr float kBaseWidth = 50.0f;   // 1桁の元幅（スプライトシート列幅）
	constexpr float kGrowDur = 0.12f;   // 0→50までの成長時間(秒)
	constexpr float kBounceFreq = 18.0f;   // バウンド周波数(ラジアン/秒)
	constexpr float kBounceAmpPx = 10.0f;   // 初期振幅(ピクセル)
	constexpr float kDamp = 4.0f;    // 減衰係数

	if (animTime < kGrowDur)
	{
		// イージング（easeOutBack）
		float t = std::clamp(animTime / kGrowDur, 0.0f, 1.0f);
		const float s = 1.70158f;
		float u = t - 1.0f;
		float ease = 1.0f + (s + 1.0f) * (u * u * u) + s * (u * u); // 0→1
		m_textureSizeX = std::clamp(kBaseWidth * ease, 0.0f, kBaseWidth);
		m_textureSizeY = std::clamp(kBaseWidth * ease, 0.0f, kBaseWidth);

	}
	else
	{
		float t = animTime - kGrowDur;
		float amp = kBounceAmpPx * std::exp(-kDamp * t);
		float value = kBaseWidth + amp * std::sin(kBounceFreq * t);
		m_textureSizeX = std::clamp(value, 0.0f, kBaseWidth); // 最大50に制限
		m_textureSizeY = std::clamp(value, 0.0f, kBaseWidth);
	}

	// 寿命
	m_timer -= deltaTime;
	if (m_timer <= 0.0f) { m_isExpired = true; }
}

void HitDamage::DrawSprite()
{
	std::string numStr = std::to_string(m_displayTime);

	// screenPosを使用
	int baseX = static_cast<int>(m_screenPos.x);
	int baseY = static_cast<int>(m_screenPos.y);

	const int digitWidth = static_cast<int>(m_textureSizeX / 2.0f);
	const int totalWidth = static_cast<int>(numStr.size()) * digitWidth;
	int startX = baseX - totalWidth / 2;

	for (size_t i = 0; i < numStr.size(); ++i)
	{
		int n = numStr[i] - '0';
		int texIndex = n;
		m_srcRect = { 50 * texIndex, 0, m_textureSizeX, m_textureSizeY };

		int textureWidth = static_cast<int>(m_srcRect.width / 1.2f);
		int textureHeight = static_cast<int>(m_srcRect.height / 1.2f);

		KdShaderManager::Instance().m_spriteShader.DrawTex(
			m_texture,
			startX + digitWidth * static_cast<int>(i),
			baseY,
			textureWidth,
			textureHeight,
			&m_srcRect,
			&m_color
		);
	}
}
