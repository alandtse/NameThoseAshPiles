#include "Manager.h"

void OnInit(SKSE::MessagingInterface::Message* a_msg)
{
	if (a_msg->type == SKSE::MessagingInterface::kDataLoaded) {
		CrosshairRefManager::GetSingleton()->Register();
	}
}

extern "C" __declspec(dllexport) constexpr auto SKSEPlugin_Version = []() {
	SKSE::PluginVersionData v{};
	v.pluginVersion = Version::MAJOR;
	v.PluginName("Name Those Ashpiles");
	v.AuthorName("powerofthree"sv);
	v.CompatibleVersions({ SKSE::RUNTIME_1_6_318 });
	return v;
}();

bool InitLogger()
{
	auto path = logger::log_directory();
	if (!path) {
		return false;
	}

	*path /= Version::PROJECT;
	*path += ".log"sv;
	auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);

	auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));

	log->set_level(spdlog::level::info);
	log->flush_on(spdlog::level::info);

	spdlog::set_default_logger(std::move(log));
	spdlog::set_pattern("[%H:%M:%S:%e] %v"s);

	logger::info(FMT_STRING("{} v{}"), Version::PROJECT, Version::NAME);

	return true;
}

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
	if (!InitLogger()) {
		return false;
	}

	logger::info("loaded");

	SKSE::Init(a_skse);

	NameManager::GetSingleton()->LoadNames();

	const auto messaging = SKSE::GetMessagingInterface();
	messaging->RegisterListener(OnInit);

	return true;
}
