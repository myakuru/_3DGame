#pragma once
#include"../Utility/SelectDraw3dModel.h"
class WeaponBase : public SelectDrawObject
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
		Math::Matrix m_swordMatrix = Math::Matrix::Identity;			// 刀の行列
		Math::Matrix m_swordTranslationMatrix = Math::Matrix::Identity;	// 刀の位置行列
		Math::Matrix m_swordRotationMatrix = Math::Matrix::Identity;	// 刀の回転行列
		Math::Vector3 m_weaponDeg = Math::Vector3::Zero;				// 刀の角度
	};

	SwordData m_swordData;	// 刀のデータ

};