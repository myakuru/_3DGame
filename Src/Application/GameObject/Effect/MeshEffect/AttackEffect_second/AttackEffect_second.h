#pragma once
#include"../MeshEffectBase.h"
class AttackEffect_second : public MeshEffectBase
{
public:
	
	static const uint32_t TypeID;

	AttackEffect_second() { m_typeID = TypeID; }
	~AttackEffect_second() override = default;

	void EffectReset();

private:

	void Init() override;
	void Update() override;
	void EffectControl() override;
	void DrawEffect() override;

};