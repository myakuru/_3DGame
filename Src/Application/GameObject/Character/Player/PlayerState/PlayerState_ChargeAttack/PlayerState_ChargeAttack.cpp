#include"PlayerState_ChargeAttack.h"
#include"../../../../../main.h"

#include"../PlayerState_ChargeAttack2/PlayerState_ChaegeAttack2.h"
#include"../../../../Weapon/Katana/Katana.h"
#include"../../../../../Scene/SceneManager.h"
#include"../../../../Camera/PlayerCamera/PlayerCamera.h"

#include"../../../../Effect/EffekseerEffect/ShineEffectBlue/ShineEffectBlue.h"
#include"../../../../Effect/EffekseerEffect/GroundFreezes/GroundFreezes.h"


void PlayerState_ChargeAttack::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("ChargeAttack0");
	m_player->GetAnimator()->SetAnimation(anime, 0.25f, false);

	PlayerStateBase::StateStart();

	m_chargeTime = 0.0f;
	m_isCharging = false;

	m_time = 0.0f;

	if (auto camera = m_player->GetPlayerCamera().lock(); camera)
	{
		camera->SetTargetLookAt({ 0.f,1.f,-2.0f });
	}

	SceneManager::Instance().GetObjectWeakPtr(m_shineEffect);
	SceneManager::Instance().GetObjectWeakPtr(m_groundFreezes);

	if (auto effect = m_shineEffect.lock(); effect)
	{
		effect->SetPlayEffect(true);
	}

	if (auto effect = m_groundFreezes.lock(); effect)
	{
		effect->SetPlayEffect(true);
	}
}

void PlayerState_ChargeAttack::StateUpdate()
{

	// アニメーション速度を変更
	m_player->SetAnimeSpeed(80.0f);

	// アニメーションが終了したら納刀する状態へ
	if (m_player->GetAnimator()->IsAnimationEnd())
	{
		auto state = std::make_shared<PlayerState_ChaegeAttack2>();
		m_player->ChangeState(state);
		return;
	}

	float deltaTime = Application::Instance().GetDeltaTime();

	m_time += deltaTime;

	//KdDebugGUI::Instance().AddLog(std::to_string(m_time).data());
	//KdDebugGUI::Instance().AddLog("\n");

	if (m_time >= 0.1f && m_time <= 0.2f)
	{
		KdShaderManager::Instance().m_postProcessShader.SetEnableStrongBlur(true);
	}
	else
	{
		KdShaderManager::Instance().m_postProcessShader.SetEnableStrongBlur(false);
	}

	// 刀は鞘の中にある状態
	UpdateUnsheathed();

	// 攻撃中の移動方向で回転を更新
	if (m_player->GetMovement() != Math::Vector3::Zero)
	{
		Math::Vector3 moveDir = m_player->GetMovement();
		moveDir.y = 0.0f;
		moveDir.Normalize();
		m_player->UpdateQuaternionDirect(moveDir);
	}

	
	m_player->SetIsMoving(m_attackDirection);
		
	// 移動を止める
	m_player->SetIsMoving(Math::Vector3::Zero);

}

void PlayerState_ChargeAttack::StateEnd()
{
	PlayerStateBase::StateEnd();

	if (auto camera = m_player->GetPlayerCamera().lock(); camera)
	{
		camera->SetTargetLookAt({ 0.f,1.f,-2.5f });
	}

	if (auto effect = m_shineEffect.lock(); effect)
	{
		effect->SetPlayEffect(false);
	}

	if (auto effect = m_groundFreezes.lock(); effect)
	{
		effect->SetPlayEffect(false);
	}
}