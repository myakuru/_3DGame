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
	SelectDrawObject::ImGuiInspector();
}

void WeaponBase::JsonSave(nlohmann::json& _json) const
{
	SelectDrawObject::JsonSave(_json);
}

void WeaponBase::JsonInput(const nlohmann::json& _json)
{
	SelectDrawObject::JsonInput(_json);
}
