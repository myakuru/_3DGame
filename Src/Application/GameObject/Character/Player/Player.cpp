#include "Player.h"
#include"../../../Scene/SceneManager.h"
#include"../../Weapon/Katana/Katana.h"
#include"../../../main.h"
#include"../../../../Framework/Json/Json.h"
#include"../../Camera/PlayerCamera/PlayerCamera.h"
#include"PlayerState/PlayerState_Idle/PlayerState_Idle.h"

#include"../Enemy/Enemy.h"

const uint32_t Player::TypeID = KdGameObject::GenerateTypeID();

void Player::Init()
{
	CharaBase::Init();

	m_animator->SetAnimation(m_modelWork->GetData()->GetAnimation("Idle"));

	StateInit();

	m_onceEffect = false;
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

void Player::UpdateAttack()
{
	float deltaTime = Application::Instance().GetDeltaTime();

	// 前方向ベクトル
	Math::Vector3 forward = m_mRotation.Forward();
	forward.Normalize();

	// 球の当たり判定情報作成
	KdCollider::SphereInfo attackSphere;
	// 球の中心座標を設定
	attackSphere.m_sphere.Center = m_position + Math::Vector3(0.0f, 0.5f, 0.0f) + forward * 1.1f;
	// 球の半径を設定
	attackSphere.m_sphere.Radius = m_attackBossEnemyRadius;
	// アタリ判定をしたいタイプを設定
	attackSphere.m_type = KdCollider::TypeDamage;

	m_pDebugWire->AddDebugSphere(attackSphere.m_sphere.Center, attackSphere.m_sphere.Radius); // デバッグ用の球を追加


	SceneManager::Instance().GetObjectWeakPtr(m_enemy);

	auto enemy = m_enemy.lock();

	if (!enemy) return;

	
	// 当たり判定
	std::list<KdCollider::CollisionResult> results;
	if (enemy->Intersects(attackSphere, &results))
	{
		if (!m_onceEffect)
		{
			// 敵にダメージを与える処理
			enemy->Damage(m_status.attack); // ダメージを与える
			enemy->SetEnemyHit(true);		// ヒットチェックを行う
			m_onceEffect = true;			// 1回だけ再生

		}
	}
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
