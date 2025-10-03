#pragma once
#include"../MeshEffectBase.h"
class AttackEffect : public MeshEffectBase
{
public:

	static const uint32_t TypeID;

	AttackEffect() { m_typeID = TypeID; }
	~AttackEffect() override = default;

	void EffectReset();

private:

	void Init() override;
	void Update() override;
	void EffectControl() override;
	void DrawEffect() override;

};