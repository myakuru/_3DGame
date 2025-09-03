#pragma once
#include"../MeshEffectBase.h"
class AttackEffect : public MeshEffectBase
{
public:

	static const uint32_t TypeID;

	AttackEffect() { m_type = TypeID; }
	~AttackEffect() override = default;

private:

	void Update() override;
	void EffectControl();

};