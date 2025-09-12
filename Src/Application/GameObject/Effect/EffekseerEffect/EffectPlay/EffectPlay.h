#pragma once
#include"../EffekseerEffectBase.h"
class EffectPlay : public EffekseerEffectBase
{
public:
	static const uint32_t TypeID;

	EffectPlay() { m_type = TypeID; }
	~EffectPlay() override =  default;

};