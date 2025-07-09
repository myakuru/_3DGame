#pragma once
#include"../Utility/SelectDraw3dModel.h"
class SkySphere:public SelectDrawObject
{
public:
	SkySphere() = default;
	~SkySphere() override = default;

	void Init() override;

};