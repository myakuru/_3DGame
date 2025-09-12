#pragma once
#include"../MeshEffectBase.h"
class Attack4Effect : public MeshEffectBase
{
public:

	static const uint32_t TypeID;

	Attack4Effect() { m_type = TypeID; }
	~Attack4Effect() override = default;

	void EffectReset();

private:

	void Init() override;
	void Update() override;
	void EffectControl() override;
	void DrawEffect() override;

	bool m_isWaitForEffect = false;
	float m_effectWaitTime = 0.0f;

	bool m_once = false;

};