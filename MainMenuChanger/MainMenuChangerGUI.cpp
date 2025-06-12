#include "pch.h"
#include "MainMenuChanger.h"

#include "IMGUI/imgui_searchablecombo.h"
#include "CustomWidgets.hpp"

std::string MainMenuChanger::GetPluginName()
{
	return "MainMenuChanger";
}

void MainMenuChanger::SetImGuiContext(uintptr_t ctx)
{
	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
}

// Render the plugin settings here
// This will show up in bakkesmod when the plugin is loaded at
//  f2 -> plugins -> MainMenuChanger
void MainMenuChanger::RenderSettings()
{
	RenderPluginSettings();

	ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y + 5.f));
	ImGui::Separator();
	ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y + 5.f));

	ImGui::BeginChild("##Config", ImVec2{ 338.f, 505.f }, true);

	ImGui::PushID("Car");

	CustomWidget::CenterNexIMGUItItem(ImGui::CalcTextSize("Car").x);
	ImGui::Text("Car");
	CustomWidget::UnderLine(ImColor(1.f, 1.f, 1.f, 0.7));

	ImGui::NewLine();

	ImGui::Text("Location :");
	ImGui::SetNextItemWidth(258.f);
	ImGui::DragFloat("X", &config.location.X);
	ImGui::SetNextItemWidth(258.f);
	ImGui::DragFloat("Y", &config.location.Y);
	ImGui::SetNextItemWidth(258.f);
	ImGui::DragFloat("Z", &config.location.Z);

	ImGui::NewLine();

	ImGui::Text("Rotation :");
	ImGui::SetNextItemWidth(258.f);
	ImGui::SliderInt("Pitch", &config.rotation.Pitch, -16364, 16340);
	ImGui::SetNextItemWidth(258.f);
	ImGui::SliderInt("Yaw", &config.rotation.Yaw, -32768, 32764);
	ImGui::SetNextItemWidth(258.f);
	ImGui::SliderInt("Roll", &config.rotation.Roll, -32768, 32764);

	ImGui::NewLine();

	ImGui::SetNextItemWidth(258.f);
	ImGui::DragFloat("DrawScale", &config.drawScale, 0.05f);

	ImGui::PopID();

	ImGui::NewLine();
	ImGui::Separator();

	ImGui::PushID("Camera");

	CustomWidget::CenterNexIMGUItItem(ImGui::CalcTextSize("Camera").x);
	ImGui::Text("Camera");
	CustomWidget::UnderLine(ImColor(1.f, 1.f, 1.f, 0.7));

	ImGui::NewLine();

	ImGui::Text("Location :");
	ImGui::SetNextItemWidth(258.f);
	ImGui::DragFloat("X", &config.cameraClipOffset.X);
	ImGui::SetNextItemWidth(258.f);
	ImGui::DragFloat("Y", &config.cameraClipOffset.Y);
	ImGui::SetNextItemWidth(258.f);
	ImGui::DragFloat("Z", &config.cameraClipOffset.Z);

	ImGui::NewLine();

	ImGui::SetNextItemWidth(258.f);
	ImGui::SliderFloat("FOV", &config.cameraFOV, 0.f, 170.f);

	ImGui::PopID();

	ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y + 5.f));
	ImGui::Separator();
	ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y + 5.f));

	CustomWidget::CenterNexIMGUItItem(316.f);


	if (ImGui::Button("Reset", ImVec2(100.f, 30.f)))
	{
		ImGui::OpenPopup("Reset Config");
	}
	RenderResetConfig();

	ImGui::SameLine();

	if (ImGui::Button("Save Config", ImVec2(100.f, 30.f)))
	{
		ImGui::OpenPopup("Save Config");
	}
	RenderSaveConfig();

	ImGui::SameLine();

	if (ImGui::Button("Load Config", ImVec2(100.f, 30.f)))
	{
		ImGui::OpenPopup("Load Config");
	}
	RenderLoadConfig();

	ImGui::EndChild();
}

