#include "PlayerState.h"
#include"../../../Weapon/Katana/Katana.h"
#include"../../../Weapon/WeaponKatanaScabbard/WeaponKatanaScabbard.h"

void PlayerStateBase::StateStart()
{
	// 敵の方向ベクトルを計算
	auto enemy = m_player->GetEnemy().lock();
	if (enemy)
	{
		Math::Vector3 playerPos = m_player->GetPos();
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

	// カタナの取得
	auto katana = m_player->GetKatana().lock();

	if (!katana) return;

	katana->SetNowAttackState(false);

}

void PlayerStateBase::StateUpdate()
{
}

void PlayerStateBase::StateEnd()
{
	// カタナの取得
	auto katana = m_player->GetKatana().lock();

	if (!katana) return;
	katana->SetHandKatanaMatrix(Math::Matrix::Identity);
	katana->SetNowAttackState(false);
}

void PlayerStateBase::UpdateKatanaPos()
{
	// 右手のワークノードを取得
	auto rightHandNode = m_player->GetModelWork()->FindWorkNode("VSB_24");
	// 左手のワークノードを取得
	auto leftHandNode = m_player->GetModelWork()->FindWorkNode("VSB_9");

	if (!rightHandNode) return;
	if (!leftHandNode) return;

	// カタナの取得
	auto katana = m_player->GetKatana().lock();
	// 鞘の取得
	auto saya = m_player->GetScabbard().lock();

	if (!katana) return;
	if (!saya) return;

	// プレイヤーに追尾する刀にするためにワークノードとプレイヤーのワールド変換を設定
	katana->SetHandKatanaMatrix(rightHandNode->m_worldTransform);
	saya->SetHandKatanaMatrix(leftHandNode->m_worldTransform);
}

void PlayerStateBase::UpdateUnsheathed()
{
	// 左手のワークノードを取得
	auto leftHandNode = m_player->GetModelWork()->FindWorkNode("VSB_9");

	if (!leftHandNode) return;

	// カタナの取得
	auto katana = m_player->GetKatana().lock();
	// 鞘の取得
	auto saya = m_player->GetScabbard().lock();

	if (!katana) return;
	if (!saya) return;

	// プレイヤーに追尾する刀にするためにワークノードとプレイヤーのワールド変換を設定
	katana->SetHandKatanaMatrix(leftHandNode->m_worldTransform);
	saya->SetHandKatanaMatrix(leftHandNode->m_worldTransform);
}