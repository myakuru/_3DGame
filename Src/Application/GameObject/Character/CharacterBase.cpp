#include "CharacterBase.h"
#include"Player/PlayerState/PlayerState.h"
#include"../../main.h"

void CharaBase::Init()
{
	ModelLoad(m_path);
	m_animator->SetAnimation(m_modelWork->GetData()->GetAnimation(0));

	m_mRotation = Math::Matrix::CreateFromYawPitchRoll
	(
		DirectX::XMConvertToRadians(m_degree.y),
		DirectX::XMConvertToRadians(m_degree.x),
		DirectX::XMConvertToRadians(m_degree.z)
	);

	m_trailPolygon.SetMaterial("Asset/Textures/System/WhiteNoise.png");
}

void CharaBase::DrawToon()
{
	KdShaderManager::Instance().m_StandardShader.DrawPolygon(m_trailPolygon);
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_modelWork, m_mWorld, m_color);
}

void CharaBase::Update()
{
	KdGameObject::Update();

	float deltaTime = Application::Instance().GetDeltaTime();

	m_animator->AdvanceTime(m_modelWork->WorkNodes(), m_fixedFrameRate * deltaTime);

	return;

	// 移動関係
	m_gravity += m_gravitySpeed * deltaTime;

	// 最終的な移動量
	m_position.x += m_movement.x * m_fixedFrameRate * deltaTime;
	m_position.y += m_movement.y * m_fixedFrameRate * deltaTime;
	m_position.y += m_gravity;

	// ステートで移動処理など管理
	m_stateManager.Update();

	// 最終的なワールド行列計算
	m_mWorld = Math::Matrix::CreateScale(m_scale);
	m_mWorld *= m_mRotation;
	m_mWorld.Translation(m_position);

	// トレイルポリゴンの更新
	m_trailPolygon.AddPoint(m_mWorld);

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
