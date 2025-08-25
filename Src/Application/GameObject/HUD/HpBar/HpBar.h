#pragma once
#include "../../Utility/SelectDraw2DTexture.h"
class Player;
class HpBar : public SelectDraw2DTexture
{
public:
	static const uint32_t TypeID;
	HpBar() { m_type = TypeID; }
	~HpBar() override = default;

	void Update() override;

private:
	Math::Rectangle m_srcRect;
	float m_hpRate = 0.0f;

	std::weak_ptr<Player> m_player;
};