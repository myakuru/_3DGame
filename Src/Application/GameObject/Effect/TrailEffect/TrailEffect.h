#pragma once
class TrailEffect : public KdGameObject
{
public:
	TrailEffect() = default;
	~TrailEffect() override = default;

	void Init() override;
	void Update() override;
	void DrawBright() override;

private:

	std::shared_ptr<KdTrailPolygon> m_trailPolygon; // 軌跡ポリゴン

};