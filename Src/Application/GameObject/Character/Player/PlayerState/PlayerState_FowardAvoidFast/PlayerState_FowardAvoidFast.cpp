#include "PlayerState_FowardAvoidFast.h"
#include"../../../../../main.h"
#include"../PlayerState_Idle/PlayerState_Idle.h"
#include"../PlayerState_AvoidAttack/PlayerState_AvoidAttack.h"

void PlayerState_FowardAvoidFast::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("AvoidForward");
	m_player->GetAnimator()->SetAnimation(anime, 0.25f, false);

	m_player->SetAvoidFlg(true);
	m_player->SetAvoidStartTime(Application::Instance().GetDeltaTime()); // 現在の時間を記録

	m_player->AddAfterImage(true, 5, 2.0f, Math::Color(0.0f, 1.0f, 1.0f, 0.2f));

	m_player->SetAnimeSpeed(120.0f);

	// 敵との当たり判定を無効化
	m_player->SetAtkPlayer(true);
}

void PlayerState_FowardAvoidFast::StateUpdate()
{
	Math::Vector3 forward = Math::Vector3::TransformNormal(Math::Vector3::Forward, Math::Matrix::CreateFromQuaternion(m_player->GetRotationQuaternion()));
	forward.Normalize();

	if (KeyboardManager::GetInstance().IsKeyJustPressed(VK_LBUTTON))
	{
		auto state = std::make_shared<PlayerState_AvoidAttack>();
		m_player->ChangeState(state);
		return;
	}

	if (m_player->GetAnimator()->IsAnimationEnd())
	{
		auto idleState = std::make_shared<PlayerState_Idle>();
		m_player->ChangeState(idleState);
		return;
	}

	PlayerStateBase::StateUpdate();

	// 刀は鞘の中にある状態
	UpdateUnsheathed();

	float dashSpeed = 3.0f;

	m_player->SetIsMoving(forward * dashSpeed);
}

void PlayerState_FowardAvoidFast::StateEnd()
{
	PlayerStateBase::StateEnd();

	m_player->SetAvoidFlg(false);
	m_player->SetAvoidStartTime(0.0f); // 現在の時間を記録

	m_player->AddAfterImage();

	// 敵との当たり判定を有効化
	m_player->SetAtkPlayer(false);
}
