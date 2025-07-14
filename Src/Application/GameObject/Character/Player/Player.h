#pragma once
#include "../CharacterBase.h"
class Katana;
class Player :public CharaBase
{
public:
	Player() = default;
	~Player() override = default;

	void Init() override;
	void PreUpdate() override;
	void Update() override;
	void DrawToon() override;
	void ImGuiInspector() override;
	void JsonInput(const nlohmann::json& _json) override;
	void JsonSave(nlohmann::json& _json) const override;

private:

	std::weak_ptr<Katana> m_katana;
};