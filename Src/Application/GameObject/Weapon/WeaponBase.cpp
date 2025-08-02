#include "WeaponBase.h"

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
}

void WeaponBase::JsonInput(const nlohmann::json& _json)
{
	SelectDraw3dModel::JsonInput(_json);
}
