#pragma once
#include"../EffekseerEffectBase.h"
class AvoidAttackSlashEffect : public EffekseerEffectBase
{
public:

	static const uint32_t TypeID;
	AvoidAttackSlashEffect() { m_type = TypeID; }
	~AvoidAttackSlashEffect() override = default;
};