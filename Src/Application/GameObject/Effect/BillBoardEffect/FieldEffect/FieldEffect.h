#pragma once
#include"../../../Utility/SelectDraw3dPolygon.h"
class PlayerCamera;
class FieldEffect : public SelectDraw3dPolygon
{
public:
	static const uint32_t TypeID;

	FieldEffect() { m_type = TypeID; }
	~FieldEffect() override = default;

private:

	void Update() override;

	std::weak_ptr<PlayerCamera> m_camera;
};