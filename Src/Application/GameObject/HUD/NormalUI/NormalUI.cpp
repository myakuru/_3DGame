#include "NormalUI.h"
#include "../../../Scene/SceneManager.h"
#include"../../../Scene/BaseScene/BaseScene.h"

const uint32_t NormalUI::TypeID = KdGameObject::GenerateTypeID();
NormalUI::NormalUI()
{
	m_typeID = TypeID;
}

void NormalUI::DrawSprite()
{
	KdShaderManager::Instance().m_spriteShader.SetMatrix(m_mWorld);
	SelectDraw2DTexture::DrawSprite();
	KdShaderManager::Instance().m_spriteShader.SetMatrix(Math::Matrix::Identity);
}

void NormalUI::Update()
{
	m_mWorld = Math::Matrix::CreateScale(m_scale);
	m_mWorld.Translation(m_position);
}
