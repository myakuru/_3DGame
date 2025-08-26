#include "Katana.h"
#include"../../../main.h"
#include"../../../../Framework/Json/Json.h"
#include"../../../Scene/SceneManager.h"

// TypeIDの定義と初期化
const uint32_t Katana::TypeID = KdGameObject::GenerateTypeID();

void Katana::Init()
{
	WeaponBase::Init();
	m_trailPolygon = std::make_shared<KdTrailPolygon>();
	m_trailPolygon->ClearPoints();
	m_trailPolygon->SetLength(80);

	m_trailTex->Load("Asset/Textures/NA_Basic hit_006.png");
	m_trailPolygon->SetMaterial(m_trailTex);
	m_trailPolygon->SetPattern(KdTrailPolygon::Trail_Pattern::eBillboard);
}

void Katana::Update()
{
	if (m_swordHandData.m_weaponTranslationMatrix != Math::Matrix::Identity)
	{
		UpdateHand();
	}
	else
	{
		m_swordData.m_weaponRotationMatrix = Math::Matrix::CreateFromYawPitchRoll
		(
			DirectX::XMConvertToRadians(m_swordData.m_weaponDeg.y),
			DirectX::XMConvertToRadians(m_swordData.m_weaponDeg.x),
			DirectX::XMConvertToRadians(m_swordData.m_weaponDeg.z)
		);

		// 移動行列はPlayer側から手の位置を取得しているので、下記で最終的に行列に入れてる。
		Math::Vector3 playerHipPos = m_swordData.m_weaponTranslationMatrix.Translation();
		// プレイヤーの後ろにある剣の位置を計算(エディターでいじれるようになっている)
		m_swordData.m_weaponTranslationMatrix.Translation(m_katanaOffset + playerHipPos);
		m_swordData.m_weaponScaleMatrix = Math::Matrix::CreateScale(m_swordData.m_scale);
		// プレイヤーに追尾するように、プレイヤーの位置を取得して、剣の位置を更新
		m_swordData.m_weaponMatrix = m_swordData.m_weaponScaleMatrix * m_swordData.m_weaponRotationMatrix * m_swordData.m_weaponTranslationMatrix * m_swordData.m_playerTranslationMatrix;
		m_trailOffset = m_swordData.m_weaponScaleMatrix * m_swordData.m_weaponRotationMatrix * m_swordData.m_playerTranslationMatrix;
	}

}

void Katana::DrawUnLit()
{
	KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_trailPolygon, Math::Matrix::Identity, { 0.0f,1.0f,1.0f,1.0f });
}

void Katana::DrawToon()
{
	//KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_trailPolygon);
}

void Katana::DrawBright()
{
	KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_trailPolygon, Math::Matrix::Identity, { 0.0f,1.0f,1.0f,1.0f });
}

void Katana::UpdateHand()
{
	m_swordHandData.m_weaponRotationMatrix = Math::Matrix::CreateFromYawPitchRoll
	(
		DirectX::XMConvertToRadians(m_swordHandData.m_weaponDeg.y),
		DirectX::XMConvertToRadians(m_swordHandData.m_weaponDeg.x),
		DirectX::XMConvertToRadians(m_swordHandData.m_weaponDeg.z)
	);

	m_swordHandData.m_weaponTranslationMatrix.Translation(m_swordHandData.m_weaponTranslationMatrix.Translation());
	m_swordData.m_weaponMatrix = m_swordData.m_weaponScaleMatrix * m_swordHandData.m_weaponRotationMatrix *m_swordHandData.m_weaponTranslationMatrix * m_swordData.m_playerTranslationMatrix;


	Math::Vector3 sakkityo = m_swordData.m_weaponMatrix.Translation() + m_swordData.m_weaponMatrix.Up() * 5.0f;

	Math::Matrix sakkityoMat = Math::Matrix::CreateTranslation(sakkityo);

	Math::Matrix inv = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(90.0f));

	Math::Matrix scale = Math::Matrix::CreateScale(1.0f);


	m_trailPolygon->AddPoint(scale * inv * (m_swordData.m_weaponRotationMatrix * sakkityoMat));

}

void Katana::ImGuiInspector()
{
	KdGameObject::ImGuiInspector();

	ImGui::Text("Katana Inspector");

	ImGui::Text(U8("刀の角度を変更"));
	ImGui::DragFloat3("deg", &m_swordData.m_weaponDeg.x, 0.1f);
	ImGui::Text(U8("刀の位置を変更"));
	ImGui::DragFloat3("offset", &m_katanaOffset.x, 0.01f);
	ImGui::Text(U8("刀のスケールを変更"));
	ImGui::DragFloat3("scale", &m_swordData.m_scale.x, 0.01f);

	ImGui::Separator();

	if (ImGui::CollapsingHeader("Hand Katana"))
	{
		ImGui::Text(U8("手持ちの刀の角度を変更"));
		ImGui::DragFloat3("handDeg", &m_swordHandData.m_weaponDeg.x, 0.1f);
	}

	UpdateHand();
}

void Katana::JsonSave(nlohmann::json& _json) const
{
	WeaponBase::JsonSave(_json);
	_json["weaponDeg"] = JSON_MANAGER.VectorToJson(m_swordData.m_weaponDeg);
	_json["katanaOffset"] = JSON_MANAGER.VectorToJson(m_katanaOffset);
	_json["scale"] = JSON_MANAGER.VectorToJson(m_swordData.m_scale);
	_json["handWeaponDeg"] = JSON_MANAGER.VectorToJson(m_swordHandData.m_weaponDeg);
}

void Katana::JsonInput(const nlohmann::json& _json)
{
	WeaponBase::JsonInput(_json);
	if (_json.contains("weaponDeg")) m_swordData.m_weaponDeg = JSON_MANAGER.JsonToVector(_json["weaponDeg"]);
	if (_json.contains("katanaOffset")) m_katanaOffset = JSON_MANAGER.JsonToVector(_json["katanaOffset"]);
	if (_json.contains("scale")) m_swordData.m_scale = JSON_MANAGER.JsonToVector(_json["scale"]);
	if (_json.contains("handWeaponDeg")) m_swordHandData.m_weaponDeg = JSON_MANAGER.JsonToVector(_json["handWeaponDeg"]);
}
