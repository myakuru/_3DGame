#include "PlayerState.h"
#include"../../../Weapon/Katana/Katana.h"
#include"../../../Weapon/WeaponKatanaScabbard/WeaponKatanaScabbard.h"
#include"../PlayerState/PlayerState_SpecialAttack/PlayerState_SpecialAttack.h"
#include"../PlayerState/PlayerState_FowardAvoidFast/PlayerState_FowardAvoidFast.h"
#include"../PlayerState/PlayerState_BackWordAvoid/PlayerState_BackWordAvoid.h"
#include"../../BossEnemy/BossEnemy.h"
#include"../../../../Scene/SceneManager.h"


void PlayerStateBase::StateStart()
{
	// プレイヤー位置をキャッシュ（m_mWorld更新タイミングが気になるなら専用アクセサを用意）
	const Math::Vector3 playerPos = m_player->GetPos();

	std::shared_ptr<KdGameObject> nearestEnemy;
	Math::Vector3                  nearestEnemyPos = Math::Vector3::Zero;
	float                          minDistSq = std::numeric_limits<float>::max();

	for (const auto& weakEnemy : m_player->GetEnemyLike())
	{
		if (auto enemy = weakEnemy.lock())
		{
			if (enemy->IsExpired()) continue;

			// ボス敵だった場合
			if (enemy->GetTypeID() == BossEnemy::TypeID)
			{
				if (!SceneManager::Instance().IsBossAppear()) continue;
			}

			const Math::Vector3 enemyPos = enemy->GetPos();
			const float distSq = (enemyPos - playerPos).LengthSquared();

			if (distSq < minDistSq)
			{
				minDistSq = distSq;
				nearestEnemyPos = enemyPos;
				nearestEnemy = std::move(enemy);
			}
		}
	}

	if (nearestEnemy)
	{
		m_attackDirection = nearestEnemyPos - playerPos;
		m_attackDirection.y = 0.0f;

		if (m_attackDirection != Math::Vector3::Zero)
		{
			m_attackDirection.Normalize();
			m_player->UpdateQuaternionDirect(m_attackDirection); // カメラ回転なし
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

	// デフォルトは刀を左手に持たない
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
