#include"PlayerState_ChargeAttack.h"
#include"../../../../../main.h"

#include"../PlayerState_Idle/PlayerState_Idle.h"
#include"../../../../Weapon/Katana/Katana.h"
#include"../../../../../Scene/SceneManager.h"

void PlayerState_ChargeAttack::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("ChargeAttack");
	m_player->GetAnimator()->AnimationBlend(anime, 10.0f,false);
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

	m_chargeTime = 0.0f;
	m_isCharging = false;
	m_isCharged = false;

	m_flag = false; // エフェクトフラグ

}

void PlayerState_ChargeAttack::StateUpdate()
{
	m_player->SetAnimeSpeed(180.0f);

	float deltaTime = Application::Instance().GetDeltaTime();

	if (m_player->GetAnimator()->IsAnimationEnd())
	{
		auto idleState = std::make_shared<PlayerState_Idle>();
		m_player->ChangeState(idleState);
		return;
	}

	UpdateKatana();

	float time = m_player->GetAnimator()->GetTime();

	if (time >= 0.0f && !m_isCharging)
	{
		KdEffekseerManager::GetInstance().Play("ZZZshine.efkefc", { m_player->GetPosition().x,m_player->GetPosition().y + 0.5f,m_player->GetPosition().z }, 0.2f, 100.0f, false);
		m_isCharging = true;
	}

	// 攻撃中の移動方向で回転を更新
	if (m_player->GetMovement() != Math::Vector3::Zero)
	{
		Math::Vector3 moveDir = m_player->GetMovement();
		moveDir.y = 0.0f;
		moveDir.Normalize();
		m_player->UpdateQuaternionDirect(moveDir);
	}

	if (time >= 60.5f && !m_isCharged)
	{
		SceneManager::Instance().SetEffectActive(true);

		if (m_time <= 1.0 / 2)
		{
			for (int i = 0; i < 5; ++i)
			{
				m_player->UpdateChargeAttack();
			}
			m_time = 0.0f;
		}

		float deltaTime = Application::Instance().GetDeltaTime();

		if (m_chargeTime < 0.3f)
		{
			float dashSpeed = 0.0f;
			m_player->SetIsMoving(m_attackDirection * dashSpeed);
			m_chargeTime += deltaTime;

			if (!m_flag)
			{
				m_forwardEffect->Init();
				SceneManager::Instance().AddObject(m_forwardEffect);
				m_flag = true;
			}

		}
		else
		{
			// 移動を止める
			m_player->SetIsMoving(Math::Vector3::Zero);
		}
	}

}

void PlayerState_ChargeAttack::StateEnd()
{
	// カタナの取得
	auto katana = m_player->GetKatana().lock();

	if (!katana) return;
	katana->SetHandKatanaMatrix(Math::Matrix::Identity);
}

void PlayerState_ChargeAttack::UpdateKatana()
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