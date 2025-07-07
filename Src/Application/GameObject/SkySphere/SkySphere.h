#pragma once
#include"../Utility/SelectDrawObject.h"
class SkySphere:public SelectDrawObject
{
public:
	SkySphere() = default;
	~SkySphere() override = default;

	void Init() override;

};