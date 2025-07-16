#include "PlayerState_Run.h"
#include"../../../CharacterBase.h"
#include"../PlayerState_Idle/PlayerState_Idle.h"

void PlayerState_Run::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation(0);
	m_player->GetAnimator()->SetAnimation(anime);
	m_player->AnimeSetFlg() = true;
}

void PlayerState_Run::StateUpdate()
{
	// キー入力から移動方向を取得
	m_player->SetIsMoving(Math::Vector3::Zero);
	if (KeyboardManager::GetInstance().IsKeyPressed('W')) m_player->SetIsMoving(Math::Vector3::Backward);
	if (KeyboardManager::GetInstance().IsKeyPressed('S')) m_player->SetIsMoving(Math::Vector3::Forward);
	if (KeyboardManager::GetInstance().IsKeyPressed('A')) m_player->SetIsMoving(Math::Vector3::Left);
	if (KeyboardManager::GetInstance().IsKeyPressed('D')) m_player->SetIsMoving(Math::Vector3::Right);

	// 移動方向がゼロならIdle状態に移行
	if (m_player->GetMovement() == Math::Vector3::Zero)
	{
		auto state = std::make_shared<PlayerState_Idle>();
		m_player->ChangeState(state);
		return;
	}

	m_player->GetMovement().Normalize(); // 移動方向を正規化

}

void PlayerState_Run::StateEnd()
{
}