void MainMenuChanger::RenderPluginSettings()
{
	//ImGui::BeginChild("##Settings", ImVec2{ 338.f, 90.f }, true);

	ImGui::Text("Settings");
	CustomWidget::UnderLine(ImColor(1.f, 1.f, 1.f, 0.7));

	ImGui::NewLine();


	if (settings.loadSelectedConfig)
	{
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}

	if (ImGui::Checkbox("Load Random Config", &settings.loadRandomConfig))
	{
		SaveSettings();
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::Text("Load a random config everytime the main menu loads");
		ImGui::EndTooltip();
	}

	if (settings.loadSelectedConfig)
	{
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}

	if (settings.loadRandomConfig)
	{
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}

	ImGui::BeginGroup();

	if (ImGui::Checkbox("Load Selected Config", &settings.loadSelectedConfig))
	{
		SaveSettings();
	}

	ImGui::SameLine();
	ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y - 2.f));
	ImGui::SetNextItemWidth(258.f);

	static char selectedConfigBuff[64] = "";
	ImGui::SetNextItemWidth(200.f);
	if (ImGui::BeginSearchableCombo("##SelectedConfigCombo", settings.selectedConfig.c_str(), selectedConfigBuff, IM_ARRAYSIZE(selectedConfigBuff), "type to search"))
	{
		for (const auto& entry : std::filesystem::directory_iterator(ConfigsPath))
		{
			std::string fileName = entry.path().filename().string();

			if (entry.is_regular_file() && fileName.find(std::string(selectedConfigBuff)) != std::string::npos)
			{
				if (ImGui::Selectable(fileName.c_str()))
				{
					settings.selectedConfig = fileName;
					SaveSettings();
				}

				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::Text(fileName.c_str());
					ImGui::EndTooltip();
				}
			}
		}

		ImGui::EndSearchableCombo();
	}

	ImGui::EndGroup();

	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::Text("Load the selected config everytime the main menu loads");
		ImGui::EndTooltip();
	}
	
	if (!ConfigFileExists(settings.selectedConfig))
	{
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "This config file doesn't exist !");
	}

	if (settings.loadRandomConfig)
	{
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}

	//ImGui::EndChild();
}

void MainMenuChanger::RenderSaveConfig()
{
	if (ImGui::BeginPopupModal("Save Config", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("File name :");
		ImGui::SameLine();
		static char FileName[32] = "";
		ImGui::InputText("##FileName", FileName, IM_ARRAYSIZE(FileName));

		if (ImGui::Button("Cancel", ImVec2(100.f, 30.f)))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();

		CustomWidget::AlignRightNexIMGUItItem(100.f, 8.f);
		if (ImGui::Button("Save", ImVec2(100.f, 25.f)))
		{
			if (FileName != "")
			{
				SaveConfig(std::string(FileName));

				ImGui::CloseCurrentPopup();
			}
		}
		ImGui::EndPopup();
	}
}

void MainMenuChanger::RenderLoadConfig()
{
	if (ImGui::BeginPopupModal("Load Config", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		if (std::filesystem::exists(ConfigsPath))
		{
			ImGui::BeginChild("##ConfigsList", ImVec2{ 200, 200 }, true);

			for (const auto& file : std::filesystem::directory_iterator(ConfigsPath))
			{
				if (!file.is_regular_file()) continue;

				if (ImGui::Selectable(file.path().filename().string().c_str()))
				{
					LoadConfig(file.path());
					ImGui::CloseCurrentPopup();
				}
			}

			ImGui::EndChild();
		}
		else
		{
			ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), std::string("The directory " + ConfigsPath.string() + " doesn't exist !").c_str());
		}

		CustomWidget::CenterNexIMGUItItem(100.f);
		if (ImGui::Button("Close", ImVec2(100.f, 30.f)))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void MainMenuChanger::RenderResetConfig()
{
	CustomWidget::YesNoPopup("Reset Config", "Do you really want to reset the current config ?", [&]() {
		config = Config();
		ImGui::CloseCurrentPopup();
		}, [&]() {
			ImGui::CloseCurrentPopup(); 
			});
}