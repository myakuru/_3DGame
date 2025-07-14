#pragma once
#include"../WeaponBase.h"
class Katana : public WeaponBase
{
public:
	Katana() = default;
	~Katana() override = default;

	// 刀の行列はPlayerで設定される
	void SetKatanaMatrix(const Math::Matrix& matrix) { m_swordData.m_swordTranslationMatrix = matrix; }

private:

	void Update() override;
	void ImGuiInspector() override;
	void JsonSave(nlohmann::json& _json) const override;
	void JsonInput(const nlohmann::json& _json) override;

};