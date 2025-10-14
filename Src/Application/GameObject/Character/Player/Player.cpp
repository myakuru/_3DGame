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
#include"../../Collition/Collition.h"

const uint32_t Player::TypeID = KdGameObject::GenerateTypeID();

void Player::Init()
{
	CharaBase::Init();

	m_animator->SetAnimation(m_modelWork->GetData()->GetAnimation("Idle"));

	// ノードの再計算
	m_modelWork->CalcNodeMatrices();

	StateInit();

	//m_position = Math::Vector3(-4.0f, 1.2f, 4.0f);

	//m_position.y = 1.0f;

	// 初期向きを-90度（Y軸）に設定
	m_rotation = Math::Quaternion::CreateFromAxisAngle(Math::Vector3::Up, DirectX::XMConvertToRadians(-90.0f));

	m_pCollider = std::make_unique<KdCollider>();

	m_pCollider->RegisterCollisionShape("PlayerSphere", sphere, KdCollider::TypeDamage);
	m_pCollider->RegisterCollisionShape("PlayerSphere", sphere, KdCollider::TypeGround);

	m_onceEffect = false;	// エフェクトの一回だけフラグ
	m_isAtkPlayer = false;	// プレイヤーが攻撃しているかどうか
	m_invincible = false;	// 無敵フラグ

	// 残像描画用 Work を元データで生成
	if (auto* src = GetModelWork())
	{
		m_afterImageWork = std::make_unique<KdModelWork>(src->GetData());
	}

	m_dissever = 0.0f;

	m_isHit = false;
}

void Player::PreUpdate()
{
	sphere.Center = m_position + Math::Vector3(0.0f, 0.5f, 0.0f); // 敵の位置＋オフセット
	sphere.Radius = 0.2f;
	m_pDebugWire->AddDebugSphere(sphere.Center, sphere.Radius,kRedColor);


	// カタナの取得
	if (auto katana = m_katana.lock(); katana)
	{
		katana->SetPlayerMatrix(m_mWorld);
		katana->SetPlayerHandMatrix(m_mWorld);
	}

	if (auto scabbard = m_scabbard.lock(); scabbard)
	{
		scabbard->SetPlayerMatrix(m_mWorld);
		scabbard->SetPlayerHandMatrix(m_mWorld);
	}

}

void Player::PostUpdate()
{
	CharaBase::PostUpdate();

	// 残像処理
	CaptureAfterImage();

	// ライトの影の中心位置をプレイヤーに合わせる
	auto& amb = KdShaderManager::Instance().WorkAmbientController();
	amb.SetShadowCenter(m_position);

	if (m_isAtkPlayer) return;

	KdCollider::SphereInfo enemyHit;
	// 球の中心座標を設定
	enemyHit.m_sphere.Center = m_position + Math::Vector3(0.0f, 0.5f, 0.0f);
	// 球の半径を設定
	enemyHit.m_sphere.Radius = 0.2f;
	// アタリ判定をしたいタイプを設定
	enemyHit.m_type = KdCollider::TypeEnemyHit; // 敵のアタリ判定
	m_pDebugWire->AddDebugSphere(enemyHit.m_sphere.Center, enemyHit.m_sphere.Radius);

	// 球に当たったオブジェクト情報を格納するリスト
	std::list<KdCollider::CollisionResult> retSpherelist;

	SceneManager::Instance().GetObjectWeakPtrList(m_enemies);

	for (const auto& enemy : m_enemies)
	{
		if (auto enemyPtr = enemy.lock())
		{
			enemyPtr->Intersects(enemyHit, &retSpherelist);
		}
	}

	//  球にあたったリストから一番近いオブジェクトを探す
	// オーバーした長さが1番長いものを探す。
	float maxOverLap = 0.0f;
	float hit = false;

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
		hitDir.y = 0.0f;
		if (hitDir.LengthSquared() > 0) hitDir.Normalize();

		// 両者が等しく離れる想定 → プレイヤー側は半分だけ動く
		Math::Vector3 push = hitDir * (maxOverLap * 0.5f);

		// 壁を貫通しないようスイープして移動
		ApplyPushWithCollision(push);
	}
}

void Player::DrawUnLit()
{
	if (m_afterImageEnable)
	{
		DrawAfterImages();
	}
}

