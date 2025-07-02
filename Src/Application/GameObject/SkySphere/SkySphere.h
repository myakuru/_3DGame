#pragma once
class SkySphere:public KdGameObject
{
public:
	SkySphere() = default;
	~SkySphere() override = default;

	void DrawUnLit() override;

private:

};