#pragma once
class Player;
class EffekseerEffectBase : public KdGameObject
{
public:

	EffekseerEffectBase() = default;
	~EffekseerEffectBase() override = default;

	void SetOnce(bool once) { m_once = once; }

protected:

	void Init() override;
	void Update() override;

	virtual void EffectUpdate();

	void DrawEffect() override;

	std::weak_ptr<Player> m_player;

	void ImGuiInspector() override;
	void JsonSave(nlohmann::json& _json) const override;
	void JsonInput(const nlohmann::json& _json) override;

	bool ModelLoad(std::string _path) override;

	bool m_once = false;	// エフェクトを１回だけ再生する

	float m_distance = 0.0f;
	float m_effectSpeed = 0.0f;

	const std::string m_prefix = "Asset\\Data\\Effect\\";

};