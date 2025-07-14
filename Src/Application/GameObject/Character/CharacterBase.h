#pragma once
#include"../Utility/SelectDraw3dModel.h"
class CharaBase:public SelectDrawObject
{
public:
	CharaBase() = default;
	~CharaBase() override = default;

protected:

	void Init() override;
	void DrawToon() override;
	void Update() override;
	bool ModelLoad(std::string _path) override;
	void ImGuiInspector() override;
	void JsonInput(const nlohmann::json& _json) override;
	void JsonSave(nlohmann::json& _json) const override;

	std::shared_ptr<KdModelWork>	m_modelWork = std::make_shared<KdModelWork>();
	std::shared_ptr <KdAnimator>	m_animator = std::make_shared<KdAnimator>();

};