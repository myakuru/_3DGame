#include "ScaleMoving.h"
#include"../../Utility/Time.h"
const uint32_t ScaleMoving::TypeID = KdGameObject::GenerateTypeID();


void ScaleMoving::Update()
{
	float timeLeft = Time::Instance().GetCountdownTimeLeft();
	float maxTime = 120; // タイマーの最大値を取得

	// 角度の定数（Vector3型で定義）
	const Math::Vector3 degreeMax(0.0f, m_degree.y, m_degree.z);      // タイマー最大
	const Math::Vector3 degreeHalf(-0.050f, m_degree.y, m_degree.z);  // タイマー半分
	const Math::Vector3 degreeZero(-0.094f, m_degree.y, m_degree.z);  // タイマーゼロ

	// 残り時間の割合
	float t = 1.0f - (timeLeft / maxTime);

	if (t < 0.5f) {
		// 最大～半分まで線形補間
		m_degree = Math::Vector3::Lerp(degreeMax, degreeHalf, t / 0.5f);
	}
	else {
		// 半分～ゼロまで線形補間
		m_degree = Math::Vector3::Lerp(degreeHalf, degreeZero, (t - 0.5f) / 0.5f);
	}

	m_mWorld = Math::Matrix::CreateScale(m_scale);
	m_mWorld *= Math::Matrix::CreateFromYawPitchRoll(
		DirectX::XMConvertToRadians(m_degree.y),
		DirectX::XMConvertToRadians(m_degree.x),
		DirectX::XMConvertToRadians(m_degree.z)
	);
	m_mWorld.Translation(m_position);

}