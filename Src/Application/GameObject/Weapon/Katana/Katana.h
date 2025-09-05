#pragma once
#include"../WeaponBase.h"

class Katana : public WeaponBase
{
public:
	// クラスごとに一意なTypeIDを持たせる
	static const uint32_t TypeID;

	Katana() { m_typeID = TypeID; }
	~Katana() override = default;

	std::shared_ptr<KdTrailPolygon> GetTrail() { return m_trailPolygon; }
	std::shared_ptr<KdTrailPolygon> GetTrail2() { return m_trailPolygon2; }
	std::shared_ptr<KdTrailPolygon> GetTrail3() { return m_trailPolygon3; }

	bool& EnableTrail() { return m_enableTrail; }


private:

	void Init() override;
	void Update() override;
	void DrawLit() override;
	void DrawUnLit() override;
	void DrawBright() override;
	void UpdateHand();
	void ImGuiInspector() override;

	Math::Vector3 CatmullRom(const Math::Vector3& p0, const Math::Vector3& p1, const Math::Vector3& p2, const Math::Vector3& p3, float t)
	{
		return 0.5f * (
			(2.0f * p1) +
			(-p0 + p2) * t +
			(2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t * t +
			(-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t * t * t
			);
	}

	std::shared_ptr<KdTrailPolygon> m_trailPolygon; // 軌跡ポリゴン
	std::shared_ptr<KdTrailPolygon> m_trailPolygon2; // 軌跡ポリゴン用モデル
	std::shared_ptr<KdTrailPolygon> m_trailPolygon3; // 軌跡ポリゴン用モデル

	std::shared_ptr<KdTexture> m_trailTex = std::make_shared<KdTexture>(); // 軌跡テクスチャ
	std::shared_ptr<KdTexture> m_trailTex2 = std::make_shared<KdTexture>(); // 軌跡テクスチャ
	std::shared_ptr<KdTexture> m_trailTex3 = std::make_shared<KdTexture>(); // 軌跡テクスチャ

	bool m_enableTrail = true;


	Math::Matrix m_trailOffset = Math::Matrix::Identity; // 軌跡ポリゴンのオフセット

	Math::Matrix m_finalMat = Math::Matrix::Identity; // 軌跡ポリゴンの最終行列
	Math::Matrix m_finalMat2 = Math::Matrix::Identity; // 軌跡ポリゴンの最終行列
	Math::Matrix m_finalMat3 = Math::Matrix::Identity; // 軌跡ポリゴンの最終行列


	std::shared_ptr<KdModelData> m_trailModel = std::make_shared<KdModelData>(); // 軌跡ポリゴン用モデル

	Math::Matrix m_trailRotation = Math::Matrix::Identity; // 軌跡ポリゴンの回転
};