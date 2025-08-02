#include "Enemy.h"

void Enemy::Init()
{
	CharaBase::Init();
}

void Enemy::Update()
{
	CharaBase::Update();

	m_animator->SetAnimation(m_modelWork->GetData()->GetAnimation("Idle"));

	// 最終的なワールド行列計算
	m_mWorld = Math::Matrix::CreateScale(m_scale);
	m_mWorld *= Math::Matrix::CreateFromQuaternion(m_rotation);
	m_mWorld.Translation(m_position);
}


void Enemy::ImGuiInspector()
{
	CharaBase::ImGuiInspector();
}

void Enemy::JsonInput(const nlohmann::json& _json)
{
	CharaBase::JsonInput(_json);
}

void Enemy::JsonSave(nlohmann::json& _json) const
{
	CharaBase::JsonSave(_json);
}