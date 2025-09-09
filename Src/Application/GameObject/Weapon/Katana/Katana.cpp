#include "Katana.h"
#include"../../../main.h"
#include"../../../Scene/SceneManager.h"
#include"../../../../Framework/Json/Json.h"

// TypeIDの定義と初期化
const uint32_t Katana::TypeID = KdGameObject::GenerateTypeID();

void Katana::Init()
{
	WeaponBase::Init();
	m_trailPolygon = std::make_shared<KdTrailPolygon>();

	m_trailPolygon->ClearPoints();
	m_trailPolygon->SetLength(100);
	m_trailTex = KdAssets::Instance().m_textures.GetData("Asset/Textures/trajectory.png");

	m_trailPolygon->SetMaterial(m_trailTex);
	m_showTrail = false;
}

void Katana::Update()
{
	// 剣の行列計算（元の処理そのまま）
	if (!m_isAttackState)
	{
		UpdateHand();
	}
	else
	{
		m_swordData.m_weaponRotationMatrix = m_swordData.m_weaponBonesMatrix.CreateFromYawPitchRoll
		(
			DirectX::XMConvertToRadians(m_swordData.m_weaponDeg.y),
			DirectX::XMConvertToRadians(m_swordData.m_weaponDeg.x),
			DirectX::XMConvertToRadians(m_swordData.m_weaponDeg.z)
		);

		m_swordData.m_weaponScaleMatrix = Math::Matrix::CreateScale(m_swordData.m_scale);

		m_swordData.m_weaponBonesMatrix.Translation(m_swordHandData.m_weaponBonesMatrix.Translation());
		Math::Matrix transOffset = Math::Matrix::CreateTranslation(m_katanaHandOffset);
		m_swordData.m_weaponMatrix = transOffset * m_swordData.m_weaponScaleMatrix * m_swordData.m_weaponRotationMatrix * m_swordHandData.m_weaponBonesMatrix * m_swordHandData.m_playerWorldMatrix;
		
		// 軌跡の先端位置を計算
		
		Math::Vector3 tip1 = Math::Vector3::Zero;

		tip1 = m_swordData.m_weaponMatrix.Translation() + m_swordData.m_weaponMatrix.Up() * 1.0f;

		Math::Matrix inv = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(70.0f));

		Math::Matrix trailTrans = Math::Matrix::CreateTranslation(tip1);

		Math::Matrix trailScale = Math::Matrix::CreateScale({ 1.0f,1.0f,1.0f });

		Math::Matrix finalMat = trailScale * inv * (m_swordData.m_weaponRotationMatrix * trailTrans);

		if (m_showTrail)
		{
			m_trailPolygon->AddPoint(finalMat);
		}
	}
}

void Katana::DrawBright()
{
	if (m_showTrail) 
	{
		KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_trailPolygon, Math::Matrix::Identity, m_trailColor);
	}
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
	KdGameObject::ImGuiInspector();

	ImGui::Text("Katana Inspector");

	ImGui::Text(U8("刀の角度を変更"));
	ImGui::DragFloat3("deg", &m_swordData.m_weaponDeg.x, 0.1f);
	ImGui::Text(U8("納刀のオフセットを変更"));
	ImGui::DragFloat3("offset", &m_katanaOffset.x, 0.01f);
	ImGui::Text(U8("手の位置の刀のを変更"));
	ImGui::DragFloat3("Handoffset", &m_katanaHandOffset.x, 0.01f);
	ImGui::Text(U8("刀のスケールを変更"));
	ImGui::DragFloat3("scale", &m_swordData.m_scale.x, 0.01f);


	ImGui::ColorEdit4("trailColor", &m_trailColor.x);

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
}

void Katana::JsonSave(nlohmann::json& _json) const
{
	WeaponBase::JsonSave(_json);
	_json["trailColor"] = JSON_MANAGER.Vector4ToJson(m_trailColor);
}

void Katana::JsonInput(const nlohmann::json& _json)
{
	WeaponBase::JsonInput(_json);
	if (_json.contains("trailColor")) m_trailColor = JSON_MANAGER.JsonToVector4(_json["trailColor"]);
}
