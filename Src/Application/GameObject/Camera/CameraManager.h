#pragma once
class CameraManager : public KdGameObject
{
public:
	CameraManager() = default;
	~CameraManager() override = default;

	void Init() override;
	void PostUpdate() override;
	void SceneCameraUpdate();

private:

};