#include "PlayerState_SpecialAttack.h"

#include"../../../../../main.h"
#include"../PlayerState_Sheathing-of-Katana/PlayerState_Sheathing-of-Katana.h"
#include"../../../../Camera/PlayerCamera/PlayerCamera.h"
#include"../../../../Weapon/Katana/Katana.h"
#include"../../../../../Scene/SceneManager.h"
#include"../../../../Effect/EffekseerEffect/SpecialAttack/SpecialAttack.h"

void PlayerState_SpecialAttack::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("PowerAttack");
	m_player->GetAnimator()->SetAnimation(anime, 0.25f, false);

	PlayerStateBase::StateStart();

	// 左手に刀をセットする
	if (auto katana = m_player->GetKatana().lock(); katana)
	{
		katana->SetNowAttackState(true);
	}

	SceneManager::Instance().GetObjectWeakPtr(m_specialAttackEffect);

}

void PlayerState_SpecialAttack::StateUpdate()
{
	// アニメーション再生速度を変更
	m_player->SetAnimeSpeed(60.0f);

	if (m_player->GetAnimator()->IsAnimationEnd())
	{
		auto sheathState = std::make_shared<PlayerState_SheathKatana>();
		m_player->ChangeState(sheathState);
		return;
	}

	// 攻撃中の移動方向で回転を更新
	if (m_player->GetMovement() != Math::Vector3::Zero)
	{
		Math::Vector3 moveDir = m_player->GetMovement();
		moveDir.y = 0.0f;
		moveDir.Normalize();
		m_player->UpdateQuaternionDirect(moveDir);
	}

	m_player->SetIsMoving(m_attackDirection);

	UpdateKatanaPos();

	float deltaTime = Application::Instance().GetDeltaTime();

	m_time += deltaTime;

	if (m_time >= 1.0f)
	{
		if (auto effect = m_specialAttackEffect.lock(); effect)
		{
			effect->SetPlayEffect(true);
		}
	}
	
	// 移動を止める
	m_player->SetIsMoving(Math::Vector3::Zero);
	

	// カタナ関連
	if (auto katana = m_player->GetKatana().lock(); katana)
	{
		// 刀のトレイルポリゴンの表示
		katana->SetShowTrail(true);
	}
}

void PlayerState_SpecialAttack::StateEnd()
{
	PlayerStateBase::StateEnd();

	if (auto camera = m_player->GetPlayerCamera().lock(); camera)
	{
		// カメラと、プレイヤーの距離を近づける（らーぷ）
		camera->SetTargetLookAt({ 0.0f,1.0f,-3.5f });
	}

	if (auto effect = m_specialAttackEffect.lock(); effect)
	{
		effect->SetPlayEffect(false);
	}
}
