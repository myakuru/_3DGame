#include "Enemy.h"
#include"../Player/Player.h"
#include"../../../Scene/SceneManager.h"
#include"EnemyState/EnemyState_Idle/EnemyState_Idle.h"
#include"EnemyState/EnemyState_Hit/EnemyState_Hit.h"
#include"../../Camera/PlayerCamera/PlayerCamera.h"
#include"../../Weapon/EnemySword/EnemySword.h"
#include"../../Weapon/EnemyShield/EnemyShield.h"
#include"../../Collition/Collition.h"
#include"../../../main.h"
#include"../../../../Framework/Json/Json.h"

const uint32_t Enemy::TypeID = KdGameObject::GenerateTypeID();

void Enemy::Init()
{
	CharaBase::Init();

	m_rotateSpeed = 10.0f;

	m_animator->SetAnimation(m_modelWork->GetData()->GetAnimation("Idle"));

	m_pCollider = std::make_unique<KdCollider>();

	m_pCollider->RegisterCollisionShape("EnemySphere", sphere, KdCollider::TypeDamage);

	m_pCollider->RegisterCollisionShape("PlayerSphere", sphere, KdCollider::TypeEnemyHit);

	StateInit();

	m_isAtkPlayer = false;
	m_dissever = 0.0f;

	m_invincible = false;
}

void Enemy::Update()
{
	SceneManager::Instance().GetObjectWeakPtrList(m_enemySwords);
	SceneManager::Instance().GetObjectWeakPtrList(m_enemyShields);

	// 球の中心座標と半径を設定
	sphere.Center = m_position + Math::Vector3(0.0f, 0.7f, 0.0f); // 敵の位置＋オフセット
	sphere.Radius = 0.2f; // 半径0.5

	m_pDebugWire->AddDebugSphere(sphere.Center, sphere.Radius, kBlueColor);

	SceneManager::Instance().GetObjectWeakPtr(m_wpPlayer);

	float deltaTime = Application::Instance().GetUnscaledDeltaTime();

	m_attackFrame += deltaTime;

	if (m_attackFrame >= 3.5f)
	{
		Application::Instance().SetFpsScale(1.f); // スローモーションにする
		SceneManager::Instance().SetDrawGrayScale(false);
		m_attackFrame = 0.0f;
		m_justAvoidSuccess = false;
	}

	if (m_Expired)
	{
		if (m_dissever < 1.0f)
		{
			m_dissever += 2.0f * deltaTime;
		}
		else
		{
			m_dissever = 1.0f;
			m_isExpired = true;
		}
	}

	CharaBase::Update();

	// ヒット処理。
	if (m_isHit && !m_invincible)
	{

		// ダメージステートに変更
		auto spDamageState = std::make_shared<EnemyState_Hit>();
		ChangeState(spDamageState);

		// HitDamage生成・初期化
		m_spHitDamage = std::make_shared<HitDamage>();
		m_spHitDamage->Init();
		m_spHitDamage->SetDamage(m_getDamage);
		m_spHitDamage->SetTrackEnemy(std::static_pointer_cast<Enemy>(shared_from_this()));
		SceneManager::Instance().AddObject(m_spHitDamage);
		m_isHit = false;
		return;
	}

	// 敵の剣の行列を更新
	for(auto weapons : m_enemySwords)
	{
		if (auto sword = weapons.lock(); sword)
		{
			if (auto rightHandNode = m_modelWork->FindWorkNode("weapon_r"); rightHandNode)
			{
				sword->SetEnemyRightHandMatrix(rightHandNode->m_worldTransform);
				sword->SetEnemyMatrix(m_mWorld);
			}
		}
	}

	for(auto shields : m_enemyShields)
	{
		if (auto shield = shields.lock(); shield)
		{
			if (auto leftHandNode = m_modelWork->FindWorkNode("weapon_l"); leftHandNode)
			{
				shield->SetEnemyLeftHandMatrix(leftHandNode->m_worldTransform);
				shield->SetEnemyMatrix(m_mWorld);
			}
		}
	}
}

void Enemy::DrawLit()
{
	//ディゾルブ処理
	KdShaderManager::Instance().m_StandardShader.SetDissolve(m_dissever,&m_dissolvePower, &m_dissolveColor);
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_modelWork, m_mWorld, m_color);
}

