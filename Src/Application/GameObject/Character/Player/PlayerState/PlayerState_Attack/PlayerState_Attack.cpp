#include "PlayerState_Attack.h"
#include"../../../../../main.h"
#include"../PlayerState_Attack1/PlayerState_Attack1.h"
#include"../PlayerState_Idle/PlayerState_Idle.h"
#include"../PlayerState_Run/PlayerState_Run.h"

void PlayerState_Attack::StateStart()
{ 
	auto anime = m_player->GetAnimeModel()->GetAnimation("Attack0");
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

void PlayerState_Attack::StateUpdate()
{
	m_player->SetAnimeSpeed(120.0f);

	float currentFrame = m_player->GetAnimator()->GetTime();

	if (KeyboardManager::GetInstance().IsKeyJustPressed(VK_LBUTTON))
	{
		auto attack1state = std::make_shared<PlayerState_Attack1>();
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

	// ルートモーション取得APIを利用
	auto animeModel = m_player->GetAnimeModel();
	auto attackRootAnime = animeModel->GetAnimation("AttackRoot");
	const auto& modelNodes = animeModel->GetDataNodes();

	if (m_player->GetAnimator()->GetRootMotion(attackRootAnime, modelNodes, "Armature", currentFrame, currentRootTranslation))
	{
		float rootMotionScale = 2.5f; // 移動量倍率
		Math::Vector3 targetDelta = (currentRootTranslation - prevRootTranslation) * rootMotionScale;

		// Y軸の移動は無視
		targetDelta.y = 0.0f;

		// プレイヤーの向きに「前進量」を乗せる
		float forwardAmount = targetDelta.z;
		Math::Vector3 moveDelta = -m_attackDirection * forwardAmount;

		static Math::Vector3 prevDelta = Math::Vector3::Zero;
		float lerpFactor = 0.2f; // 補間係数

		Math::Vector3 smoothDelta = Math::Vector3::Lerp(prevDelta, moveDelta, lerpFactor);

		m_player->SetPosition(m_player->GetPosition() + smoothDelta);
		prevDelta = smoothDelta;
		prevRootTranslation = currentRootTranslation;
	}

	m_player->SetIsMoving(Math::Vector3::Zero);

}

void PlayerState_Attack::StateEnd()
{
}
