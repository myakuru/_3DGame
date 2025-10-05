#pragma once
#include"../WeaponBase.h"
class WeaponKatanaScabbard : public WeaponBase
{
public:

	static const uint32_t TypeID;

	WeaponKatanaScabbard() {m_type = TypeID;}
	~WeaponKatanaScabbard() override = default;

private:

	void Init() override;
	void Update() override;
	void UpdateMatrix();
	void ImGuiInspector() override;

};