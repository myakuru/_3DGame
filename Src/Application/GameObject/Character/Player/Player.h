#pragma once
#include "../CharacterBase.h"
class Katana;
class Saya;
class Player :public CharaBase
{
public:
	Player() = default;
	~Player() override = default;

	void Init() override;
	void PreUpdate() override;
	void Update() override;
	void DrawToon() override;
	void ImGuiInspector() override;
	void JsonInput(const nlohmann::json& _json) override;
	void JsonSave(nlohmann::json& _json) const override;

	void StateInit() override;
	void ChangeState(std::shared_ptr<PlayerStateBase> _state) override;

private:

	std::weak_ptr<Katana>	m_katana;
	std::weak_ptr<Saya>		m_saya;

	// 腰のノード情報取得用変数
	const struct KdModelWork::Node* backWorkNode = nullptr;
	const struct KdModelWork::Node* handWorkNode = nullptr;

};