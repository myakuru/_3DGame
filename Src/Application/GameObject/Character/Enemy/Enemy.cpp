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

	m_pCollider->RegisterCollisionShape("EnemySphere", sphere, KdCollider::TypeDamage);

	m_pCollider->RegisterCollisionShape("PlayerSphere", sphere, KdCollider::TypeGround);

	StateInit();
}

void Enemy::Update()
{
	m_position.y = 1.0f;

	// 球の中心座標と半径を設定
	sphere.Center = m_position + Math::Vector3(0.0f, 0.7f, 0.0f); // 敵の位置＋オフセット
	sphere.Radius = 0.2f; // 半径0.5

	m_pDebugWire->AddDebugSphere(sphere.Center, sphere.Radius,kBlueColor);

	SceneManager::Instance().GetObjectWeakPtr(m_wpPlayer);

	if (m_wpPlayer.expired()) return;

	float deltaTime = Application::Instance().GetUnscaledDeltaTime();

	m_attackFrame += deltaTime;

	if (m_attackFrame >= 3.5f) {
		Application::Instance().SetFpsScale(1.f); // スローモーションにする
		SceneManager::Instance().SetDrawGrayScale(false);
		m_attackFrame = 0.0f; // 必要ならリセット
	}

	// ヒット処理。
	if (m_isHit)
	{
		// ダメージステートに変更
		auto spDamageState = std::make_shared<EnemyState_Hit>();
		ChangeState(spDamageState);
		m_isHit = false;	// ダメージフラグをリセット
		return;
	}

	if (m_Expired)
	{
		if (m_dissever < 1.0f)
		{
			m_dissever += 5.0f * deltaTime;
		}
		else
		{
			m_isExpired = true;	// 破棄フラグを立てる
			SceneManager::Instance().m_gameClear = true;
		}
	}

	CharaBase::Update();
}

void Enemy::DrawLit()
{
	//ディゾルブ処理
	KdShaderManager::Instance().m_StandardShader.SetDissolve(m_dissever);
	SelectDraw3dModel::DrawLit();
}

void Enemy::UpdateAttack()
{
	m_isAtkPlayer = false;

	// クォータニオンから前方向ベクトルを取得
	Math::Vector3 forward = Math::Vector3::TransformNormal(Math::Vector3::Forward, Math::Matrix::CreateFromQuaternion(m_rotation));
	forward.Normalize();

	// 球の当たり判定情報作成
	KdCollider::SphereInfo attackSphere;
	// 球の中心座標を設定
	attackSphere.m_sphere.Center = m_position + Math::Vector3(0.0f, 0.5f, 0.0f) + forward * 1.1f;
	// 球の半径を設定
	attackSphere.m_sphere.Radius = m_attackRadius;
	// アタリ判定をしたいタイプを設定
	attackSphere.m_type = KdCollider::TypeDamage;

	m_pDebugWire->AddDebugSphere(attackSphere.m_sphere.Center, attackSphere.m_sphere.Radius); // デバッグ用の球を追加


	auto player = m_wpPlayer.lock();

	if (!player) return;

	// プレイヤーが回避中か判定
	if (player->GetAvoidFlg())
	{
		int avoidFrame = Application::Instance().GetDeltaTime() - player->GetAvoidStartTime();
		if (avoidFrame >= 0 && avoidFrame <= 30) // 3フレーム以内ならジャスト回避
		{
			Application::Instance().SetFpsScale(0.5f); // スローモーションにする
			SceneManager::Instance().SetDrawGrayScale(true);
			return; // ダメージ処理をスキップ
		}
	}


	// 当たり判定
	std::list<KdCollider::CollisionResult> results;
	if (player->Intersects(attackSphere, &results))
	{
		if (!m_onceEffect)
		{
			// 敵にダメージを与える処理
			player->TakeDamage(m_status.attack); // ダメージを与える
			m_isAtkPlayer = true;				 // ヒットチェックを行う
			player->m_isHit = true;				 // プレイヤーのヒットフラグを立てる
			m_onceEffect = true;				 // 1回だけ再生

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
