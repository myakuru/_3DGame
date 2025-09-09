#pragma once
class Katana;
class TrailEffect : public KdGameObject
{
public:

	static const uint32_t TypeID;
	TrailEffect() { m_type = TypeID; }
	~TrailEffect() override = default;

	void Clear() { m_trailPolygon->ClearPoints(); }

	void SetEnableTrail(bool enable) { m_enableTrail = enable; }

private:

	void Init() override;
	void DrawBright() override;
	void DrawToon() override;
	void Update() override;

	void ImGuiInspector() override;
	void JsonSave(nlohmann::json& _json) const override;
	void JsonInput(const nlohmann::json& _json) override;

	bool m_enableTrail = false;
	std::shared_ptr<KdTrailPolygon> m_trailPolygon; // 軌跡ポリゴン
	std::shared_ptr<KdTrailPolygon> m_trailPolygon1; // 軌跡ポリゴン
	std::shared_ptr<KdTrailPolygon> m_trailPolygon2; // 軌跡ポリゴン

	std::shared_ptr<KdTexture> m_trailTex = std::make_shared<KdTexture>(); // 軌跡テクスチャ
	Math::Matrix m_katanaMat = Math::Matrix::Identity; // 軌跡ポリゴンの最終行列

	Math::Vector3 m_trailScale = Math::Vector3::Zero;
	Math::Matrix m_trailRot = Math::Matrix::Identity;

	Math::Matrix m_finalMat = Math::Matrix::Identity; // 軌跡ポリゴンの最終行列

	std::weak_ptr<Katana> m_katana;

};