#include "Hooks.cpp"
#include "Settings.h"

// SFSE message listener, use this to do stuff at specific moments during runtime
void Listener(SFSE::MessagingInterface::Message* message) noexcept
{
    if (message->type == SFSE::MessagingInterface::kPostLoad) {
        Settings::LoadSettings();
        Hooks::Install();
    }
}

// Main SFSE plugin entry point, initialize everything here
SFSEPluginLoad(const SFSE::LoadInterface* sfse)
{
    Init(sfse);

    logger::info("{} {} is loading...", Plugin::Name, Plugin::Version.string());

    if (const auto messaging{ SFSE::GetMessagingInterface() }; !messaging->RegisterListener(Listener)) {
        return false;
    }

    logger::info("{} has finished loading.", Plugin::Name);
    logger::info("");

    return true;
}
