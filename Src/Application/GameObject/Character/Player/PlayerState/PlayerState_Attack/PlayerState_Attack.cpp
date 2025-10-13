#include "PlayerState_Attack.h"
#include"../../../../../main.h"
#include"../PlayerState_Attack1/PlayerState_Attack1.h"
#include"../PlayerState_Sheathing-of-Katana/PlayerState_Sheathing-of-Katana.h"
#include"../PlayerState_Run/PlayerState_Run.h"
#include"../../../../Weapon/Katana/Katana.h"
#include"../../../../Weapon/WeaponKatanaScabbard/WeaponKatanaScabbard.h"
#include"../../../../../Scene/SceneManager.h"
#include"../../../../Effect/MeshEffect/AttackEffect/AttackEffect.h"
#include"../../../../Effect/TrailEffect/TrailEffect.h"
#include"../../../../Effect/EffekseerEffect/SwordFlash/SwordFlash.h"

#include"../PlayerState_BackWordAvoid/PlayerState_BackWordAvoid.h"
#include"../PlayerState_FowardAvoid/PlayerState_FowardAvoid.h"

void PlayerState_Attack::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("newAttack1");
	m_player->GetAnimator()->SetAnimation(anime, 0.25f, false);

	PlayerStateBase::StateStart();

	m_attackParam = m_player->GetPlayerConfig().GetAttack2Param();
	m_attackParam.m_dashTimer = 0.0f;

	// 当たり判定リセット
	m_player->ResetAttackCollision();

	// 攻撃時はtrueにする
	if (auto katana = m_player->GetKatana().lock(); katana)
	{
		katana->SetNowAttackState(true);
	}

	m_LButtonkeyInput = false;		// 次段コンボ予約フラグ初期化
	m_time = 0.0f;					// 当たり判定用

	SceneManager::Instance().GetObjectWeakPtr(m_slashEffect);
	m_player->SetAnimeSpeed(70.0f);
}

void PlayerState_Attack::StateUpdate()
{

	// アニメーション時間のデバッグ表示
	{
		m_animeTime = m_player->GetAnimator()->GetPlayProgress();

		m_maxAnimeTime = m_player->GetAnimator()->GetMaxAnimationTime();

		if (m_animeTime > m_maxAnimeTime)
		{
			KdDebugGUI::Instance().AddLog(U8("Attackアニメ時間: %f"), m_animeTime);
			KdDebugGUI::Instance().AddLog("\n");
		}
	}

	if (m_player->GetEnemy().lock())
	{
		m_player->GetEnemy().lock()->GetPos();
	}

	float deltaTime = Application::Instance().GetDeltaTime();
	m_time += deltaTime;

	// 当たり判定有効時間: 最初の0.5秒のみ
	m_player->UpdateAttackCollision(3.0f, 3.0f, 1, m_maxAnimeTime, { 0.2f, 0.0f }, 0.3f);


	// 攻撃中の移動方向で回転を更新
	if (m_player->GetMovement() != Math::Vector3::Zero)
	{
		Math::Vector3 moveDir = m_player->GetMovement();
		moveDir.y = 0.0f;
		moveDir.Normalize();
		m_player->UpdateQuaternionDirect(moveDir);
	}

	if (KeyboardManager::GetInstance().IsKeyPressed('W') && KeyboardManager::GetInstance().IsKeyJustPressed(VK_RBUTTON))
	{
		auto sheath = std::make_shared<PlayerState_BackWordAvoid>();
		m_player->ChangeState(sheath);
		return;
	}

	if (KeyboardManager::GetInstance().IsKeyJustPressed(VK_RBUTTON))
	{
		auto sheath = std::make_shared<PlayerState_ForwardAvoid>();
		m_player->ChangeState(sheath);
		return;
	}


	if (KeyboardManager::GetInstance().IsKeyJustPressed(VK_LBUTTON))
	{
		m_LButtonkeyInput = true;
	}

	// 先行ダッシュ処理
	if (m_attackParam.m_dashTimer < 0.2f)
	{
		const float dashSpeed = 0.7f;
		m_player->SetIsMoving(m_attackDirection * dashSpeed);
		m_attackParam.m_dashTimer += deltaTime;
	}
	else
	{
		// エフェクト再生・移動停止
		if (auto effect = m_slashEffect.lock(); effect)
		{
			effect->SetPlayEffect(true);
		}
		m_player->SetIsMoving(Math::Vector3::Zero);

		// コンボ受付
		if (m_LButtonkeyInput)
		{
			// 70%以降で受付
			if (m_animeTime < 0.7f) return;
			auto next = std::make_shared<PlayerState_Attack1>();
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

	PlayerStateBase::StateUpdate();

	// カタナ関連
	auto katana = m_player->GetKatana().lock();
	if (!katana) return;

	katana->SetShowTrail(true);
	UpdateKatanaPos();


}

void PlayerState_Attack::StateEnd()
{
	PlayerStateBase::StateEnd();

	if (auto swordEffect = m_slashEffect.lock(); swordEffect)
	{
		swordEffect->SetPlayEffect(false);
		swordEffect->StopEffect();
	}
}