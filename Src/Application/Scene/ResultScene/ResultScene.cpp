#include "ResultScene.h"
#include"../SceneManager.h"

void ResultScene::Event()
{
	KdShaderManager::Instance().m_postProcessShader.SetBrightThreshold(m_brightThreshold);
	KdShaderManager::Instance().WorkAmbientController().SetFogEnable(m_fogEnable, m_fogUseRange);
	KdShaderManager::Instance().WorkAmbientController().SetDistanceFog({ m_fogColor }, m_fogDensity);
	KdShaderManager::Instance().WorkAmbientController().SetheightFog({ 0.7f, 0.7f, 0.7f }, 100.0f, -5.0f, 0.0f);
}

void ResultScene::Init()
{
	KdShaderManager::Instance().m_postProcessShader.SetEnableGray(false);

	// PostProcess用パラメータ
	m_brightThreshold = 0.9f;
	m_fogEnable = true;
	m_fogUseRange = true;
	m_fogColor = { 0.7f, 0.7f, 0.7f };
	m_fogDensity = 0.005f;
}
