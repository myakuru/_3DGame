#include "Player.h"

void Player::Init()
{
	m_model = std::make_shared<KdModelWork>();
	m_model->SetModelData("Asset/Models/Player/Run.gltf");

	m_animator = std::make_shared<KdAnimator>();
	m_animator->SetAnimation(m_model->GetData()->GetAnimation(0));

}

void Player::Update()
{
	m_animator->AdvanceTime(m_model->WorkNodes());
}

void Player::DrawLit()
{
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model);
}
