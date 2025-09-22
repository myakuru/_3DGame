#pragma once
#include"../EffekseerEffectBase.h"
class Rotation : public EffekseerEffectBase
{
public:

	static const uint32_t TypeID;
	Rotation() { m_type = TypeID; }
	~Rotation() override = default;
};