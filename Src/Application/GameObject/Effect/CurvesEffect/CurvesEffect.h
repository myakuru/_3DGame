#pragma once
class Player;
class PlayerCamera;
class CurvesEffect : public KdGameObject
{
public:

	static const uint32_t TypeID;

	CurvesEffect() { m_typeID = TypeID; }
	~CurvesEffect() override = default;

	void Init() override;
	void Update() override;
	void DrawBright() override;

private:

	std::shared_ptr<KdTrailPolygon> m_trailPolygon; // 軌跡ポリゴン
	std::shared_ptr<KdTrailPolygon> m_trailPolygon2; // 軌跡ポリゴン用モデル
	std::shared_ptr<KdTrailPolygon> m_trailPolygon3; // 軌跡ポリゴン用モデル

	std::shared_ptr<KdTexture> m_trailTex = std::make_shared<KdTexture>(); // 軌跡テクスチャ
	float m_trailProgress = 0.0f;

	Math::Vector3 m_trailStartPos = Math::Vector3::Zero;
	Math::Vector3 m_trailDirection = Math::Vector3::Zero;

	std::vector<std::shared_ptr<KdTrailPolygon>> m_trails; // 軌跡ポリゴンのリスト

	std::weak_ptr<Player> m_player;
	std::weak_ptr<PlayerCamera> m_playerCamera;


	bool m_isTrailActive = false;
	float m_time = 0.0f;

};