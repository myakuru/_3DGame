#include "HpBar.h"
#include"../../Utility/Time.h"
const uint32_t HpBar::TypeID = KdGameObject::GenerateTypeID();

void HpBar::Update()
{
	// HPバーの割合をキー入力で変更
	if (KeyboardManager::GetInstance().IsKeyPressed('1')) { // 1キー
		m_hpRate -= 0.01f;
	}
	if (KeyboardManager::GetInstance().IsKeyPressed('2')) { // 2キー
		m_hpRate += 0.01f;
	}
	m_hpRate = std::clamp(m_hpRate, 0.0f, 1.0f);

	// 角度の定数
	const Math::Vector3 degreeMax(0.01f, m_degree.y, m_degree.z);      // 最大
	const Math::Vector3 degreeZero(-0.284f, m_degree.y, m_degree.z);  // 最小

	// マックスからゼロまでラープ
	m_degree = Math::Vector3::Lerp(degreeMax, degreeZero, m_hpRate);

	m_mWorld = Math::Matrix::CreateScale(m_scale);
	m_mWorld *= Math::Matrix::CreateFromYawPitchRoll(
		DirectX::XMConvertToRadians(m_degree.y),
		DirectX::XMConvertToRadians(m_degree.x),
		DirectX::XMConvertToRadians(m_degree.z)
	);
	m_mWorld.Translation(m_position);
}
