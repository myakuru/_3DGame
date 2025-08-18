#pragma once
#include "../../Utility/SelectDraw2DTexture.h"
class NowHp :public SelectDraw2DTexture
{
public:
	static const uint32_t TypeID;

	NowHp() { m_typeID = TypeID; }
	~NowHp() override = default;


private:

	void Init() override;
	void Update() override;
	void DrawSprite() override;

	int m_displayTime = 0;
	bool m_isIncreasing = false;
};