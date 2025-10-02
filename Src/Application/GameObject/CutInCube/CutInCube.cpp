#include "CutInCube.h"
#include"../../Scene/SceneManager.h"
#include"../Character/Player/Player.h"
#include"../../main.h"
#include"../../../Framework/Json/Json.h"

const uint32_t CutInCube::TypeID = KdGameObject::GenerateTypeID();

void CutInCube::Init()
{
	SelectDraw3dModel::Init();
	SceneManager::Instance().GetObjectWeakPtr(m_player);
}

void CutInCube::Update()
{
	if (auto player = m_player.lock(); player)
	{
		m_playerPos = player->GetPos();
	}

	m_position = m_playerPos;
	
	m_mWorld.Translation(m_offset + m_position);
}

void CutInCube::DrawUnLit()
{
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld, m_color);
}

void CutInCube::JsonInput(const nlohmann::json& _json)
{
	SelectDraw3dModel::JsonInput(_json);
	if (_json.contains("offset")) m_offset = JSON_MANAGER.JsonToVector(_json["offset"]);
}

void CutInCube::JsonSave(nlohmann::json& _json) const
{
	SelectDraw3dModel::JsonSave(_json);
	_json["offset"] = JSON_MANAGER.VectorToJson(m_offset);
}

void CutInCube::ImGuiInspector()
{
	SelectDraw3dModel::ImGuiInspector();
	ImGui::DragFloat3("PlayerPos", &m_offset.x, 0.1f, -100.0f, 100.0f);
}
