﻿#include "NotCharge1.h"
#include"../../../Character/Player/Player.h"
#include"../../../../Scene/SceneManager.h"

const uint32_t NotCharge1UI::TypeID = KdGameObject::GenerateTypeID();

void NotCharge1UI::Init()
{
	SelectDraw2DTexture::Init();
	m_texture = KdAssets::Instance().m_textures.GetData("Asset/Textures/GameUI/NotCharge.png");
	m_ChargeTexture = KdAssets::Instance().m_textures.GetData("Asset/Textures/GameUI/Charge.png");
	SceneManager::Instance().GetObjectWeakPtr(m_player);
	m_showUI = true;
}

void NotCharge1UI::Update()
{
	SelectDraw2DTexture::Update();

	if (auto player = m_player.lock(); player)
	{
		if (player->GetPlayerStatus().chargeCount >= 2)
		{
			m_showUI = false;
		}
		else
		{
			m_showUI = true;
		}
	}

	m_mWorld = Math::Matrix::CreateScale(m_scale);
	m_mWorld *= Math::Matrix::CreateFromYawPitchRoll(
		DirectX::XMConvertToRadians(m_degree.y),
		DirectX::XMConvertToRadians(m_degree.x),
		DirectX::XMConvertToRadians(m_degree.z)
	);
	m_mWorld.Translation(m_position);
}

void NotCharge1UI::DrawSprite()
{
	if (m_showUI)
	{
		// 現在のビューポートサイズ取得
		Math::Viewport vp;
		KdDirect3D::Instance().CopyViewportInfo(vp);

		// 伸張（Stretch）：XとYを個別にスケーリング（画面サイズにピッタリ）
		const float sx = vp.width / kRefW;
		const float sy = vp.height / kRefH;

		Math::Matrix uiScale = Math::Matrix::CreateScale(sx, sy, 1.0f);

		// UIスケールをワールド行列に後掛け（平行移動も含め全体をスケール）
		Math::Matrix m = m_mWorld * uiScale;

		KdShaderManager::Instance().m_spriteShader.SetMatrix(m);

		KdShaderManager::Instance().m_spriteShader.DrawTex(
			m_texture,
			static_cast<int>(m_position.x), // 1920x1080基準の座標そのまま
			static_cast<int>(m_position.y),
			nullptr,
			&m_color
		);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(Math::Matrix::Identity);
	}
	else
	{
		// 現在のビューポートサイズ取得
		Math::Viewport vp;
		KdDirect3D::Instance().CopyViewportInfo(vp);

		// 伸張（Stretch）：XとYを個別にスケーリング（画面サイズにピッタリ）
		const float sx = vp.width / kRefW;
		const float sy = vp.height / kRefH;


		Math::Matrix uiScale = Math::Matrix::CreateScale(sx, sy, 1.0f);

		// UIスケールをワールド行列に後掛け（平行移動も含め全体をスケール）
		Math::Matrix m = m_mWorld * uiScale;

		KdShaderManager::Instance().m_spriteShader.SetMatrix(m);

		KdShaderManager::Instance().m_spriteShader.DrawTex(
			m_ChargeTexture,
			static_cast<int>(m_position.x), // 1920x1080基準の座標そのまま
			static_cast<int>(m_position.y),
			nullptr,
			&m_color
		);
		KdShaderManager::Instance().m_spriteShader.SetMatrix(Math::Matrix::Identity);
	}
}
