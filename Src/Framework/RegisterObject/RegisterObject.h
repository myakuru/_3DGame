#pragma once
class KdGameObject;
class RegisterObject
{
public:
	RegisterObject() = default;
	~RegisterObject() = default;

	template<typename T>
	void Register()
	{
		std::string className = typeid(T).name();

		auto registerFunc = [className]()
			{
				std::shared_ptr<T> NewClass = std::make_shared<T>();
				return NewClass;
			};

		m_RegisterObject.emplace(className, registerFunc);
	}

	// m_RegisterObjectのゲッター
	const auto &GetRegisterObject() const
	{
		return m_RegisterObject;
	}

private:
	std::map<std::string, std::function<std::shared_ptr<KdGameObject>()>,std::less<> > m_RegisterObject;

public:
	// シングルトンインスタンスを取得する関数
	static RegisterObject& GetInstance()
	{
		static RegisterObject instance;
		return instance;
	}
};