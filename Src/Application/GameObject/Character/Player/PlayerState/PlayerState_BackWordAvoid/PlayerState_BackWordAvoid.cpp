#include "PlayerState_BackWordAvoid.h"
#include"../../../../../main.h"
#include"../../../../Weapon/Katana/Katana.h"

#include"../PlayerState_Idle/PlayerState_Idle.h"
#include"../PlayerState_Run/PlayerState_Run.h"
#include"../../../../Weapon/WeaponKatanaScabbard/WeaponKatanaScabbard.h"

void PlayerState_BackWordAvoid::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("AvoidForward");
	m_player->GetAnimator()->SetAnimation(anime, 0.25f, false);

	// 攻撃開始時に直前の移動方向を保存
	m_attackDirection = m_player->GetLastMoveDirection();

	// 攻撃開始時に向きを合わせる
	if (m_attackDirection != Math::Vector3::Zero)
	{
		m_player->UpdateQuaternion(m_attackDirection);
	}

	m_player->SetAvoidFlg(true);
	m_player->SetAvoidStartTime(Application::Instance().GetDeltaTime()); // 現在の時間を記録

}

void PlayerState_BackWordAvoid::StateUpdate()
{
	m_player->SetAnimeSpeed(120.0f);

	if (m_player->GetAnimator()->IsAnimationEnd())
	{
		auto idleState = std::make_shared<PlayerState_Idle>();
		m_player->ChangeState(idleState);
		return;
	}

	PlayerStateBase::StateUpdate();

	UpdateKatanaPos();

	float dashSpeed = 1.0f;
	
	m_player->SetIsMoving(m_attackDirection * dashSpeed);

}

void PlayerState_BackWordAvoid::StateEnd()
{
	PlayerStateBase::StateEnd();

	m_player->SetAvoidFlg(false);
	m_player->SetAvoidStartTime(0.0f); // 現在の時間を記録
}
