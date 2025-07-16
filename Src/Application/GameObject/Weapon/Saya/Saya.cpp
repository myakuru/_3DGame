#include "Saya.h"
#include"../../../main.h"
#include"../../../../Framework/Json/Json.h"

void Saya::Update()
{
	m_sayaData.m_weaponRotationMatrix = Math::Matrix::CreateFromYawPitchRoll
	(
		DirectX::XMConvertToRadians(m_sayaData.m_weaponDeg.y),
		DirectX::XMConvertToRadians(m_sayaData.m_weaponDeg.x),
		DirectX::XMConvertToRadians(m_sayaData.m_weaponDeg.z)
	);

	// 回転行列を適用
	m_sayaData.m_weaponMatrix = m_sayaData.m_weaponRotationMatrix;

	Math::Vector3 pos = m_sayaData.m_weaponTranslationMatrix.Translation();

	// 移動行列はPlayer側から手の位置を取得しているので、下記で最終的に行列に入れてる。
	m_sayaData.m_weaponTranslationMatrix.Translation(Math::Vector3{ 0.25f,0.0f,0.0f } + pos);

	m_sayaData.m_weaponMatrix *= m_sayaData.m_weaponTranslationMatrix;

}

void Saya::ImGuiInspector()
{
	ImGui::Text("Katana Inspector");
	ImGui::DragFloat3("deg", &m_sayaData.m_weaponDeg.x, 0.1f);
}

void Saya::DrawToon()
{
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_sayaData.m_weaponMatrix);
}

void Saya::JsonSave(nlohmann::json& _json) const
{
	WeaponBase::JsonSave(_json);
	_json["weaponDeg"] = JSON_MANAGER.VectorToJson(m_sayaData.m_weaponDeg);
}

void Saya::JsonInput(const nlohmann::json& _json)
{
	WeaponBase::JsonInput(_json);
	if (_json.contains("weaponDeg")) m_sayaData.m_weaponDeg = JSON_MANAGER.JsonToVector(_json["weaponDeg"]);
}