void Enemy::UpdateAttackCollision(float _radius, float _distance, int _attackCount, float _attackTimer)
{
	Math::Vector3 forward = Math::Vector3::TransformNormal(Math::Vector3::Forward, Math::Matrix::CreateFromQuaternion(m_rotation));
	forward.Normalize();

	float deltaTime = Application::Instance().GetDeltaTime();

	KdCollider::SphereInfo attackSphere;
	attackSphere.m_sphere.Center = m_position + Math::Vector3(0.0f, 0.5f, 0.0f) + forward * _distance;
	attackSphere.m_sphere.Radius = _radius;
	attackSphere.m_type = KdCollider::TypeDamage;

	m_pDebugWire->AddDebugSphere(attackSphere.m_sphere.Center, attackSphere.m_sphere.Radius);

	SceneManager::Instance().GetObjectWeakPtrList(m_player);

	// 初回セットアップ: 初期タイマを0に
	if (!m_hitOnce)
	{
		m_isChargeAttackActive = true;
		m_chargeAttackCount = 0;
		m_chargeAttackTimer = 0.0f;
		m_hitOnce = true;
	}

	if (!m_isChargeAttackActive) return;

	// ジャスト回避成功
	for (const auto& players : m_player)
	{
		if (auto playerPtr = players.lock())
		{
			std::list<KdCollider::CollisionResult> results;
			if (playerPtr->Intersects(attackSphere, &results) && !results.empty())
			{
				// プレイヤーが回避中か判定
				if (playerPtr->GetAvoidFlg())
				{
					const float kJustAvoidWindowSec = 0.5f; // 30f/60fps
					const float avoidElapsed = playerPtr->GetAvoidStartTime();
					if (avoidElapsed >= 0.0f && avoidElapsed <= kJustAvoidWindowSec)
					{
						m_justAvoidSuccess = true;
						Application::Instance().SetFpsScale(0.5f); // スローモーション
						SceneManager::Instance().SetDrawGrayScale(true);
						return; // ダメージ処理は行わない
					}
				}
			}
		}
	}

	// ジャスト回避中はダメージを与えない
	if (m_justAvoidSuccess) return;

	m_chargeAttackTimer += deltaTime;

	if (m_chargeAttackCount < _attackCount && m_chargeAttackTimer >= _attackTimer)
	{

		for (const auto& players : m_player)
		{
			if (auto playerPtr = players.lock())
			{
				std::list<KdCollider::CollisionResult> results;
				if (playerPtr->Intersects(attackSphere, &results) && !results.empty())
				{
					playerPtr->TakeDamage(m_status.attack);
					playerPtr->SetHitCheck(true);
				}
			}
		}

		m_chargeAttackCount++;
		m_chargeAttackTimer = 0.0f;

		if (m_chargeAttackCount >= _attackCount)
		{
			m_isChargeAttackActive = false;
		}
	}
}

void Enemy::PostUpdate()
{
	// 球判定
	// 球判定用の変数
	KdCollider::SphereInfo sphereInfo;
	// 球の中心座標を設定
	sphereInfo.m_sphere.Center = m_position + Math::Vector3(0.0f, 0.5f, 0.0f);
	// 球の半径を設定
	sphereInfo.m_sphere.Radius = 0.2f;
	// アタリ判定をしたいタイプを設定
	sphereInfo.m_type = KdCollider::TypeBump; // 地面のアタリ判定

	m_pDebugWire->AddDebugSphere(sphereInfo.m_sphere.Center, sphereInfo.m_sphere.Radius);

	// 球に当たったオブジェクト情報を格納するリスト
	std::list<KdCollider::CollisionResult> retSpherelist;

	SceneManager::Instance().GetObjectWeakPtrList(m_collisionList);

	for(auto & weakCol : m_collisionList)
	{
		if (auto col = weakCol.lock(); col)
		{
			col->Intersects(sphereInfo, &retSpherelist);
		}
	}

	// 球にあたったリストから一番近いオブジェクトを探す
	// オーバーした長さが1番長いものを探す。
	// 使いまわしの変数を使う
	float maxOverLap = 0.0f;
	bool hit = false;
	// 当たった方向を格納する変数
	Math::Vector3 hitDir;

	for (auto& ret : retSpherelist)
	{
		// 球からはみ出た長さが１番長いものを探す。
		if (maxOverLap < ret.m_overlapDistance)
		{
			maxOverLap = ret.m_overlapDistance;
			hitDir = ret.m_hitDir;
			hit = true;
		}
	}

	 if (hit)
    {
        // 正規化して押し出す方向を求める
        hitDir.Normalize();

        // Y方向の押し出しを無効化（XZ平面のみ）
        hitDir.y = 0.0f;
        hitDir.Normalize();

        //当たってたらその方向から押し出す
        m_position += hitDir * maxOverLap;
    }
}

