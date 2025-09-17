#pragma once
class Player;
class EffekseerEffectBase : public KdGameObject
{
public:

	EffekseerEffectBase() = default;
	~EffekseerEffectBase() override = default;

	void SetPlayEffect(bool once) { m_load = once; }

protected:

	void Init() override;
	void Update() override;
	void DrawEffect() override;

	virtual void EffectUpdate();

	std::weak_ptr<Player> m_player;

	void ImGuiInspector() override;
	void JsonSave(nlohmann::json& _json) const override;
	void JsonInput(const nlohmann::json& _json) override;

	bool ModelLoad(std::string _path) override;

	bool m_once = false;	// エフェクトを１回だけ再生する
	bool m_load = false; // エフェクトがロードされたか

	float m_distance = 0.0f;
	float m_effectSpeed = 0.0f;

};