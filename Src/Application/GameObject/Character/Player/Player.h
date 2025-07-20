#pragma once
#include "../CharacterBase.h"
class Katana;
class Player :public CharaBase
{
public:

	// クラスごとに一意なTypeIDを持たせる
	static const uint32_t TypeID;
	Player() { m_typeID = TypeID; };
	~Player() override = default;

	void Init() override;
	void PreUpdate() override;
	void Update() override;
	void DrawToon() override;
	void ImGuiInspector() override;
	void JsonInput(const nlohmann::json& _json) override;
	void JsonSave(nlohmann::json& _json) const override;

	void DrawDebug() override;

	void StateInit();
	void ChangeState(std::shared_ptr<PlayerStateBase> _state);

private:

	std::weak_ptr<Katana>	m_katana;

	// 腰のノード情報取得用変数
	const struct KdModelWork::Node* backWorkNode = nullptr;
	const struct KdModelWork::Node* handWorkNode = nullptr;

};