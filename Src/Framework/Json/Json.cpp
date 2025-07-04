﻿#include "Json.h"
#include"../../Application/Scene/SceneManager.h"
#include"../../Application/Scene/BaseScene/BaseScene.h"
#include"../RegisterObject/RegisterObject.h"

void JsonManager::JsonToObj() const
{
	auto name = SceneManager::GetInstance().GetCurrentScene()->GetSceneName();
	nlohmann::json json = JsonDeserialize("Json/" + name);

	for (auto& it : json)
	{
		// ゲームオブジェクトに追加
		AddJsonObject(it["Name"], it);
	}
}

void JsonManager::AllSave() const
{
	// jsonの配列の生成
	nlohmann::json json = nlohmann::json::array();
	
	for (auto& it : SceneManager::GetInstance().GetObjList())
	{
		nlohmann::json jsonObj;
		it->JsonSave(jsonObj);
		json.push_back(jsonObj);
	}

	std::string nowScene = SceneManager::GetInstance().GetCurrentScene()->GetSceneName();

	JsonSerialize(json, "Json/" + nowScene);
}

std::shared_ptr<KdGameObject> JsonManager::AddJsonObject(const std::string& _className, const nlohmann::json& _json) const
{

	// 一発検索して、インスタンスを生成させる
	if (auto found = RegisterObject::GetInstance().GetRegisterObject().find(_className);
		found != RegisterObject::GetInstance().GetRegisterObject().end())
	{
		// バリューにインスタンスを生成するラムダ式入ってるからそれを呼び出す
		std::shared_ptr<KdGameObject> obj = found->second();

		// 各オブジェクトを,jsonファイルから読み込む
		if (!_json.is_null()) obj->JsonInput(_json);

		SceneManager::GetInstance().AddObject(obj);	// シーンに追加
		obj->Init();

		return obj;

	}
	// 見つからなかった場合は,nullptrを返す
	return nullptr;

}

void JsonManager::JsonSerialize(const nlohmann::json& _json, const std::string& _path) const
{
	std::ofstream outPut((_path + ".json").data());

	// 開かれてない場合リターン
	if (!outPut.is_open()) return;
	outPut << _json.dump(2);	// jsonにフォーマットする(2スペース)
	outPut.close();

}

nlohmann::json JsonManager::JsonDeserialize(const std::string & _path)const
{
	std::ifstream input((_path + ".json").data());
	nlohmann::json json;

	if (!input.is_open())return json;
	input >> json;
	input.close();

	return json;
}

Math::Vector3 JsonManager::JsonToVector(const nlohmann::json& _json) const
{
	return Math::Vector3
	{
		_json["x"],
		_json["y"],
		_json["z"]
	};
}

nlohmann::json JsonManager::VectorToJson(const Math::Vector3& _vec) const
{
	return nlohmann::json
	{
		{"x", _vec.x },
		{"y",_vec.y },
		{"z", _vec.z}
	};
}
