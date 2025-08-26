#include "PlayerCamera.h"
#include"../../../main.h"
#include"../../../../MyFramework/Manager/KeyboardManager.h"
#include"../../../Scene/SceneManager.h"
#include"../../Character/Player/Player.h"
#include"../../../../Framework/Json/Json.h"


const uint32_t PlayerCamera::TypeID = KdGameObject::GenerateTypeID();

void PlayerCamera::Init()
{
	CameraBase::Init();

	// エフェクシアのカメラをセット
	KdEffekseerManager::GetInstance().SetCamera(m_spCamera);

	m_pDebugWire = std::make_unique<KdDebugWireFrame>();
	SetCursorPos(m_FixMousePos.x, m_FixMousePos.y);

	m_cameraPos = Math::Vector3(-4.0f, 1.2f, 4.0f);
}

void PlayerCamera::PostUpdate()
{
	if (SceneManager::Instance().m_sceneCamera) return;

	float deltaTime = Application::Instance().GetDeltaTime();

	// ターゲットの行列(有効な場合利用する)
	Math::Matrix _targetMat = Math::Matrix::Identity;

	SceneManager::Instance().GetObjectWeakPtr(m_Player);
	auto _spTarget = m_Player.lock();

	if (!_spTarget) return;

	if (SceneManager::Instance().IsIntroCamera())
	{
		UpdateIntroCamera();
		return;
	}


	if (SceneManager::Instance().m_gameClear)
	{
		UpdateWinnerCamera();
		return;
	}

	// カメラの回転
	UpdateRotateByMouse();
	m_mRotation = GetRotationMatrix();

	// シェイク用オフセット
	Math::Vector3 shakeOffset = Math::Vector3::Zero;
	if (m_shakeTime > 0.0f)
	{
		shakeOffset.x = KdRandom::GetFloat(-m_shakePower, m_shakePower);
		shakeOffset.y = KdRandom::GetFloat(-m_shakePower, m_shakePower);
		m_shakeTime -= deltaTime;
		if (m_shakeTime <= 0.0f)
		{
			m_shakeTime = 0.0f;
			shakeOffset = Math::Vector3::Zero;
		}
	}
	else
	{
		shakeOffset = Math::Vector3::Zero;
	}

	
	// カメラの位置をターゲットの位置に設定
	m_mWorld = Math::Matrix::CreateTranslation(m_targetLookAt);
	m_mWorld = m_mWorld * m_mRotation;

	Math::Vector3 targetPos = _spTarget->GetPosition() + shakeOffset;

	m_cameraPos = Math::Vector3::Lerp(m_cameraPos, targetPos, m_smooth * deltaTime);

	m_mWorld.Translation(m_mWorld.Translation() + m_cameraPos);

	m_spCamera->SetCameraMatrix(m_mWorld);

	// ↓めり込み防止の為の座標補正計算↓
	// ①当たり判定(レイ判定)用の情報作成
	KdCollider::RayInfo rayInfo;
	// レイの発射位置を設定
	rayInfo.m_pos = GetPos();

	// レイの発射方向を設定
	rayInfo.m_dir = Math::Vector3::Down;
	// レイの長さを設定
	rayInfo.m_range = 1000.f;
	if (_spTarget)
	{
		Math::Vector3 _targetPos = _spTarget->GetPos() + Math::Vector3{ 0, 1.0f, 0 };
		_targetPos.y += 0.1f;
		rayInfo.m_dir = _targetPos - GetPos();
		rayInfo.m_range = rayInfo.m_dir.Length();
		rayInfo.m_dir.Normalize();
	}

	// 当たり判定をしたいタイプを設定
	rayInfo.m_type = KdCollider::TypeGround;

	// ②HIT判定対象オブジェクトに総当たり
	for (std::weak_ptr<KdGameObject> wpGameObj : m_wpHitObjectList)
	{
		std::shared_ptr<KdGameObject> spGameObj = wpGameObj.lock();
		if (spGameObj)
		{
			std::list<KdCollider::CollisionResult> retRayList;
			spGameObj->Intersects(rayInfo, &retRayList);

			// ③ 結果を使って座標を補完する
			// レイに当たったリストから一番近いオブジェクトを検出
			float maxOverLap = 0;
			Math::Vector3 hitPos = {};
			bool hit = false;
			for (auto& ret : retRayList)
			{
				// レイを遮断しオーバーした長さが
				// 一番長いものを探す
				if (maxOverLap < ret.m_overlapDistance)
				{
					maxOverLap = ret.m_overlapDistance;
					hitPos = ret.m_hitPos;
					hit = true;
				}
			}
			if (hit)
			{
				// 何かしらの障害物に当たっている
				Math::Vector3 _hitPos = hitPos;
				_hitPos += rayInfo.m_dir * 0.4f;
				SetPos(_hitPos);
			}
		}
	}
}

