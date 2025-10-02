#include "NormalUI.h"
#include "../../../Scene/SceneManager.h"
#include"../../../Scene/BaseScene/BaseScene.h"

const uint32_t NormalUI::TypeID = KdGameObject::GenerateTypeID();
NormalUI::NormalUI()
{
	m_typeID = TypeID;
}

void NormalUI::SetSvgPath(const std::string& path)
{
	m_svgPath = path;
	m_rasterizedW = m_rasterizedH = 0; // 次回描画時に作り直す
}

void NormalUI::SetTargetSizePx(uint32_t w, uint32_t h)
{
	m_targetW = w;
	m_targetH = h;
}

void NormalUI::Init()
{
	SelectDraw2DTexture::Init();
}

void NormalUI::DrawSprite()
{
	SelectDraw2DTexture::DrawSprite();
}

void NormalUI::Update()
{
	SelectDraw2DTexture::Update();
}

void NormalUI::EnsureSvgTexture()
{
	if (m_svgPath.empty()) return;

	// 実際に画面に出したいピクセルサイズ（DPI/UIスケール反映）
	const uint32_t desiredW = static_cast<uint32_t>(std::round(m_targetW * m_uiScale));
	const uint32_t desiredH = static_cast<uint32_t>(std::round(m_targetH * m_uiScale));
	if (desiredW == 0 || desiredH == 0) return;

	// 初回 or しきい値以上のサイズ差で再ラスタライズ
	bool needRegen = (m_rasterizedW == 0 || m_rasterizedH == 0);
	if (!needRegen) {
		const float rw = static_cast<float>(m_rasterizedW);
		const float rh = static_cast<float>(m_rasterizedH);
		const float sx = desiredW / rw;
		const float sy = desiredH / rh;
		const float up = std::max(sx, sy);
		const float down = std::min(sx, sy);
		if (up >= m_regenScaleUpRatio || down <= m_regenScaleDownRatio) {
			needRegen = true;
		}
	}

	if (!needRegen) return;

	// ミップ生成有効で縮小品質を確保
	if (m_texture.LoadSvg(m_svgPath, static_cast<int>(desiredW), static_cast<int>(desiredH),
		/*renderTarget*/false, /*generateMipmap*/true)) {
		m_rasterizedW = m_texture.GetWidth();
		m_rasterizedH = m_texture.GetHeight();
	}
}
