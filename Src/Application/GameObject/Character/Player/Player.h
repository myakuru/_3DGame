#pragma once
#include"../../Utility/SelectDraw3dModel.h"
class Player :public KdGameObject
{
public:
	Player() = default;
	~Player() override = default;

	void Init() override;
	void Update() override;	
	void DrawLit() override;

private:
	std::shared_ptr<KdModelWork> m_model;
	std::shared_ptr <KdAnimator>				m_animator = nullptr;
};