#include "Katana.h"
#include"../../../main.h"
#include"../../../../Framework/Json/Json.h"
#include"../../../Scene/SceneManager.h"

// TypeIDの定義と初期化
const uint32_t Katana::TypeID = KdGameObject::GenerateTypeID();

void Katana::Update()
{
	m_swordData.m_weaponRotationMatrix = Math::Matrix::CreateFromYawPitchRoll
	(
		DirectX::XMConvertToRadians(m_swordData.m_weaponDeg.y),
		DirectX::XMConvertToRadians(m_swordData.m_weaponDeg.x),
		DirectX::XMConvertToRadians(m_swordData.m_weaponDeg.z)
	);

	// 回転行列を適用
	m_swordData.m_weaponMatrix = m_swordData.m_weaponRotationMatrix;
	
	// 移動行列はPlayer側から手の位置を取得しているので、下記で最終的に行列に入れてる。
	Math::Vector3 playerHipPos = m_swordData.m_weaponTranslationMatrix.Translation();
	// プレイヤーの後ろにある剣の位置を計算(エディターでいじれるようになっている)
	m_swordData.m_weaponTranslationMatrix.Translation(m_katanaOffset + playerHipPos);
	m_swordData.m_weaponScaleMatrix = Math::Matrix::CreateScale(m_swordData.m_scale);
	// プレイヤーに追尾するように、プレイヤーの位置を取得して、剣の位置を更新
	m_swordData.m_weaponMatrix = m_swordData.m_weaponScaleMatrix * m_swordData.m_weaponTranslationMatrix * m_swordData.m_playerTranslationMatrix;
}

void Katana::UpdateHand()
{
	m_swordHandData.m_weaponRotationMatrix = Math::Matrix::CreateFromYawPitchRoll
	(
		DirectX::XMConvertToRadians(m_swordData.m_weaponDeg.y),
		DirectX::XMConvertToRadians(m_swordData.m_weaponDeg.x),
		DirectX::XMConvertToRadians(m_swordData.m_weaponDeg.z)
	);

	m_swordHandData.m_weaponMatrix = m_swordHandData.m_weaponRotationMatrix;

	m_swordHandData.m_weaponTranslationMatrix.Translation(m_katanaOffset + m_swordData.m_playerTranslationMatrix.Translation());
	m_swordHandData.m_weaponScaleMatrix = Math::Matrix::CreateScale(m_swordData.m_scale);
	m_swordHandData.m_weaponMatrix = m_swordHandData.m_weaponScaleMatrix * m_swordHandData.m_weaponTranslationMatrix * m_swordData.m_playerTranslationMatrix;

}

void Katana::ImGuiInspector()
{
	KdGameObject::ImGuiInspector();

	ImGui::Text("Katana Inspector");

	ImGui::Text(U8("刀の角度を変更"));
	ImGui::DragFloat3("deg", &m_swordData.m_weaponDeg.x, 0.1f);
	ImGui::Text(U8("刀の位置を変更"));
	ImGui::DragFloat3("offset", &m_katanaOffset.x, 0.01f);
	ImGui::Text(U8("刀のスケールを変更"));
	ImGui::DragFloat3("scale", &m_swordData.m_scale.x, 0.01f);
}

void Katana::JsonSave(nlohmann::json& _json) const
{
	WeaponBase::JsonSave(_json);
	_json["weaponDeg"] = JSON_MANAGER.VectorToJson(m_swordData.m_weaponDeg);
	_json["katanaOffset"] = JSON_MANAGER.VectorToJson(m_katanaOffset);
	_json["scale"] = JSON_MANAGER.VectorToJson(m_swordData.m_scale);
}

void Katana::JsonInput(const nlohmann::json& _json)
{
	WeaponBase::JsonInput(_json);
	if (_json.contains("weaponDeg")) m_swordData.m_weaponDeg = JSON_MANAGER.JsonToVector(_json["weaponDeg"]);
	if (_json.contains("katanaOffset")) m_katanaOffset = JSON_MANAGER.JsonToVector(_json["katanaOffset"]);
	if (_json.contains("scale")) m_swordData.m_scale = JSON_MANAGER.JsonToVector(_json["scale"]);
}
