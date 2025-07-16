#include "Player.h"
#include"../../../Scene/SceneManager.h"
#include"../../Weapon/Katana/Katana.h"
#include"../../Weapon/Saya/Saya.h"
#include"../Player/PlayerState/PlayerState.h"
#include"../Player/PlayerState/PlayerState_Idle/PlayerState_Idle.h"
#include"../../../main.h"
#include"../../../../Framework/Json/Json.h"

void Player::Init()
{
	CharaBase::Init();
}

void Player::PreUpdate()
{
	// アニメーション適用後にノード行列を再計算
	m_modelWork->CalcNodeMatrices();

	// 手のワークノードを取得
	//handWorkNode = m_modelWork->FindWorkNode("ring_01_r");
	backWorkNode = m_modelWork->FindWorkNode("spine_01");
	if (!backWorkNode) return;

	//手のワークノードを取得して、カタナのワールド変換を設定
	{
		SceneManager::GetInstance().GetObjectWeakPtr(m_katana);

		if (m_katana.expired()) return;

		std::shared_ptr<Katana> katana = m_katana.lock();

		katana->SetKatanaMatrix(backWorkNode->m_worldTransform);
	}
}

void Player::Update()
{
	CharaBase::Update();
}

void Player::DrawToon()
{
	CharaBase::DrawToon();
}

void Player::ImGuiInspector()
{
	CharaBase::ImGuiInspector();

	ImGui::DragFloat(U8("重力の大きさ"), &m_gravitySpeed, 0.01f);
	ImGui::DragFloat(U8("フレームレート制限"), &m_fixedFrameRate, 1.f);

}

void Player::JsonInput(const nlohmann::json& _json)
{
	CharaBase::JsonInput(_json);
	if (_json.contains("GravitySpeed")) m_gravitySpeed = _json["GravitySpeed"].get<float>();
	if (_json.contains("fixedFps")) m_fixedFrameRate = _json["fixedFps"].get<float>();
}

void Player::JsonSave(nlohmann::json& _json) const
{
	CharaBase::JsonSave(_json);
	_json["GravitySpeed"] = m_gravitySpeed;
	_json["fixedFps"] = m_fixedFrameRate;
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
