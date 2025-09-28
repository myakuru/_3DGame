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

void PlayerState_Attack::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("Attack");
	m_player->GetAnimator()->SetAnimation(anime, 0.3f, false);
	

	PlayerStateBase::StateStart();

	m_attackParam = m_player->GetPlayerConfig().GetAttack2Param();
	m_attackParam.m_dashTimer = 0.0f;

	m_player->m_onceEffect = false;

	// 攻撃時はtrueにする
	if (auto katana = m_player->GetKatana().lock(); katana)
	{
		katana->SetNowAttackState(true);
	}

	m_keyInput = false;          // 次段コンボ予約フラグ初期化
	m_time = 0.0f;               // 当たり判定用
}

void PlayerState_Attack::StateUpdate()
{
	SceneManager::Instance().GetObjectWeakPtr(m_slashEffect);

	auto swordEffect = m_slashEffect.lock();
	if (!swordEffect) return;

	if (m_player->GetEnemy().lock())
	{
		m_player->GetEnemy().lock()->GetPos();
	}

	float deltaTime = Application::Instance().GetDeltaTime();
	m_time += deltaTime;

	// 当たり判定有効時間: 最初の0.5秒のみ
	if (m_time <= 0.5f)
	{
		m_player->UpdateAttack();
	}

	float animTime = m_player->GetAnimator()->GetTime();


	if (KeyboardManager::GetInstance().IsKeyJustPressed(VK_LBUTTON))
	{
		m_keyInput = true;
	}

	// アニメ速度制御：予約があれば加速
	if (m_keyInput)
	{
		m_player->SetAnimeSpeed(150.0f);
	}
	else
	{
		m_player->SetAnimeSpeed(80.0f);
	}

	// アニメ終了時の遷移
	if (m_player->GetAnimator()->IsAnimationEnd())
	{
		if (m_keyInput)
		{
			auto next = std::make_shared<PlayerState_Attack1>();
			m_player->ChangeState(next);
		}
		else
		{
			auto sheath = std::make_shared<PlayerState_SheathKatana>();
			m_player->ChangeState(sheath);
		}
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
		swordEffect->SetPlayEffect(true);
		swordEffect->IsEffectPlaying();
		m_player->SetIsMoving(Math::Vector3::Zero);
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
	}
}