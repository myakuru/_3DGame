#include "PlayerState_FowardAvoid.h"
#include"../PlayerState_Idle/PlayerState_Idle.h"

#include"../../../../Weapon/Katana/Katana.h"

void PlayerState_ForwardAvoid::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("AvoidFoward");
	m_player->GetAnimator()->AnimationBlend(anime, 10.0f, false);
	m_player->AnimeSetFlg() = true;

	// 攻撃開始時に直前の移動方向を保存
	m_attackDirection = m_player->GetLastMoveDirection();

	// 攻撃開始時に向きを合わせる
	if (m_attackDirection != Math::Vector3::Zero)
	{
		m_player->UpdateQuaternion(m_attackDirection);
	}
}

void PlayerState_ForwardAvoid::StateUpdate()
{
	m_player->SetAnimeSpeed(120.0f);

	if (m_player->GetAnimator()->IsAnimationEnd())
	{
		auto idleState = std::make_shared<PlayerState_Idle>();
		m_player->ChangeState(idleState);
		return;
	}

	UpdateKatana();

	float dashSpeed = -0.5f;

	m_player->SetIsMoving(m_attackDirection * dashSpeed);
}

void PlayerState_ForwardAvoid::StateEnd()
{
	// カタナの取得
	auto katana = m_player->GetKatana().lock();

	if (!katana) return;
	katana->SetHandKatanaMatrix(Math::Matrix::Identity);
}

void PlayerState_ForwardAvoid::UpdateKatana()
{
	auto hipNode = m_player->GetModelWork()->FindDataNode("Hips");

	if (!hipNode) return;

	// カタナの取得
	auto katana = m_player->GetKatana().lock();

	if (!katana) return;

	// プレイヤーに追尾する刀にするためにワークノードとプレイヤーのワールド変換を設定
	katana->SetKatanaMatrix(hipNode->m_worldTransform);
}