void Player::Update()
{
	KdGameObject::Update();

	SceneManager::Instance().GetObjectWeakPtr(m_playerCamera);
	SceneManager::Instance().GetObjectWeakPtr(m_enemy);
	SceneManager::Instance().GetObjectWeakPtr(m_katana);
	SceneManager::Instance().GetObjectWeakPtr(m_scabbard);

	float deltaTime = Application::Instance().GetDeltaTime();

	m_animator->AdvanceTime(m_modelWork->WorkNodes(), m_fixedFrameRate * deltaTime);
	m_modelWork->CalcNodeMatrices();

	m_isMoving = m_movement.LengthSquared() > 0;

	// 移動前位置を保存
	m_prevPosition = m_position;

	// 重力更新
	m_gravity += m_gravitySpeed * deltaTime;

	// 水平移動
	ApplyHorizontalMove(m_movement, deltaTime);

	// 垂直
	m_position.y += m_gravity;

	m_stateManager.Update();

	Math::Matrix scale = Math::Matrix::CreateScale(m_scale);
	Math::Matrix quaternion = Math::Matrix::CreateFromQuaternion(m_rotation);
	Math::Matrix translation = Math::Matrix::CreateTranslation(m_position);
	m_mWorld = scale * quaternion * translation;


	// ヒット処理。
	if (m_isHit)
	{
		if (m_invincible) return; // 無敵状態ならヒットしない

		// ダメージステートに変更
		auto spDamageState = std::make_shared<PlayerState_Hit>();
		ChangeState(spDamageState);

		m_isHit = false;	// ダメージフラグをリセット
		return;
	}

}

