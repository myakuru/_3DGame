#include"Field.h"
#include"../../Scene/SceneManager.h"
const uint32_t Field::TypeID = KdGameObject::GenerateTypeID();

void Field::DrawToon()
{
	KdShaderManager::Instance().ChangeRasterizerState(KdRasterizerState::CullNone);
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld, m_color);
	KdShaderManager::Instance().UndoRasterizerState();
}

void Field::DrawGrayScale()
{
	if (!SceneManager::Instance().GetDrawGrayScale()) return;
	//KdShaderManager::Instance().ChangeRasterizerState(KdRasterizerState::CullNone);

	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld, m_color);
	//KdShaderManager::Instance().UndoRasterizerState();
}
