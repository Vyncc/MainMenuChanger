#pragma once

#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"

#include "version.h"
constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);

#include "fstream"
#include "nlohmann/json.hpp"


struct Settings
{
	std::string pluginVersion;
	bool loadRandomConfig = true;
	bool loadSelectedConfig = false;
	std::string selectedConfig;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Settings, pluginVersion, loadRandomConfig, loadSelectedConfig, selectedConfig)

struct Config
{
	std::string pluginVersion;
	Vector location = Vector(0, 0, 0);
	Rotator rotation = Rotator(0, 0, 0);
	float drawScale = 1.f;

	float cameraFOV = 90.f;
	Vector cameraClipOffset = Vector(0, 0, 0);
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Vector, X, Y, Z)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Rotator, Pitch, Yaw, Roll)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Config, pluginVersion, location, rotation, drawScale, cameraFOV, cameraClipOffset)

class MainMenuChanger: public BakkesMod::Plugin::BakkesModPlugin, public BakkesMod::Plugin::PluginSettingsWindow/*, public BakkesMod::Plugin::PluginWindow*/
{
	std::string PluginVersion = "1.0";

	std::filesystem::path PluginDataPath;
	std::filesystem::path SettingsPath;
	std::filesystem::path ConfigsPath;

	Settings settings;
	void SaveSettings();
	void LoadSettings();
	void RenderPluginSettings();

	Config config;
	void SaveConfig(std::string configName);
	void RenderSaveConfig();
	void LoadConfig(std::filesystem::path configPath);
	void RenderLoadConfig();
	void RenderResetConfig();

	void OnMainMenuAdded(std::string eventName);
	void CarPreviewActorTick(ActorWrapper caller, void* params, std::string eventName);
	void CameraTick(ActorWrapper caller, void* params, std::string eventName);

	std::vector<std::filesystem::path> GetFilesInDirectory(std::filesystem::path directoryPath);
	int RandomNumber(int min, int max);
	bool ConfigFileExists(std::string configFileName);

	//Boilerplate
	virtual void onLoad();
	virtual void onUnload();

	// Inherited via PluginSettingsWindow
	void RenderSettings() override;
	std::string GetPluginName() override;
	void SetImGuiContext(uintptr_t ctx) override;
};

