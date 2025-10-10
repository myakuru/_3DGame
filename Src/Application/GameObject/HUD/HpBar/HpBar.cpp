#include "HpBar.h"
#include"../../Utility/Time.h"
#include"../../../Scene/SceneManager.h"
#include"../../Character/Player/Player.h"

const uint32_t HpBar::TypeID = KdGameObject::GenerateTypeID();

void HpBar::DrawSprite()
{
	// 現在のビューポートサイズ取得
	Math::Viewport vp;
	KdDirect3D::Instance().CopyViewportInfo(vp);

	// スケーリング
	const float sx = vp.width / kRefW;
	const float sy = vp.height / kRefH;

	Math::Matrix uiScale = Math::Matrix::CreateScale(sx, sy, 1.0f);


	KdShaderManager::Instance().m_spriteShader.SetMatrix(m_mWorld * uiScale);

		KdShaderManager::Instance().m_spriteShader.DrawTex(
			m_texture,
			0,
			0,
			m_rect.width,
			m_rect.height,
			&m_rect,
			&m_color,
			{0.0f,0.5f}
		);
	KdShaderManager::Instance().m_spriteShader.SetMatrix(Math::Matrix::Identity);
}

void HpBar::Update()
{
	SceneManager::Instance().GetObjectWeakPtr(m_player);

	auto player = m_player.lock();
	if (!player) return;

	//int hp = player->GetPlayerStatus().hp;

	m_mWorld = Math::Matrix::CreateScale(m_scale);
	m_mWorld *= Math::Matrix::CreateFromYawPitchRoll
	(
		m_degree.y,
		m_degree.x,
		m_degree.z
	);
	m_mWorld.Translation(m_position);
}

void HpBar::ImGuiInspector()
{
	SelectDraw2DTexture::ImGuiInspector();
	if (KeyboardManager::GetInstance().IsKeyPressed('F'))
	{
		m_rect.width--;
	}
}
