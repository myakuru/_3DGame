#include "CharacterBase.h"
#include"Player/PlayerState/PlayerState.h"
#include"../../main.h"
#include"../../Scene/SceneManager.h"

void CharaBase::UpdateRotation(const Math::Vector3& _movevVector)
{
	if (_movevVector.LengthSquared() == 0.0f) return;

	// キャラの正面方向のベクトル
	Math::Vector3 _nowDir = GetMatrix().Backward();

	// 移動方向のベクトル
	Math::Vector3 _targetDir = _movevVector;

	_nowDir.Normalize();
	_targetDir.Normalize();

	float _nowAng = atan2(_nowDir.x, _nowDir.z);
	_nowAng = DirectX::XMConvertToDegrees(_nowAng);

	float _targetAng = atan2(_targetDir.x, _targetDir.z);
	_targetAng = DirectX::XMConvertToDegrees(_targetAng);

	// 角度の差分を求める
	float _betweenAng = _targetAng - _nowAng;
	if (_betweenAng > 180)
	{
		_betweenAng -= 360;
	}
	else if (_betweenAng < -180)
	{
		_betweenAng += 360;
	}

	float rotateAng = std::clamp(_betweenAng, -8.0f, 8.0f);
	m_degree.y += rotateAng;
}

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
	Math::Matrix scaleMat = Math::Matrix::CreateScale(m_scale);
	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_position);

	m_mWorld = scaleMat * m_mRotation * transMat;

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
