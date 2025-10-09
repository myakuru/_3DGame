#include "PlayerState_SpecialAttack1.h"
#include"../PlayerState_Sheathing-of-Katana/PlayerState_Sheathing-of-Katana.h"
#include"../../../../../main.h"
#include"../../../../Effect/EffekseerEffect/SpecialAttack1/SpecialAttack1.h"
#include"../../../../../Scene/SceneManager.h"
#include"../../../../Camera/PlayerCamera/PlayerCamera.h"


void PlayerState_SpecialAttack1::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("ChargeAttack0");
	m_player->GetAnimator()->SetAnimation(anime, 0.25f, false);
	PlayerStateBase::StateStart();

	SceneManager::Instance().GetObjectWeakPtr(m_effect);

	// カメラの位置を変更
	if (auto camera = m_player->GetPlayerCamera().lock(); camera)
	{
		camera->SetTargetLookAt({ 0.f,1.0f,-1.5f });
	}

	// アニメーション再生速度を変更
	m_player->SetAnimeSpeed(20.0f);
}

void PlayerState_SpecialAttack1::StateUpdate()
{
	Math::Vector3 moveDir = m_player->GetMovement();

	// 攻撃中の移動方向で回転を更新
	if (m_player->GetMovement() != Math::Vector3::Zero)
	{
		moveDir.y = 0.0f;
		moveDir.Normalize();
		m_player->UpdateQuaternionDirect(moveDir);
	}

	if (moveDir != Math::Vector3::Zero)
	{
		moveDir.Normalize();
		if (auto camera = m_player->GetPlayerCamera().lock(); camera)
		{
			const float yawRad = std::atan2(-moveDir.x, -moveDir.z);
			const float yawDeg = DirectX::XMConvertToDegrees(yawRad);

			// カメラをプレイヤーの後ろに回す
			camera->SetTargetRotation({ 0.0f, yawDeg , 0.0f });
			//camera->SetRotationSmooth(20.0f);
			//camera->SetDistanceSmooth(2.0f);
		}
	}

	m_player->SetIsMoving(m_attackDirection);

	UpdateKatanaPos();

	// 移動を止める
	m_player->SetIsMoving(Math::Vector3::Zero);

	if(auto effect = m_effect.lock())
	{
		effect->SetPlayEffect(true);
	}

	if (m_player->GetAnimator()->IsAnimationEnd())
	{
		auto state = std::make_shared<PlayerState_SheathKatana>();
		m_player->ChangeState(state);
		return;
	}
}

void PlayerState_SpecialAttack1::StateEnd()
{
	PlayerStateBase::StateEnd();

	if (auto effect = m_effect.lock())
	{
		effect->SetPlayEffect(false);
	}
}
