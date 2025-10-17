#include "PlayerState_FowardAvoid.h"
#include"../PlayerState_Idle/PlayerState_Idle.h"
#include"../../../../../main.h"

#include"../../../../Weapon/Katana/Katana.h"
#include"../../../../Weapon/WeaponKatanaScabbard/WeaponKatanaScabbard.h"
#include"../PlayerState_AvoidAttack/PlayerState_AvoidAttack.h"
#include"../PlayerState_JustAvoidAttack/PlayerState_JustAvoidAttack.h"
#include"../../../../Camera/PlayerCamera/PlayerCamera.h"
#include"../../../Enemy/Enemy.h"

void PlayerState_ForwardAvoid::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("AvoidBackward");
	m_player->GetAnimator()->SetAnimation(anime, 0.25f, false);

	m_player->SetAvoidFlg(true);

	if (auto camera = m_player->GetPlayerCamera().lock(); camera)
	{
		camera->SetTargetLookAt({ 0.f,1.0f,-4.5f });
	}

	m_time = 0.0f;

	// アニメーション速度を変更
	m_player->SetAnimeSpeed(120.0f);

	// 回避時の処理
	m_player->SetAvoidStartTime(0.0f);

	m_afterImagePlayed = false;
	m_justAvoided = false;
}

void PlayerState_ForwardAvoid::StateUpdate()
{
	float deltaTime = Application::Instance().GetDeltaTime();
	m_time += deltaTime; // これが回避開始からの経過時間になる

	// 経過時間を Player クラスに伝える
	m_player->SetAvoidStartTime(m_time);

	// 途中で敵のジャスト回避成功フラグが立ったら残像発生
	if (!m_afterImagePlayed) 
	{
		for(const auto& enemies: m_player->GetEnemies())
		{
			if (auto enemyPtr = enemies.lock(); enemyPtr)
			{
				// ジャスト回避成功時の残像エフェクト
				if (enemyPtr->GetJustAvoidSuccess())
				{
					m_player->AddAfterImage(true, 5, 1.0f, Math::Color(0.0f, 1.0f, 1.0f, 0.5f), 0.7f);
				
					m_justAvoided = true;

					m_afterImagePlayed = true;

					m_player->SetJustAvoidSuccess(true);

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
			auto state = std::make_shared<PlayerState_AvoidAttack>();
			m_player->ChangeState(state);
			return;
		}
	}

	// アニメーションが終了したらIdleへ移行
	if (m_player->GetAnimator()->IsAnimationEnd())
	{
		auto idleState = std::make_shared<PlayerState_Idle>();
		m_player->ChangeState(idleState);
		return;
	}

	PlayerStateBase::StateUpdate();

	// 刀は鞘の中にある状態
	UpdateUnsheathed();

	// 回避中の移動処理
	if (m_time < 0.3f)
	{
		const float dashSpeed = -1.0f;
		m_player->SetIsMoving(forward * dashSpeed);
	}
	else
	{
		m_player->SetIsMoving(Math::Vector3::Zero);
	}
}

void PlayerState_ForwardAvoid::StateEnd()
{
	PlayerStateBase::StateEnd();

	m_player->SetAvoidFlg(false);
	m_player->SetAvoidStartTime(0.0f);

	if (auto camera = m_player->GetPlayerCamera().lock(); camera)
	{
		camera->SetTargetLookAt({ 0.f,1.0f,-3.5f });
	}

	m_justAvoided = false;

	m_player->AddAfterImage();
		
}
