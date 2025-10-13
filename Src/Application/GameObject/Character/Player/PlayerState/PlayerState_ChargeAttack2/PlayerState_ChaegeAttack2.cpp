#include "PlayerState_ChaegeAttack2.h"
#include"../../../../../main.h"
#include"../PlayerState_Attack1/PlayerState_Attack1.h"
#include"../PlayerState_Idle/PlayerState_Idle.h"
#include"../PlayerState_Run/PlayerState_Run.h"
#include"../../../../Weapon/Katana/Katana.h"
#include"../PlayerState_Sheathing-of-Katana/PlayerState_Sheathing-of-Katana.h"
#include"../../../../../Scene/SceneManager.h"
#include"../../../../Effect/EffekseerEffect/EffectPlay/EffectPlay.h"
#include"../../../../Effect/EffekseerEffect/SmokeEffect/SmokeEffect.h"
#include"../../../../Effect/EffekseerEffect/ShineEffect/ShineEffect.h"
#include"../../../../Camera/PlayerCamera/PlayerCamera.h"

void PlayerState_ChaegeAttack2::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("ChargeAttack1");
	m_player->GetAnimator()->SetAnimation(anime, 0.25f, false);
	PlayerStateBase::StateStart();

	SceneManager::Instance().GetObjectWeakPtr(m_effect);
	SceneManager::Instance().GetObjectWeakPtr(m_smokeEffect);
	SceneManager::Instance().GetObjectWeakPtr(m_shineEffect);

	m_player->m_onceEffect = false;
}

void PlayerState_ChaegeAttack2::StateUpdate()
{
	
	auto smokeEffect = m_smokeEffect.lock();
	auto shineEffect = m_shineEffect.lock();

	if (auto effect = m_effect.lock(); effect && smokeEffect && shineEffect)
	{
		// スラッシュエフェクトの再生
		effect->SetPlayEffect(true);
		// スモークエフェクトの再生
		smokeEffect->SetPlayEffect(true);
		// キラキラエフェクトの再生
		shineEffect->SetPlayEffect(true);
	}

	float deltaTime = Application::Instance().GetDeltaTime();

	m_chargeAttackTimer += deltaTime;

	// 0.3秒後に画面の揺れとぼかし処理を実行
	if (m_chargeAttackTimer >= 0.1f && m_chargeAttackTimer <= 0.2f)
	{
		if (auto camera = m_player->GetPlayerCamera().lock(); camera)
		{
			camera->StartShake({ 0.0f,0.5f }, 0.01f);
		}
	}

	// アニメーション速度を変更
	m_player->SetAnimeSpeed(60.0f);

	// アニメーションが終了したら納刀する状態へ
	if (m_player->GetAnimator()->IsAnimationEnd())
	{
		auto state = std::make_shared<PlayerState_SheathKatana>();
		m_player->ChangeState(state);
		return;
	}

	PlayerStateBase::StateUpdate();

	// 刀は鞘の中にある状態
	UpdateUnsheathed();

	// 攻撃中の移動方向で回転を更新
	if (m_player->GetMovement() != Math::Vector3::Zero)
	{
		Math::Vector3 moveDir = m_player->GetMovement();
		moveDir.y = 0.0f;
		moveDir.Normalize();
		m_player->UpdateQuaternionDirect(moveDir);
	}

	// 敵の方向を向く
	m_player->SetIsMoving(m_attackDirection);

	// 移動を止める
	m_player->SetIsMoving(Math::Vector3::Zero);
}

void PlayerState_ChaegeAttack2::StateEnd()
{
	PlayerStateBase::StateEnd();

	auto smokeEffect = m_smokeEffect.lock();
	auto shineEffect = m_shineEffect.lock();

	if (auto effect = m_effect.lock(); effect && smokeEffect && shineEffect)
	{
		// スラッシュエフェクトの再生
		effect->SetPlayEffect(false);
		// スモークエフェクトの再生
		smokeEffect->SetPlayEffect(false);
		// キラキラエフェクトの再生
		shineEffect->SetPlayEffect(false);
	}


}