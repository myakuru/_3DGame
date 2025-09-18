#include "TestStage.h"
#include"../SceneManager.h"
#include"../../GameObject/Utility/Time.h"
#include"../../GameObject/Character/Player/Player.h"

#include"../../../Framework/Json/Json.h"
#include"../../main.h"

void TestScene::Event()
{
	if (SceneManager::Instance().m_gameClear)
	{
		KdShaderManager::Instance().WorkAmbientController().SetheightFog({1,1,1}, 2.0f, 0.0f, 0.0f);
		m_brightThreshold = 1.0f;
		m_fogEnable = true;
		m_fogUseRange = true;
		m_fogColor = { 0.93f, 0.86f, 0.0f };
		m_fogDensity = 0.001f;
	}

	if (KeyboardManager::GetInstance().IsKeyPressed('P'))
	{
		KdShaderManager::Instance().m_postProcessShader.SetEnableNoise(true);	// ノイズON
		KdShaderManager::Instance().m_postProcessShader.SetNoiseStrength(0.07f);
	}
	else
	{
		KdShaderManager::Instance().m_postProcessShader.SetEnableNoise(false);	// ノイズON
	}

	SceneManager::Instance().GetObjectWeakPtr(m_player);

	auto spPlayer = m_player.lock();

	if (!spPlayer) return;

	if (spPlayer->GetPos().x >= 4.0f && !m_isCountDown)
	{
		m_isCountDown = true; // カウントダウン開始

		if (m_isCountDown)
		{
			Time::Instance().StartCountdown(m_countDownTimer);
			SceneManager::Instance().SetTime(m_countDownTimer);
		}
	}

	Time::Instance().Update();

	if (KeyboardManager::GetInstance().IsKeyPressed('C'))
	{
		KdShaderManager::Instance().m_postProcessShader.SetEnableStrongBlur(true);
	}
	else
	{
		KdShaderManager::Instance().m_postProcessShader.SetEnableStrongBlur(false);
	}
}

void TestScene::Init()
{
	//KdShaderManager::Instance().m_postProcessShader.SetEnableGray(false);

	m_isCountDown = false;	// カウントダウンフラグを初期化

	m_countDownTimer = 120.0f; // カウントダウンタイマーを120秒に設定

	SceneManager::Instance().SetIntroCamera(true); // カメラのイントロを開始

	KdShaderManager::Instance().m_postProcessShader.SetBrightThreshold(m_brightThreshold);
	KdShaderManager::Instance().WorkAmbientController().SetFogEnable(m_fogEnable, m_fogUseRange);
	KdShaderManager::Instance().WorkAmbientController().SetDistanceFog({ m_fogColor }, m_fogDensity);
	KdShaderManager::Instance().WorkAmbientController().SetheightFog({ m_highFogColor }, m_highFogHeight, m_lowFogHeight, m_highFogDistance);

	SceneManager::Instance().SetDrawGrayScale(false);
	SceneManager::Instance().m_gameClear = false;	// ゲームクリアフラグを初期化
	SceneManager::Instance().SetResultFlag(false);	// 結果フラグを初期化

}

void TestScene::DrawImGui()
{
	ImGui::Begin("PostProcess");
	{
		ImGui::Text(U8("光の調整"));
		ImGui::DragFloat(U8("ブライトの調整"), &m_brightThreshold, 0.1f);
		ImGui::ColorEdit3(U8("霧の色"), &m_fogColor.x);
		ImGui::DragFloat(U8("霧の濃さ"), &m_fogDensity, 0.0001f, 0.0f, 0.1f);

		ImGui::Separator();
		ImGui::Text(U8("高さFOGの調整"));
		ImGui::ColorEdit3(U8("高い位置の霧の色"), &m_highFogColor.x);
		ImGui::DragFloat(U8("Fogの最大値"), &m_highFogHeight, 0.1f);
		ImGui::DragFloat(U8("Fogの最小値"), &m_lowFogHeight, 0.1f);
		ImGui::DragFloat(U8("Fogの開始距離"), &m_highFogDistance, 0.1f);

		ImGui::Separator();
		ImGui::Text(U8("平行光の調整"));
		ImGui::DragFloat3(U8("平行光の方向"), &m_directionalLightDir.x, 0.1f);
		ImGui::ColorEdit3(U8("平行光の色"), &m_directionalLightColor.x);

		ImGui::Separator();

		ImGui::Checkbox(U8("霧を使う"), &m_fogEnable);
		ImGui::Checkbox(U8("距離で霧を使う"), &m_fogUseRange);

		ImGui::Separator();
		ImGui::DragFloat2(U8("影の射影行列"), &m_lightingArea.x, 0.1f, 0.1f);
		ImGui::DragFloat(U8("影の高さ"), &m_dirLightHeight, 1.0f, 0.1f);
	}
	ImGui::End();

	KdShaderManager::Instance().m_postProcessShader.SetBrightThreshold(m_brightThreshold);
	KdShaderManager::Instance().WorkAmbientController().SetFogEnable(m_fogEnable, m_fogUseRange);
	KdShaderManager::Instance().WorkAmbientController().SetDistanceFog({ m_fogColor }, m_fogDensity);
	KdShaderManager::Instance().WorkAmbientController().SetheightFog({ m_highFogColor }, m_highFogHeight, m_lowFogHeight, m_highFogDistance);

	KdShaderManager::Instance().WorkAmbientController().SetDirLight(m_directionalLightDir, m_directionalLightColor);
	KdShaderManager::Instance().WorkAmbientController().SetDirLightShadowArea(m_lightingArea, m_dirLightHeight);
}

