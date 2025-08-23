#include "Enemy.h"
#include"../Player/Player.h"
#include"../../../Scene/SceneManager.h"
#include"EnemyState/EnemyState_Idle/EnemyState_Idle.h"
#include"EnemyState/EnemyState_Hit/EnemyState_Hit.h"

const uint32_t Enemy::TypeID = KdGameObject::GenerateTypeID();

void Enemy::Init()
{
	CharaBase::Init();

	m_animator->SetAnimation(m_modelWork->GetData()->GetAnimation("Idle"));

	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("EnemyMesh", std::make_unique<KdModelCollision>(m_modelWork, KdCollider::TypeDamage));

	StateInit();
}

void Enemy::Update()
{
	SceneManager::Instance().GetObjectWeakPtr(m_wpPlayer);

	if (m_wpPlayer.expired()) return;

	// ヒット処理。
	if (m_isHit)
	{
		// ダメージステートに変更
		auto spDamageState = std::make_shared<EnemyState_Hit>();
		ChangeState(spDamageState);
		m_isHit = false;	// ダメージフラグをリセット
		return;
	}

	CharaBase::Update();
}

void Enemy::PostUpdate()
{
	// 球判定
	// 球判定用の変数
	KdCollider::SphereInfo sphereInfo;
	// 球の中心座標を設定
	sphereInfo.m_sphere.Center = m_position + Math::Vector3(0.0f, 0.5f, 0.0f);
	// 球の半径を設定
	sphereInfo.m_sphere.Radius = 0.5f;
	// アタリ判定をしたいタイプを設定
	sphereInfo.m_type = KdCollider::TypeGround; // 敵のアタリ判定

	m_pDebugWire->AddDebugSphere(sphereInfo.m_sphere.Center, sphereInfo.m_sphere.Radius);

	// 球に当たったオブジェクト情報を格納するリスト
	std::list<KdCollider::CollisionResult> retSpherelist;

	// 球とアタリ判定を行う
	for (auto& obj : SceneManager::Instance().GetObjList())
	{
		obj->Intersects(sphereInfo, &retSpherelist);
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

		//当たってたらその方向から押し出す
		m_position += hitDir * maxOverLap;
	}
}

void Enemy::ImGuiInspector()
{
	CharaBase::ImGuiInspector();

	ImGui::DragFloat(U8("重力の大きさ"), &m_gravitySpeed, 0.01f);
	ImGui::DragFloat(U8("アニメーション速度"), &m_fixedFrameRate, 1.f);

	ImGui::Text(U8("現在の状態"));
	ImGui::DragFloat(U8("移動速度"), &m_moveSpeed, 0.1f);
}

void Enemy::JsonInput(const nlohmann::json& _json)
{
	CharaBase::JsonInput(_json);
	if (_json.contains("GravitySpeed")) m_gravitySpeed = _json["GravitySpeed"].get<float>();
	if (_json.contains("fixedFps")) m_fixedFrameRate = _json["fixedFps"].get<float>();
	if (_json.contains("moveSpeed")) m_moveSpeed = _json["moveSpeed"].get<float>();
}

void Enemy::JsonSave(nlohmann::json& _json) const
{
	CharaBase::JsonSave(_json);
	_json["GravitySpeed"] = m_gravitySpeed;
	_json["fixedFps"] = m_fixedFrameRate;
	_json["moveSpeed"] = m_moveSpeed;
}

void Enemy::StateInit()
{
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
	m_status.hp -= _damage;
	if (m_status.hp <= 0)
	{
		m_Expired = true;
	}
}
