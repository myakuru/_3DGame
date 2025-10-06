#include "PlayerState_ChargeAttackMax.h"
#include"../../../../../main.h"
#include"../PlayerState_ChargeAttackMax1/PlayerState_ChargeAttackMax1.h"
#include"../../../Enemy/Enemy.h"
#include"../../../../Camera/PlayerCamera/PlayerCamera.h"

void PlayerState_ChargeAttackMax::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("Eskill");
	m_player->GetAnimator()->SetAnimation(anime, 0.25f, false);
	PlayerStateBase::StateStart();
	m_time = 0.0f;

	// 敵との当たり判定を無効化
	m_player->SetAtkPlayer(true);

	// アニメーション速度を変更
	m_player->SetAnimeSpeed(100.0f);

	// 残像の設定
	m_player->AddAfterImage(true, 5, 2, Math::Color(0.0f, 1.0f, 1.0f, 0.2f));
}

void PlayerState_ChargeAttackMax::StateUpdate()
{

	if (m_player->GetAnimator()->IsAnimationEnd())
	{
		auto state = std::make_shared<PlayerState_ChargeAttackMax1>();
		m_player->ChangeState(state);
		return;
	}
	PlayerStateBase::StateUpdate();

	UpdateKatanaPos();

	// 攻撃中の移動方向で回転を更新
	if (m_player->GetMovement() != Math::Vector3::Zero)
	{
		Math::Vector3 moveDir = m_player->GetMovement();
		moveDir.y = 0.0f;
		moveDir.Normalize();
		m_player->UpdateQuaternionDirect(moveDir);
	}

	float deltaTime = Application::Instance().GetDeltaTime();

	// 先行ダッシュ処理
	if (auto enemy = m_player->GetEnemy().lock(); enemy)
	{
		// 敵の奥に行くようにする距離
		constexpr float overshootDist = 5.f;

		// 攻撃方向が指定されていない場合は敵の方向に向かう
		Math::Vector3 fallbackDir = enemy->GetPos() - m_player->GetPos();

		const Math::Vector3 dashDir = (m_attackDirection != Math::Vector3::Zero) ? m_attackDirection : fallbackDir;

		const Math::Vector3 desiredPoint = enemy->GetPos() + dashDir * overshootDist;

		Math::Vector3 toDesired = desiredPoint - m_player->GetPos();
		toDesired.y = 0.0f;
		float distance = toDesired.Length();

		const float arriveEps = 1.0f;

		if (distance <= arriveEps)
		{
			m_player->SetIsMoving(Math::Vector3::Zero);

			// キャラを敵の方を向ける
			if (fallbackDir != Math::Vector3::Zero)
			{
				fallbackDir.y = 0.0f;
				fallbackDir.Normalize();
				m_player->UpdateQuaternionDirect(fallbackDir);
			}

			// カメラをキャラの後ろに回す（セッター使用）
			if (auto camera = m_player->GetPlayerCamera().lock(); camera)
			{
				constexpr Math::Vector3 kCamOffset = { 0.0f, 1.2f, -4.0f };
				camera->SetTargetLookAt(kCamOffset);
				// カメラの回転を滑らかに
				camera->SetRotationSmooth(4.0f);
				// カメラの距離を滑らかに
				camera->SetDistanceSmooth(3.0f);

				// キャラ前方からヨー角(deg)を計算してカメラ回転に反映
				if (fallbackDir != Math::Vector3::Zero)
				{
					fallbackDir.Normalize();
					const float yawRad = std::atan2(fallbackDir.x, fallbackDir.z);
					const float yawDeg = DirectX::XMConvertToDegrees(yawRad);
					camera->SetTargetRotation({ 0.0f, yawDeg, 0.0f });
				}
			}
		}
		else
		{
			toDesired.Normalize();
			float maxStep = 10.0f * deltaTime;
			float speedThisFrame = (distance < maxStep) ? (distance / deltaTime) : 10.0f;
			m_player->SetIsMoving(toDesired * speedThisFrame);
			m_time += deltaTime;
		}
	}
}

void PlayerState_ChargeAttackMax::StateEnd()
{
	PlayerStateBase::StateEnd();
	// 敵との当たり判定をもとに戻す
	m_player->SetAtkPlayer(false);

	// 残像の設定を元に戻す
	m_player->AddAfterImage();
}
