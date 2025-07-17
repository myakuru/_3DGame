#pragma once
#include "../CameraBase.h"

class TPSCamera : public CameraBase
{
public:

	static const uint32_t TypeID;

	TPSCamera()							{ m_typeID = TypeID; }
	~TPSCamera()			override	{}

	uint32_t GetTypeID() const override { return m_typeID; }

	void Init()				override;
	void PostUpdate()		override;
};