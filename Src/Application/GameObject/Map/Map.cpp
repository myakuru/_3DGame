#include "Map.h"
#include"../../Scene/SceneManager.h"

const uint32_t Map::TypeID = KdGameObject::GenerateTypeID();

Map::Map()
{
	m_typeID = TypeID;
}

void Map::DrawLit()
{
	KdShaderManager::Instance().ChangeRasterizerState(KdRasterizerState::CullFrontWireframe);
	SelectDraw3dModel::DrawLit();
	KdShaderManager::Instance().UndoRasterizerState();
}

void Map::DrawGrayScale()
{
	if (!SceneManager::Instance().GetDrawGrayScale()) return;

	SelectDraw3dModel::DrawGrayScale();

}
