#include "Map.h"
#include"../../Scene/SceneManager.h"

const uint32_t Map::TypeID = KdGameObject::GenerateTypeID();

Map::Map()
{
	m_typeID = TypeID;
}

void Map::DrawLit()
{
	//KdShaderManager::Instance().ChangeRasterizerState(KdRasterizerState::CullFrontWireframe);
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld, m_color);
	//KdShaderManager::Instance().UndoRasterizerState();
}

void Map::DrawGrayScale()
{
	if (!SceneManager::Instance().GetDrawGrayScale()) return;
	
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld, m_color);

}
