#pragma once
#include"../WeaponBase.h"

class Katana : public WeaponBase
{
public:
	// クラスごとに一意なTypeIDを持たせる
	static const uint32_t TypeID;

	Katana() { m_typeID = TypeID; }
	~Katana() override = default;

	// 刀の行列はPlayerで設定される
	void SetKatanaMatrix(const Math::Matrix& matrix) { m_swordData.m_weaponTranslationMatrix = matrix; }
	void SetPlayerMatrix(const Math::Matrix& matrix) { m_swordData.m_playerTranslationMatrix = matrix; }

private:
	void Update() override;
	void ImGuiInspector() override;
	void JsonSave(nlohmann::json& _json) const override;
	void JsonInput(const nlohmann::json& _json) override;

	Math::Vector3 m_katanaOffset = Math::Vector3::Zero;
};