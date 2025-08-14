#include "CharacterBase.h"
#include"../../main.h"
#include"../../Scene/SceneManager.h"
#include"../Camera/PlayerCamera/PlayerCamera.h"

void CharaBase::Init()
{
	KdGameObject::Init();
	m_pDebugWire = std::make_unique<KdDebugWireFrame>();

	ModelLoad(m_path);

	//m_trailPolygon.SetMaterial("Asset/Textures/System/WhiteNoise.png");
}

std::shared_ptr<KdModelWork> CharaBase::GetAnimeModel()
{
	return m_modelWork; 
}

void CharaBase::UpdateQuaternion(Math::Vector3& _moveVector)
{
	float deltaTime = Application::Instance().GetDeltaTime();

	if (_moveVector == Math::Vector3::Zero) return;

	SceneManager::Instance().GetObjectWeakPtr(m_playerCamera);
	if (m_playerCamera.expired()) return;

	const auto camera = m_playerCamera.lock();

	// カメラのY軸回転と移動ベクトルをかけ合わせて、WASDから入力された値に基づく方向を計算
	_moveVector = Math::Vector3::TransformNormal(_moveVector, camera->GetRotationYMatrix());

	_moveVector.Normalize();

	// 向きたい方向をクォータニオンに変換 第1引数：前を向く方向, 第2引数：どっちが上か
	Math::Quaternion targetRotation = Math::Quaternion::LookRotation(_moveVector, Math::Vector3::Up);

	// 滑らかに回転させるために、現在の回転と目標の回転を補間
	m_rotation = Math::Quaternion::Slerp(m_rotation, targetRotation, deltaTime * m_fixedFrameRate);

}

void CharaBase::Update()
{
	KdGameObject::Update();

	float deltaTime = Application::Instance().GetDeltaTime();

	m_stateManager.Update();

	m_animator->AdvanceTime(m_modelWork->WorkNodes(), m_fixedFrameRate * deltaTime);

	m_isMoving = m_movement.LengthSquared() > 0;

	// 移動関係
	m_gravity += m_gravitySpeed * deltaTime;

	// 最終的な移動量
	m_position.x += m_movement.x * m_moveSpeed * m_fixedFrameRate * deltaTime;
	m_position.z += m_movement.z * m_moveSpeed * m_fixedFrameRate * deltaTime;
	m_position.y += m_gravity;

	// 最終的なワールド行列計算
	Math::Matrix scale = Math::Matrix::CreateScale(m_scale);
	Math::Matrix quaternion = Math::Matrix::CreateFromQuaternion(m_rotation);
	Math::Matrix translation = Math::Matrix::CreateTranslation(m_position);

	m_mWorld = scale * quaternion * translation;

	// ステートで移動処理など管理

	// トレイルポリゴンの更新
	//m_trailPolygon.AddPoint(m_mWorld);
}

void CharaBase::PostUpdate()
{
	// ====================================================
	// レイの当り判定::::::::::::::::::::ここから::::::::::::::
	// ====================================================

	// レイ判定用に必要なParameter
	KdCollider::RayInfo rayInfo;

	// レイの何処から発射するか
	rayInfo.m_pos = m_position;

	// 段差の許容範囲を設定
	static const float enableStepHigh = 0.2f;
	rayInfo.m_pos.y += enableStepHigh;			// 0.2f までの段差は登れる

	// レイの方向を設定
	rayInfo.m_dir = { 0.0f,-1.0f,0.0f };

	// レイの長さを設定
	rayInfo.m_range = enableStepHigh + m_gravity;

	// アタリ判定したいタイプを設定
	rayInfo.m_type = KdCollider::TypeGround;

	m_pDebugWire->AddDebugLine(rayInfo.m_pos, rayInfo.m_dir, rayInfo.m_range);

	// レイに当たったオブジェクト情報を格納するリスト
	std::list<KdCollider::CollisionResult> retRayList;
	// 作成したレイ情報でオブジェクトリストと当たり判定をする
	for (auto& obj : SceneManager::Instance().GetMapList())
	{
		obj->Intersects(rayInfo, &retRayList);
	}

	// レイに当たったリストから一番近いオブジェクトを検出
	bool hit = false;
	float maxOverLap = 0;
	Math::Vector3 groundPos = {};	// レイが遮断された(Hitした)座標

	for (auto& ret : retRayList)
	{
		// レイが当たったオブジェクトの中から
		// 「m_overlapDistance = 貫通した長さ」が一番長いものを探す
		// 「m_overlapDistance が一番長い = 一番近くで当たった」と判定できる
		if (maxOverLap < ret.m_overlapDistance)
		{
			maxOverLap = ret.m_overlapDistance;
			groundPos = ret.m_hitPos;
			hit = true;
		}
	}

	// 当たっていたら
	if (hit)
	{
		m_gravity = 0.0f;	// 重力をリセット
		m_position = groundPos;
	}

	//=====================================================
	// レイ当り判定::::::::::::::::::::ここまで::::::::::::::::
	//=====================================================

	// 球判定
	// 球判定用の変数
	KdCollider::SphereInfo sphereInfo;
	// 球の中心座標を設定
	sphereInfo.m_sphere.Center = m_position + Math::Vector3(0.0f, 0.2f, 0.0f);
	// 球の半径を設定
	sphereInfo.m_sphere.Radius = 0.2f;
	// アタリ判定をしたいタイプを設定
	sphereInfo.m_type = KdCollider::TypeGround; // 敵のアタリ判定

	m_pDebugWire->AddDebugSphere(sphereInfo.m_sphere.Center, sphereInfo.m_sphere.Radius);

	// 球に当たったオブジェクト情報を格納するリスト
	std::list<KdCollider::CollisionResult> retSpherelist;

	// 球とアタリ判定を行う
	for (auto& obj : SceneManager::Instance().GetMapList())
	{
		obj->Intersects(sphereInfo, &retSpherelist);
	}

	//  球にあたったリストから一番近いオブジェクトを探す
	// オーバーした長さが1番長いものを探す。
	// 使いまわしの変数を使う
	maxOverLap = 0.0f;
	hit = false;
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

bool CharaBase::ModelLoad(std::string _path)
{
	if (m_modelWork)
	{
		m_modelWork->SetModelData(_path);
		return true;
	}
	return false;
}

void CharaBase::ImGuiInspector()
{
	SelectDraw3dModel::ImGuiInspector();
}

void CharaBase::JsonInput(const nlohmann::json& _json)
{
	SelectDraw3dModel::JsonInput(_json);
}

void CharaBase::JsonSave(nlohmann::json& _json) const
{
	SelectDraw3dModel::JsonSave(_json);
}
