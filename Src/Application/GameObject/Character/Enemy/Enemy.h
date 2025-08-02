#pragma once
#include "../CharacterBase.h"
class Enemy :public CharaBase
{
public:
	// クラスごとに一意なTypeIDを持たせる
	static const uint32_t TypeID;
	Enemy() { m_typeID = TypeID; }
	~Enemy() override = default;

	void Init() override;
	void PreUpdate() override;
	void SkirtUpdate();
	void Update() override;
	void DrawToon() override;
	void ImGuiInspector() override;
	void JsonInput(const nlohmann::json& _json) override;
	void JsonSave(nlohmann::json& _json) const override;

	void StateInit();
	void ChangeState(std::shared_ptr<PlayerStateBase> _state);



private:

};