void PlayerCamera::UpdateWinnerCamera()
{
	Application::Instance().SetFpsScale(0.0);

	enum Step { ToDeg60, ToDeg180, ToDeg0, End };
	static Step step = ToDeg60;
	static float timer = 0.0f;
	static Math::Vector3 camOffset = { 0, 1.0f, -3.0f };
	float deltaTime = Application::Instance().GetUnscaledDeltaTime();
	timer += deltaTime;

	Math::Vector3 playerPos = m_Player.lock()->GetPos();

	switch (step)
	{
	case ToDeg60:
	{
		// -60度の位置へLerp
		m_degree.y = 60.0f;
		m_mRotation = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_degree.y));
		Math::Vector3 targetPos = playerPos + camOffset;
		m_cameraPos = Math::Vector3::Lerp(m_cameraPos, targetPos, 2.0f * deltaTime); // 0.1fで滑らかに
		if ((m_cameraPos - targetPos).Length() < 0.01f)
		{
			step = ToDeg180;
			timer = 0.0f;
		}
		break;
	}
	case ToDeg180:
		m_degree.y = 180.0f;
		m_mRotation = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_degree.y));
		if (timer > 1.5f) { step = ToDeg0; timer = 0.0f; }
		break;
	case ToDeg0:
		m_degree.y = 0.0f;
		m_mRotation = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_degree.y));
		if (timer > 1.5f) { step = End; timer = 0.0f; }
		break;
	case End:
		// カメラの向いている方向に向かって移動
	{
		Math::Vector3 targetOffset = Math::Vector3::Up;
		camOffset = Math::Vector3::Lerp(camOffset, targetOffset, 5.0f * deltaTime);
		m_cameraPos = playerPos + camOffset;
		if ((camOffset - targetOffset).Length() < 0.1f)
		{
			camOffset = targetOffset;
			Application::Instance().SetFpsScale(1.0);
			SceneManager::Instance().m_gameClear = false;
			SceneManager::Instance().SetNextScene(SceneManager::SceneType::Result);
		}
		break;
	}
	}

	// カメラのワールド行列を更新
	m_mWorld = Math::Matrix::CreateTranslation(m_cameraPos - playerPos) * m_mRotation;
	m_mWorld.Translation(playerPos + m_mWorld.Translation());
	m_spCamera->SetCameraMatrix(m_mWorld);
}

void PlayerCamera::UpdateIntroCamera()
{
	float deltaTime = Application::Instance().GetDeltaTime();
	m_introTimer += deltaTime;

	// 回転速度
	constexpr float ROTATE_SPEED = 40.0f; // 1秒で40度回転

	// 目標相対位置
	const Math::Vector3 startPos(0.0f, 1.0f, -1.2f);
	const Math::Vector3 endPos(0.0f, 1.3f, -3.0f);

	// 初回のみ初期化
	if (m_introTimer == deltaTime) {
		m_degree = { 0.0f, -20.0f, 0.0f };
		m_introCamPos = startPos;
	}

	// 目標角度まで加算
	m_degree.y -= ROTATE_SPEED * deltaTime;

	// 目標位置に十分近づいたら終了
	if (m_degree.y >= -261.0f && m_degree.y <= -240.0f)
	{
		// カメラ位置を補間
		constexpr float LERP_SPEED = 3.0f; // 1秒で1.0進む
		m_introCamPos = Math::Vector3::Lerp(m_introCamPos, endPos, LERP_SPEED * deltaTime);
	}
	else if (m_degree.y <= -261.0f)
	{
		m_introCamPos = endPos;
		m_degree.y = -261.0f;
		SceneManager::Instance().SetIntroCamera(false); // カメラのイントロを終了
		m_introTimer = 0.0f;
	}

	m_mRotation = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_degree.y));

	// カメラのワールド行列を更新
	m_mWorld = Math::Matrix::CreateTranslation(m_introCamPos); // プレイヤーからの相対位置
	m_mWorld *= m_mRotation; // 回転
	m_mWorld.Translation(m_mWorld.Translation() + m_Player.lock()->GetPos());

	m_spCamera->SetCameraMatrix(m_mWorld);
}

void PlayerCamera::ImGuiInspector()
{
	CameraBase::ImGuiInspector();

	ImGui::Text(U8("プレイヤーとカメラの距離"));
	ImGui::DragFloat3("offsetPos", &m_targetLookAt.x, 0.1f);
	ImGui::DragFloat("Camera Smooth", &m_smooth, 0.01f);
}

void PlayerCamera::JsonSave(nlohmann::json& _json) const
{
	CameraBase::JsonSave(_json);
	_json["targetLookAt"] = JSON_MANAGER.VectorToJson(m_targetLookAt);
	_json["smooth"] = m_smooth;
}

void PlayerCamera::JsonInput(const nlohmann::json& _json)
{
	CameraBase::JsonInput(_json);
	if(_json.contains("targetLookAt")) m_targetLookAt = JSON_MANAGER.JsonToVector(_json["targetLookAt"]);
	if (_json.contains("smooth")) m_smooth = _json["smooth"].get<float>();
}

DirectX::BoundingFrustum PlayerCamera::CreateFrustum() const
{
	DirectX::BoundingFrustum frustum;
	DirectX::BoundingFrustum::CreateFromMatrix(frustum, m_spCamera->GetProjMatrix());
	// カメラのワールド行列で変換
	frustum.Transform(frustum, m_spCamera->GetCameraMatrix());

	return frustum;
}

void PlayerCamera::DebugDraw(DirectX::BoundingFrustum _frustum)
{

	// 8つのコーナー座標を取得
	DirectX::XMFLOAT3 corners[8];
	_frustum.GetCorners(corners);

	// 視錐台の12本のエッジを線で描画
	auto addLine = [this, &corners](int i0, int i1) {
		m_pDebugWire->AddDebugLine(
			Math::Vector3(corners[i0].x, corners[i0].y, corners[i0].z),
			Math::Vector3(corners[i1].x, corners[i1].y, corners[i1].z),
			Math::Color(1, 1, 0, 1) // 黄色など任意の色
		);
		};

	// エッジのインデックス（Near面:0-3, Far面:4-7）
	const int edgeIndices[12][2] = {
		{0,1},{1,2},{2,3},{3,0}, // Near
		{4,5},{5,6},{6,7},{7,4}, // Far
		{0,4},{1,5},{2,6},{3,7}  // Side
	};
	for (const auto& edge : edgeIndices) {
		addLine(edge[0], edge[1]);
	}
}
