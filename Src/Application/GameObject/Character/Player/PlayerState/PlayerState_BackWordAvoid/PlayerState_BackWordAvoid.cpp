﻿#include "PlayerState_BackWordAvoid.h"
#include"../../../../../main.h"
#include"../../../../Weapon/Katana/Katana.h"

#include"../PlayerState_Idle/PlayerState_Idle.h"
#include"../PlayerState_Run/PlayerState_Run.h"
#include"../../../../Weapon/WeaponKatanaScabbard/WeaponKatanaScabbard.h"
#include"../PlayerState_AvoidAttack/PlayerState_AvoidAttack.h"
#include"../../../BossEnemy/BossEnemy.h"

#include"../PlayerState_Skill/PlayerState_Skill.h"
#include"../../../../../Scene/SceneManager.h"
#include"../PlayerState_JustAvoidAttack/PlayerState_JustAvoidAttack.h"
#include"../../../../Camera/PlayerCamera/PlayerCamera.h"
#include"../PlayerState_SpecialAttackCutIn/PlayerState_SpecialAttackCutIn.h"

void PlayerState_BackWordAvoid::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("AvoidForward");
	m_player->GetAnimator()->SetAnimation(anime, 0.25f, false);

	m_player->SetAvoidFlg(true);
	m_player->SetAvoidStartTime(Application::Instance().GetDeltaTime()); // 現在の時間を記録

	m_player->SetAnimeSpeed(120.0f);

	// 回避時の処理
	m_player->SetAvoidStartTime(0.0f);

	m_afterImagePlayed = false;

	for (const auto& enemies : m_player->GetEnemies())
	{
		if (auto enemyPtr = enemies.lock(); enemyPtr)
		{
			// ジャスト回避成功時の残像エフェクト
			if (enemyPtr->GetJustAvoidSuccess())
			{
				m_player->AddAfterImage(true, 5, 1.0f, Math::Color(0.0f, 1.0f, 1.0f, 0.5f), 0.7f);
			}
		}
	}

	if (auto camera = m_player->GetPlayerCamera().lock(); camera)
	{
		if (auto bossEnemy = m_bossEnemy.lock(); bossEnemy)
		{
			camera->SetTargetLookAt({ 0.0f,1.0f,-7.5f });
		}
		else
		{
			camera->SetTargetLookAt({ 0.0f, 1.0f, -4.5f });
		}
	}

	m_afterImagePlayed = false;
	m_justAvoided = false;

	KdAudioManager::Instance().Play("Asset/Sound/Player/BackWardAvoid.WAV", false)->SetVolume(1.0f);

}

void PlayerState_BackWordAvoid::StateUpdate()
{

	float deltaTime = Application::Instance().GetDeltaTime();
	m_time += deltaTime; // これが回避開始からの経過時間になる

	// 経過時間を Player クラスに伝える
	m_player->SetAvoidStartTime(m_time);

	// 途中で敵のジャスト回避成功フラグが立ったら残像発生
	if (!m_afterImagePlayed)
	{
		for (const auto& wk : m_player->GetEnemyLike())
		{
			if (auto obj = wk.lock())
			{
				bool just = false;

				if (obj->GetTypeID() == Enemy::TypeID)
				{
					auto e = std::static_pointer_cast<Enemy>(obj);
					just = e->GetJustAvoidSuccess();
					if (just) e->SetJustAvoidSuccess(false); // 消費
				}
				else if (obj->GetTypeID() == BossEnemy::TypeID)
				{
					auto b = std::static_pointer_cast<BossEnemy>(obj);
					just = b->GetJustAvoidSuccess();
					if (just) b->SetJustAvoidSuccess(false); // 消費
				}

				if (just)
				{
					// ジャスト回避成功時の残像エフェクト
					m_player->AddAfterImage(true, 5, 1.0f, Math::Color(0.0f, 1.0f, 1.0f, 0.5f), 0.7f);

					KdAudioManager::Instance().Play("Asset/Sound/Player/SlowMotion.WAV", false)->SetVolume(1.0f);

					// ゲームのメインサウンドのピッチを下げる
					if (auto bgm = SceneManager::Instance().GetGameSound())
					{
						bgm->SetPitch(-1.0f);
					}

					m_justAvoided = true;
					m_afterImagePlayed = true;
					m_player->SetJustAvoidSuccess(true);

					// 保険：ここでも演出をON（更新順の揺れ対策）
					{
						const auto& justCfg = m_player->GetPlayerConfig().GetJustAvoidParam();
						Application::Instance().SetFpsScale(justCfg.m_slowMoScale);
						SceneManager::Instance().SetDrawGrayScale(justCfg.m_useGrayScale);
					}
					break; // 多重発火防止
				}
			}
		}
	}


	// 前方ベクトルを取得
	Math::Vector3 forward = Math::Vector3::TransformNormal(Math::Vector3::Forward, Math::Matrix::CreateFromQuaternion(m_player->GetRotationQuaternion()));
	forward.Normalize();

	if (m_justAvoided)
	{
		if (KeyboardManager::GetInstance().IsKeyJustPressed(VK_LBUTTON))
		{
			m_justAvoided = false;
			auto state = std::make_shared<PlayerState_JustAvoidAttack>();
			m_player->ChangeState(state);
			return;
		}
	}
	else
	{
		// 回避中に攻撃ボタンが押されたら回避攻撃へ移行
		if (KeyboardManager::GetInstance().IsKeyJustPressed(VK_LBUTTON))
		{
			m_justAvoided = false;
			auto state = std::make_shared<PlayerState_AvoidAttack>();
			m_player->ChangeState(state);
			return;
		}

		// Eキー先行入力の予約
		if (KeyboardManager::GetInstance().IsKeyJustPressed('E'))
		{
			if (m_player->GetPlayerStatus().skillPoint >= 30)
			{
				m_player->GetPlayerStatus().skillPoint -= 30;
				auto state = std::make_shared<PlayerState_Skill>();
				m_player->ChangeState(state);
				return;
			}
		}

		if (KeyboardManager::GetInstance().IsKeyJustPressed('Q'))
		{
			if (m_player->GetPlayerStatus().specialPoint == m_player->GetPlayerStatus().specialPointMax)
			{
				m_player->GetPlayerStatus().specialPoint = 0;
				auto specialAttackState = std::make_shared<PlayerState_SpecialAttackCutIn>();
				m_player->ChangeState(specialAttackState);
				return;
			}
		}
	}

	// アニメーションが終了したらIdleへ移行
	if (m_player->GetAnimator()->IsAnimationEnd())
	{
		// スローモーション解除（ここを終点にする）
		Application::Instance().SetFpsScale(1.f);
		SceneManager::Instance().SetDrawGrayScale(false);
		m_justAvoided = false;

		// ゲームのメインサウンドのピッチを下げる
		if (auto bgm = SceneManager::Instance().GetGameSound())
		{
			bgm->SetPitch(0.0f);
		}

		auto idleState = std::make_shared<PlayerState_Idle>();
		m_player->ChangeState(idleState);
		return;
	}

	PlayerStateBase::StateUpdate();

	UpdateUnsheathed();

	float dashSpeed = 1.0f;
	
	m_player->SetIsMoving(forward * dashSpeed);

}

void PlayerState_BackWordAvoid::StateEnd()
{
	PlayerStateBase::StateEnd();

	m_player->SetAvoidFlg(false);
	m_player->SetAvoidStartTime(0.0f); // 現在の時間を記録


	m_player->AddAfterImage();
		
}
