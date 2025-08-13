#include "PlayerState_Idle.h"
#include"../../../CharacterBase.h"
#include"../../../../../../MyFramework/Manager/KeyboardManager.h"
#include"../../../../../main.h"

#include"../../../../../Scene/SceneManager.h"

#include"../PlayerState_Run/PlayerState_Run.h"
#include"../PlayerState_Attack/PlayerState_Attack.h"
#include"../../../../Weapon/Katana/Katana.h"

void PlayerState_Idle::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("Idle");
	m_player->GetAnimator()->AnimationBlend(anime, 10.0f);
	m_player->AnimeSetFlg() = true;
}

void PlayerState_Idle::StateUpdate()
{
	m_player->SetAnimeSpeed(60.0f);

	UpdateKatana();

	// キーが押されたらRunステートへ
	if (KeyboardManager::GetInstance().IsKeyPressed('W') ||
		KeyboardManager::GetInstance().IsKeyPressed('A') ||
		KeyboardManager::GetInstance().IsKeyPressed('S') ||
		KeyboardManager::GetInstance().IsKeyPressed('D'))
	{
		auto spRunState = std::make_shared<PlayerState_Run>();
		m_player->ChangeState(spRunState);
		return;
	}

	if (KeyboardManager::GetInstance().IsKeyJustPressed(VK_LBUTTON))
	{
		auto attackState = std::make_shared<PlayerState_Attack>();
		m_player->ChangeState(attackState);
		return;
	}

	// 移動量リセット
	m_player->SetIsMoving(Math::Vector3::Zero);

}

void PlayerState_Idle::StateEnd()
{
	m_player->AnimeSetFlg() = false;
}

void PlayerState_Idle::ImGuiInspector()
{
	ImGui::Text("PlayerState_Idle");
}

void PlayerState_Idle::UpdateKatana()
{
	// Idle時はHipsノードをhandWorkNodeにセット
	auto hipNode = m_player->GetModelWork()->FindWorkNode("Hips");

	if (!hipNode) return;

	// カタナの取得
	auto katana = m_player->GetKatana().lock();

	if (!katana) return;

	// プレイヤーに追尾する刀にするためにワークノードとプレイヤーのワールド変換を設定
	katana->SetKatanaMatrix(hipNode->m_worldTransform);
}
