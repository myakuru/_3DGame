#include "Attack4Effect.h"
#include"../../../../Scene/SceneManager.h"
#include"../../../../GameObject/Character/Player/Player.h"
#include"../../../../main.h"

const uint32_t Attack4Effect::TypeID = KdGameObject::GenerateTypeID();

void Attack4Effect::Init()
{
	MeshEffectBase::Init();
	m_isEffectTime = 0.0f;
	m_once = false;
}

void Attack4Effect::Update()
{
	float deltaTime = Application::Instance().GetDeltaTime();

	// Fキーが押されたらタイマー開始
	if (KeyboardManager::GetInstance().IsKeyJustPressed('F'))
	{
		m_isWaitForEffect = true;
		m_effectWaitTime = 0.0f;
		m_once = false;
	}

	// タイマーが動作中なら経過時間を加算
	if (m_isWaitForEffect)
	{
		m_effectWaitTime += deltaTime;
		if (m_effectWaitTime >= m_isStartEffectTime)
		{
			m_time = 0.0f;				// リセット
			m_alphaFade = 1.0f;			// アルファもリセット
			m_isEffectTime = 0.0f;
			m_isWaitForEffect = false;	// タイマー終了
		}
	}

	// 基底にプレイヤーの取得を任せる
	MeshEffectBase::Update();

	auto spPlayer = m_player.lock();
	if (!spPlayer) return;

	EffectControl(); // エフェクト制御

	// プレイヤーの前方ベクトル
	Math::Vector3 forward = Math::Vector3::TransformNormal(Math::Vector3::Forward, Math::Matrix::CreateFromQuaternion(spPlayer->GetRotationQuaternion()));
	forward.Normalize();

	m_mWorld = Math::Matrix::CreateScale(m_scale);
	m_mWorld *= Math::Matrix::CreateFromYawPitchRoll
	(
		DirectX::XMConvertToRadians(m_degree.y),
		DirectX::XMConvertToRadians(m_degree.x),
		DirectX::XMConvertToRadians(m_degree.z)
	) * Math::Matrix::CreateFromQuaternion(spPlayer->GetRotationQuaternion());

	m_mWorld.Translation(m_position + spPlayer->GetPos() + forward * m_distance);
}

void Attack4Effect::EffectControl()
{
	float deltaTime = Application::Instance().GetDeltaTime();

	m_time += deltaTime * m_fadeTime;

	// 0.0fから1.0fまで増加
	if (m_time <= 1.0f)
	{
		m_fadeAmount = m_time; // 0.0fから1.0fまで増加
	}
	else
	{
		m_alphaFade -= deltaTime * m_fadeTime;
		if (m_alphaFade < 0.0f) m_alphaFade = 0.0f;
	}
}

void Attack4Effect::DrawEffect()
{
	// ここでエフェクトの色やグラデーションの設定も可能
	KdShaderManager::Instance().m_StandardShader.SetFadeAmount(m_fadeAmount, m_outColor, m_inColor, m_colorGradation, m_alphaFade);
	MeshEffectBase::DrawEffect();
}
