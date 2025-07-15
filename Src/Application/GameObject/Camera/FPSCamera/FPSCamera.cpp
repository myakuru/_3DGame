#include "FPSCamera.h"
#include"../../../Scene/SceneManager.h"

void FPSCamera::Init()
{
	// 親クラスの初期化呼び出し
	CameraBase::Init();

	// 基準点(ターゲット)の目線
	m_mLocalPos = Math::Matrix::CreateTranslation(0, 1.5f, 0.0f);

	SetCursorPos(m_FixMousePos.x, m_FixMousePos.y);
}

void FPSCamera::CameraUpdate()
{
	if (!SceneManager::GetInstance().m_sceneCamera) return;
	{
		// カメラの回転
		UpdateRotateByMouse();
		m_mRotation = GetRotationMatrix();
		UpdateMoveKey();
		m_mWorld = m_mLocalPos * m_mRotation;
		m_mWorld.Translation(m_pos);
		m_spCamera->SetCameraMatrix(m_mWorld);
	}
}