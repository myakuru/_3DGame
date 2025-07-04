#pragma once
class ImGuiManager : public KdGameObject
{
public:
	ImGuiManager() = default;
	~ImGuiManager() override = default;

	void ImGuiUpdate() const;

	void Hierarchy() const;

	void MainMenuBar() const;

	void TreeNode() const;

private:
	std::string m_modelPath = "Asset/";

};