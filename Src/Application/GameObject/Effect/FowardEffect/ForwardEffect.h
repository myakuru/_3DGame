#pragma once
class Player;
class ForwardEffect : public KdGameObject
{
public:

	static const uint32_t TypeID;

	ForwardEffect() { m_typeID = TypeID; }
	~ForwardEffect() override = default;

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


	std::weak_ptr<Player> m_player;

	float m_time = 0.0f;
	bool m_once = false;

};