#include "PlayerState_Attack1.h"
#include"../PlayerState_Attack2/PlayerState_Attack2.h"
#include"../PlayerState_Idle/PlayerState_Idle.h"
#include"../PlayerState_Run/PlayerState_Run.h"
#include"../../../../../main.h"
#include"../../../../Weapon/Katana/Katana.h"

void PlayerState_Attack1::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("Attack1");
	m_player->GetAnimator()->AnimationBlend(anime, 10.0f, false);
	m_player->AnimeSetFlg() = true;

	// 攻撃開始時に直前の移動方向を保存
	m_attackDirection = m_player->GetLastMoveDirection();

	// 攻撃開始時に向きを合わせる
	if (m_attackDirection != Math::Vector3::Zero)
	{
		m_player->UpdateQuaternion(m_attackDirection);
	}

	m_attackParam = m_player->GetPlayerConfig().GetAttack1Param();

	m_attackParam.m_dashTimer = 0.0f;
}

void PlayerState_Attack1::StateUpdate()
{
	m_player->SetAnimeSpeed(120.0f);

	if (KeyboardManager::GetInstance().IsKeyJustPressed(VK_LBUTTON))
	{
		auto attack1state = std::make_shared<PlayerState_Attack2>();
		m_player->ChangeState(attack1state);
		return;
	}

	if (m_player->GetAnimator()->IsAnimationEnd())
	{
		auto idleState = std::make_shared<PlayerState_Idle>();
		m_player->ChangeState(idleState);
		return;
	}
	else if (m_player->GetMoving() && m_player->GetAnimator()->IsAnimationEnd())
	{
		auto idleState = std::make_shared<PlayerState_Run>();
		m_player->ChangeState(idleState);
		return;
	}

	if (!m_flag)
	{
		// エフェクトの表示位置（前方0.5f）
		Math::Vector3 effectPos = m_player->GetPosition() + m_attackDirection * 3.0f;

		// プレイヤーの回転行列
		Math::Matrix rotationMat = Math::Matrix::CreateFromQuaternion(m_player->GetRotation());

		// エフェクトのワールド行列（回転＋位置）
		Math::Matrix effectWorld = rotationMat * Math::Matrix::CreateTranslation(effectPos);

		// Effekseerエフェクト再生
		auto effect = KdEffekseerManager::GetInstance().Play("Attack.efkefc", { effectPos }, 1.0f, 300.0f, false);
		if (auto spEffect = effect.lock())
		{
			KdEffekseerManager::GetInstance().SetWorldMatrix(spEffect->GetHandle(), effectWorld);
		}
		m_flag = true;
	}

	UpdateKatanaPos();

	float deltaTime = Application::Instance().GetDeltaTime();
	if (m_attackParam.m_dashTimer < 0.2f)
	{
		float dashSpeed = 0.4f;
		m_player->SetIsMoving(m_attackDirection * dashSpeed);
		m_attackParam.m_dashTimer += deltaTime;
	}
	else
	{
		// 移動を止める
		m_player->SetIsMoving(Math::Vector3::Zero);
	}

}

void PlayerState_Attack1::StateEnd()
{
	// カタナの取得
	auto katana = m_player->GetKatana().lock();

	if (!katana) return;

	// カタナの行列をリセット
	katana->SetHandKatanaMatrix(Math::Matrix::Identity);
}

void PlayerState_Attack1::UpdateKatanaPos()
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
