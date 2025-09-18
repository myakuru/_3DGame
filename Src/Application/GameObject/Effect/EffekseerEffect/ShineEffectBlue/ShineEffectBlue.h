#pragma once
#include"../EffekseerEffectBase.h"
class ShineEffectBlue : public EffekseerEffectBase
{
public:

	static const uint32_t TypeID;

	ShineEffectBlue() { m_type = TypeID; }
	~ShineEffectBlue() override = default;

};