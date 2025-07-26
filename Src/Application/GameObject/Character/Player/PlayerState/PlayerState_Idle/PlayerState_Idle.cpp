#include "PlayerState_Idle.h"
#include"../../../CharacterBase.h"
#include"../../../../../../MyFramework/Manager/KeyboardManager.h"

#include"../PlayerState_Run/PlayerState_Run.h"

void PlayerState_Idle::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("Idle");
	m_player->GetAnimator()->AnimationBlend(anime, 100.0f); // 0.3秒かけて補間
	m_player->AnimeSetFlg() = true;
}

void PlayerState_Idle::StateUpdate()
{
	// Wキーが押されたらRunステートへ
	if (KeyboardManager::GetInstance().IsKeyPressed('W') ||
		KeyboardManager::GetInstance().IsKeyPressed('A') ||
		KeyboardManager::GetInstance().IsKeyPressed('S') ||
		KeyboardManager::GetInstance().IsKeyPressed('D'))
	{
		auto spRunState = std::make_shared<PlayerState_Run>();
		m_player->ChangeState(spRunState);
		return;
	}

	// 移動量リセット
	m_player->SetIsMoving(Math::Vector3::Zero);

}

void PlayerState_Idle::StateEnd()
{
	m_player->AnimeSetFlg() = false;
}
