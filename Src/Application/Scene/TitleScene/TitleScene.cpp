#include "TitleScene.h"
#include "../SceneManager.h"

#include"../../../Framework/Json/Json.h"
#include"../../main.h"

void TitleScene::Event()
{
	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		SceneManager::Instance().SetNextScene
		(
			SceneManager::SceneType::Test
		);
	}

	// イントロBGMが再生終了したらループBGMへ切り替え
	{
		auto bgm = SceneManager::Instance().GetGameSound(); // 値取得
		const bool needSwitch = (!bgm) || !bgm->IsPlaying();
		if (needSwitch)
		{
			auto loopBgm = KdAudioManager::Instance().Play(
				"Asset/Sound/FieldBGM/ToDo_game_bgm_loop.wav",
				true
			);
			SceneManager::Instance().SetGameSound(loopBgm);

			if (loopBgm)
			{
				loopBgm->SetVolume(1.0f);
			}
		}
	}
}

void TitleScene::Init()
{

	SceneManager::Instance().SetIntroCamera(false);

	SceneManager::Instance().SetDrawGrayScale(false);
	SceneManager::Instance().m_gameClear = false;	// ゲームクリアフラグを初期化
	SceneManager::Instance().SetResultFlag(false);	// 結果フラグを初期化

	// イントロBGM（非ループ）: SetGameSound を使う
	{
		auto intro = KdAudioManager::Instance().Play(
			"Asset/Sound/FieldBGM/ToDo_game_bgm.wav",
			false
		);
		SceneManager::Instance().SetGameSound(intro);

		if (intro)
		{
			intro->SetVolume(1.0f);
		}
	}
}
