#include "Player.h"
#include"../../../Scene/SceneManager.h"
#include"../../Weapon/Katana/Katana.h"
#include"../Player/PlayerState/PlayerState.h"
#include"../Player/PlayerState/PlayerState_Idle/PlayerState_Idle.h"
#include"../../../main.h"
#include"../../../../Framework/Json/Json.h"
#include"../../Camera/PlayerCamera/PlayerCamera.h"

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
	// アニメーション適用後にノード行列を再計算
	m_modelWork->CalcNodeMatrices();

	// 手のワークノードを取得
	//handWorkNode = m_modelWork->FindWorkNode("ring_01_r");
	backWorkNode = m_modelWork->FindWorkNode("Hips");
	if (!backWorkNode) return;

	//手のワークノードを取得して、カタナのワールド変換を設定
	{
		SceneManager::Instance().GetObjectWeakPtr(m_katana);

		if (m_katana.expired()) return;

		// katanaが有効な場合は、katanaのポインタを取得
		std::shared_ptr<Katana> katana = m_katana.lock();

		// プレイヤーに追尾する刀にするためにワークノードとプレイヤーのワールド変換を設定
		katana->SetKatanaMatrix(backWorkNode->m_worldTransform);
		katana->SetPlayerMatrix(m_mWorld);
	}
}

void Player::SkirtUpdate()
{
}

void Player::Update()
{
	SceneManager::Instance().GetObjectWeakPtr(m_playerCamera);

	if (m_playerCamera.expired()) return;

	CharaBase::Update();

	// 最終的なワールド行列計算
	m_mWorld = Math::Matrix::CreateScale(m_scale);
	m_mWorld *= Math::Matrix::CreateFromQuaternion(m_rotation);
	m_mWorld.Translation(m_position);
}

void Player::ImGuiInspector()
{
	CharaBase::ImGuiInspector();

	ImGui::DragFloat(U8("重力の大きさ"), &m_gravitySpeed, 0.01f);
	ImGui::DragFloat(U8("フレームレート制限"), &m_fixedFrameRate, 1.f);

	ImGui::Text(U8("プレイヤーの状態"));
	ImGui::DragFloat(U8("移動速度"), &m_moveSpeed, 0.1f);
}

void Player::JsonInput(const nlohmann::json& _json)
{
	CharaBase::JsonInput(_json);
	if (_json.contains("GravitySpeed")) m_gravitySpeed = _json["GravitySpeed"].get<float>();
	if (_json.contains("fixedFps")) m_fixedFrameRate = _json["fixedFps"].get<float>();
	if (_json.contains("moveSpeed")) m_moveSpeed = _json["moveSpeed"].get<float>();
}

void Player::JsonSave(nlohmann::json& _json) const
{
	CharaBase::JsonSave(_json);
	_json["GravitySpeed"] = m_gravitySpeed;
	_json["fixedFps"] = m_fixedFrameRate;
	_json["moveSpeed"] = m_moveSpeed;
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
