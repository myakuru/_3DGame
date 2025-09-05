#include "Player.h"
#include"../../../Scene/SceneManager.h"
#include"../../Weapon/Katana/Katana.h"
#include"../../Weapon/WeaponKatanaScabbard/WeaponKatanaScabbard.h"
#include"../../../main.h"
#include"../../../../Framework/Json/Json.h"
#include"../../Camera/PlayerCamera/PlayerCamera.h"
#include"PlayerState/PlayerState_Idle/PlayerState_Idle.h"

#include"PlayerState/PlayerState_Hit/PlayerState_Hit.h"

#include"../Enemy/Enemy.h"

const uint32_t Player::TypeID = KdGameObject::GenerateTypeID();

void Player::Init()
{
	CharaBase::Init();

	m_animator->SetAnimation(m_modelWork->GetData()->GetAnimation("Idle"));

	StateInit();

	m_position = Math::Vector3(-4.0f, 1.2f, 4.0f);

	m_position.y = 1.0f;

	// 初期向きを-90度（Y軸）に設定
	m_rotation = Math::Quaternion::CreateFromAxisAngle(Math::Vector3::Up, DirectX::XMConvertToRadians(-90.0f));

	m_pCollider = std::make_unique<KdCollider>();

	m_pCollider->RegisterCollisionShape("PlayerSphere", sphere, KdCollider::TypeDamage);
	m_pCollider->RegisterCollisionShape("PlayerSphere", sphere, KdCollider::TypeGround);

	m_onceEffect = false;
}

void Player::PreUpdate()
{
	sphere.Center = m_position + Math::Vector3(0.0f, 0.5f, 0.0f); // 敵の位置＋オフセット
	sphere.Radius = 0.2f;
	m_pDebugWire->AddDebugSphere(sphere.Center, sphere.Radius,kRedColor);

	// カタナの取得
	auto katana = m_katana.lock();
	// 鞘の取得
	auto scabbard = m_scabbard.lock();

	if (!katana) return;
	if (!scabbard) return;

	katana->SetPlayerMatrix(m_mWorld);
	katana->SetPlayerHandMatrix(m_mWorld);
	scabbard->SetPlayerMatrix(m_mWorld);
	scabbard->SetPlayerHandMatrix(m_mWorld);
}

void Player::SkirtUpdate()
{
}

void Player::Update()
{
	SceneManager::Instance().GetObjectWeakPtr(m_playerCamera);
	SceneManager::Instance().GetObjectWeakPtr(m_enemy);
	SceneManager::Instance().GetObjectWeakPtr(m_katana);
	SceneManager::Instance().GetObjectWeakPtr(m_scabbard);

	if (m_playerCamera.expired()) return;
	if (m_enemy.expired()) return;
	if (m_katana.expired()) return;
	if (m_scabbard.expired()) return;


	KdGameObject::Update();

	float deltaTime = Application::Instance().GetDeltaTime();

	m_animator->AdvanceTime(m_modelWork->WorkNodes(), m_fixedFrameRate * deltaTime);

	m_isMoving = m_movement.LengthSquared() > 0;

	// 移動関係
	m_gravity += m_gravitySpeed * deltaTime;

	// 最終的な移動量
	m_position.x += m_movement.x * m_moveSpeed * m_fixedFrameRate * deltaTime;
	m_position.z += m_movement.z * m_moveSpeed * m_fixedFrameRate * deltaTime;
	m_position.y += m_gravity;

	m_stateManager.Update();

	// 最終的なワールド行列計算
	Math::Matrix scale = Math::Matrix::CreateScale(m_scale);
	Math::Matrix quaternion = Math::Matrix::CreateFromQuaternion(m_rotation);
	Math::Matrix translation = Math::Matrix::CreateTranslation(m_position);

	m_mWorld = scale * quaternion * translation;

}

void Player::UpdateAttack()
{
	// クォータニオンから前方向ベクトルを取得
	Math::Vector3 forward = Math::Vector3::TransformNormal(Math::Vector3::Forward, Math::Matrix::CreateFromQuaternion(m_rotation));
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

			// カメラシェイク
			if (auto camera = m_playerCamera.lock(); camera)
			{
				camera->StartShake(0.3f, 0.2f);
			}

		}
	}
}

void Player::UpdateChargeAttack()
{
	// クォータニオンから前方向ベクトルを取得
	Math::Vector3 forward = Math::Vector3::TransformNormal(Math::Vector3::Forward, Math::Matrix::CreateFromQuaternion(m_rotation));
	forward.Normalize();

	// 球の当たり判定情報作成
	KdCollider::SphereInfo attackSphere;
	attackSphere.m_sphere.Center = m_position + Math::Vector3(0.0f, 0.5f, 0.0f) + forward * 1.1f;
	attackSphere.m_sphere.Radius = 10.0f;
	attackSphere.m_type = KdCollider::TypeDamage;

	m_pDebugWire->AddDebugSphere(attackSphere.m_sphere.Center, attackSphere.m_sphere.Radius);

	SceneManager::Instance().GetObjectWeakPtr(m_enemy);
	auto enemy = m_enemy.lock();
	if (!enemy) return;

	float deltaTime = Application::Instance().GetDeltaTime();

	// 0.3秒間隔で5回ダメージを与える処理
	if (m_isChargeAttackActive)
	{
		m_chargeAttackTimer += deltaTime;

		if (m_chargeAttackCount < 5 && m_chargeAttackTimer >= 0.3f)
		{
			std::list<KdCollider::CollisionResult> results;
			if (enemy->Intersects(attackSphere, &results))
			{
				// 毎回ダメージを与える
				enemy->Damage(m_status.attack);
				enemy->SetEnemyHit(true);

				// カメラシェイク
				if (auto camera = m_playerCamera.lock(); camera)
				{
					camera->StartShake(0.3f, 0.3f);
				}
			}
			m_chargeAttackCount++;
			m_chargeAttackTimer = 0.0f;
		}

		// 5回終わったら終了
		if (m_chargeAttackCount >= 5)
		{
			m_isChargeAttackActive = false;
		}
	}
	else
	{
		// 初回起動条件（m_onceEffectがfalseのとき）
		if (!m_onceEffect)
		{
			m_isChargeAttackActive = true;
			m_chargeAttackCount = 0;
			m_chargeAttackTimer = 0.3f; // 最初のダメージを即時発生させたい場合は0.3f
			m_onceEffect = true;
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
