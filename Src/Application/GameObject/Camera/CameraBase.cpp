#include "CameraBase.h"
#include"../../main.h"

void CameraBase::Init()
{
	if (!m_spCamera)
	{
		m_spCamera = std::make_shared<KdCamera>();
	}
	// ↓画面中央座標
	m_FixMousePos.x = 640;
	m_FixMousePos.y = 360;
}

void CameraBase::Update()
{
	if (!m_spCamera) { return; }

	m_mWorld.Translation(m_pos);
	m_spCamera->SetCameraMatrix(m_mWorld);
}

void CameraBase::PreDraw()
{
	if (!m_spCamera) { return; }
	m_spCamera->SetToShader();
}

void CameraBase::ImGuiInspector()
{
	KdGameObject::ImGuiInspector();

	if (!m_spCamera) { return; }
	ImGui::DragFloat("moveSpeed", &moveSpeed, 1.0f);

}

void CameraBase::JsonSave(nlohmann::json& _json) const
{
	KdGameObject::JsonSave(_json);
	_json["moveSpeed"] = moveSpeed;
}

void CameraBase::JsonInput(const nlohmann::json& _json)
{
	KdGameObject::JsonInput(_json);
	if (_json.contains("moveSpeed")) moveSpeed = _json["moveSpeed"].get<float>();
}

void CameraBase::SetTarget(const std::shared_ptr<KdGameObject>& target)
{
	if (!target) { return; }

	m_wpTarget = target;
}

void CameraBase::UpdateMoveKey()
{
	if (!m_enabled) return;

	float deltaTime = Application::Instance().GetDeltaTime();

	// 回転行列から前方向と右方向を取得
	Math::Vector3 backward = m_mRotation.Backward();
	Math::Vector3 right = m_mRotation.Right();
	float moveUpSpeed = 10.0f; // 上下移動速度

	// 斜め移動はやくならないように正規化
	backward.Normalize();
	right.Normalize();

	// 距離の移動量を計算
	Math::Vector3 move = {};

	if (KeyboardManager::GetInstance().IsKeyPressed('W')) move += backward * moveSpeed * deltaTime;
	if (KeyboardManager::GetInstance().IsKeyPressed('A')) move -= right * moveSpeed * deltaTime;
	if (KeyboardManager::GetInstance().IsKeyPressed('S')) move -= backward * moveSpeed * deltaTime;
	if (KeyboardManager::GetInstance().IsKeyPressed('D')) move += right * moveSpeed * deltaTime;
	if (KeyboardManager::GetInstance().IsKeyPressed(VK_SPACE)) move.y += moveUpSpeed * deltaTime; // 上に移動
	if (KeyboardManager::GetInstance().IsKeyPressed(VK_LSHIFT)) move.y -= moveUpSpeed * deltaTime; // 下に移動

	// 距離 = 速度 * 時間
	m_pos += move;
}

void CameraBase::UpdateRotateByMouse()
{
	static bool prevTab = false; // 前フレームのTABキー状態

	// TABキーの現在の状態
	bool nowTab = (GetAsyncKeyState(VK_TAB) & 0x8000);

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
		m_deg.x += _mouseMove.y * 0.15f;
		m_deg.y += _mouseMove.x * 0.15f;
	}

	// 回転制御
	m_deg.x = std::clamp(m_deg.x, -45.f, 45.f);
}