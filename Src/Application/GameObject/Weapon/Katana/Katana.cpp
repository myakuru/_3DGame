#include "Katana.h"
#include"../../../main.h"
#include"../../../Scene/SceneManager.h"
#include"../../../../Framework/Json/Json.h"
#include"../../../../Framework/ImGuiManager/ImGuiManager.h"

// TypeIDの定義と初期化
const uint32_t Katana::TypeID = KdGameObject::GenerateTypeID();

void Katana::Init()
{
	WeaponBase::Init();
	m_trailPolygon = std::make_shared<KdTrailPolygon>();

	m_trailPolygon->ClearPoints();
	m_trailPolygon->SetLength(50);
	//m_trailTex = KdAssets::Instance().m_textures.GetData("Asset/Textures/trajectory.png");

	m_scale = { 0.004f,0.004f,0.004f };

	//m_trailPolygon->SetMaterial(m_trailTex);
	m_showTrail = false;
}

void Katana::Update()
{
	if (!m_isAttackState)
	{
		// 攻撃していない間は軌跡を常にクリア
		m_trailPolygon->ClearPoints();
		UpdateHand();
		return;
	}

	// 攻撃中は軌跡を追加
	m_swordData.m_weaponRotationMatrix = m_swordData.m_weaponBonesMatrix.CreateFromYawPitchRoll(
		DirectX::XMConvertToRadians(m_swordData.m_weaponDeg.y),
		DirectX::XMConvertToRadians(m_swordData.m_weaponDeg.x),
		DirectX::XMConvertToRadians(m_swordData.m_weaponDeg.z)
	);

	m_swordData.m_weaponScaleMatrix = Math::Matrix::CreateScale(m_swordData.m_scale);

	m_swordData.m_weaponBonesMatrix.Translation(m_swordHandData.m_weaponBonesMatrix.Translation());
	Math::Matrix transOffset = Math::Matrix::CreateTranslation(m_katanaHandOffset);
	m_swordData.m_weaponMatrix = transOffset * m_swordData.m_weaponScaleMatrix * m_swordData.m_weaponRotationMatrix * m_swordHandData.m_weaponBonesMatrix * m_swordHandData.m_playerWorldMatrix;

	UpdateTrailPolygon();

}

void Katana::UpdateTrailPolygon()
{
	if (!IMGUI_MANAGER.GetShowEffect()) return;

	Math::Vector3 tip1 = m_swordData.m_weaponMatrix.Translation() + m_swordData.m_weaponMatrix.Backward() * m_trailKatanaPoint;
	Math::Matrix inv = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(-90));
	Math::Matrix trailTrans = Math::Matrix::CreateTranslation(tip1);
	Math::Matrix trailScale = Math::Matrix::CreateScale(m_trailScale);
	Math::Matrix finalMat = trailScale * inv * (m_swordData.m_weaponRotationMatrix * trailTrans);

	// Catmull-Romスプラインで補間点を追加
	static std::deque<Math::Matrix> history;
	constexpr size_t maxHistory = 4;

	history.push_back(finalMat);
	if (history.size() > maxHistory) history.pop_front();

	if (history.size() == maxHistory)
	{
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

			Math::Matrix interpMat = history[3];
			interpMat.Translation(interpPos);

			if (m_showTrail)
			{
				m_trailPolygon->AddPoint(interpMat);
			}
		}
	}
}

void Katana::DrawBright()
{
	if (!IMGUI_MANAGER.GetShowEffect()) return;
	KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_trailPolygon, Math::Matrix::Identity, m_trailColor);
}

void Katana::UpdateHand()
{
	m_swordHandData.m_weaponRotationMatrix = Math::Matrix::CreateFromYawPitchRoll
	(
		DirectX::XMConvertToRadians(m_swordHandData.m_weaponDeg.y),
		DirectX::XMConvertToRadians(m_swordHandData.m_weaponDeg.x),
		DirectX::XMConvertToRadians(m_swordHandData.m_weaponDeg.z)
	);

	m_swordData.m_weaponScaleMatrix = Math::Matrix::CreateScale(m_swordData.m_scale);

	m_swordHandData.m_weaponBonesMatrix.Translation(m_swordHandData.m_weaponBonesMatrix.Translation());
	Math::Matrix transOffset = Math::Matrix::CreateTranslation(m_katanaOffset);
	m_swordData.m_weaponMatrix = transOffset * m_swordData.m_weaponScaleMatrix * m_swordHandData.m_weaponRotationMatrix *m_swordHandData.m_weaponBonesMatrix * m_swordData.m_playerWorldMatrix;

}

void Katana::ImGuiInspector()
{
	WeaponBase::ImGuiInspector();

	ImGui::Text("Katana Inspector");

	ImGui::Text(U8("刀の角度を変更"));
	ImGui::DragFloat3("deg", &m_swordData.m_weaponDeg.x, 0.1f);
	ImGui::Text(U8("納刀のオフセットを変更"));
	ImGui::DragFloat3("offset", &m_katanaOffset.x, 0.01f);
	ImGui::Text(U8("手の位置の刀のを変更"));
	ImGui::DragFloat3("Handoffset", &m_katanaHandOffset.x, 0.01f);
	ImGui::Text(U8("刀のスケールを変更"));
	ImGui::DragFloat3("scale", &m_swordData.m_scale.x, 0.01f);

	ImGui::Separator();

	if (ImGui::CollapsingHeader("Sheathing of Katana"))
	{
		ImGui::Text(U8("納刀時の角度を変更"));
		ImGui::DragFloat3("handDeg", &m_swordHandData.m_weaponDeg.x, 0.1f);
	}

	if (ImGui::CollapsingHeader("Hand Katana"))
	{
		ImGui::Text(U8("手持ちの刀の角度を変更"));
		ImGui::DragFloat3("sheathDeg", &m_swordData.m_weaponDeg.x, 0.1f);
	}

	if (ImGui::CollapsingHeader("Trail"))
	{
		ImGui::Text(U8("トレイルの原点"));
		ImGui::DragFloat("trailKatanaPoint", &m_trailKatanaPoint, 0.1f);
		ImGui::Text(U8("トレイルのカラー"));
		ImGui::ColorEdit4("trailColor", &m_trailColor.x);
		ImGui::Text(U8("軌跡のスケールを変更"));
		ImGui::DragFloat3("trailScale", &m_trailScale.x, 0.1f);
	}
}

void Katana::JsonSave(nlohmann::json& _json) const
{
	WeaponBase::JsonSave(_json);
	_json["trailColor"] = JSON_MANAGER.Vector4ToJson(m_trailColor);
	_json["trailKatanaPoint"] = m_trailKatanaPoint;
	_json["trailScale"] = JSON_MANAGER.VectorToJson(m_trailScale);
}

void Katana::JsonInput(const nlohmann::json& _json)
{
	WeaponBase::JsonInput(_json);
	if (_json.contains("trailColor")) m_trailColor = JSON_MANAGER.JsonToVector4(_json["trailColor"]);
	if (_json.contains("trailKatanaPoint")) m_trailKatanaPoint = _json["trailKatanaPoint"].get<float>();
	if (_json.contains("trailScale")) m_trailScale = JSON_MANAGER.JsonToVector(_json["trailScale"]);
}
