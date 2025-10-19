#include"Field.h"
#include"../../Scene/SceneManager.h"
const uint32_t Field::TypeID = KdGameObject::GenerateTypeID();

void Field::DrawLit()
{
	//KdShaderManager::Instance().ChangeRasterizerState(KdRasterizerState::CullFrontWireframe);
	//KdShaderManager::Instance().m_StandardShader.SetDitherEnable(false);
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld, m_color);
	//KdShaderManager::Instance().UndoRasterizerState();
}

void Field::DrawGrayScale()
{
	if (!SceneManager::Instance().GetDrawGrayScale()) return;

	SelectDraw3dModel::DrawGrayScale();
}

