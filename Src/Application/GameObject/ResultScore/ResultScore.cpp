#include "ResultScore.h"
#include"../../../Framework/Json/Json.h"
#include"../../main.h"
#include"../../Scene/SceneManager.h"
#include"../Camera/TPSCamera/TPSCamera.h"
#include"../Utility/Time.h"
const uint32_t ResultScore::TypeID = KdGameObject::GenerateTypeID();

void ResultScore::Init()
{
	SelectDraw3dModel::Init();
	m_model_S = KdAssets::Instance().m_modeldatas.GetData("Asset/3DAssets/Score/S.gltf");
	m_model_A = KdAssets::Instance().m_modeldatas.GetData("Asset/3DAssets/Score/A.gltf");
	m_model_B = KdAssets::Instance().m_modeldatas.GetData("Asset/3DAssets/Score/B.gltf");
	m_model_X = KdAssets::Instance().m_modeldatas.GetData("Asset/3DAssets/Score/X.gltf");

	KdShaderManager::Instance().m_StandardShader.SetGradientColor(m_gradientColor);

}

void ResultScore::Update()
{
	float deltatime = Application::Instance().GetDeltaTime();

	m_time = Time::Instance().GetElapsedTime();

	if (m_time <= 5.0f) return;

	SelectDraw3dModel::Update();

	SceneManager::Instance().GetObjectWeakPtr(m_camera);

	auto _spCamera = m_camera.lock();
	if (!_spCamera) return;

	// カメラのワールド座標を取得
	Math::Vector3 cameraPos = _spCamera->GetMatrix().Translation();

	// カメラのY軸回転のみ取得
	cameraRot = _spCamera->GetRotationMatrix();
	Math::Vector3 forward = cameraRot.Backward();

	forward.Normalize();

	if (m_distance < 15.0f)
	{
		m_distance += 8.f * deltatime;
	}
	else
	{
		m_distance = 15.0f;
	}

	// オブジェクトの位置をカメラのX軸＋m_distance、Y=80、Z=100に固定
	m_position.x = cameraPos.x + forward.x * m_distance;
	m_position.y = cameraPos.y + forward.y * m_distance;
	m_position.z = cameraPos.z + forward.z * m_distance;

	// --- ビルボード処理（Y軸のみ反映＋自分のY軸回転） ---
	m_mWorld =
		Math::Matrix::CreateScale(m_scale) *
		cameraRot *
		Math::Matrix::CreateTranslation(m_position);
}

void ResultScore::DrawGradation()
{
	if (m_time <= 5.0f) return;

	if (SceneManager::Instance().GetScore() == 0)
	{
		m_color = { 1.0f, 0.43f, 0.04f, 1.0f };
		m_gradientColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model_S, m_mWorld, m_color);
	}
	else if (SceneManager::Instance().GetScore() == 1)
	{
		m_color = { 0.71f, 0.24f, 1.0f, 1.0f };
		m_gradientColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model_A, m_mWorld, m_color);
	}
	else if (SceneManager::Instance().GetScore() == 2)
	{
		m_color = { 0.19f, 0.62f, 1.0f, 1.0f };
		m_gradientColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model_B, m_mWorld, m_color);
	}
	else if (SceneManager::Instance().GetScore() == 3)
	{
		m_color = { 0.0f, 0.0f, 0.0f, 1.0f };
		m_gradientColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model_X, m_mWorld, m_color);
	}
	else
	{
		return; // スコアが0〜3以外の場合は何もしない
	}
}

void ResultScore::ImGuiInspector()
{
	SelectDraw3dModel::ImGuiInspector();
	ImGui::ColorEdit3(U8("グラデーションのカラー"), &m_gradientColor.x);
	KdShaderManager::Instance().m_StandardShader.SetGradientColor(m_gradientColor);
}

void ResultScore::JsonSave(nlohmann::json& _json) const
{
	SelectDraw3dModel::JsonSave(_json);
	_json["gradientColor"] = JSON_MANAGER.Vector4ToJson(m_gradientColor);
}

void ResultScore::JsonInput(const nlohmann::json& _json)
{
	SelectDraw3dModel::JsonInput(_json);
	if (_json.contains("gradientColor"))
	{
		m_gradientColor = JSON_MANAGER.JsonToVector4(_json["gradientColor"]);
		KdShaderManager::Instance().m_StandardShader.SetGradientColor(m_gradientColor);
	}
}
