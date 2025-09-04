#include "FieldEffect.h"
#include"../../../Camera/PlayerCamera/PlayerCamera.h"
#include"../../../../Scene/SceneManager.h"

const uint32_t FieldEffect::TypeID = KdGameObject::GenerateTypeID();

void FieldEffect::Update()
{
	SceneManager::Instance().GetObjectWeakPtr(m_camera);

	auto spCamera = m_camera.lock();

	if (!spCamera) return;

	Math::Matrix rot = spCamera->GetRotationMatrix().CreateBillboard(m_position, spCamera->GetPos(), spCamera->GetPos().Up);

	m_mWorld = Math::Matrix::CreateScale(m_scale) * rot * Math::Matrix::CreateTranslation(m_position);
}
