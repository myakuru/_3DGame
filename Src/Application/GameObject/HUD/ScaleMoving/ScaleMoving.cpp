#include "ScaleMoving.h"
const uint32_t ScaleMoving::TypeID = KdGameObject::GenerateTypeID();


void ScaleMoving::Update()
{
	Math::Matrix scale = Math::Matrix::CreateScale(m_scale);
	Math::Matrix translation = Math::Matrix::CreateTranslation(m_position);
	Math::Matrix rotationY = Math::Matrix::CreateRotationY(m_degree.y);

	Math::Matrix rotationX = Math::Matrix::CreateRotationX(m_degree.x);
	Math::Matrix rotationZ = Math::Matrix::CreateRotationZ(m_degree.z);

	m_mWorld = scale * rotationX *rotationY * rotationZ * translation;
}