void Enemy::ImGuiInspector()
{
	CharaBase::ImGuiInspector();

	ImGui::DragFloat(U8("重力の大きさ"), &m_gravitySpeed, 0.01f);
	ImGui::DragFloat(U8("アニメーション速度"), &m_fixedFrameRate, 1.f);
	ImGui::Text(U8("プレイヤーの回転速度"));
	ImGui::DragFloat(U8("回転速度"), &m_rotateSpeed, 0.1f);

	ImGui::Text(U8("現在の状態"));
	ImGui::DragFloat(U8("移動速度"), &m_moveSpeed, 0.1f);

	// ディゾルブ関係
	ImGui::ColorEdit3(U8("ディゾルブカラー"), &m_dissolveColor.x);
	ImGui::DragFloat(U8("ディゾルブ進行度"), &m_dissolvePower, 0.01f, 0.0f, 1.0f);
}

void Enemy::JsonInput(const nlohmann::json& _json)
{
	CharaBase::JsonInput(_json);
	if (_json.contains("GravitySpeed")) m_gravitySpeed = _json["GravitySpeed"].get<float>();
	if (_json.contains("fixedFps")) m_fixedFrameRate = _json["fixedFps"].get<float>();
	if (_json.contains("moveSpeed")) m_moveSpeed = _json["moveSpeed"].get<float>();
	if (_json.contains("rotationspeed")) m_rotateSpeed = _json["rotationspeed"].get<float>();
	if (_json.contains("dissolveColor")) m_dissolveColor = JSON_MANAGER.JsonToVector(_json["dissolveColor"]);
	if (_json.contains("dissolvePower")) m_dissolvePower = _json["dissolvePower"].get<float>();
}

void Enemy::JsonSave(nlohmann::json& _json) const
{
	CharaBase::JsonSave(_json);
	_json["GravitySpeed"] = m_gravitySpeed;
	_json["fixedFps"] = m_fixedFrameRate;
	_json["moveSpeed"] = m_moveSpeed;
	_json["rotationspeed"] = m_rotateSpeed;
	_json["dissolveColor"] = JSON_MANAGER.VectorToJson(m_dissolveColor);
	_json["dissolvePower"] = m_dissolvePower;
}

void Enemy::UpdateQuaternion(Math::Vector3& _moveVector)
{
	float deltaTime = Application::Instance().GetUnscaledDeltaTime();

	if (_moveVector == Math::Vector3::Zero) return;

	_moveVector.Normalize();

	// 敵方向ベクトルからクォータニオンを作成
	Math::Quaternion targetRotation = Math::Quaternion::LookRotation(_moveVector, Math::Vector3::Up);

	// 滑らかに回転させる
	m_rotation = Math::Quaternion::Slerp(m_rotation, targetRotation, deltaTime * m_fixedFrameRate);
}

void Enemy::StateInit()
{
	m_isAtkPlayer = false;
	m_hitOnce = false;

	// 初期状態を設定
	auto state = std::make_shared<EnemyState_Idle>();
	ChangeState(state);
}

void Enemy::ChangeState(std::shared_ptr<EnemyStateBase> _state)
{
	_state->SetEnemy(this);
	m_stateManager.ChangeState(_state);
}

void Enemy::Damage(int _damage)
{
	m_getDamage = _damage;
	m_status.hp -= _damage;
	if (m_status.hp <= 0)
	{
		m_Expired = true;
	}
}
