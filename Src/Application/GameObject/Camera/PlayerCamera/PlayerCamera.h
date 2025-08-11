#pragma once
#include"../CameraBase.h"
class Player;
class PlayerCamera :public CameraBase
{
public:

	static const uint32_t TypeID;

	PlayerCamera() { m_typeID = TypeID; }
	~PlayerCamera() override = default;

	uint32_t GetTypeID() const override { return m_typeID; }

	void Init() override;
	void PostUpdate() override;
	void ImGuiInspector()	override;
	void JsonSave(nlohmann::json& _json) const override;
	void JsonInput(const nlohmann::json& _json) override;
	DirectX::BoundingFrustum CreateFrustum() const override;

private:

	std::weak_ptr<Player> m_player;
	Math::Vector3 m_cameraPos = Math::Vector3::Zero; // カメラの現在位置
	Math::Vector3 m_targetPos = Math::Vector3::Zero;
	Math::Vector3 m_targetLookAt = Math::Vector3::Zero;
};