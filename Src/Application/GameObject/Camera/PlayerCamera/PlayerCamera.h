#pragma once
#include"../CameraBase.h"
class PlayerCamera :public CameraBase
{
public:
	PlayerCamera() = default;
	~PlayerCamera() override = default;

	void Init() override;
	void Update() override;

private:

};