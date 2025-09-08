#include "TrailEffect.h"
#include"../../Weapon/Katana/Katana.h"
#include"../../../Scene/SceneManager.h"
#include"../../../main.h"
#include"../../../../Framework/Json/Json.h"

const uint32_t TrailEffect::TypeID = KdGameObject::GenerateTypeID();

void TrailEffect::Init()
{
	KdGameObject::Init();
	m_trailPolygon = std::make_shared<KdTrailPolygon>();
	m_trailPolygon->ClearPoints();
	m_trailPolygon->SetLength(30);
	m_trailTex = KdAssets::Instance().m_textures.GetData("Asset/Textures/NA_Basic hit_006.png");
	m_trailPolygon->SetMaterial(m_trailTex);
	m_trailPolygon->SetPattern(KdTrailPolygon::Trail_Pattern::eBillboard);
	m_enableTrail = false;
}

void TrailEffect::DrawBright()
{
	if (!m_enableTrail) return;
	KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_trailPolygon, Math::Matrix::Identity, m_color);
}

void TrailEffect::DrawToon()
{
	//KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_trailPolygon, Math::Matrix::Identity, { 1.0f,1.0f,1.0f,0.7f });
}

void TrailEffect::Update()
{
	if (!m_enableTrail) return;

	SceneManager::Instance().GetObjectWeakPtr(m_katana);
	auto katana = m_katana.lock();
	if (!katana) return;

	m_katanaMat = katana->GetKatanaMatrix();

	Math::Vector3 tip1 = m_katanaMat.Translation() + m_katanaMat.Backward() * 100.0f;

	Math::Matrix trailScale = Math::Matrix::CreateScale(m_trailScale);
	Math::Matrix trailTrans = Math::Matrix::CreateTranslation(tip1);
	Math::Matrix finalMat = trailScale * trailTrans;
	m_trailPolygon->AddPoint(finalMat);
}

void TrailEffect::ImGuiInspector()
{
	KdGameObject::ImGuiInspector();
	ImGui::Text("TrailEffect Inspector");
	ImGui::Text(U8("軌跡の拡大縮小"));
	ImGui::DragFloat3("Scale", &m_trailScale.x, 0.1f);
}

void TrailEffect::JsonSave(nlohmann::json& _json) const
{
	KdGameObject::JsonSave(_json);
	_json["TrailEffect"] = JSON_MANAGER.VectorToJson(m_trailScale);
}

void TrailEffect::JsonInput(const nlohmann::json& _json)
{
	KdGameObject::JsonInput(_json);
	if (_json.contains("TrailEffect")) m_trailScale = JSON_MANAGER.JsonToVector(_json["TrailEffect"]);
}
