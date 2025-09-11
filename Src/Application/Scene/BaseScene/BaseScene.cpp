#include "BaseScene.h"
#include"../SceneManager.h"
#include"../../GameObject/Camera/CameraBase.h"
#include"../../GameObject/Camera/PlayerCamera/PlayerCamera.h"

void BaseScene::PreUpdate()
{
	// Updateの前の更新処理
	// オブジェクトリストの整理 ・・・ 無効なオブジェクトを削除
	auto it = m_objList.begin();

	while (it != m_objList.end())
	{
		if ((*it)->IsExpired())	// IsExpired() ・・・ 無効ならtrue
		{
			// 無効なオブジェクトをリストから削除
			it = m_objList.erase(it);
		}
		else
		{
			++it;	// 次の要素へイテレータを進める
		}
	}

	auto mapIt = m_MapObjectList.begin();
	while (mapIt != m_MapObjectList.end())
	{
		if ((*mapIt)->IsExpired())	// IsExpired() ・・・ 無効ならtrue
		{
			// 無効なオブジェクトをリストから削除
			mapIt = m_MapObjectList.erase(mapIt);
		}
		else
		{
			++mapIt;	// 次の要素へイテレータを進める
		}
	}

	// ↑の後には有効なオブジェクトだけのリストになっている

	for (auto& obj : m_objList)
	{
		obj->PreUpdate();
	}

	for (auto& obj : m_MapObjectList)
	{
		obj->PreUpdate();
	}

	SceneManager::Instance().GetObjectWeakPtr(m_playerCamera);

	if (m_playerCamera.expired()) return;

}

void BaseScene::Update()
{
	if (!SceneManager::Instance().m_sceneCamera)
	{
		// シーン毎のイベント処理
		Event();

		KdEffekseerManager::GetInstance().Update();

		// KdGameObjectを継承した全てのオブジェクトの更新 (ポリモーフィズム)
		for (auto& obj : m_objList)
		{
			obj->Update();
		}
	}
}

void BaseScene::PostUpdate()
{
	if (!SceneManager::Instance().m_sceneCamera)
	{
		for (auto& obj : m_objList)
		{
			obj->PostUpdate();
		}
	}
	else
	{
		for (auto& obj : m_CameraObjList)
		{
			obj->CameraUpdate();
		}
	}
}

void BaseScene::PreDraw()
{
	for (auto& obj : m_objList)
	{
		obj->PreDraw();
	}

	if (SceneManager::Instance().m_sceneCamera)
	{
		for (auto& cameraObj : m_CameraObjList)
		{
			cameraObj->PreDraw();
		}
	}

	auto playerCamera = m_playerCamera.lock();

	if (playerCamera)
	{
		// カメラからフラスタム生成
		DirectX::BoundingFrustum frustum = playerCamera->CreateFrustum();

		int cunter = 0;

		// Mapリストからカリング
		for (auto& obj : m_MapObjectList)
		{
			bool result = obj->CheckInScreen(frustum);
			if (result)
			{
				cunter++;
				m_drawObjectList.push_back(obj);
			}
		}
	}
	else
	{
		for (auto& obj : m_MapObjectList)
		{
			m_drawObjectList.push_back(obj);
		}
	}
}

