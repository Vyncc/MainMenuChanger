#include "pch.h"
#include "MainMenuChanger.h"


BAKKESMOD_PLUGIN(MainMenuChanger, "MainMenuChanger", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

void MainMenuChanger::onLoad()
{
	_globalCvarManager = cvarManager;

	PluginDataPath = gameWrapper->GetDataFolder() / "MainMenuChanger";
	SettingsPath = PluginDataPath / "settings.json";
	ConfigsPath = PluginDataPath / "Configs";

	if (!std::filesystem::exists(ConfigsPath))
	{
		create_directories(ConfigsPath);
	}

	LoadSettings();

	gameWrapper->HookEvent("Function TAGame.GFxData_MainMenu_TA.MainMenuAdded", std::bind(&MainMenuChanger::OnMainMenuAdded, this, std::placeholders::_1));
	gameWrapper->HookEventWithCaller<ActorWrapper>("Function TAGame.CarPreviewActor_TA.Tick", std::bind(&MainMenuChanger::CarPreviewActorTick, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	gameWrapper->HookEventWithCaller<ActorWrapper>("Function TAGame.Camera_MainMenu_TA.PostProcessCameraState", std::bind(&MainMenuChanger::CameraTick, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void MainMenuChanger::OnMainMenuAdded(std::string eventName)
{
	try
	{
		std::vector<std::filesystem::path> config_files = GetFilesInDirectory(ConfigsPath);

		if (config_files.size() <= 0)
		{
			LOG("No config files detected in configs directory : {}", ConfigsPath.string());
			return;
		}

		if (settings.loadRandomConfig)
		{
			std::filesystem::path random_config_file = config_files[RandomNumber(0, config_files.size() - 1)];
			LoadConfig(random_config_file);
		}
		else if (settings.loadSelectedConfig && settings.selectedConfig != "")
		{
			if (ConfigFileExists(settings.selectedConfig))
			{
				LoadConfig(ConfigsPath / settings.selectedConfig);
			}
		}
	}
	catch (const std::filesystem::filesystem_error& e)
	{
		LOG("Error : {}", e.what());
	}
}

void MainMenuChanger::CarPreviewActorTick(ActorWrapper caller, void* params, std::string eventName)
{
	//identifies the right car/preview actor
	if (!caller.GetbHidden() && !caller.GetbHiddenSelf() && caller.GetOwner())
	{
		//Really ugly thing, but SetLocation from ActoWrapper doesn't work
		CameraWrapper previewActor = CameraWrapper(caller.memory_address);
		previewActor.SetLocation(config.location);

		caller.SetRotation(config.rotation);
		caller.SetDrawScale(config.drawScale);
	}
}

void MainMenuChanger::CameraTick(ActorWrapper caller, void* params, std::string eventName)
{
	CameraXWrapper camera = CameraXWrapper(caller.memory_address);

	camera.SetFOV(config.cameraFOV);
	camera.SetClipOffset(config.cameraClipOffset);
}

void MainMenuChanger::SaveSettings()
{
	settings.pluginVersion = PluginVersion;
	nlohmann::json settings_json = settings;
	if (!std::filesystem::exists(SettingsPath.parent_path()))
	{
		create_directories(SettingsPath.parent_path());
	}
	auto settings_file = std::ofstream(SettingsPath);
	settings_file << settings_json.dump();
	LOG("Settings saved in : {}", SettingsPath.string());
}

void MainMenuChanger::LoadSettings()
{
	if (std::filesystem::exists(SettingsPath))
	{
		auto settings_file = std::ifstream(SettingsPath);
		nlohmann::json asd_read_as_json = nlohmann::json::parse(settings_file);
		settings = asd_read_as_json.get<Settings>();
		LOG("Settings loaded");
	}
	else
	{
		SaveSettings();
	}
}

void MainMenuChanger::SaveConfig(std::string configName)
{
	config.pluginVersion = PluginVersion;
	nlohmann::json config_json = config;
	auto config_path = ConfigsPath / std::string(configName + ".json");
	if (!std::filesystem::exists(ConfigsPath))
	{
		create_directories(ConfigsPath);
	}
	auto config_file = std::ofstream(config_path);
	config_file << config_json.dump();
	LOG("Config saved : {}", config_path.filename().string());
}

void MainMenuChanger::LoadConfig(std::filesystem::path configPath)
{
	if (std::filesystem::exists(SettingsPath))
	{
		auto config_file = std::ifstream(configPath);
		nlohmann::json config_json = nlohmann::json::parse(config_file);
		config = config_json.get<Config>();
		LOG("Config loaded : {}", configPath.filename().string());
	}
	else
	{
		LOG("Can't load config file : {}", configPath.string());
	}
}

std::vector<std::filesystem::path> MainMenuChanger::GetFilesInDirectory(std::filesystem::path directoryPath)
{
	std::vector<std::filesystem::path> files;

	for (const auto& entry : std::filesystem::directory_iterator(directoryPath))
	{
		if (std::filesystem::is_regular_file(entry))
		{
			files.push_back(entry.path());
		}
	}

	return files;
}

int MainMenuChanger::RandomNumber(int min, int max)
{
	int range = max - min + 1;
	return rand() % range + min;
}

bool MainMenuChanger::ConfigFileExists(std::string configFileName)
{
	for (const auto& entry : std::filesystem::directory_iterator(ConfigsPath))
	{
		if (entry.is_regular_file() && entry.path().filename().string() == configFileName)
		{
			return true;
		}
	}

	return false;
}

void MainMenuChanger::onUnload()
{
}