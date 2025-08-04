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
	void SkirtUpdate();
	void Update() override;
	void ImGuiInspector() override;
	void JsonInput(const nlohmann::json& _json) override;
	void JsonSave(nlohmann::json& _json) const override;

	void StateInit();
	void ChangeState(std::shared_ptr<PlayerStateBase> _state);

	KdModelWork* GetModelWork() { return m_modelWork.get(); }

	const std::weak_ptr<Katana>& GetKatana() const { return m_katana; }

private:

	std::weak_ptr<Katana>	m_katana;
};