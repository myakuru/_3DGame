#include "PlayerState_Hit.h"
#include"../PlayerState_Idle/PlayerState_Idle.h"
#include"../PlayerState_Attack/PlayerState_Attack.h"

#include"../../../../Weapon/Katana/Katana.h"

void PlayerState_Hit::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("Hit");
	m_player->GetAnimator()->AnimationBlend(anime, 10.0f, false);
	m_player->AnimeSetFlg() = true;

	m_player->m_onceEffect = false;

	// 攻撃開始時に直前の移動方向を保存
	m_attackDirection = m_player->GetLastMoveDirection();

	// 攻撃開始時に向きを合わせる
	if (m_attackDirection != Math::Vector3::Zero)
	{
		m_player->UpdateQuaternion(m_attackDirection);
	}
}

void PlayerState_Hit::StateUpdate()
{
	m_player->SetAnimeSpeed(120.0f);

	if (m_player->GetAnimator()->IsAnimationEnd())
	{
		auto idleState = std::make_shared<PlayerState_Idle>();
		m_player->ChangeState(idleState);
		return;
	}

	UpdateKatana();

	if (KeyboardManager::GetInstance().IsKeyJustPressed(VK_LBUTTON))
	{
		auto attack1state = std::make_shared<PlayerState_Attack>();
		m_player->ChangeState(attack1state);
		return;
	}

	m_player->SetIsMoving(Math::Vector3::Zero);
}

void PlayerState_Hit::StateEnd()
{
	// カタナの取得
	auto katana = m_player->GetKatana().lock();

	if (!katana) return;
	katana->SetHandKatanaMatrix(Math::Matrix::Identity);
}

void PlayerState_Hit::UpdateKatana()
{
	// 手のワークノードを取得
	auto handNode = m_player->GetModelWork()->FindWorkNode("VSB_10");

	if (!handNode) return;

	// カタナの取得
	auto katana = m_player->GetKatana().lock();

	if (!katana) return;

	// プレイヤーに追尾する刀にするためにワークノードとプレイヤーのワールド変換を設定
	katana->SetHandKatanaMatrix(handNode->m_worldTransform);
}
