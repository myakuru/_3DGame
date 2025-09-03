#include "AttackEffect.h"
#include"../../../main.h"
#include"../../../Scene/SceneManager.h"
#include"../../../GameObject/Character/Player/Player.h"
#include"../../../../Framework/Json/Json.h"

const uint32_t AttackEffect::TypeID = KdGameObject::GenerateTypeID();

void AttackEffect::Update()
{
	SceneManager::Instance().GetObjectWeakPtr(m_player);

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

	m_mWorld.Translation(spPlayer->GetPos() + forward + m_offset);
}

void AttackEffect::EffectControl()
{
	float deltaTime = Application::Instance().GetDeltaTime();

	m_time += deltaTime * m_fadeTime;

	if(KeyboardManager::GetInstance().IsKeyJustPressed('F'))
	{
		m_time = 0.0f; // リセット
	}

	// 0.0fから1.0fまで増加
	if (m_time <= 1.0f)
	{
		m_fadeAmount = m_time; // 0.0fから1.0fまで増加
	}

	// ここでエフェクトの色やグラデーションの設定も可能
	KdShaderManager::Instance().m_StandardShader.SetFadeAmount(m_fadeAmount);
}

void AttackEffect::DrawEffect()
{
	KdShaderManager::Instance().ChangeRasterizerState(KdRasterizerState::CullNone);
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld, m_color);
	KdShaderManager::Instance().UndoRasterizerState();
}

void AttackEffect::ImGuiInspector()
{
	KdGameObject::ImGuiInspector();

	ImGui::DragFloat3("Offset", &m_offset.x, 0.1f);
	ImGui::DragFloat("FadeTime", &m_fadeTime, 0.1f);
}

void AttackEffect::JsonInput(const nlohmann::json& _json)
{
	KdGameObject::JsonInput(_json);
	if(_json.contains("offset")) m_offset = JSON_MANAGER.JsonToVector(_json["offset"]);
	if (_json.contains("FadeTime")) m_fadeTime = _json["FadeTime"].get<float>();
}

void AttackEffect::JsonSave(nlohmann::json& _json) const
{
	KdGameObject::JsonSave(_json);
	_json["offset"] = JSON_MANAGER.VectorToJson(m_offset);
	_json["FadeTime"] = m_fadeTime;
}
