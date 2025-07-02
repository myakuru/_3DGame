#pragma once
class ImGuiManager : public KdGameObject
{
public:
	ImGuiManager() = default;
	~ImGuiManager() override = default;

	void ImGuiUpdate();

	void Hierarchy();

	void MainMenuBar() const;

private:
	std::string m_modelPath = "Asset/";

};