#include "FieldEffect.h"
#include"../../../Camera/PlayerCamera/PlayerCamera.h"
#include"../../../../Scene/SceneManager.h"

const uint32_t FieldEffect::TypeID = KdGameObject::GenerateTypeID();

void FieldEffect::Update()
{
	SceneManager::Instance().GetObjectWeakPtr(m_camera);

	auto spCamera = m_camera.lock();

	if (!spCamera) return;

	// ビルボード行列
	Math::Matrix rot = Math::Matrix::CreateBillboard
	(
		m_position,													// オブジェクトの位置
		spCamera->GetCamera()->GetCameraMatrix().Translation(),		// カメラの位置
		Math::Vector3::UnitY										// カメラの上方向
	);

	// 最終的なワールド行列
	m_mWorld = Math::Matrix::CreateScale(m_scale) * rot;
}
