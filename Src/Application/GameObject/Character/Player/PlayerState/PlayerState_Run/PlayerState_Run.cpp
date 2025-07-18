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
	Math::Vector3 m_movement = m_player->GetMovement();

	m_movement = Math::Vector3::Zero; // 初期化
	// キー入力から移動方向を取得
	if (KeyboardManager::GetInstance().IsKeyPressed('W')) m_movement = Math::Vector3::Backward;
	if (KeyboardManager::GetInstance().IsKeyPressed('S')) m_movement = Math::Vector3::Forward;
	if (KeyboardManager::GetInstance().IsKeyPressed('A')) m_movement = Math::Vector3::Left;
	if (KeyboardManager::GetInstance().IsKeyPressed('D')) m_movement = Math::Vector3::Right;

	// 移動方向がゼロならIdle状態に移行
	if (m_movement == Math::Vector3::Zero)
	{
		auto state = std::make_shared<PlayerState_Idle>();
		m_player->ChangeState(state);
		return;
	}

	m_movement.Normalize(); // 移動方向を正規化

	if (!m_player->GetPlayerCamera()) return;

	auto playerCamera = m_player->GetPlayerCamera();

	// 第1引数の移動ベクトルと第２引数の回転ベクトルを掛け合わせて、戻り値として移動ベクトルを返す
	m_movement = m_movement.TransformNormal(m_movement, playerCamera->GetRotationYMatrix());

	m_player->UpdateRotation(m_movement);

	m_player->SetIsMoving(m_movement);

	KdDebugGUI::Instance().AddLog(std::to_string(m_movement.x).data());
	KdDebugGUI::Instance().AddLog(", ");
	KdDebugGUI::Instance().AddLog(std::to_string(m_movement.y).data());
	KdDebugGUI::Instance().AddLog(", ");
	KdDebugGUI::Instance().AddLog(std::to_string(m_movement.z).data());
	KdDebugGUI::Instance().AddLog("\n");
}

void PlayerState_Run::StateEnd()
{
}
