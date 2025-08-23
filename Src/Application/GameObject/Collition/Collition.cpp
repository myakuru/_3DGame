#include "Collition.h"

const uint32_t Collision::TypeID = KdGameObject::GenerateTypeID();

Collision::Collision()
{
	m_type = TypeID;
}
