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
	void SetKatanaMatrix(const Math::Matrix& _matrix) { m_swordData.m_weaponTranslationMatrix = _matrix; }
	void SetPlayerMatrix(const Math::Matrix& _matrix) { m_swordData.m_playerTranslationMatrix = _matrix; }

	void SetHandKatanaMatrix(const Math::Matrix& _matrix) { m_swordHandData.m_weaponTranslationMatrix = _matrix; }
	void SetPlayerHandMatrix(const Math::Matrix& _matrix) { m_swordHandData.m_playerTranslationMatrix = _matrix; }

private:

	void Init() override;
	void Update() override;
	void DrawUnLit() override;
	void DrawToon() override;
	void DrawBright() override;
	void UpdateHand();
	void ImGuiInspector() override;
	void JsonSave(nlohmann::json& _json) const override;
	void JsonInput(const nlohmann::json& _json) override;

	std::shared_ptr<KdTrailPolygon> m_trailPolygon; // 軌跡ポリゴン

	std::shared_ptr<KdTexture> m_trailTex = std::make_shared<KdTexture>(); // 軌跡テクスチャ

	Math::Matrix m_trailOffset = Math::Matrix::Identity; // 軌跡ポリゴンのオフセット

	Math::Matrix m_trailRotation = Math::Matrix::Identity; // 軌跡ポリゴンの回転

	Math::Vector3 m_katanaOffset = Math::Vector3::Zero;
	Math::Vector3 m_katanaHandOffset = Math::Vector3::Zero; // 手に持つ刀のオフセット
};