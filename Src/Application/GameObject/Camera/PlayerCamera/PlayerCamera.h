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
	void Update() override;

	void UpdatePlayerRotetionMouse();

	void SetPosition(Math::Vector3 _pos) { m_targetPos = _pos; }

private:

	std::weak_ptr<Player> m_player;
	Math::Vector3 m_targetPos = Math::Vector3::Zero;
};