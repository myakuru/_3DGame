#pragma once
#include"../../Utility/SelectDraw3dPolygon.h"
class SlashEffect : public SelectDraw3dPolygon
{
public:

	static const uint32_t TypeID;
	SlashEffect() { m_type = TypeID; }
	~SlashEffect() override = default;

private:

	void Init() override;
	void Update() override;

	float m_time = 0.0f;
};