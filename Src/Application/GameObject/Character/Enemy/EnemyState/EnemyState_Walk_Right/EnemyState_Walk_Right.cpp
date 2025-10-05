#include "EnemyState_Walk_Right.h"
#include"../../../Player/Player.h"
#include"../EnemyState_Idle/EnemyState_Idle.h"
#include"../EnemyState_Attack/EnemyState_Attack.h"
#include"../../../../../main.h"

void EnemyState_Walk_Right::StateStart()
{
	auto anime = m_enemy->GetAnimeModel()->GetAnimation("Walk_Right");
	m_enemy->GetAnimator()->SetAnimation(anime);
	m_enemy->SetAnimeSpeed(60.0f);

	m_time = 0.0f;
}

void EnemyState_Walk_Right::StateUpdate()
{

	float deltaTime = Application::Instance().GetDeltaTime();

	m_time += deltaTime;


	// 自身の横方向ベクトル
	Math::Vector3 right = Math::Vector3::TransformNormal
	(
		Math::Vector3::Right, 
		Math::Matrix::CreateFromQuaternion(m_enemy->GetRotationQuaternion())
	);

	right.Normalize();

	// 右に少しずつ移動
	m_enemy->SetIsMoving(-right * 0.5f);

	if(m_time >= 5.0f)
	{
		//Attackステートに移行
		auto attack = std::make_shared<EnemyState_Attack>();
		m_enemy->ChangeState(attack);
		return;
	}
	
}

void EnemyState_Walk_Right::StateEnd()
{
}
