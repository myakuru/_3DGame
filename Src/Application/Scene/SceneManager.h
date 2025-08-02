#pragma once
class BaseScene;
class SceneManager
{
public :

	// シーン情報
	enum class SceneType
	{
		Title,
		Game,
	};

	// シーンの初期化
	void Init();

	void PreUpdate();
	void Update();
	void PostUpdate();

	void PreDraw();
	void Draw();
	void DrawSprite();
	void DrawDebug();

	// 次のシーンをセット (次のフレームから切り替わる)
	void SetNextScene(SceneType _nextScene)
	{
		m_nextSceneType = _nextScene;
	}

	// 現在のシーンのオブジェクトリストを取得
	std::list<std::shared_ptr<KdGameObject>>& GetObjList();
	std::list<std::shared_ptr<KdGameObject>>& GetCameraList();
	std::list<std::shared_ptr<KdGameObject>>& GetMapList();

	// 現在のシーンにオブジェクトを追加
	void AddObject(const std::shared_ptr<KdGameObject>& _obj);

	// シーンの種類をゲットするゲッター
	std::shared_ptr<BaseScene> GetCurrentScene() { return m_currentScene; }

	// 選択したオブジェクトをセット
	std::shared_ptr<KdGameObject> m_selectObject = nullptr;

	// 指定した型のオブジェクトを取得
	template<class T>
	std::shared_ptr<T> FindObjectOfType()
	{
		uint32_t typeId = T::TypeID;
		for (auto& obj : GetObjList())
		{
			if (obj->GetTypeID() == typeId)
			{
				return std::static_pointer_cast<T>(obj);
			}
		}
		return nullptr;
	}

	// 指定されたオブジェクトのウェークポインタを取得
	template<class T>
	void GetObjectWeakPtr(std::weak_ptr<T>& outPtr)
	{
		if (outPtr.expired())
		{
			if (auto findObj = FindObjectOfType<T>())
			{
				outPtr = findObj;
			}
		}
	}

	// カメラのオブジェクトを取得
	template<typename T>
	std::shared_ptr<T> FindCameraObj()
	{
		uint32_t typeId = T::TypeID;
		for (auto& cameraObj : GetCameraList())
		{
			if (cameraObj->GetTypeID() == typeId)
			{
				// キャストして返す
				return std::static_pointer_cast<T>(cameraObj);
			}
		}
		return nullptr;
	}

	// カメラのウィークポインタを取得
	template<typename T>
	void GetCameraWeakPtr(std::weak_ptr<T>& _ptr)
	{
		if (_ptr.expired())
		{
			if (auto findCamera = FindCameraObj<T>())
			{
				_ptr = findCamera;
			}
		}
	}

	// シーンカメラかどうか
	bool m_sceneCamera = true;

private :

	// RegisterObjectからオブジェクトを登録する関数
	void Register() const;

	// シーン切り替え関数
	void ChangeScene(SceneType _sceneType);

	// 現在のシーンのインスタンスを保持しているポインタ
	std::shared_ptr<BaseScene> m_currentScene = nullptr;

	// 現在のシーンの種類を保持している変数
	SceneType m_currentSceneType = SceneType::Game;
	
	// 次のシーンの種類を保持している変数
	SceneType m_nextSceneType = m_currentSceneType;

	SceneManager() = default;
	~SceneManager() = default;

public:

	// シングルトンパターン
	// 常に存在する && 必ず1つしか存在しない(1つしか存在出来ない)
	// どこからでもアクセスが可能で便利だが
	// 何でもかんでもシングルトンという思考はNG
	static SceneManager& Instance()
	{
		static SceneManager instance;
		return instance;
	}
};