#include "MeshEffectBase.h"
#include"../../Character/Player/Player.h"
#include"../../../Scene/SceneManager.h"
#include"../../../main.h"
#include"../../../../Framework/Json/Json.h"

void MeshEffectBase::Update()
{
	SceneManager::Instance().GetObjectWeakPtr(m_player);

	if (m_player.expired()) return;
}

void MeshEffectBase::DrawEffect()
{
	KdShaderManager::Instance().ChangeRasterizerState(KdRasterizerState::CullNone);
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld, m_color);
	KdShaderManager::Instance().UndoRasterizerState();
}

void MeshEffectBase::ImGuiInspector()
{
	KdGameObject::ImGuiInspector();

	ImGui::DragFloat("FadeTime", &m_fadeTime, 0.1f);
	ImGui::ColorEdit3("OutColor", &m_outColor.x);
	ImGui::ColorEdit3("InColor", &m_inColor.x);
}

void MeshEffectBase::JsonInput(const nlohmann::json& _json)
{
	KdGameObject::JsonInput(_json);
	if (_json.contains("FadeTime")) m_fadeTime = _json["FadeTime"].get<float>();
	if (_json.contains("OutColor")) m_outColor = JSON_MANAGER.JsonToVector(_json["OutColor"]);
	if (_json.contains("InColor")) m_inColor = JSON_MANAGER.JsonToVector(_json["InColor"]);
}

void MeshEffectBase::JsonSave(nlohmann::json& _json) const
{
	KdGameObject::JsonSave(_json);
	_json["FadeTime"] = m_fadeTime;
	_json["OutColor"] = JSON_MANAGER.VectorToJson(m_outColor);
	_json["InColor"] = JSON_MANAGER.VectorToJson(m_inColor);
}
