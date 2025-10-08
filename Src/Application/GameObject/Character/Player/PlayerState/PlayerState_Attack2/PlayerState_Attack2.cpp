#include "PlayerState_Attack2.h"
#include"../PlayerState_Idle/PlayerState_Idle.h"
#include"../PlayerState_Sheathing-of-Katana/PlayerState_Sheathing-of-Katana.h"
#include"../../../../../main.h"
#include"../../../../Weapon/Katana/Katana.h"

#include"../PlayerState_Attack3/PlayerState_Attack3.h"
#include"../PlayerState_BackWordAvoid/PlayerState_BackWordAvoid.h"

#include"../../../../../Scene/SceneManager.h"
#include"../../../../Effect/EffekseerEffect/Rotation/Rotation.h"

void PlayerState_Attack2::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("newAttack3");
	m_player->GetAnimator()->SetAnimation(anime, 0.25f, false);
	PlayerStateBase::StateStart();

	m_attackParam = m_player->GetPlayerConfig().GetAttackParam();

	m_player->m_onceEffect = false;

	// 攻撃時はtrueにする
	if (auto katana = m_player->GetKatana().lock(); katana)
	{
		katana->SetNowAttackState(true);
	}

	m_LButtonkeyInput = false;				// 次段コンボ予約フラグ初期化

	SceneManager::Instance().GetObjectWeakPtr(m_slashEffect);

	m_player->SetAnimeSpeed(80.0f);

}

void PlayerState_Attack2::StateUpdate()
{
	// アニメーション時間のデバッグ表示
	{
		m_animeTime = m_player->GetAnimator()->GetPlayProgress();

		m_maxAnimeTime = m_player->GetAnimator()->GetMaxAnimationTime();

		if (m_animeTime > m_maxAnimeTime)
		{
			KdDebugGUI::Instance().AddLog(U8("Attack2アニメ時間: %f"), m_animeTime);
			KdDebugGUI::Instance().AddLog("\n");
		}
		else
		{
			m_animeTime = m_maxAnimeTime;
		}
	}

	float deltaTime = Application::Instance().GetDeltaTime();

	m_time += deltaTime;

	// 0.5秒間当たり判定有効
	if (m_time <= 1.0 / 2)
	{
		m_player->UpdateAttack();
		m_time = 0.0f;
	}

	Math::Vector3 moveDir = m_player->GetMovement();

	// 攻撃中の移動方向で回転を更新
	if (m_player->GetMovement() != Math::Vector3::Zero)
	{
		moveDir.y = 0.0f;
		moveDir.Normalize();
		m_player->UpdateQuaternionDirect(moveDir);
	}

	// コンボ受付
	{

		if (KeyboardManager::GetInstance().IsKeyJustPressed(VK_LBUTTON))
		{
			m_LButtonkeyInput = true;
		}

		// アニメ終了時の遷移
		if (m_player->GetAnimator()->IsAnimationEnd())
		{
			auto sheath = std::make_shared<PlayerState_SheathKatana>();
			m_player->ChangeState(sheath);
			return;
		}
	}

	UpdateKatanaPos();

	if (m_attackParam.m_dashTimer < 0.2f)
	{
		float dashSpeed = 0.5f;
		m_player->SetIsMoving(m_attackDirection * dashSpeed);
		m_attackParam.m_dashTimer += deltaTime;
	}
	else
	{
		if (auto effect = m_slashEffect.lock(); effect)
		{
			effect->SetPlayEffect(true);
		}

		// 移動を止める
		m_player->SetIsMoving(Math::Vector3::Zero);

		// コンボ受付
		if (m_LButtonkeyInput)
		{
			// 80%以降で受付
			if (m_animeTime < 0.4f) return;
			auto next = std::make_shared<PlayerState_Attack3>();
			m_player->ChangeState(next);
			return;
		}
		else if (m_player->GetAnimator()->IsAnimationEnd())
		{
			auto sheath = std::make_shared<PlayerState_SheathKatana>();
			m_player->ChangeState(sheath);
			return;
		}
	}
}

void PlayerState_Attack2::StateEnd()
{
	PlayerStateBase::StateEnd();

	if (auto effect = m_slashEffect.lock(); effect)
	{
		effect->SetPlayEffect(false);
	}
}
