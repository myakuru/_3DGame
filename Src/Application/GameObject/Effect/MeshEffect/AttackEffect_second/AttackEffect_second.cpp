#include"AttackEffect_second.h"
#include"../../../../main.h"
#include"../../../../Scene/SceneManager.h"
#include"../../../../GameObject/Character/Player/Player.h"

const uint32_t AttackEffect_second::TypeID = KdGameObject::GenerateTypeID();

void  AttackEffect_second::EffectReset()
{
	m_time = 0.0f;      // リセット
	m_alphaFade = 1.0f; // アルファもリセット
}

void AttackEffect_second::Init()
{
	MeshEffectBase::Init();
	EffectReset();
}

void AttackEffect_second::Update()
{
	if (KeyboardManager::GetInstance().IsKeyJustPressed('F'))
	{
		Init();
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

void AttackEffect_second::EffectControl()
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

void AttackEffect_second::DrawEffect()
{
	// ここでエフェクトの色やグラデーションの設定も可能
	KdShaderManager::Instance().m_StandardShader.SetFadeAmount(m_fadeAmount, m_outColor, m_inColor, m_colorGradation, m_alphaFade);
	MeshEffectBase::DrawEffect();
}
