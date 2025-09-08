#include "TrailEffect.h"

void TrailEffect::Init()
{
	KdGameObject::Init();
	m_trailPolygon = std::make_shared<KdTrailPolygon>();
	m_trailPolygon->ClearPoints();
	m_trailPolygon->SetLength(100);
	auto trailTex = KdAssets::Instance().m_textures.GetData("Asset/Textures/gradation_B.png");
	m_trailPolygon->SetMaterial(trailTex);
	m_trailPolygon->SetPattern(KdTrailPolygon::Trail_Pattern::eBillboard);
}

void TrailEffect::Update()
{
}

void TrailEffect::DrawBright()
{
	KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_trailPolygon, Math::Matrix::Identity, { 1.0f,1.0f,1.0f,0.7f });
}
