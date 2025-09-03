#pragma once
class Player;
class AttackEffect : public KdGameObject
{
public:

	static const uint32_t TypeID;

	AttackEffect() { m_type = TypeID; }
	~AttackEffect() override = default;

private:

	void Update() override;
	void EffectControl();
	void DrawEffect() override;

	void ImGuiInspector() override;
	void JsonInput(const nlohmann::json& _json) override;
	void JsonSave(nlohmann::json& _json) const override;

	std::weak_ptr<Player> m_player;
	
	Math::Vector3 m_offset = Math::Vector3::Zero;

	float m_time = 0.0f;
	float m_fadeTime = 0.0f;
	float m_fadeAmount = 0.0f;

};