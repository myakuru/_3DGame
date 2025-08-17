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
	void DrawSprite() override;
	void Update() override;

	Math::Rectangle m_srcRect;	// スコアの表示位置

};