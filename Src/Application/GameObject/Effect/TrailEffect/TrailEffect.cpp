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
	m_trailPolygon1 = std::make_shared<KdTrailPolygon>();

	m_trailPolygon->ClearPoints();
	m_trailPolygon1->ClearPoints();
	
	m_trailPolygon->SetLength(50);
	m_trailPolygon1->SetLength(20);

	m_trailTex = KdAssets::Instance().m_textures.GetData("Asset/Textures/trajectory.png");

	m_trailPolygon->SetMaterial(m_trailTex);
	m_trailPolygon1->SetMaterial(m_trailTex);

	//m_trailPolygon->SetPattern(KdTrailPolygon::Trail_Pattern::eVertices);
	//m_trailPolygon1->SetPattern(KdTrailPolygon::Trail_Pattern::eBillboard);

	m_enableTrail = false;
}

void TrailEffect::DrawBright()
{
	if (!m_enableTrail) return;
	KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_trailPolygon, Math::Matrix::Identity, m_color);
}

void TrailEffect::DrawToon()
{
	if (!m_enableTrail) return;
	//KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_trailPolygon, Math::Matrix::Identity, { 1.0f,1.0f,1.0f,0.7f });
}

void TrailEffect::Update()
{
	if (!m_enableTrail) return;

	SceneManager::Instance().GetObjectWeakPtr(m_katana);
	auto katana = m_katana.lock();
	if (!katana) return;

	m_katanaMat = katana->GetKatanaMatrix();
	m_trailRot = katana->GetKatanaRotation();

	Math::Vector3 tip1 = m_katanaMat.Translation() + m_trailRot.Up() * 0.5f;

	Math::Matrix inv = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(90.0f));
	Math::Matrix trailScale = Math::Matrix::CreateScale(m_trailScale);
	Math::Matrix trailTrans = Math::Matrix::CreateTranslation(tip1);
	Math::Matrix finalMat = trailScale * inv * (m_trailRot * trailTrans);

	// --- Catmull-Rom用の点列を管理 ---
	static std::deque<Math::Matrix> history;
	constexpr size_t maxHistory = 4; // Catmull-Romは4点必要

	// 新しい点を追加
	history.push_back(finalMat);
	if (history.size() > maxHistory) history.pop_front();

	// 補間点を追加
	if (history.size() == maxHistory)
	{
		// 0.25, 0.5, 0.75の3点を補間
		for (float t : {0.25f, 0.5f, 0.75f})
		{
			Math::Vector3 p0 = history[0].Translation();
			Math::Vector3 p1 = history[1].Translation();
			Math::Vector3 p2 = history[2].Translation();
			Math::Vector3 p3 = history[3].Translation();

			using namespace DirectX;
			XMVECTOR v0 = XMLoadFloat3(&p0);
			XMVECTOR v1 = XMLoadFloat3(&p1);
			XMVECTOR v2 = XMLoadFloat3(&p2);
			XMVECTOR v3 = XMLoadFloat3(&p3);

			XMVECTOR interp = XMVectorCatmullRom(v0, v1, v2, v3, t);
			Math::Vector3 interpPos;
			XMStoreFloat3(&interpPos, interp);

			// 補間位置で新しい行列を作成（回転・スケールはそのまま）
			Math::Matrix interpMat = history[2];
			interpMat.Translation(interpPos);

			m_trailPolygon->AddPoint(interpMat);
		}
	}

	// 最後に最新点も追加
	m_trailPolygon->AddPoint(finalMat);
	//m_trailPolygon1->AddPoint(finalMat2); // 必要なら同様に
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
