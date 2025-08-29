#include "PlayerState_ChaegeAttack2.h"
#include"../../../../../main.h"
#include"../PlayerState_Attack1/PlayerState_Attack1.h"
#include"../PlayerState_Idle/PlayerState_Idle.h"
#include"../PlayerState_Run/PlayerState_Run.h"
#include"../../../../Weapon/Katana/Katana.h"

void PlayerState_ChaegeAttack2::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("ChargeAttack");
	m_player->GetAnimator()->AnimationBlend(anime, 10.0f, false);
	m_player->AnimeSetFlg() = true;

	// 敵の方向ベクトルを計算
	auto enemy = m_player->GetEnemy().lock();
	if (enemy)
	{
		Math::Vector3 playerPos = m_player->GetPosition();
		Math::Vector3 enemyPos = enemy->GetPos();
		m_attackDirection = enemyPos - playerPos;
		m_attackDirection.y = 0.0f;
		if (m_attackDirection != Math::Vector3::Zero)
		{
			m_attackDirection.Normalize();
			m_player->UpdateQuaternionDirect(m_attackDirection); // カメラ回転を掛けない
		}
	}
	else
	{
		// 敵がいない場合は直前の移動方向
		m_attackDirection = m_player->GetLastMoveDirection();
		if (m_attackDirection != Math::Vector3::Zero)
		{
			m_player->UpdateQuaternionDirect(m_attackDirection);
		}
	}

	m_attackParam = m_player->GetPlayerConfig().GetAttack2Param();
	m_attackParam.m_dashTimer = 0.0f;

	m_player->m_onceEffect = false;
}

void PlayerState_ChaegeAttack2::StateUpdate()
{
}

void PlayerState_ChaegeAttack2::StateEnd()
{
	// カタナの取得
	auto katana = m_player->GetKatana().lock();

	if (!katana) return;
	katana->SetHandKatanaMatrix(Math::Matrix::Identity);
}

void PlayerState_ChaegeAttack2::UpdateKatanaPos()
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