#include "PlayerState_Skill.h"
#include"../../../../../main.h"
#include"../PlayerState_Idle/PlayerState_Idle.h"
#include"../PlayerState_Run/PlayerState_Run.h"
#include"../../../../Weapon/Katana/Katana.h"

void PlayerState_Skill::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("Skill");
	m_player->GetAnimator()->AnimationBlend(anime, 10.0f, false);
	m_player->AnimeSetFlg() = true;

	// 攻撃開始時に直前の移動方向を保存
	m_attackDirection = m_player->GetLastMoveDirection();

	// 攻撃開始時に向きを合わせる
	if (m_attackDirection != Math::Vector3::Zero)
	{
		m_player->UpdateQuaternion(m_attackDirection);
	}

	m_attackParam = m_player->GetPlayerConfig().GetAttack2Param();
	m_attackParam.m_dashTimer = 0.0f;
}

void PlayerState_Skill::StateUpdate()
{
	m_player->SetAnimeSpeed(180.0f);

	if (m_player->GetAnimator()->IsAnimationEnd())
	{
		auto idleState = std::make_shared<PlayerState_Idle>();
		m_player->ChangeState(idleState);
		return;
	}

	if (!m_flag)
	{
		// エフェクトの表示位置（前方0.5f）
		Math::Vector3 effectPos = m_player->GetPos() + m_attackDirection * 5.0f;

		// プレイヤーの回転行列
		Math::Matrix rotationMat = Math::Matrix::CreateFromQuaternion(m_player->GetRotationQuaternion());

		// エフェクトのワールド行列（回転＋位置）
		Math::Matrix effectWorld = rotationMat * Math::Matrix::CreateTranslation(effectPos);

		// Effekseerエフェクト再生
		auto effect = KdEffekseerManager::GetInstance().Play("Skill.efkefc", { effectPos }, 1.0f, 100.0f, false);
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
		float dashSpeed = 1.0f;
		m_player->SetIsMoving(m_attackDirection * dashSpeed);
		m_attackParam.m_dashTimer += deltaTime;
	}
	else
	{
		// 移動を止める
		m_player->SetIsMoving(Math::Vector3::Zero);
	}

}

void PlayerState_Skill::StateEnd()
{
	PlayerStateBase::StateEnd();
}
