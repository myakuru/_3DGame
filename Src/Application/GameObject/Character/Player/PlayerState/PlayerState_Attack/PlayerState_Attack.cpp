#include "PlayerState_Attack.h"
#include"../../../../../main.h"
#include"../PlayerState_Attack1/PlayerState_Attack1.h"
#include"../PlayerState_Sheathing-of-Katana/PlayerState_Sheathing-of-Katana.h"
#include"../PlayerState_Run/PlayerState_Run.h"
#include"../../../../Weapon/Katana/Katana.h"
#include"../../../../Weapon/WeaponKatanaScabbard/WeaponKatanaScabbard.h"
#include"../../../../../Scene/SceneManager.h"
#include"../../../../Effect/MeshEffect/AttackEffect/AttackEffect.h"

void PlayerState_Attack::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("Attack");
	m_player->GetAnimator()->AnimationBlend(anime, 0.1f, false);
	m_player->AnimeSetFlg() = true;

	PlayerStateBase::StateStart();

	m_attackParam = m_player->GetPlayerConfig().GetAttack2Param();
	m_attackParam.m_dashTimer = 0.0f;

	m_player->m_onceEffect = false;
	m_once = false;
}

void PlayerState_Attack::StateUpdate()
{
	m_player->GetEnemy().lock()->GetPos();

	float deltaTime = Application::Instance().GetDeltaTime();

	m_time += deltaTime;

	// 0.5秒間当たり判定有効
	if (m_time <= 1.0 / 2)
	{
		m_player->UpdateAttack();
		m_time = 0.0f;
	}

	float time = m_player->GetAnimator()->GetTime();

	m_player->SetAnimeSpeed(80.0f);

	if (m_player->GetAnimator()->IsAnimationEnd())
	{
		auto state = std::make_shared<PlayerState_SheathKatana>();
		m_player->ChangeState(state);
		return;
	}

	if (KeyboardManager::GetInstance().IsKeyJustPressed(VK_LBUTTON))
	{
		auto attack1state = std::make_shared<PlayerState_Attack1>();
		m_player->ChangeState(attack1state);
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

	if (m_attackParam.m_dashTimer < 0.2f)
	{
		float dashSpeed = 0.7f;
		m_player->SetIsMoving(m_attackDirection * dashSpeed);
		m_attackParam.m_dashTimer += deltaTime;
	}
	else
	{
		// 移動を止める
		m_player->SetIsMoving(Math::Vector3::Zero);
	}


	// カタナの取得
	auto katana = m_player->GetKatana().lock();

	if (!katana) return;

	if (time >= 10.0f)
	{
		katana->SetNowAttackState(true);
		UpdateKatanaPos();
	}
	else
	{
		//m_player->SetAnimeSpeed(0.01f);
		katana->SetNowAttackState(false);
		UpdateUnsheathed();
	}

	SceneManager::Instance().GetObjectWeakPtr(m_attackEffect);

	auto attackEffect = m_attackEffect.lock();

	if (!attackEffect) return;

	if (!m_once)
	{
		attackEffect->EffectReset();
		m_once = true;
	}
}

void PlayerState_Attack::StateEnd()
{
	PlayerStateBase::StateEnd();
	m_once = false;
}
