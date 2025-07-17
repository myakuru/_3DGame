#include "PlayerCamera.h"
#include"../../../main.h"
#include"../../../../MyFramework/Manager/KeyboardManager.h"
#include"../../../Scene/SceneManager.h"
#include"../../Character/Player/Player.h"

const uint32_t PlayerCamera::TypeID = KdGameObject::GenerateTypeID();

void PlayerCamera::Init()
{
	CameraBase::Init();
}

void PlayerCamera::Update()
{
	if (SceneManager::GetInstance().m_sceneCamera) return;

	//float deltaTime = Application::Instance().GetDeltaTime();

	SceneManager::GetInstance().GetObjectWeakPtr(m_player);

	if (m_player.expired()) return;

	auto player = m_player.lock();


	// カメラの回転
	UpdatePlayerRotetionMouse();

	m_mRotation = GetRotationMatrix();

	m_mWorld = Math::Matrix::CreateTranslation(0, 2.f, -3.f);

	Math::Vector3 targetPos = m_player.lock()->GetPos();

	//Math::Matrix transMatrix = Math::Matrix::CreateTranslation(targetPos);

	m_mWorld = m_mWorld * m_mRotation;

	// 最終的なカメラの移動行列更新
	//m_mWorld.Translation(m_mWorld.Translation() + targetPos + Math::Vector3{ 0.0f,2.0f,2.0f });

	m_mWorld *= Math::Matrix::CreateTranslation( m_mWorld.Translation() + targetPos);

	m_spCamera->SetCameraMatrix(m_mWorld);

}

void PlayerCamera::UpdatePlayerRotetionMouse()
{
	static bool prevTab = false; // 前フレームのTABキー状態

	// TABキーの現在の状態
	bool nowTab = (KeyboardManager::GetInstance().IsKeyJustPressed(VK_TAB));

	// TABキーが押された瞬間だけトグル
	if (nowTab && !prevTab)
	{
		m_enabled = !m_enabled;
		SwitchShowCursor(false);
	}

	if (m_enabled) SwitchShowCursor(false);

	if (!m_enabled) SwitchShowCursor(true);

	prevTab = nowTab;

	if (m_enabled)
	{
		// マウスでカメラを回転させる処理
		POINT _nowPos;
		GetCursorPos(&_nowPos);

		POINT _mouseMove{};
		_mouseMove.x = _nowPos.x - m_FixMousePos.x;
		_mouseMove.y = _nowPos.y - m_FixMousePos.y;

		SetCursorPos(m_FixMousePos.x, m_FixMousePos.y);

		// 実際にカメラを回転させる処理(0.15はただの補正値)
		m_degree.x += (float)_mouseMove.y * 0.15f;
		m_degree.y += (float)_mouseMove.x * 0.15f;
	}

	// 回転制御
	m_degree.x = std::clamp(m_degree.x, -30.0f, 30.0f);
}
