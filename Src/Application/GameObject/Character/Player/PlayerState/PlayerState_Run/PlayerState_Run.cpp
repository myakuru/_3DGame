#include "PlayerState_Run.h"
#include"../../../CharacterBase.h"
#include"../PlayerState_Idle/PlayerState_Idle.h"
#include"../../../../Camera/PlayerCamera/PlayerCamera.h"

void PlayerState_Run::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation(0);
	m_player->GetAnimator()->SetAnimation(anime);
	m_player->AnimeSetFlg() = true;
}

void PlayerState_Run::StateUpdate()
{
	Math::Vector3 movement = m_player->GetMovement();

	// キー入力から移動方向を取得
	m_player->SetIsMoving(Math::Vector3::Zero);
	if (KeyboardManager::GetInstance().IsKeyPressed('W')) movement = Math::Vector3::Backward;
	if (KeyboardManager::GetInstance().IsKeyPressed('S')) movement = Math::Vector3::Forward;
	if (KeyboardManager::GetInstance().IsKeyPressed('A')) movement = Math::Vector3::Left;
	if (KeyboardManager::GetInstance().IsKeyPressed('D')) movement = Math::Vector3::Right;

	// 移動方向がゼロならIdle状態に移行
	if (movement == Math::Vector3::Zero)
	{
		auto state = std::make_shared<PlayerState_Idle>();
		m_player->ChangeState(state);
		return;
	}

	movement.Normalize(); // 移動方向を正規化

	if (!m_player->GetPlayerCamera()) return;

	auto playerCamera = m_player->GetPlayerCamera();

	movement = Math::Vector3::TransformNormal(movement, playerCamera->GetRotationYMatrix());

	m_player->SetIsMoving(movement);

}

void PlayerState_Run::StateEnd()
{
}
