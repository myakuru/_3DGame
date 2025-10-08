#include "PlayerState.h"
#include"../../../Weapon/Katana/Katana.h"
#include"../../../Weapon/WeaponKatanaScabbard/WeaponKatanaScabbard.h"
#include"../PlayerState/PlayerState_SpecialAttack/PlayerState_SpecialAttack.h"
#include"../PlayerState/PlayerState_FowardAvoidFast/PlayerState_FowardAvoidFast.h"
#include"../PlayerState/PlayerState_BackWordAvoid/PlayerState_BackWordAvoid.h"


void PlayerStateBase::StateStart()
{
	// 敵が複数いる場合は最も近い敵の方向ベクトルを計算
	std::shared_ptr<Enemy> nearestEnemy;

	// 最小距離の初期値を大きな値に設定
	float minDistSq = std::numeric_limits<float>::max();

	// 敵リストをループして最も近い敵を見つける
	for (const auto& weakEnemy : m_player->GetEnemies())
	{
		// 一時的にシェアポインタを取得
		if (auto enemy = weakEnemy.lock())
		{
			// 敵の位置を取得して距離を計算
			Math::Vector3 enemyPos = enemy->GetPos();

			// プレイヤーとの距離の二乗を計算（平方根を取らないことで計算コストを削減）
			float distSq = (enemyPos - m_player->GetPos()).LengthSquared();

			// 最小距離を更新
			if (distSq < minDistSq)
			{
				// 最も近い敵を更新
				minDistSq = distSq;
				nearestEnemy = enemy;
			}
		}
	}

	if (nearestEnemy)
	{
		Math::Vector3 enemyPos = nearestEnemy->GetPos();
		m_attackDirection = enemyPos - m_player->GetPos();
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

	// デフォルトは刀を左手に持たないようにする。
	if (auto katana = m_player->GetKatana().lock(); katana)
	{
		katana->SetNowAttackState(false);
	}

	m_isKeyPressing = false;
	m_time = 0.0f;
	m_animeTime = 0.0f;
	m_maxAnimeTime = 0.0f;
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
	auto rightHandNode = m_player->GetModelWork()->FindWorkNode("Katana");
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
