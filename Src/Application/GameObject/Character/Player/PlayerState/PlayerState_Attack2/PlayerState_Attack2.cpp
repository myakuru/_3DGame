#include "PlayerState_Attack2.h"
#include"../PlayerState_Idle/PlayerState_Idle.h"
#include"../PlayerState_Run/PlayerState_Run.h"
#include"../../../../../main.h"
#include"../../../../Weapon/Katana/Katana.h"

void PlayerState_Attack2::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("Attack2");
	m_player->GetAnimator()->AnimationBlend(anime, 10.0f, false);
	m_player->AnimeSetFlg() = true;

	// 攻撃開始時に直前の移動方向を保存
	m_attackDirection = m_player->GetLastMoveDirection();

	// 攻撃開始時に向きを合わせる
	if (m_attackDirection != Math::Vector3::Zero)
	{
		m_player->UpdateQuaternion(m_attackDirection);
	}

	m_attackParam = m_player->GetPlayerConfig().GetAttackParam();

	m_flag = false; // 攻撃フラグ
}

void PlayerState_Attack2::StateUpdate()
{
	m_player->SetAnimeSpeed(120.0f);

	if (!m_flag)
	{
		// Effekseerエフェクト再生
		auto effect = KdEffekseerManager::GetInstance().Play("NA_slash_008.efkefc", {m_player->GetPos().x,m_player->GetPos().y + 1.0f,m_player->GetPos().z}, 0.1f, 100.0f, false);
		m_flag = true;
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

	UpdateKatanaPos();

	float deltaTime = Application::Instance().GetDeltaTime();
	if (m_attackParam.m_dashTimer < 0.2f)
	{
		float dashSpeed = 0.5f;
		m_player->SetIsMoving(m_attackDirection * dashSpeed);
		m_attackParam.m_dashTimer += deltaTime;
	}
	else
	{
		// 移動を止める
		m_player->SetIsMoving(Math::Vector3::Zero);
	}
}

void PlayerState_Attack2::StateEnd()
{
	m_player->AnimeSetFlg() = false;
	// カタナの取得
	auto katana = m_player->GetKatana().lock();

	if (!katana) return;

	// カタナの行列をリセット
	katana->SetHandKatanaMatrix(Math::Matrix::Identity);

}

void PlayerState_Attack2::UpdateKatanaPos()
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