void TestScene::JsonInput(const nlohmann::json& _json)
{
	if (_json.contains("m_brightThreshold")) m_brightThreshold = _json["m_brightThreshold"];
	if (_json.contains("m_fogEnable")) m_fogEnable = _json["m_fogEnable"];
	if (_json.contains("m_fogUseRange")) m_fogUseRange = _json["m_fogUseRange"];
	if (_json.contains("m_fogColor")) m_fogColor = JSON_MANAGER.JsonToVector(_json["m_fogColor"]);
	if (_json.contains("m_fogDensity")) m_fogDensity = _json["m_fogDensity"];
	if (_json.contains("m_highFogColor")) m_highFogColor = JSON_MANAGER.JsonToVector(_json["m_highFogColor"]);
	if (_json.contains("m_highFogHeight")) m_highFogHeight = _json["m_highFogHeight"];
	if (_json.contains("m_lowFogHeight")) m_lowFogHeight = _json["m_lowFogHeight"];
	if (_json.contains("m_highFogDistance")) m_highFogDistance = _json["m_highFogDistance"];
	if (_json.contains("m_directionalLightDir")) m_directionalLightDir = JSON_MANAGER.JsonToVector(_json["m_directionalLightDir"]);
	if (_json.contains("m_directionalLightColor")) m_directionalLightColor = JSON_MANAGER.JsonToVector(_json["m_directionalLightColor"]);
	if (_json.contains("m_lightingArea")) m_lightingArea = JSON_MANAGER.JsonToVector2(_json["m_lightingArea"]);
	if (_json.contains("m_dirLightHeight")) m_dirLightHeight = _json["m_dirLightHeight"];
}

void TestScene::JsonSave(nlohmann::json& _json) const
{
	_json["m_brightThreshold"] = m_brightThreshold;
	_json["m_fogEnable"] = m_fogEnable;
	_json["m_fogUseRange"] = m_fogUseRange;
	_json["m_fogColor"] = JSON_MANAGER.VectorToJson(m_fogColor);
	_json["m_fogDensity"] = m_fogDensity;
	_json["m_highFogColor"] = JSON_MANAGER.VectorToJson(m_highFogColor);
	_json["m_highFogHeight"] = m_highFogHeight;
	_json["m_lowFogHeight"] = m_lowFogHeight;
	_json["m_highFogDistance"] = m_highFogDistance;
	_json["m_directionalLightDir"] = JSON_MANAGER.VectorToJson(m_directionalLightDir);
	_json["m_directionalLightColor"] = JSON_MANAGER.VectorToJson(m_directionalLightColor);
	_json["m_lightingArea"] = JSON_MANAGER.Vector2ToJson(m_lightingArea);
	_json["m_dirLightHeight"] = m_dirLightHeight;
}

void TestScene::SaveSceneSettingsToJson(const std::string& filePath) const
{
	nlohmann::json json;
	JsonSave(json);

	JSON_MANAGER.JsonSerialize(json, filePath); // ファイルに保存
}

void TestScene::LoadSceneSettingsFromJson(const std::string& filePath)
{
	nlohmann::json json = JSON_MANAGER.JsonDeserialize(filePath);
	if (!json.is_null())
	{
		JsonInput(json);
	}
}
