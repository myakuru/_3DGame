#include "CharacterBase.h"
#include"Player/PlayerState/PlayerState.h"

void CharaBase::Init()
{
	ModelLoad(m_path);
	m_animator->SetAnimation(m_modelWork->GetData()->GetAnimation(0));
}

void CharaBase::DrawToon()
{
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_modelWork, m_mWorld, m_color);
}

void CharaBase::Update()
{
	KdGameObject::Update();
}

bool CharaBase::ModelLoad(std::string _path)
{
	if (m_modelWork)
	{
		m_modelWork->SetModelData(_path);
		return true;
	}
	return false;
}

void CharaBase::ImGuiInspector()
{
	KdGameObject::ImGuiInspector();
}

void CharaBase::JsonInput(const nlohmann::json& _json)
{
	KdGameObject::JsonInput(_json);
}

void CharaBase::JsonSave(nlohmann::json& _json) const
{
	KdGameObject::JsonSave(_json);
}
