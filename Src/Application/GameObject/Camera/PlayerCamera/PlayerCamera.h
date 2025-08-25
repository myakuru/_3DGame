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
	void UpdateIntroCamera();
	void ImGuiInspector()	override;
	void JsonSave(nlohmann::json& _json) const override;
	void JsonInput(const nlohmann::json& _json) override;
	DirectX::BoundingFrustum CreateFrustum() const override;

	void DebugDraw(DirectX::BoundingFrustum _frustum);

	// 強さ、時間
	void StartShake(float power, float time)
	{
		m_shakePower = power;
		m_shakeTime = time;
	}

	Math::Vector3 GetCameraPos() const { return m_cameraPos; }

private:

	//std::weak_ptr<Player> m_player;
	Math::Vector3 m_targetLookAt = Math::Vector3::Zero;
	Math::Vector3 m_introCamPos = Math::Vector3::Zero;	// Introカメラの位置
	Math::Vector3 m_cameraPos = Math::Vector3::Zero; // カメラの現在位置

	// カメラシェイク用変数
	float m_shakePower = 0.0f;
	float m_shakeTime = 0.0f;

	float m_smooth = 0.0f; // カメラ追従のスムージング係数
	float m_introTimer = 0.0f;	// Introカメラのタイマー
};