#include "PlayerCamera.h"
#include"../../../main.h"
#include"../../../../MyFramework/Manager/KeyboardManager.h"
#include"../../../Scene/SceneManager.h"
#include"../../Character/Player/Player.h"

const uint32_t PlayerCamera::TypeID = KdGameObject::GenerateTypeID();

void PlayerCamera::Init()
{
	CameraBase::Init();

	// 注視点
	m_mLocalPos = Math::Matrix::CreateTranslation(0, 1.5f, -5.0f);

	SetCursorPos(m_FixMousePos.x, m_FixMousePos.y);
}

void PlayerCamera::PostUpdate()
{
	if (SceneManager::GetInstance().m_sceneCamera) return;

	// ターゲットの行列(有効な場合利用する)
	Math::Matrix	_targetMat = Math::Matrix::Identity;
	
	SceneManager::GetInstance().GetObjectWeakPtr(m_Player);
	auto _spTarget = m_Player.lock();

	if (_spTarget)
	{
		_targetMat = Math::Matrix::CreateTranslation(_spTarget->GetPos());
	}

	// カメラの回転
	UpdateRotateByMouse();
	m_mRotation = GetRotationMatrix();
	UpdateMoveKey();
	m_mWorld = m_mLocalPos * m_mRotation * _targetMat;

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

void PlayerCamera::ImGuiInspector()
{
	KdGameObject::ImGuiInspector();
}

void PlayerCamera::JsonSave(nlohmann::json& _json) const
{
	KdGameObject::JsonSave(_json);
}

void PlayerCamera::JsonInput(const nlohmann::json& _json)
{
	KdGameObject::JsonInput(_json);
}
