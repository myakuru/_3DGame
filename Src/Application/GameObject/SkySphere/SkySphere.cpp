#include "SkySphere.h"

void SkySphere::DrawUnLit()
{
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model);
}
