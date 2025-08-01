﻿#include "BaseScene.h"
#include"../SceneManager.h"

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

	// ↑の後には有効なオブジェクトだけのリストになっている

	for (auto& obj : m_objList)
	{
		obj->PreUpdate();
	}
}

void BaseScene::Update()
{
	if (!SceneManager::Instance().m_sceneCamera)
	{
		// シーン毎のイベント処理
		Event();

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

}

void BaseScene::Draw()
{
	if (KdDebugGUI::Instance().ShowImGUiFlg())
	{
		m_renderTargetPack.ClearTexture();
		m_renderTargetChanger.ChangeRenderTarget(m_renderTargetPack);
	}

	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// 光を遮るオブジェクト(不透明な物体や2Dキャラ)はBeginとEndの間にまとめてDrawする
	KdShaderManager::Instance().m_StandardShader.BeginGenerateDepthMapFromLight();
	{
		for (auto& obj : m_objList)
		{
			obj->GenerateDepthMapFromLight();
		}
		for (auto& obj : m_MapObjectList)
		{
			obj->GenerateDepthMapFromLight();
		}
	}
	KdShaderManager::Instance().m_StandardShader.EndGenerateDepthMapFromLight();

	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// 陰影のないオブジェクト(背景など)はBeginとEndの間にまとめてDrawする
	KdShaderManager::Instance().m_StandardShader.BeginUnLit();
	{
		for (auto& obj : m_objList)
		{
			obj->DrawUnLit();
		}
		for (auto& obj : m_MapObjectList)
		{
			obj->DrawUnLit();
		}

	}
	KdShaderManager::Instance().m_StandardShader.EndUnLit();

	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// 陰影のあるオブジェクト(不透明な物体や2Dキャラ)はBeginとEndの間にまとめてDrawする
	KdShaderManager::Instance().m_StandardShader.BeginLit();
	{
		for (auto& obj : m_objList)
		{
			obj->DrawLit();
		}
		for (auto& obj : m_MapObjectList)
		{
			obj->DrawLit();
		}
	}
	KdShaderManager::Instance().m_StandardShader.EndLit();

	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// 陰影のないオブジェクト(エフェクトなど)はBeginとEndの間にまとめてDrawする
	KdShaderManager::Instance().m_StandardShader.BeginUnLit();
	{
		for (auto& obj : m_objList)
		{
			obj->DrawEffect();
		}
		for (auto& obj : m_MapObjectList)
		{
			obj->DrawEffect();
		}
	}
	KdShaderManager::Instance().m_StandardShader.EndUnLit();

	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// 光源オブジェクト(自ら光るオブジェクトやエフェクト)はBeginとEndの間にまとめてDrawする
	KdShaderManager::Instance().m_postProcessShader.BeginBright();
	{
		for (auto& obj : m_objList)
		{
			obj->DrawBright();
		}
		for (auto& obj : m_MapObjectList)
		{
			obj->DrawBright();
		}
	}
	KdShaderManager::Instance().m_postProcessShader.EndBright();

	// Toonシェーダーの描画
	KdShaderManager::Instance().m_StandardShader.BeginToon();
	{
		for (auto& obj : m_objList)
		{
			obj->DrawToon();
		}
		for (auto& obj : m_MapObjectList)
		{
			obj->DrawToon();
		}
	}
	KdShaderManager::Instance().m_StandardShader.EndToon();

	m_renderTargetChanger.UndoRenderTarget();
}

void BaseScene::DrawSprite()
{
	if (KdDebugGUI::Instance().ShowImGUiFlg())
	{
		m_renderTargetChanger.ChangeRenderTarget(m_renderTargetPack);
	}

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

	m_renderTargetChanger.UndoRenderTarget();

}

void BaseScene::DrawDebug()
{
	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// デバッグ情報の描画はこの間で行う
	KdShaderManager::Instance().m_StandardShader.BeginUnLit();
	{
		for (auto& obj : m_objList)
		{
			obj->DrawDebug();
		}
	}
	KdShaderManager::Instance().m_StandardShader.EndUnLit();
}

void BaseScene::Event()
{
	// 各シーンで必要な内容を実装(オーバーライド)する
}

void BaseScene::Init()
{
	m_renderTargetPack.CreateRenderTarget(1280, 720, true);
	// 各シーンで必要な内容を実装(オーバーライド)する
}
