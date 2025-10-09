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
	m_offset = Math::Vector3(0, 0, 0);
}

void HitDamage::Update()
{
	SceneManager::Instance().GetObjectWeakPtrList(m_enemies);
	SceneManager::Instance().GetObjectWeakPtr(m_camera);

	auto camera = m_camera.lock()->GetCamera();

	if (!camera) return;

	for (const auto& enemy : m_enemies)
	{
		if (auto enemyPtr = enemy.lock())
		{
			// 敵のワールド座標を取得
			Math::Vector3 enemyPos = enemyPtr->GetPos();

			// ワールド座標→スクリーン座標へ変換
			camera->ConvertWorldToScreenDetail(enemyPos, m_screenPos);
		}
	}

	// m_scale.xを0.1まで徐々に減少
	if (m_scale.x > 0.1f)
	{
		m_scale.x -= 0.01f;
		if (m_scale.x < 0.1f)
		{
			m_scale.x = 0.1f;
		}
	}

	// m_scale.yをバウンドさせて1.0fに収束
	static float bounceTime = 0.0f;
	bounceTime += 0.2f; // バウンド速度

	float amplitude = (m_scale.x - 0.1f) * 2.0f; // xが小さくなるほどバウンド幅も小さく
	m_scale.y = 1.0f + amplitude * std::sin(bounceTime);

	float deltaTime = Application::Instance().GetUnscaledDeltaTime();
	m_timer -= deltaTime;

	if (m_timer <= 0.0f) 
	{
		m_isExpired = true;
	}
}

void HitDamage::DrawSprite()
{
	// 現在のビューポートサイズ取得
	Math::Viewport vp;
	KdDirect3D::Instance().CopyViewportInfo(vp);

	// 伸張（Stretch）：XとYを個別にスケーリング（画面サイズにピッタリ）
	const float sx = vp.width / kRefW;
	const float sy = vp.height / kRefH;

	// 等方（Fit）にしたい場合は以下を使う（必要なら切替）
	const float scale = std::min(sx, sy);
	Math::Matrix uiScale = Math::Matrix::CreateScale(scale, scale, 1.0f);

	//Math::Matrix uiScale = Math::Matrix::CreateScale(sx, sy, 1.0f);

	// UIスケールをワールド行列に後掛け（平行移動も含め全体をスケール）
	Math::Matrix matrix = m_mWorld * uiScale;

	KdShaderManager::Instance().m_spriteShader.SetMatrix(matrix);

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
