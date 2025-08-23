#include "PlayerState_Idle.h"
#include"../../../CharacterBase.h"
#include"../../../../../../MyFramework/Manager/KeyboardManager.h"
#include"../../../../../main.h"

#include"../../../../../Scene/SceneManager.h"

#include"../PlayerState_Run/PlayerState_Run.h"
#include"../PlayerState_Attack/PlayerState_Attack.h"
#include"../PlayerState_FowardAvoid/PlayerState_FowardAvoid.h"
#include"../PlayerState_ChargeAttack/PlayerState_ChargeAttack.h"
#include"../PlayerState_Skill/PlayerState_Skill.h"
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

	// 左ボタンの押下時間取得
	float lButtonDuration = KeyboardManager::GetInstance().GetKeyPressDuration(VK_LBUTTON);

	// 押し始めたらフラグON
	if (KeyboardManager::GetInstance().IsKeyJustPressed(VK_LBUTTON))
	{
		m_lButtonPressing = true;
	}

	// 押し続けている間に0.3秒超えたらチャージ攻撃
	if (m_lButtonPressing && lButtonDuration >= 0.3f)
	{
		m_lButtonPressing = false;
		auto chargeAttackState = std::make_shared<PlayerState_ChargeAttack>();
		m_player->ChangeState(chargeAttackState);
		return;
	}

	// 離した瞬間に0.3秒未満なら通常攻撃
	if (m_lButtonPressing && KeyboardManager::GetInstance().IsKeyJustReleased(VK_LBUTTON))
	{
		m_lButtonPressing = false;
		if (lButtonDuration < 0.3f)
		{
			auto attackState = std::make_shared<PlayerState_Attack>();
			m_player->ChangeState(attackState);
			return;
		}
	}

	// 前方回避
	if (KeyboardManager::GetInstance().IsKeyJustPressed(VK_RBUTTON))
	{
		auto fowardAvoidState = std::make_shared<PlayerState_ForwardAvoid>();
		m_player->ChangeState(fowardAvoidState);
		return;
	}

	// Eスキル
	if (KeyboardManager::GetInstance().IsKeyJustPressed('E'))
	{
		auto skillState = std::make_shared<PlayerState_Skill>();
		m_player->ChangeState(skillState);
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
