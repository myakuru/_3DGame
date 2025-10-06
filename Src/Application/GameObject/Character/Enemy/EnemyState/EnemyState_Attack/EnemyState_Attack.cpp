#include "EnemyState_Attack.h"
#include"../EnemyState_Attack1/EnemyState_Attack1.h"
#include"../../../../../Scene/SceneManager.h"
#include"../../../../../main.h"
#include"../../../../Effect/EffekseerEffect/EnemyShineBlue/EnemyShineBlue.h"

void EnemyState_Attack::StateStart()
{
	EnemyStateBase::StateStart();

	auto anime = m_enemy->GetAnimeModel()->GetAnimation("Attack");
	m_enemy->GetAnimator()->SetAnimation(anime, 0.25f, false);

	// エフェクトの再生
	SceneManager::Instance().GetObjectWeakPtr(m_shineEffectBlue);

	if (auto effect = m_shineEffectBlue.lock(); effect)
	{
		effect->SetPlayEffect(true);
	}

	m_time = 0.0f;
}

void EnemyState_Attack::StateUpdate()
{
	float deltaTime = Application::Instance().GetDeltaTime();

	m_time += deltaTime;

	// 0.5秒間当たり判定有効
	if (!m_hasHitPlayer && m_time >= 0.1f && m_time <= 0.5f)
	{
		m_enemy->UpdateAttack();
		m_enemy->SetOnceEffect(false);
		m_hasHitPlayer = true;
	}

	if (m_enemy->GetAnimator()->IsAnimationEnd())
	{
		auto attack1 = std::make_shared<EnemyState_Attack1>();
		m_enemy->ChangeState(attack1);
		return;
	}

	if (m_time < 0.2f)
	{
		const float dashSpeed = 2.0f;
		m_enemy->SetIsMoving(m_attackDirection * dashSpeed);
	}
	else
	{
		m_enemy->SetIsMoving(Math::Vector3::Zero);
	}

}

void EnemyState_Attack::StateEnd()
{
	if (auto effect = m_shineEffectBlue.lock(); effect)
	{
		effect->SetPlayEffect(false);
	}
}
