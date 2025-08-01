﻿#pragma once

class BaseScene
{
public :

	BaseScene() { Init(); }
	virtual ~BaseScene() {}

	void PreUpdate();
	void Update();
	void PostUpdate();

	void PreDraw();
	void Draw();
	void DrawSprite();
	void DrawDebug();

	// オブジェクトリストを取得
	std::list<std::shared_ptr<KdGameObject>>& GetObjList()
	{
		return m_objList;
	}
	
	std::list<std::shared_ptr<KdGameObject>>& WorkObjList()
	{
		return m_objList;
	}

	// オブジェクトリストに追加
	void AddObject(const std::shared_ptr<KdGameObject>& _obj)
	{
		m_objList.push_back(_obj);
	}
	
	// カメラオブジェクトリストを取得
	void AddCameraObject(const std::shared_ptr<KdGameObject>& _obj)
	{
		m_CameraObjList.push_back(_obj);
	}
	// カメラオブジェクトリストを取得
	std::list<std::shared_ptr<KdGameObject>>& GetCameraObjList()
	{
		return m_CameraObjList;
	}

	void AddMapObject(const std::shared_ptr<KdGameObject>& _obj)
	{
		m_MapObjectList.push_back(_obj);
	}

	std::list<std::shared_ptr<KdGameObject>>& GetMapObjectList()
	{
		return m_MapObjectList;
	}

	virtual std::string GetSceneName() const = 0;

	const KdRenderTargetPack& GetRenderTargetPack() const
	{
		return m_renderTargetPack;
	}

protected :

	// 継承先シーンで必要ならオーバーライドする
	virtual void Event();
	virtual void Init();

	KdTexture m_defaultTexture;
	KdRenderTargetChanger m_renderTargetChanger;
	KdRenderTargetPack m_renderTargetPack;

	// 全オブジェクトのアドレスをリストで管理
	std::list<std::shared_ptr<KdGameObject>> m_objList;
	std::list<std::shared_ptr<KdGameObject>> m_CameraObjList;
	std::list<std::shared_ptr<KdGameObject>> m_MapObjectList;
};
