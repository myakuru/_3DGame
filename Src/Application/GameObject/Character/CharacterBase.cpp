#include "CharacterBase.h"
#include"Player/PlayerState/PlayerState.h"
#include"../../main.h"
#include"../../Scene/SceneManager.h"
#include"../Camera/PlayerCamera/PlayerCamera.h"

void CharaBase::UpdateQuaternion(Math::Vector3& _moveVector)
{
	float deltaTime = Application::Instance().GetDeltaTime();

	if (_moveVector == Math::Vector3::Zero) return;

	SceneManager::Instance().GetObjectWeakPtr(m_playerCamera);
	if (m_playerCamera.expired()) return;

	const auto camera = m_playerCamera.lock();

	// カメラのY軸回転と移動ベクトルをかけ合わせて、WASDから入力された値に基づく方向を計算
	_moveVector = Math::Vector3::TransformNormal(_moveVector, camera->GetRotationYMatrix());

	_moveVector.Normalize();

	// 向きたい方向をクォータニオンに変換 第1引数：前を向く方向, 第2引数：どっちが上か
	Math::Quaternion targetRotation = Math::Quaternion::LookRotation(_moveVector, Math::Vector3::Up);

	// 滑らかに回転させるために、現在の回転と目標の回転を補間
	m_rotation = Math::Quaternion::Slerp(m_rotation, targetRotation, deltaTime * m_fixedFrameRate);

}

void CharaBase::Init()
{
	KdGameObject::Init();
	ModelLoad(m_path);
	m_animator->SetAnimation(m_modelWork->GetData()->GetAnimation("Idle"));
	//m_trailPolygon.SetMaterial("Asset/Textures/System/WhiteNoise.png");
}

void CharaBase::DrawToon()
{
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_modelWork, m_mWorld, m_color);
	//KdShaderManager::Instance().m_StandardShader.DrawPolygon(m_trailPolygon);
}

void CharaBase::DrawLit()
{
	//KdShaderManager::Instance().m_StandardShader.DrawModel(*m_modelWork, m_mWorld, m_color);
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
	m_mWorld *= Math::Matrix::CreateFromQuaternion(m_rotation);
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
