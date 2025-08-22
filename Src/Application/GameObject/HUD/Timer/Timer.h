#pragma once
#include "../../Utility/SelectDraw2DTexture.h"
class Timer :public SelectDraw2DTexture
{
public:

	static const uint32_t TypeID;

	Timer();
	~Timer() override = default;

private:
	void Init() override;
	void Update() override;

	void ResultTimerUpdate();

	void DrawSprite() override;

	float m_time = 0.0;	// 経過時間

	float m_lastTime = 0.0;	// 最後に更新した時間

	Math::Rectangle m_srcRect;
	int m_displayTime = 0;
	std::shared_ptr<KdTexture> m_resultTexture = std::make_shared<KdTexture>();	// 結果画面用のテクスチャ

	bool m_notDraw = false;	// 描画しないフラグ

};