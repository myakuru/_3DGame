#pragma once
#include"../Utility/SelectDraw3dModel.h"
#include"../../../MyFramework/State/StateManager/StateManager.h"
class PlayerStateBase;
class CharaBase:public SelectDrawObject
{
public:
	CharaBase() = default;
	~CharaBase() override = default;

protected:

	/// <summary>
	/// KdGameObjectから派生された関数
	/// </summary>
	void Init() override;
	void DrawToon() override;
	void Update() override;
	bool ModelLoad(std::string _path) override;
	void ImGuiInspector() override;
	void JsonInput(const nlohmann::json& _json) override;
	void JsonSave(nlohmann::json& _json) const override;

	/// <summary>
	/// ステート関係の関数
	/// </summary>
	virtual void StateInit() {};
	virtual void ChangeState(std::shared_ptr<PlayerStateBase> _state) {};
	StateManager m_stateManager;

	/// <summary>
	/// 変数関係
	/// </summary>
	std::shared_ptr<KdModelWork>	m_modelWork = std::make_shared<KdModelWork>();
	std::shared_ptr <KdAnimator>	m_animator = std::make_shared<KdAnimator>();

};