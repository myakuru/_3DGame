#pragma once
#include"../Utility/SelectDraw3dModel.h"
class WeaponBase : public SelectDraw3dModel
{
public:
	WeaponBase() = default;
	~WeaponBase() override = default;

protected:

	void Init() override;
	void DrawToon() override;
	void Update() override;
	void ImGuiInspector() override;
	void JsonSave(nlohmann::json& _json) const override;
	void JsonInput(const nlohmann::json& _json) override;

	// 刀の構造体
	struct SwordData
	{
		Math::Matrix m_weaponMatrix = Math::Matrix::Identity;			// 刀の行列
		Math::Matrix m_weaponTranslationMatrix = Math::Matrix::Identity;	// 刀の位置行列
		Math::Matrix m_playerTranslationMatrix = Math::Matrix::Identity;// プレイヤーの位置行列
		Math::Matrix m_weaponRotationMatrix = Math::Matrix::Identity;	// 刀の回転行列
		Math::Matrix m_weaponScaleMatrix = Math::Matrix::Identity;		// 刀の拡大縮小行列
		Math::Vector3 m_weaponDeg = Math::Vector3::Zero;				// 刀の角度
		Math::Vector3 m_scale = { 1.0f,1.0f,1.0f };	// 刀の拡大縮小
	};

	SwordData m_swordData;	// 刀のデータ
	SwordData m_swordHandData;
};