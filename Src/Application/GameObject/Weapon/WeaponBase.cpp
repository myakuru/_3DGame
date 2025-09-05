#include "WeaponBase.h"
#include"../../main.h"
#include"../../../Framework/Json/Json.h"

const uint32_t WeaponBase::TypeID = KdGameObject::GenerateTypeID();

void WeaponBase::Init()
{
	KdGameObject::Init();
}

void WeaponBase::DrawToon()
{
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_swordData.m_weaponMatrix);
}

void WeaponBase::Update()
{
	KdGameObject::Update();
}

void WeaponBase::ImGuiInspector()
{
	SelectDraw3dModel::ImGuiInspector();
}

void WeaponBase::JsonSave(nlohmann::json& _json) const
{
	SelectDraw3dModel::JsonSave(_json);
	_json["weaponDeg"] = JSON_MANAGER.VectorToJson(m_swordData.m_weaponDeg);
	_json["katanaOffset"] = JSON_MANAGER.VectorToJson(m_katanaOffset);
	_json["katanaHandOffset"] = JSON_MANAGER.VectorToJson(m_katanaHandOffset);
	_json["scale"] = JSON_MANAGER.VectorToJson(m_swordData.m_scale);
	_json["handWeaponDeg"] = JSON_MANAGER.VectorToJson(m_swordHandData.m_weaponDeg);
}

void WeaponBase::JsonInput(const nlohmann::json& _json)
{
	SelectDraw3dModel::JsonInput(_json);
	if (_json.contains("weaponDeg")) m_swordData.m_weaponDeg = JSON_MANAGER.JsonToVector(_json["weaponDeg"]);
	if (_json.contains("katanaOffset")) m_katanaOffset = JSON_MANAGER.JsonToVector(_json["katanaOffset"]);
	if (_json.contains("katanaHandOffset")) m_katanaHandOffset = JSON_MANAGER.JsonToVector(_json["katanaHandOffset"]);
	if (_json.contains("scale")) m_swordData.m_scale = JSON_MANAGER.JsonToVector(_json["scale"]);
	if (_json.contains("handWeaponDeg")) m_swordHandData.m_weaponDeg = JSON_MANAGER.JsonToVector(_json["handWeaponDeg"]);
}
