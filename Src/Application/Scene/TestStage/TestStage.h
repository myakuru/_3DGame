#pragma once
#include"../BaseScene/BaseScene.h"
class TestScene : public BaseScene
{
public:	

	TestScene() { Init(); }
	~TestScene() override = default;
		
	std::string GetSceneName() const override { return "Test"; }

private:
	void Event() override;
	void Init()  override;

};