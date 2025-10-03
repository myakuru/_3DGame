#pragma once
#include"../WeaponBase.h"
class EnemyShield : public WeaponBase
{
public:

	static const uint32_t TypeID;

	EnemyShield() { m_typeID = TypeID; }
	~EnemyShield() override = default;

private:

	void Update() override;

};