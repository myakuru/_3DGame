#include "SlashEffect.h"

const uint32_t SlashEffect::TypeID = KdGameObject::GenerateTypeID();

void SlashEffect::Init()
{
	SelectDraw3dPolygon::Init();
	m_polygon->SetPivot(KdSquarePolygon::PivotType::Center_Bottom);
	// 画像を分割
	m_polygon->SetSplit(4, 4);
}

void SlashEffect::Update()
{
	m_polygon->SetUVRect((int)m_time);

	m_time += 0.1f;

	if (m_time >= 16)
	{
		m_time = 0.0f;
	}
}
