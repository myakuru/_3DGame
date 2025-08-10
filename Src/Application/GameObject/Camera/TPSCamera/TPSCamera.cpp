#include "TPSCamera.h"
#include"../../../Scene/SceneManager.h"

const uint32_t TPSCamera::TypeID = KdGameObject::GenerateTypeID();

void TPSCamera::Init()
{
	// 親クラスの初期化呼び出し
	CameraBase::Init();

	// 注視点
	m_mLocalPos = Math::Matrix::CreateTranslation(0, 2.f, -2.0f);

	SetCursorPos(m_FixMousePos.x, m_FixMousePos.y);
}

void TPSCamera::PostUpdate()
{
	if (!SceneManager::Instance().m_sceneCamera)
	{
		// カメラの回転
		UpdateRotateByMouse();
		m_mRotation = GetRotationMatrix();
		UpdateMoveKey();
		m_mWorld = m_mLocalPos * m_mRotation;
		m_mWorld.Translation(m_position);
		m_spCamera->SetCameraMatrix(m_mWorld);
	}
}
