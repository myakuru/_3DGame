﻿#pragma once
#include"../EffekseerEffectBase.h"
class SlashAttack2Effect :public EffekseerEffectBase
{
public:

	static const uint32_t TypeID;

	SlashAttack2Effect() { m_type = TypeID; }
	~SlashAttack2Effect() override = default;

};