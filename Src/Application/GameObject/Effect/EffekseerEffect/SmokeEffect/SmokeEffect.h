#pragma once
#include"../EffekseerEffectBase.h"
class SmokeEffect : public EffekseerEffectBase
{
public:

	static const uint32_t TypeID;

	SmokeEffect() { m_type = TypeID; }
	~SmokeEffect() override = default;

};