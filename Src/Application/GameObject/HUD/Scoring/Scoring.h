#pragma once
#include "../../Utility/SelectDraw2DTexture.h"
class Scoring :public SelectDraw2DTexture
{
public:
	static const uint32_t TypeID;

	Scoring() { m_typeID = TypeID; }
	~Scoring() override = default;

private:

	void Init() override;
	void Update() override;
	void DrawSprite() override;

	Math::Rectangle m_srcRect = { 0,0,150,50 };	// スコアの表示位置

	int m_displayTime = 0;

};