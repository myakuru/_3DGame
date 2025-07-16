#pragma once
#include"../CameraBase.h"
class Player;
class PlayerCamera :public CameraBase
{
public:
	PlayerCamera() = default;
	~PlayerCamera() override = default;

	void Init() override;
	void Update() override;

	void UpdatePlayerRotetionMouse();

	void SetPosition(Math::Vector3 _pos) { m_targetPos = _pos; }

private:

	std::weak_ptr<Player> m_player;
	Math::Vector3 m_targetPos = Math::Vector3::Zero;
};