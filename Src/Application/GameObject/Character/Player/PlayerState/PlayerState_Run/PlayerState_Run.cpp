﻿#include "PlayerState_Run.h"
#include"../../../CharacterBase.h"
#include"../PlayerState_RunEnd/PlayerState_RunEnd.h"
#include"../PlayerState_BackWordAvoid/PlayerState_BackWordAvoid.h"
#include"../../../../Camera/PlayerCamera/PlayerCamera.h"
#include"../PlayerState_Hit/PlayerState_Hit.h"
#include"../../../../Weapon/Katana/Katana.h"
#include"../../../../Weapon/WeaponKatanaScabbard/WeaponKatanaScabbard.h"
#include"../PlayerState_FowardAvoidFast/PlayerState_FowardAvoidFast.h"

void PlayerState_Run::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("Run");
	m_player->GetAnimator()->SetAnimation(anime);
	m_player->SetAnimeSpeed(70.0f);
	PlayerStateBase::StateStart();
}

void PlayerState_Run::StateUpdate()
{
	UpdateUnsheathed();
	m_player->UpdateMoveDirectionFromInput();
	Math::Vector3 moveDir = m_player->GetMoveDirection();

	if (moveDir == Math::Vector3::Zero)
	{
		auto state = std::make_shared<PlayerState_RunEnd>();
		m_player->ChangeState(state);
		return;
	}

	// =========================
	// 右ボタン押下 長押し / 短押し判定
	// =========================
	const float kShortPressMin = 0.1f; // 短押し有効開始
	const float kLongPressThreshold = 0.2f; // 長押し閾値(これ以上で長押しアクション)

	float rDuration = KeyboardManager::GetInstance().GetKeyPressDuration(VK_RBUTTON);

	// 押された瞬間
	if (KeyboardManager::GetInstance().IsKeyJustPressed(VK_RBUTTON))
	{
		m_isKeyPressing = true; // 判定開始
	}

	// 長押し判定
	if (m_isKeyPressing && rDuration >= kLongPressThreshold && !KeyboardManager::GetInstance().IsKeyJustReleased(VK_RBUTTON))
	{
		m_isKeyPressing = false;
		auto avoidFast = std::make_shared<PlayerState_FowardAvoidFast>();
		m_player->ChangeState(avoidFast);
		return;
	}

	// 短押し判定
	if (m_isKeyPressing && KeyboardManager::GetInstance().IsKeyJustReleased(VK_RBUTTON))
	{
		if (rDuration >= kShortPressMin && rDuration < kLongPressThreshold)
		{
			auto backAvoid = std::make_shared<PlayerState_BackWordAvoid>();
			m_player->ChangeState(backAvoid);
			m_isKeyPressing = false;
			return;
		}

		// 0.1秒未満なら何もしない
		m_isKeyPressing = false;
	}

	// 方向更新
	m_player->UpdateQuaternion(moveDir);
	m_player->SetIsMoving(moveDir);
}

void PlayerState_Run::StateEnd()
{
	PlayerStateBase::StateEnd();
}
