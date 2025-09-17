#pragma once
#include"../EffekseerEffectBase.h"
class CloudEffect : public EffekseerEffectBase
{
public:
	static const uint32_t TypeID;

	CloudEffect() { m_type = TypeID; }
	~CloudEffect() override = default;

private:

	void Init() override;
	void EffectUpdate() override;
	void Update() override;
	void DrawEffect() override;
};