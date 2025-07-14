#pragma once
#include "../CharacterBase.h"
class Player :public CharaBase
{
public:
	Player() = default;
	~Player() override = default;

	void Init() override;
	void Update() override;	
	void DrawToon() override;
	void ImGuiInspector() override;
	void JsonInput(const nlohmann::json& _json) override;
	void JsonSave(nlohmann::json& _json) const override;

private:

	Math::Matrix m_swordMatrix = Math::Matrix::Identity;
	Math::Matrix m_swordRotetionMatrix = Math::Matrix::Identity;

	Math::Vector3 m_weapomDeg = Math::Vector3::Zero;

	std::shared_ptr<KdModelData>	m_swordModelData;
};