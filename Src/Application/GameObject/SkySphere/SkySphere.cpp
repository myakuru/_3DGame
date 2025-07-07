#include "SkySphere.h"

void SkySphere::Init()
{
	KdGameObject::Init();
	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("ModelMesh", m_model, KdCollider::TypeEvent);
}
