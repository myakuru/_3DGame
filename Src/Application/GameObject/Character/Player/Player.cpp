#include "Player.h"
#include"../../../Scene/SceneManager.h"
#include"../../Weapon/Katana/Katana.h"
#include"../../../main.h"
#include"../../../../Framework/Json/Json.h"
#include"../../Camera/PlayerCamera/PlayerCamera.h"
#include"PlayerState/PlayerState_Idle/PlayerState_Idle.h"

const uint32_t Player::TypeID = KdGameObject::GenerateTypeID();

void Player::Init()
{
	CharaBase::Init();

	m_animator->SetAnimation(m_modelWork->GetData()->GetAnimation("Idle"));

	StateInit();

	m_mRotation = Math::Matrix::CreateFromYawPitchRoll
	(
		DirectX::XMConvertToRadians(m_degree.y), // Y軸回転
		DirectX::XMConvertToRadians(m_degree.x), // X軸回転
		DirectX::XMConvertToRadians(m_degree.z)  // Z軸回転
	);
}

void Player::PreUpdate()
{
	// カタナの取得
	auto katana = m_katana.lock();

	if (!katana) return;

	katana->SetPlayerMatrix(m_mWorld);
	katana->SetPlayerHandMatrix(m_mWorld);
}

void Player::SkirtUpdate()
{
}

void Player::Update()
{
	SceneManager::Instance().GetObjectWeakPtr(m_playerCamera);
	SceneManager::Instance().GetObjectWeakPtr(m_katana);

	if (m_playerCamera.expired()) return;
	if (m_katana.expired()) return;

	CharaBase::Update();
}

void Player::ImGuiInspector()
{
	CharaBase::ImGuiInspector();

	ImGui::Text(U8("プレイヤーの設定"));

	ImGui::DragFloat(U8("重力の大きさ"), &m_gravitySpeed, 0.01f);
	ImGui::DragFloat(U8("フレームレート制限"), &m_fixedFrameRate, 1.f);

	ImGui::Text(U8("プレイヤーの状態"));
	ImGui::DragFloat(U8("移動速度"), &m_moveSpeed, 0.1f);

	ImGui::Separator();
	m_playerConfig.InGuiInspector();
}

void Player::JsonInput(const nlohmann::json& _json)
{
	CharaBase::JsonInput(_json);
	if (_json.contains("GravitySpeed")) m_gravitySpeed = _json["GravitySpeed"].get<float>();
	if (_json.contains("fixedFps")) m_fixedFrameRate = _json["fixedFps"].get<float>();
	if (_json.contains("moveSpeed")) m_moveSpeed = _json["moveSpeed"].get<float>();

	m_playerConfig.JsonInput(_json);
}

void Player::JsonSave(nlohmann::json& _json) const
{
	CharaBase::JsonSave(_json);
	_json["GravitySpeed"] = m_gravitySpeed;
	_json["fixedFps"] = m_fixedFrameRate;
	_json["moveSpeed"] = m_moveSpeed;

	m_playerConfig.JsonSave(_json);
}

void Player::StateInit()
{
	// 初期状態を設定
	auto state = std::make_shared<PlayerState_Idle>();
	ChangeState(state);
}

void Player::ChangeState(std::shared_ptr<PlayerStateBase> _state)
{
	_state->SetPlayer(this);
	m_stateManager.ChangeState(_state);
}

void Player::UpdateMoveDirectionFromInput()
{
	m_moveDirection = Math::Vector3::Zero;
	if (KeyboardManager::GetInstance().IsKeyPressed('W')) m_moveDirection += Math::Vector3::Backward;
	if (KeyboardManager::GetInstance().IsKeyPressed('S')) m_moveDirection += Math::Vector3::Forward;
	if (KeyboardManager::GetInstance().IsKeyPressed('A')) m_moveDirection += Math::Vector3::Left;
	if (KeyboardManager::GetInstance().IsKeyPressed('D')) m_moveDirection += Math::Vector3::Right;
	if (m_moveDirection.LengthSquared() > 0.0f)
	{
		m_moveDirection.Normalize();
		m_lastMoveDirection = m_moveDirection; // 入力があった時だけ更新
	}
}
