#include "TestStage.h"
#include"../SceneManager.h"
#include"../../GameObject/Utility/Time.h"
#include"../../GameObject/Character/Player/Player.h"

void TestScene::Event()
{
	KdShaderManager::Instance().m_postProcessShader.SetBrightThreshold(m_brightThreshold);
	KdShaderManager::Instance().WorkAmbientController().SetFogEnable(m_fogEnable, m_fogUseRange);
	KdShaderManager::Instance().WorkAmbientController().SetDistanceFog({ m_fogColor }, m_fogDensity);
	KdShaderManager::Instance().WorkAmbientController().SetheightFog({ 0.7f, 0.7f, 0.7f }, 0.0f, -50.0f, 0.0f);

	KdShaderManager::Instance().WorkAmbientController().SetDirLight({ -0.01f, -0.03f, 0.0f }, { 0,0,0 });

	if (KeyboardManager::GetInstance().IsKeyPressed('P'))
	{
		KdShaderManager::Instance().m_postProcessShader.SetEnableNoise(true); // ノイズON
		KdShaderManager::Instance().m_postProcessShader.SetNoiseStrength(0.07f);
	}
	else
	{
		KdShaderManager::Instance().m_postProcessShader.SetEnableNoise(false); // ノイズON
	}

	SceneManager::Instance().GetObjectWeakPtr(m_player);

	auto spPlayer = m_player.lock();

	if (!spPlayer) return;

	if (spPlayer->GetPos().x >= 4.0f && !m_isCountDown)
	{
		m_isCountDown = true; // カウントダウン開始

		if (m_isCountDown)
		{
			Time::Instance().StartCountdown(m_countDownTimer);
			SceneManager::Instance().SetTime(m_countDownTimer);
		}
	}

	Time::Instance().Update();

}

void TestScene::Init()
{
	KdShaderManager::Instance().m_postProcessShader.SetEnableGray(false);

	// PostProcess用パラメータ
	m_brightThreshold = 0.9f;
	m_fogEnable = true;
	m_fogUseRange = true;
	m_fogColor = { 0.3f, 0.3f, 0.3f };
	m_fogDensity = 0.005f;

	m_isCountDown = false;	// カウントダウンフラグを初期化

	m_countDownTimer = 120.0f; // カウントダウンタイマーを120秒に設定

	KdShaderManager::Instance().m_postProcessShader.SetBrightThreshold(m_brightThreshold);
	KdShaderManager::Instance().WorkAmbientController().SetFogEnable(m_fogEnable, m_fogUseRange);
	KdShaderManager::Instance().WorkAmbientController().SetDistanceFog({ m_fogColor }, m_fogDensity);
	KdShaderManager::Instance().WorkAmbientController().SetheightFog({ 0.7f, 0.7f, 0.7f }, 0.0f, -50.0f, 0.0f);

	SceneManager::Instance().SetDrawGrayScale(false);
	SceneManager::Instance().m_gameClear = false; // ゲームクリアフラグを初期化
	SceneManager::Instance().SetResultFlag(false); // 結果フラグを初期化

}
