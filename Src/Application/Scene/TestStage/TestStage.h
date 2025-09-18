#pragma once
#include"../BaseScene/BaseScene.h"
class Player;
class TestScene : public BaseScene
{
public:

	TestScene() { Init(); }
	~TestScene() override = default;
		
	std::string GetSceneName() const override { return "Test"; }

private:
	void Event() override;
	void Init()  override;
	void DrawImGui() override;

	void JsonInput(const nlohmann::json& _json) override;
	void JsonSave(nlohmann::json& _json) const override;

	// シーン固有の設定をJsonに保存する関数
	void SaveSceneSettingsToJson(const std::string& filePath) const override;

	// シーンの設定を呼び出す関数
	void LoadSceneSettingsFromJson(const std::string& filePath) override;

	bool m_isCountDown = false; // カウントダウン中かどうか

	float m_countDownTimer = 0.0f; // カウントダウンタイマー

	std::weak_ptr<Player> m_player;

};