#pragma once
#include"../EffekseerEffectBase.h"
class ShineEffect :public EffekseerEffectBase
{
public:

	static const uint32_t TypeID;

	ShineEffect() { m_type = TypeID; }
	~ShineEffect() override = default;

};