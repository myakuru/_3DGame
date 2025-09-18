#pragma once
#include"../EffekseerEffectBase.h"
class SwordFlash : public EffekseerEffectBase
{
public:

	static const uint32_t TypeID;

	SwordFlash() { m_type = TypeID; }
	~SwordFlash() override = default;

};