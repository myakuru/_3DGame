#pragma once

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
	const std::list<std::shared_ptr<KdGameObject>>& GetObjList()
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
	
	virtual std::string GetSceneName() const = 0;

	KdRenderTargetPack GetRenderTargetPack()
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
};
