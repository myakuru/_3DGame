#include "PlayerState_SpecialAttack1.h"
#include"../PlayerState_Sheathing-of-Katana/PlayerState_Sheathing-of-Katana.h"
#include"../../../../../main.h"
#include"../../../../Effect/EffekseerEffect/SpecialAttack1/SpecialAttack1.h"
#include"../../../../../Scene/SceneManager.h"

void PlayerState_SpecialAttack1::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("ChargeAttack0");
	m_player->GetAnimator()->SetAnimation(anime, 0.25f, false);
	PlayerStateBase::StateStart();

	SceneManager::Instance().GetObjectWeakPtr(m_effect);
}

void PlayerState_SpecialAttack1::StateUpdate()
{
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

	// 移動を止める
	m_player->SetIsMoving(Math::Vector3::Zero);

	if(auto effect = m_effect.lock())
	{
		effect->SetPlayEffect(true);

		// エフェクトが再生し終わったら鞘に納めるステートへ
		if (effect->IsEffectEnd())
		{
			auto sheath = std::make_shared<PlayerState_SheathKatana>();
			m_player->ChangeState(sheath);
			return;
		}
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
