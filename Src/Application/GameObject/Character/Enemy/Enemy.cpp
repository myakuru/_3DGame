#include "Enemy.h"

void Enemy::Init()
{
}

void Enemy::PreUpdate()
{
}

void Enemy::Update()
{
	CharaBase::Update();

	// 最終的なワールド行列計算
	m_mWorld = Math::Matrix::CreateScale(m_scale);
	m_mWorld *= Math::Matrix::CreateFromQuaternion(m_rotation);
	m_mWorld.Translation(m_position);
}


void Enemy::DrawToon()
{
}

void Enemy::ImGuiInspector()
{
}

void Enemy::JsonInput(const nlohmann::json& _json)
{
}

void Enemy::JsonSave(nlohmann::json& _json) const
{
}