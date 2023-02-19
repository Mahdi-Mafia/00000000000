#include <SDK/mta_sdk.h>
#include <SDK/game/CEntity.h>
#include <discord_game_sdk.h>

struct DiscordEventHandlers handlers;
DiscordUserId userId;
DiscordUser user;
DiscordRichPresence discordPresence;

// MTA:SA module
MTAEXPORT bool InitModule(char* moduleName)
{
    // Initialize Discord GameSDK
    Discord_Initialize("Discord Application Client ID", &handlers, 1, "Discord User Hash", &userId);
    
    // Setup Rich Presence
    memset(&discordPresence, 0, sizeof(discordPresence));
    discordPresence.details = "Playing MTA:SA";
    discordPresence.startTimestamp = time(NULL);
    Discord_UpdatePresence(&discordPresence);
    
    //joinSecret for button
    handlers.joinGame = [](const char* joinSecret) {
        // Connect to server using the joinSecret as server IP
        char cmd[256];
        sprintf(cmd, "connect %s", joinSecret);
        executeCommandHandler(cmd);
    };
    
    return true;
}

MTAEXPORT bool ShutdownModule()
{
    Discord_Shutdown();    
    return true;
}

MTAEXPORT bool ResourceStopping(lua_State* luaVM)
{
    // Clear Rich Presence
    memset(&discordPresence, 0, sizeof(discordPresence));
    Discord_UpdatePresence(&discordPresence);
    
    return true;
}

MTAEXPORT void onPlayerConnect(int playerId)
{
    //Update Rich Presence
    char presenceState[256];
    sprintf(presenceState, "Server: %s | Players: %d/%d", getPlayerServerName(playerId), getPlayerCount(), getMaxPlayers());
    discordPresence.state = presenceState;
    discordPresence.joinSecret = getServerIP(); // Set the joinSecret to the server IP and port
    Discord_UpdatePresence(&discordPresence);
}

MTAEXPORT void onPlayerDisconnect(int playerId, const char* reason)
{
    char presenceState[256];
    sprintf(presenceState, "Server: %s | Players: %d/%d", getServerName(), getPlayerCount(), getMaxPlayers());
    discordPresence.state = presenceState;
    Discord_UpdatePresence(&discordPresence);
}
