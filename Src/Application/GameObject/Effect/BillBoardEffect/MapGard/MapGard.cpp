#include "MapGard.h"
#include"../../../Camera/PlayerCamera/PlayerCamera.h"
#include"../../../../Scene/SceneManager.h"
#include"../../../Character/Player/Player.h"

const uint32_t MapGard::TypeID = KdGameObject::GenerateTypeID();

void MapGard::Init()
{
	SelectDraw3dPolygon::Init();
	SceneManager::Instance().GetObjectWeakPtr(m_camera);

	m_polygon->SetMaterial("Asset/Textures/GameUI/Collison.png");

	SceneManager::Instance().GetObjectWeakPtr(m_player);

	m_scale = { 0.15f,0.8f,1.0f };
}

void MapGard::Update()
{
	if (auto player = m_player.lock(); player)
	{
		m_playerDistance = (player->GetPos() - m_position).LengthSquared();
	}

	if (m_playerDistance < 16.0f)
	{
		m_color = { 0.8f,0.8f,0.8f,1.0f };
	}
	else if(m_playerDistance < 20.0f)
	{
		m_color = { 0.8f,0.8f,0.8f,0.8f };
	}
	else
	{
		m_color = { 0.8f,0.8f,0.8f,0.5f };
	}

	// 常に微細に揺れる
	m_offset.x = KdRandom::GetFloat(-0.1f, 0.1f);
	m_offset.y = KdRandom::GetFloat(-0.1f, 0.1f);
	m_offset.z = KdRandom::GetFloat(-0.1f, 0.1f);

	// カメラが存在するなら
	if (auto spCamera = m_camera.lock(); spCamera)
	{
		// ビルボード行列
		Math::Matrix rot = Math::Matrix::CreateBillboard
		(
			m_position,													// オブジェクトの位置
			spCamera->GetCamera()->GetCameraMatrix().Translation(),		// カメラの位置
			Math::Vector3::UnitY										// カメラの上方向
		);

		// 微細に揺らすオフセットを加算
		m_position += m_offset * 0.01f;

		// 最終的なワールド行列
		m_mWorld = Math::Matrix::CreateScale(m_scale) * rot;
	}
}

void MapGard::DrawToon()
{
	SelectDraw3dPolygon::DrawToon();
}
