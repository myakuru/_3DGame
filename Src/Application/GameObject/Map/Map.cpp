#include "Map.h"
#include"../../Scene/SceneManager.h"

const uint32_t Map::TypeID = KdGameObject::GenerateTypeID();

Map::Map()
{
	m_typeID = TypeID;
}

void Map::DrawToon()
{
	//KdShaderManager::Instance().ChangeRasterizerState(KdRasterizerState::CullNone);
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld, m_color);
	//KdShaderManager::Instance().UndoRasterizerState();
}

void Map::DrawGrayScale()
{
	if (!SceneManager::Instance().GetDrawGrayScale()) return;
	//KdShaderManager::Instance().ChangeRasterizerState(KdRasterizerState::CullNone);
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld, m_color);
	//KdShaderManager::Instance().UndoRasterizerState();
}

void Map::Init()
{
	SelectDraw3dModel::Init();
}