void BaseScene::Draw()
{
	WithDebugRenderTarget([&]()
		{
			m_renderTargetPack.ClearTexture();
			// 光を遮るオブジェクト(不透明な物体や2Dキャラ)
			KdShaderManager::Instance().m_StandardShader.BeginGenerateDepthMapFromLight();
			{
				for (auto& obj : m_objList) obj->GenerateDepthMapFromLight();
				for (auto& obj : m_drawObjectList) obj->GenerateDepthMapFromLight();
			}
			KdShaderManager::Instance().m_StandardShader.EndGenerateDepthMapFromLight();

			// 陰影のないオブジェクト(背景など)
			KdShaderManager::Instance().m_StandardShader.BeginUnLit();
			{
				for (auto& obj : m_objList) obj->DrawUnLit();
				for (auto& obj : m_drawObjectList) obj->DrawUnLit();
			}
			KdShaderManager::Instance().m_StandardShader.EndUnLit();

			// 陰影のあるオブジェクト
			KdShaderManager::Instance().m_StandardShader.BeginLit();
			{
				for (auto& obj : m_objList) obj->DrawLit();
				for (auto& obj : m_drawObjectList) obj->DrawLit();
			}
			KdShaderManager::Instance().m_StandardShader.EndLit();

			// 陰影のないエフェクト
			KdShaderManager::Instance().m_StandardShader.BeginUnLit();
			{
				for (auto& obj : m_objList) obj->DrawEffect();
				for (auto& obj : m_drawObjectList) obj->DrawEffect();
			}
			KdShaderManager::Instance().m_StandardShader.EndUnLit();

			// 光源オブジェクト
			KdShaderManager::Instance().m_postProcessShader.BeginBright();
			{
				for (auto& obj : m_objList) obj->DrawBright();
				for (auto& obj : m_drawObjectList) obj->DrawBright();
			}
			KdShaderManager::Instance().m_postProcessShader.EndBright();

			// Toonシェーダー
			KdShaderManager::Instance().m_StandardShader.BeginToon();
			{
				for (auto& obj : m_objList) obj->DrawToon();
				for (auto& obj : m_drawObjectList) obj->DrawToon();
				KdEffekseerManager::GetInstance().Draw();
			}
			KdShaderManager::Instance().m_StandardShader.EndToon();

			// グラデーション
			KdShaderManager::Instance().m_StandardShader.BeginGradient();
			{
				for (auto& obj : m_objList) obj->DrawGradation();
				for (auto& obj : m_drawObjectList) obj->DrawGradation();
			}
			KdShaderManager::Instance().m_StandardShader.EndGradient();

			// グレースケール
			KdShaderManager::Instance().m_StandardShader.BeginGrayscale();
			{
				for (auto& obj : m_objList) obj->DrawGrayScale();
				for (auto& obj : m_drawObjectList) obj->DrawGrayScale();
			}
			KdShaderManager::Instance().m_StandardShader.EndGrayscale();

			// エフェクト
			KdShaderManager::Instance().m_StandardShader.BeginEffect();
			{
				for (auto& obj : m_objList) obj->DrawEffect();
				for (auto& obj : m_drawObjectList) obj->DrawEffect();
			}
			KdShaderManager::Instance().m_StandardShader.EndEffect();

			m_drawObjectList.clear();
		});
}

void BaseScene::DrawSprite()
{
	m_renderTargetUIChanger.ChangeRenderTarget(m_renderTargetUIPack);

	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// 2Dの描画はこの間で行う
	KdShaderManager::Instance().m_spriteShader.Begin();
	{
		for (auto& obj : m_objList)
		{
			obj->DrawSprite();
		}
	}
	KdShaderManager::Instance().m_spriteShader.End();

	m_renderTargetUIChanger.UndoRenderTarget();

	KdShaderManager::Instance().m_spriteShader.Begin();
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_renderTargetUIPack.m_RTTexture.get(), 0, 0, m_gameWindowSizeX, m_gameWindowSizeY);
	}
	KdShaderManager::Instance().m_spriteShader.End();
}

void BaseScene::DrawDebug()
{
	WithDebugRenderTarget([&]()
		{
			KdShaderManager::Instance().m_StandardShader.BeginUnLit();
			{
				for (auto& obj : m_objList) obj->DrawDebug();
				for (auto& obj : m_MapObjectList) obj->DrawDebug();
			}
			KdShaderManager::Instance().m_StandardShader.EndUnLit();
		});
}

void BaseScene::Event()
{
}

void BaseScene::Init()
{
	KdCSVData windowData("Asset/Data/WindowSettings.csv");
	const std::vector<std::string>& sizeData = windowData.GetLine(0);

	m_renderTargetPack.CreateRenderTarget(atoi(sizeData[0].data()), atoi(sizeData[1].data()), true);
	m_renderTargetUIPack.CreateRenderTarget(1920,1080, true);
	KdEffekseerManager::GetInstance().Create(atoi(sizeData[0].data()), atoi(sizeData[1].data()));

	m_gameWindowSizeX = atoi(sizeData[0].data());
	m_gameWindowSizeY = atoi(sizeData[1].data());

}
