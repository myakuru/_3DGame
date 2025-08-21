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
	void UpdateWinnerCamera();
	void ImGuiInspector()	override;
	void JsonSave(nlohmann::json& _json) const override;
	void JsonInput(const nlohmann::json& _json) override;
	DirectX::BoundingFrustum CreateFrustum() const override;

	void DebugDraw(DirectX::BoundingFrustum _frustum);

private:

	//std::weak_ptr<Player> m_player;
	Math::Vector3 m_cameraPos = Math::Vector3::Zero; // カメラの現在位置
	Math::Vector3 m_targetLookAt = Math::Vector3::Zero;

	float m_smooth = 0.0f; // カメラ追従のスムージング係数
};