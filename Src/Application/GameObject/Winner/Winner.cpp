#include "Winner.h"
#include"../../../Framework/Json/Json.h"
#include"../../main.h"

const uint32_t Winner::TypeID = KdGameObject::GenerateTypeID();

void Winner::Init()
{
	KdShaderManager::Instance().m_StandardShader.SetGradientColor(m_gradientColor);
	//m_texture = KdAssets::Instance().m_textures.GetData("Asset/Textures/GameUI/WINNER.png");
	m_polygon->SetMaterial("Asset/Textures/GameUI/WINNER.png");
}

void Winner::Update()
{
}

void Winner::ImGuiInspector()
{
	SelectDraw3dPolygon::ImGuiInspector();
	ImGui::ColorEdit4(U8("グラデーションのカラー"), &m_gradientColor.x);
	KdShaderManager::Instance().m_StandardShader.SetGradientColor(m_gradientColor);
}

void Winner::JsonSave(nlohmann::json& _json) const
{
	SelectDraw3dPolygon::JsonSave(_json);
	_json["gradientColor"] = JSON_MANAGER.Vector4ToJson(m_gradientColor);
}

void Winner::JsonInput(const nlohmann::json& _json)
{
	if (_json.contains("gradientColor"))
	{
		m_gradientColor = JSON_MANAGER.JsonToVector4(_json["gradientColor"]);
		KdShaderManager::Instance().m_StandardShader.SetGradientColor(m_gradientColor);
	}
	SelectDraw3dPolygon::JsonInput(_json);
}
