#include"PlayerState_ChargeAttack.h"
#include"../../../../../main.h"

#include"../PlayerState_Idle/PlayerState_Idle.h"
#include"../../../../Weapon/Katana/Katana.h"

void PlayerState_ChargeAttack::StateStart()
{
	auto anime = m_player->GetAnimeModel()->GetAnimation("ChargeAttack");
	m_player->GetAnimator()->AnimationBlend(anime, 10.0f,false);
	m_player->AnimeSetFlg() = true;

	// 攻撃開始時に直前の移動方向を保存
	m_attackDirection = m_player->GetLastMoveDirection();

	// 攻撃開始時に向きを合わせる
	if (m_attackDirection != Math::Vector3::Zero)
	{
		m_player->UpdateQuaternion(m_attackDirection);
	}

	m_chargeTime = 0.0f;
	m_isCharging = false;
	m_isCharged = false;
}

void PlayerState_ChargeAttack::StateUpdate()
{
	m_player->SetAnimeSpeed(120.0f);

	if (m_player->GetAnimator()->IsAnimationEnd())
	{
		auto idleState = std::make_shared<PlayerState_Idle>();
		m_player->ChangeState(idleState);
		return;
	}

	UpdateKatana();

	float time = m_player->GetAnimator()->GetTime();

	if (time >= 0.0f && !m_isCharging)
	{
		KdEffekseerManager::GetInstance().Play("o-ra.efkefc", { m_player->GetPosition().x,m_player->GetPosition().y + 0.2f,m_player->GetPosition().z }, 0.3f, 100.0f, false);
		KdEffekseerManager::GetInstance().Play("Charge2.efkefc", { m_player->GetPosition().x,m_player->GetPosition().y + 0.2f,m_player->GetPosition().z }, 0.3f, 100.0f, false);
		
		m_isCharging = true;
	}

	if (time >= 30.5f && !m_isCharged)
	{

		// エフェクトの表示位置（前方0.5f）
		Math::Vector3 effectPos = m_player->GetPosition() + m_attackDirection * -3.0f;

		// プレイヤーの回転行列
		Math::Matrix rotationMat = Math::Matrix::CreateFromQuaternion(m_player->GetRotation());

		// エフェクトのワールド行列（回転＋位置）
		Math::Matrix effectWorld = rotationMat * Math::Matrix::CreateTranslation(effectPos);

		// Effekseerエフェクト再生
		auto effect = KdEffekseerManager::GetInstance().Play("ClawStrike.efkefc", effectPos, 0.1f, 10.0f, false);
		if (auto spEffect = effect.lock())
		{
			KdEffekseerManager::GetInstance().SetWorldMatrix(spEffect->GetHandle(), effectWorld);
		}
		m_isCharged = true;
	}


	float deltaTime = Application::Instance().GetDeltaTime();
	if (m_chargeTime < 0.3f)
	{
		float dashSpeed = -0.2f;
		m_player->SetIsMoving(m_attackDirection * dashSpeed);
		m_chargeTime += deltaTime;
	}
	else
	{
		// 移動を止める
		m_player->SetIsMoving(Math::Vector3::Zero);
	}
}

void PlayerState_ChargeAttack::StateEnd()
{
	// カタナの取得
	auto katana = m_player->GetKatana().lock();

	if (!katana) return;
	katana->SetHandKatanaMatrix(Math::Matrix::Identity);
}

void PlayerState_ChargeAttack::UpdateKatana()
{
	// 手のワークノードを取得
	auto handNode = m_player->GetModelWork()->FindWorkNode("VSB_10");

	if (!handNode) return;

	// カタナの取得
	auto katana = m_player->GetKatana().lock();

	if (!katana) return;

	// プレイヤーに追尾する刀にするためにワークノードとプレイヤーのワールド変換を設定
	katana->SetHandKatanaMatrix(handNode->m_worldTransform);
}