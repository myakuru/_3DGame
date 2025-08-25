#include "HpBar.h"
#include"../../Utility/Time.h"
#include"../../../Scene/SceneManager.h"
#include"../../Character/Player/Player.h"

const uint32_t HpBar::TypeID = KdGameObject::GenerateTypeID();

void HpBar::Update()
{
	SceneManager::Instance().GetObjectWeakPtr(m_player);

	auto player = m_player.lock();
	if (!player) return;

	int hp = player->GetPlayerStatus().hp;

	m_hpRate = static_cast<float>(hp) / static_cast<float>(player->GetPlayerStatus().hpMax);

	m_hpRate = std::clamp(m_hpRate, 0.0f, 1.0f);

	// 角度の定数
	const Math::Vector3 degreeMax(0.01f, m_degree.y, m_degree.z);      // 最大
	const Math::Vector3 degreeZero(-0.284f, m_degree.y, m_degree.z);  // 最小

	m_degree = Math::Vector3::Lerp(degreeZero, degreeMax, m_hpRate);

	m_mWorld = Math::Matrix::CreateScale(m_scale);
	m_mWorld *= Math::Matrix::CreateFromYawPitchRoll
	(
		DirectX::XMConvertToRadians(m_degree.y),
		DirectX::XMConvertToRadians(m_degree.x),
		DirectX::XMConvertToRadians(m_degree.z)
	);
	m_mWorld.Translation(m_position);
}
