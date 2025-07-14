#include "Player.h"
#include"../../../main.h"
#include"../../../../Framework/Json/Json.h"

void Player::Init()
{
	CharaBase::Init();
	m_swordModelData = std::make_shared<KdModelData>();
	m_swordModelData->Load("Asset/Models/Player/katana.gltf");
}

void Player::Update()
{
	CharaBase::Update();

	m_animator->AdvanceTime(m_modelWork->WorkNodes(), 100.f);

	// アニメーション適用後にノード行列を再計算
	m_modelWork->CalcNodeMatrices();

	// ワークノード（アニメーション適用後の行列など）を使う場合
	auto handWorkNode = m_modelWork->FindWorkNode("ring_01_r");
	if (!handWorkNode) return;

	m_swordRotetionMatrix = Math::Matrix::CreateFromYawPitchRoll
	(
		DirectX::XMConvertToRadians(m_weapomDeg.y),
		DirectX::XMConvertToRadians(m_weapomDeg.x),
		DirectX::XMConvertToRadians(m_weapomDeg.z)
	);

	m_swordMatrix = m_swordRotetionMatrix;

	// 手のボーンのワールド行列
	m_swordMatrix *= handWorkNode->m_worldTransform;

}

void Player::DrawToon()
{
	CharaBase::DrawToon();
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_swordModelData, m_swordMatrix);
}

void Player::ImGuiInspector()
{
	CharaBase::ImGuiInspector();

	if (ImGui::Begin("Weapon"))
	{
		ImGui::DragFloat3("pos", &m_weapomDeg.x, 0.1f);
	}
	ImGui::End();
}

void Player::JsonInput(const nlohmann::json& _json)
{
	CharaBase::JsonInput(_json);

	if (_json.contains("weaponDeg")) { m_weapomDeg = JSON_MANAGER.JsonToVector(_json["weaponDeg"]); }
}

void Player::JsonSave(nlohmann::json& _json) const
{
	CharaBase::JsonSave(_json);
	_json["weaponDeg"] = JSON_MANAGER.VectorToJson(m_weapomDeg);
}
