#include "CharacterBase.h"
#include"Player/PlayerState/PlayerState.h"
#include"../../main.h"
#include"../../Scene/SceneManager.h"

void CharaBase::Init()
{
	ModelLoad(m_path);
	//m_animator->SetAnimation(m_modelWork->GetData()->GetAnimation(0));

	m_mRotation = Math::Matrix::CreateFromYawPitchRoll
	(
		DirectX::XMConvertToRadians(m_degree.y),
		DirectX::XMConvertToRadians(m_degree.x),
		DirectX::XMConvertToRadians(m_degree.z)
	);

	//m_trailPolygon.SetMaterial("Asset/Textures/System/WhiteNoise.png");
}

void CharaBase::DrawToon()
{
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_modelWork, m_mWorld, m_color);
	//KdShaderManager::Instance().m_StandardShader.DrawPolygon(m_trailPolygon);
}

void CharaBase::DrawLit()
{
}

void CharaBase::Update()
{
	KdGameObject::Update();

	float deltaTime = Application::Instance().GetDeltaTime();

	m_animator->AdvanceTime(m_modelWork->WorkNodes(), m_fixedFrameRate * deltaTime);

	// 移動関係
	m_gravity += m_gravitySpeed * deltaTime;

	// 最終的な移動量
	m_position.x += m_movement.x * m_moveSpeed * m_fixedFrameRate * deltaTime;
	m_position.z += m_movement.z * m_moveSpeed * m_fixedFrameRate * deltaTime;
	//m_position.y += m_gravity;

	// ステートで移動処理など管理
	m_stateManager.Update();

	// 最終的なワールド行列計算
	m_mWorld = Math::Matrix::CreateScale(m_scale);
	m_mWorld *= m_mRotation;
	m_mWorld.Translation(m_position);

	// トレイルポリゴンの更新
	//m_trailPolygon.AddPoint(m_mWorld);

}

void CharaBase::PreUpdate()
{

}

bool CharaBase::ModelLoad(std::string _path)
{
	if (m_modelWork)
	{
		m_modelWork->SetModelData(_path);
		return true;
	}
	return false;
}

void CharaBase::ImGuiInspector()
{
	KdGameObject::ImGuiInspector();
}

void CharaBase::JsonInput(const nlohmann::json& _json)
{
	KdGameObject::JsonInput(_json);
}

void CharaBase::JsonSave(nlohmann::json& _json) const
{
	KdGameObject::JsonSave(_json);
}

void CharaBase::RotationUpdate(float _target, float _angle, float _rotSpeed)
{
	float targetAngle = atan2(-_target, -_angle);
	float currentAngle = DirectX::XMConvertToRadians(m_degree.y);
	float angleDiff = targetAngle - currentAngle;

	// 角度の正規化
	if (angleDiff > DirectX::XM_PI) angleDiff -= DirectX::XM_2PI;
	if (angleDiff < -DirectX::XM_PI) angleDiff += DirectX::XM_2PI;

	// 回転速度に基づいて補間
	m_degree.y += DirectX::XMConvertToDegrees(
		std::clamp(angleDiff, -_rotSpeed, _rotSpeed)
	);

	// 回転行列の更新
	m_mRotation = Math::Matrix::CreateRotationY(
		DirectX::XMConvertToRadians(m_degree.y)
	);
}
