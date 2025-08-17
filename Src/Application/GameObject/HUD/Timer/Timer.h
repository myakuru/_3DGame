#pragma once
#include "../../Utility/SelectDraw2DTexture.h"
class Timer :public SelectDraw2DTexture
{
public:

	static const uint32_t TypeID;

	Timer();
	~Timer() override = default;

private:
	void Init() override;
	void Update() override;
	void DrawSprite() override;

	float m_time = 0.0;	// 経過時間
	Math::Rectangle m_srcRect;
	int m_displayTime = 0;

};