#include "discord_state.h"

#include <fstream>

DiscordState::DiscordState(PlayManager& playManager) : playManager(playManager) {
    std::ifstream ifs("priv/discord.conf");
    discordConf.load(ifs);

    conn.setToken(discordConf.get("token"));
    conn.setSessionId(discordConf.get("session_id"), discordConf.get_int("session_seq"));
    conn.connect(api);
    conn.setMessageCallback([&playManager](discord::gateway::Message const& m) {
    });

    discord::gateway::StatusInfo status;
    status.since = std::chrono::system_clock::now();
    status.status = "online";
    status.activity.name = "over Mojang";
    status.activity.type = (discord::gateway::Activity::Type) 3;
    conn.setStatus(status);
}

void DiscordState::onMessage(discord::gateway::Message const& m) {
    if (m.content.size() > 0 && m.content[0] == '!') {
        std::string command = m.content;
        auto it = command.find(' ');
        if (it == std::string::npos)
            command = m.content.substr(0, it);
        if (command == "!get_version") {
            auto detailsARM = playManager.getDeviceARM().getApi().details("com.mojang.minecraftpe");
            printf("version = %i\n", detailsARM.payload().detailsresponse().docv2().details().appdetails().versioncode());
        }
    }
}

void DiscordState::loop() {
    conn.loop();
}

void DiscordState::storeSessionInfo() {
    printf("Storing session information\n");
    discordConf.set("session_id", conn.getSession());
    discordConf.set_int("session_seq", conn.getSessionSeq());
    std::ofstream ofs("priv/discord.conf");
    discordConf.save(ofs);
}