void Player::UpdateAttackCollision(float _radius, float _distance, int _attackCount, float _attackTimer, Math::Vector2 _cameraShakePow, float _cameraTime)
{
	Math::Vector3 forward = Math::Vector3::TransformNormal(Math::Vector3::Forward, Math::Matrix::CreateFromQuaternion(m_rotation));
	forward.Normalize();

	float deltaTime = Application::Instance().GetDeltaTime();

	KdCollider::SphereInfo attackSphere;
	attackSphere.m_sphere.Center = m_position + Math::Vector3(0.0f, 0.5f, 0.0f) + forward * _distance;
	attackSphere.m_sphere.Radius = _radius;
	attackSphere.m_type = KdCollider::TypeDamage;

	m_pDebugWire->AddDebugSphere(attackSphere.m_sphere.Center, attackSphere.m_sphere.Radius);

	SceneManager::Instance().GetObjectWeakPtrList(m_enemies);

	// 初回セットアップ: 初期タイマを0に
	if (!m_onceEffect)
	{
		m_isChargeAttackActive = true;
		m_chargeAttackCount = 0;
		m_chargeAttackTimer = 0.0f;
		m_onceEffect = true;
	}

	if (!m_isChargeAttackActive) return;

	m_chargeAttackTimer += deltaTime;

	if (m_chargeAttackCount < _attackCount && m_chargeAttackTimer >= _attackTimer)
	{
		bool hitAny = false;

		for (const auto& enemyies : m_enemies)
		{
			if (auto enemyPtr = enemyies.lock())
			{
				std::list<KdCollider::CollisionResult> results;
				if (enemyPtr->Intersects(attackSphere, &results) && !results.empty())
				{
					enemyPtr->Damage(m_status.attack);
					enemyPtr->SetEnemyHit(true);
					hitAny = true;
				}
			}
		}

		if (hitAny)
		{
			if (auto camera = m_playerCamera.lock(); camera)
			{
				camera->StartShake(_cameraShakePow, _cameraTime);
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

void Player::ImGuiInspector()
{
	CharaBase::ImGuiInspector();

	ImGui::Text(U8("プレイヤーの設定"));

	ImGui::DragFloat(U8("重力の大きさ"), &m_gravitySpeed, 0.01f);
	ImGui::DragFloat(U8("フレームレート制限"), &m_fixedFrameRate, 1.f);

	ImGui::Text(U8("プレイヤーの状態"));
	ImGui::DragFloat(U8("移動速度"), &m_moveSpeed, 0.1f);

	ImGui::Text(U8("Attack１のカメラの揺れ"));
	ImGui::DragFloat2(U8("揺れの大きさ"), &m_cameraShakePower.x, 0.01f);

	ImGui::Text(U8("Attack1のカメラの揺れ時間"));
	ImGui::DragFloat(U8("揺れの時間"), &m_cameraShakeTime, 0.01f);

	ImGui::Text(U8("プレイヤーの回転速度"));
	ImGui::DragFloat(U8("回転速度"), &m_rotateSpeed, 0.1f);

	ImGui::Separator();
	m_playerConfig.InGuiInspector();
}

void Player::JsonInput(const nlohmann::json& _json)
{
	CharaBase::JsonInput(_json);
	if (_json.contains("GravitySpeed")) m_gravitySpeed = _json["GravitySpeed"].get<float>();
	if (_json.contains("fixedFps")) m_fixedFrameRate = _json["fixedFps"].get<float>();
	if (_json.contains("moveSpeed")) m_moveSpeed = _json["moveSpeed"].get<float>();

	if (_json.contains("cameraShake")) m_cameraShakePower = JSON_MANAGER.JsonToVector2(_json["cameraShake"]);
	if (_json.contains("cameraShakeTime")) m_cameraShakeTime = _json["cameraShakeTime"].get<float>();
	if (_json.contains("rotateSpeed")) m_rotateSpeed = _json["rotateSpeed"].get<float>();

	m_playerConfig.JsonInput(_json);
}

void Player::JsonSave(nlohmann::json& _json) const
{
	CharaBase::JsonSave(_json);
	_json["GravitySpeed"] = m_gravitySpeed;
	_json["fixedFps"] = m_fixedFrameRate;
	_json["moveSpeed"] = m_moveSpeed;
	_json["cameraShake"] = JSON_MANAGER.Vector2ToJson(m_cameraShakePower);
	_json["cameraShakeTime"] = m_cameraShakeTime;
	_json["rotateSpeed"] = m_rotateSpeed;

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

void Player::CaptureAfterImage()
{


	if (!m_afterImageEnable)
	{
		// 残像無効ならクリアして終了
		m_afterImages.clear();
		m_afterImageCounter = 0;
		return;
	}

	// モデルワークが無効なら処理しない
	KdModelWork* work = GetModelWork();
	if (!work || !work->IsEnable()) return;

	// m_afterImageIntervalを超えるまでカウンタを進める
	if (m_afterImageCounter++ < m_afterImageInterval) return;
	m_afterImageCounter = 0.0f;

	// ノード worldTransform をスナップショット
	const auto& nodes = work->GetNodes();

	AfterImageFrame frame;

	// ノード worldTransform を保存
	frame.nodeWorlds.resize(nodes.size());

	// 各ノードの worldTransform を保存
	for (size_t i = 0; i < nodes.size(); ++i)
	{
		frame.nodeWorlds[i] = nodes[i].m_worldTransform;
	}
	// 現在の m_mWorld を保存
	frame.ownerWorld = m_mWorld;

	Math::Matrix scale = Math::Matrix::CreateScale(1,1,1);

	frame.ownerWorld = scale * frame.ownerWorld;

	// 先頭に追加し、上限を超えたら末尾を捨てる
	m_afterImages.push_front(std::move(frame));
	while ((int)m_afterImages.size() > m_afterImageMax) m_afterImages.pop_back();
}

void Player::DrawAfterImages()
{
	if (!m_afterImageEnable) return;
	if (!m_afterImageWork) return;
	if (m_afterImages.empty()) return;

	auto& stdShader = KdShaderManager::Instance().m_StandardShader;

	// 古いもの→新しいものの順
	for (int i = (int)m_afterImages.size() -1; i >= 0; --i)
	{
		// フレームデータ取得
		const auto& frameData = m_afterImages[i];

		// ノード数が異なる場合はスキップ
		auto& workNodes = m_afterImageWork->WorkNodes();
		if (workNodes.size() != frameData.nodeWorlds.size()) continue;

		// ノード worldTransform を上書き
		for (size_t nodeIndex = 0; nodeIndex < workNodes.size(); ++nodeIndex)
		{
			workNodes[nodeIndex].m_worldTransform = frameData.nodeWorlds[nodeIndex];
		}

		m_afterImageWork->SetNeedCalcNodeMatrices(false);

		// 残像の描画
		float a = 1.0f;
		Math::Vector3 color = {0,1,1};
		KdShaderManager::Instance().m_StandardShader.SetDissolve(m_dissever, &a, &color);
		stdShader.DrawModel(*m_afterImageWork, frameData.ownerWorld, m_afterImageColor);
	}
}

void Player::ApplyHorizontalMove(const Math::Vector3& inputMove, float deltaTime)
{
	if (inputMove == Math::Vector3::Zero) return;

	// ワールドに既にカメラ回転等を反映済み前提で呼ぶならそのまま
	Math::Vector3 desired = inputMove * m_moveSpeed * m_fixedFrameRate * deltaTime;
	float desiredLen = desired.Length();
	if (desiredLen <= FLT_EPSILON) return;

	Math::Vector3 dir = desired / desiredLen;

	// レイ(スイープ)を組む
	KdCollider::RayInfo ray;
	ray.m_pos = m_prevPosition + Math::Vector3(0.0f, kBumpSphereYOffset, 0.0f);
	ray.m_dir = dir;
	ray.m_range = desiredLen + kBumpSphereRadius; // 半径分も伸ばす
	ray.m_type = KdCollider::TypeBump;

	// デバック
	m_pDebugWire->AddDebugLine(ray.m_pos, ray.m_dir, ray.m_range, kRedColor);

	std::list<KdCollider::CollisionResult> rayHits;

	SceneManager::Instance().GetObjectWeakPtrList(m_collisionList);

	for(auto & weakCol : m_collisionList)
	{
		if (auto col = weakCol.lock(); col)
		{
			col->Intersects(ray, &rayHits);
		}
	}

	bool blocked = false;
	float bestOverlap = 0.0f;
	Math::Vector3 hitPos{};
	for (auto& h : rayHits)
	{
		if (bestOverlap < h.m_overlapDistance)
		{
			bestOverlap = h.m_overlapDistance;
			hitPos = h.m_hitPos;
			blocked = true;
		}
	}

	if (blocked)
	{
		// 衝突点までの実距離
		float hitDist = (hitPos - ray.m_pos).Length();

		// 衝突点手前(球が触れる直前)までの許可距離
		float allow = std::max(0.0f, hitDist - kBumpSphereRadius - kCollisionMargin);

		// 位置をクランプ
		m_position = m_prevPosition + dir * allow;

		// これ以上進ませないため入力移動を殺す
		// (スライドさせたいなら法線成分だけゼロにして再計算する)
		// 今回は「絶対に止まる」要求なので全停止。
		// ※ m_movement が他で再利用されるならここでゼロ化
		m_movement = Math::Vector3::Zero;

	}
	else
	{
		// 衝突なし そのまま移動
		m_position = m_prevPosition + desired;
	}
}

void Player::ApplyPushWithCollision(const Math::Vector3& rawPush)
{
	if (rawPush.LengthSquared() <= 1e-8f) return;

	// 水平のみ扱う（既存処理と整合）
	Math::Vector3 push = rawPush;
	push.y = 0.0f;
	float len = push.Length();
	if (len <= 1e-6f) return;
	Math::Vector3 dir = push / len;

	// スイープレイ生成（ApplyHorizontalMove と同等思想）
	KdCollider::RayInfo ray;
	ray.m_pos = m_position + Math::Vector3(0.0f, kBumpSphereYOffset, 0.0f);
	ray.m_dir = dir;
	ray.m_range = len + kBumpSphereRadius;
	ray.m_type = KdCollider::TypeBump;

	std::list<KdCollider::CollisionResult> rayHits;
	SceneManager::Instance().GetObjectWeakPtrList(m_collisionList);
	for (auto& wk : m_collisionList)
	{
		if (auto col = wk.lock())
		{
			col->Intersects(ray, &rayHits);
		}
	}

	bool blocked = false;
	float bestOverlap = 0.0f;
	Math::Vector3 hitPos{};
	for (auto& h : rayHits)
	{
		if (bestOverlap < h.m_overlapDistance)
		{
			bestOverlap = h.m_overlapDistance;
			hitPos = h.m_hitPos;
			blocked = true;
		}
	}

	if (blocked)
	{
		float hitDist = (hitPos - ray.m_pos).Length();
		float allow = std::max(0.0f, hitDist - kBumpSphereRadius - kCollisionMargin);
		if (allow > 0.0f)
		{
			m_position += dir * allow;
		}
		// 衝突点を超える押し出しは捨てる（滑らせたいなら法線投影をここで実施）
	}
	else
	{
		m_position += push;
	}
}
