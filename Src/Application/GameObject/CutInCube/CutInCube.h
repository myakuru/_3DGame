#pragma once
#include"../Utility/SelectDraw3dModel.h"
class Player;
class CutInCube :public SelectDraw3dModel
{
public:

	static const uint32_t TypeID;

	CutInCube() { m_typeID = TypeID; }
	~CutInCube() override = default;

private:

	void Init() override;
	void Update() override;
	void DrawUnLit() override;

	void JsonInput(const nlohmann::json& _json);
	void JsonSave(nlohmann::json& _json) const override;
	void ImGuiInspector() override;

	std::weak_ptr<Player> m_player;

	Math::Vector3 m_playerPos = Math::Vector3::Zero;
	Math::Vector3 m_offset = Math::Vector3::Zero;

};