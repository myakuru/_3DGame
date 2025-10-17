#include "TestStage.h"
#include"../SceneManager.h"
#include"../../GameObject/Utility/Time.h"
#include"../../GameObject/Character/Player/Player.h"
#include"../../GameObject/Character/Enemy/Enemy.h"
#include"../../GameObject/Character/BossEnemy/BossEnemy.h"

#include"../../../Framework/Json/Json.h"
#include"../../main.h"

void TestScene::Event()
{

	// 敵を探して、いなかったらゲームクリアさせる
	SearchEnemy();

	// イントロBGMが再生終了したらループBGMへ切り替え
	if (!m_gameSound->IsPlaying())
	{
		m_gameSound = KdAudioManager::Instance().Play
		(
			"Asset/Sound/FieldBGM/ToDo_game_bgm_loop.wav",
			true
		);
		if (m_gameSound) { m_gameSound->SetVolume(1.0f); }
	}

	if (SceneManager::Instance().m_gameClear)
	{
		KdShaderManager::Instance().WorkAmbientController().SetheightFog({ 1,1,1 }, 2.0f, 0.0f, 0.0f);
		m_brightThreshold = 1.0f;
		m_fogEnable = true;
		m_fogUseRange = true;
		m_fogColor = { 0.93f, 0.86f, 0.0f };
		m_fogDensity = 0.001f;
	}

	if (SceneManager::Instance().IsCutInScene())
	{
		KdShaderManager::Instance().m_postProcessShader.SetBrightThreshold(m_brightThreshold);
		KdShaderManager::Instance().WorkAmbientController().SetFogEnable(true, true);
		KdShaderManager::Instance().WorkAmbientController().SetDistanceFog({ m_fogColor }, m_fogDensity);
		KdShaderManager::Instance().WorkAmbientController().SetheightFog({ m_highFogColor }, m_highFogHeight, m_lowFogHeight, m_highFogDistance);
	}

	if (KeyboardManager::GetInstance().IsKeyPressed('P'))
	{
		KdShaderManager::Instance().m_postProcessShader.SetEnableNoise(true);	// ノイズON
		KdShaderManager::Instance().m_postProcessShader.SetNoiseStrength(0.07f);
	}
	else
	{
		KdShaderManager::Instance().m_postProcessShader.SetEnableNoise(false);	// ノイズOFF
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

	if (KeyboardManager::GetInstance().IsKeyPressed('C'))
	{
		KdShaderManager::Instance().m_postProcessShader.SetEnableStrongBlur(true);
	}
	else
	{
		KdShaderManager::Instance().m_postProcessShader.SetEnableStrongBlur(false);
	}
}

void TestScene::Init()
{
	//KdShaderManager::Instance().m_postProcessShader.SetEnableGray(false);

	m_isCountDown = false;	// カウントダウンフラグを初期化

	m_countDownTimer = 120.0f; // カウントダウンタイマーを120秒に設定

	SceneManager::Instance().SetIntroCamera(true); // カメラのイントロを開始

	KdShaderManager::Instance().m_postProcessShader.SetBrightThreshold(m_brightThreshold);
	KdShaderManager::Instance().WorkAmbientController().SetAmbientLight(m_anviLightColor);
	KdShaderManager::Instance().WorkAmbientController().SetFogEnable(m_fogEnable, m_fogUseRange);
	KdShaderManager::Instance().WorkAmbientController().SetDistanceFog({ m_fogColor }, m_fogDensity);
	KdShaderManager::Instance().WorkAmbientController().SetheightFog({ m_highFogColor }, m_highFogHeight, m_lowFogHeight, m_highFogDistance);

	SceneManager::Instance().SetDrawGrayScale(false);
	SceneManager::Instance().m_gameClear = false;	// ゲームクリアフラグを初期化
	SceneManager::Instance().SetResultFlag(false);	// 結果フラグを初期化

	m_bossAppear = false; // ボス出現フラグを初期化

	// イントロBGM（非ループ）
	m_gameSound = KdAudioManager::Instance().Play
	(
		"Asset/Sound/FieldBGM/ToDo_game_bgm.wav",
		false
	);

	if (m_gameSound)
	{
		m_gameSound->SetVolume(1.0f);
	}
}

void TestScene::SearchEnemy()
{
	// 敵を探す
	bool enemyExists = false;
	bool bossenemyExists = false;
	
	SceneManager::Instance().GetObjectWeakPtrList(m_enemies);
	SceneManager::Instance().GetObjectWeakPtrList(m_bossEnemies);

	for(const auto& weakEnemy : m_enemies)
	{
		if (const auto spEnemy = weakEnemy.lock())
		{
			enemyExists = true; // 敵が存在する
			break;
		}
	}

	for (const auto& weakBoss : m_bossEnemies)
	{
		if (const auto spBoss = weakBoss.lock())
		{
			bossenemyExists = true; // 敵が存在する
			break;
		}
	}

	// 敵がいなかったらゲームクリア
	if (!enemyExists)
	{
		SceneManager::Instance().SetBossAppear(true);
	}

	// ボスが出現するかどうか
	if (SceneManager::Instance().IsBossAppear() && !m_bossAppear)
	{
		m_bossAppear = true;

		// ボスを出現させる処理をここに追加
		auto bossEnemy = std::make_shared<BossEnemy>();
		bossEnemy->Init();
		SceneManager::Instance().AddObject(bossEnemy);
	}

	if (!bossenemyExists && !enemyExists)
	{
		SceneManager::Instance().m_gameClear = true;
	}

}

void TestScene::DrawImGui()
{
	BaseScene::DrawImGui();
	ImGui::Begin("BossEnemySpawn");
	{
		ImGui::Text("BossEnemySpawn");
		if (ImGui::Button("SpawnBossEnemy"))
		{
			SceneManager::Instance().SetBossAppear(true);
		}
	}
	ImGui::End();
}
