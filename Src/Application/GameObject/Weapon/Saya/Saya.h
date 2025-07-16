#pragma once
#include"../WeaponBase.h"
class Saya : public WeaponBase
{
public:
	Saya() = default;
	~Saya() override = default;

	// 鞘の行列はPlayerで設定される
	void SetSayaMatrix(const Math::Matrix& matrix) { m_sayaData.m_weaponTranslationMatrix = matrix; }
	
	// 鞘の最終的な行列を取得する
	Math::Matrix GetTransSwordMatrix() const { return m_sayaData.m_weaponTranslationMatrix; }
	Math::Matrix GetRotationSwordMatrix() const { return m_sayaData.m_weaponRotationMatrix; }

	

private:

	void Update() override;
	void ImGuiInspector() override;
	void DrawToon() override;
	void JsonSave(nlohmann::json& _json) const override;
	void JsonInput(const nlohmann::json& _json) override;
};