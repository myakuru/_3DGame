#pragma once
class Player;
class MeshEffectBase : public KdGameObject
{
public:
	MeshEffectBase() = default;
	~MeshEffectBase() override = default;

protected:

	void Update() override;
	void DrawEffect() override;

	void ImGuiInspector() override;
	void JsonInput(const nlohmann::json& _json) override;
	void JsonSave(nlohmann::json& _json) const override;

	float m_time = 0.0f;
	float m_fadeTime = 0.0f;
	float m_fadeAmount = 0.0f;

	Math::Vector3 m_outColor = { 0.2f,0.7f,1.0f }; // オブジェクトの外側の色
	Math::Vector3 m_inColor = { 1,1,1 };		 // オブジェクトの内側の色

	std::weak_ptr<Player> m_player;

};