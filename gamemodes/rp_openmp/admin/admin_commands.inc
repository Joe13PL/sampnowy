/*
 * =============================================================================
 *  ADMIN COMMANDS - Administrative commands
 * =============================================================================
 *
 *  Module: admin/admin_commands.inc
 *  Description: Commands for server administrators
 *
 *  CHANGES IN REFACTORING:
 *  - Replaced ZCMD with y_commands (YCMD)
 *  - Added access level verification
 *  - Organized commands by category
 *  - Improved action logging
 *
 * =============================================================================
 */

// Suppress warning about tag mismatch
#pragma warning disable 213

#if defined _admin_commands_included
    #endinput
#endif
#define _admin_commands_included

// ===========================================================================
// ADMIN LEVELS
// ===========================================================================

#define ADMIN_LEVEL_HELPER      1       // Helper
#define ADMIN_LEVEL_SUPPORT     2       // Support
#define ADMIN_LEVEL_MODERATOR   3       // Moderator
#define ADMIN_LEVEL_ADMIN       4       // Administrator
#define ADMIN_LEVEL_HEADADMIN   5       // Head Admin
#define ADMIN_LEVEL_MANAGEMENT  6       // Management

// ===========================================================================
// MACROS FOR CHECKING PERMISSIONS
// ===========================================================================

#define ADMIN_CHECK(%0) \
    if(!Admin_HasAccess(playerid, %0)) return SendClientMessage(playerid, COLOR_RED, "You do not have permission to use this command!")

// ===========================================================================
// HELPER FUNCTIONS
// ===========================================================================

/**
 * Checks if a player has admin permissions
 * @param playerid Player ID
 * @param level Required level
 * @return true if they have
 */
stock bool:Admin_HasAccess(playerid, level)
{
    if(!IsValidPlayerId(playerid) || !pInfo[playerid][player_logged])
    {
        return false;
    }

    return pGlobal[playerid][glo_admin] >= level;
}

/**
 * Gets the admin level of a player
 * @param playerid Player ID
 * @return Admin level
 */
stock Admin_GetLevel(playerid)
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }

    return pGlobal[playerid][glo_admin];
}

/**
 * Gets the name of the admin rank
 * @param level Level
 * @param output Output buffer
 * @param size Size of the buffer
 */
stock Admin_GetRankName(level, output[], size = 32)
{
    switch(level)
    {
        case 1: strcopy(output, "Helper", size);
        case 2: strcopy(output, "Support", size);
        case 3: strcopy(output, "Moderator", size);
        case 4: strcopy(output, "Administrator", size);
        case 5: strcopy(output, "Head Admin", size);
        case 6: strcopy(output, "Management", size);
        default: strcopy(output, "Player", size);
    }
}

// -------------------------------------------------------------------------
// Admin group flags (fallback defaults used if not defined elsewhere)
// -------------------------------------------------------------------------
new Admin_GroupFlags[][] = { {1}, {2}, {4}, {8}, {16}, {32}, {64}, {128} };
new Admin_GroupFlagNames[][] = { "ManageMembers", "ManageFinances", "EditRanks", "Invite", "Kick", "Promote", "Demote", "Other" };

// Abilities defaults
new Admin_GroupAbilities[][] = { {1}, {2}, {4}, {8}, {16}, {32}, {64} };
new Admin_GroupAbilityNames[][] = { "Kick players", "Ban players", "Manage funds", "Edit jobs", "Edit properties", "Manage servers", "Developer tools" };

/**
 * Logs an administrative action
 * @param adminid Admin ID
 * @param action Action
 */
stock Admin_LogAction(adminid, const action[])
{
    new logMsg[256];
    new adminName[MAX_PLAYER_NAME + 1];

    if(adminid != -1 && IsPlayerConnected(adminid))
    {
        GetPlayerName(adminid, adminName, sizeof(adminName));
    }
    else
    {
        strcopy(adminName, "SYSTEM", sizeof(adminName));
    }

    new esc[256]; MySQL_EscapeString(action, esc, sizeof(esc));
    new admin_uid = 0;
    if(adminid != -1 && IsPlayerConnected(adminid)) admin_uid = pInfo[adminid][player_uid];
    MySQL_ExecuteFormat("INSERT INTO `admin_logs` (`log_admin`,`log_action`,`log_time`) VALUES (%d, '%s', %d)", admin_uid, esc, gettime());
    return 1;
}

// ===========================================================================
// COMMANDS - HELP
// ===========================================================================

// Debug command to check admin level
YCMD:myadmin(playerid, params[], help)
{
    new msg[128];
    format(msg, sizeof(msg), "Your admin level: pGlobal=%d, pInfo=%d",
        pGlobal[playerid][glo_admin],
        pInfo[playerid][player_admin]);
    SendClientMessage(playerid, COLOR_YELLOW, msg);
    return 1;
}

/**
 * Admin: Run phone self-tests (creates sample SMS/call, populates lists, tests queue flush)
 * Usage: /phone_test
 */
YCMD:phone_test(playerid, params[], help)
{
    ADMIN_CHECK(ADMIN_LEVEL_HELPER);

    if(help)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "Runs phone system self-tests and basic validations.");
        return 1;
    }

    Phone_RunSelfTests(playerid);
    return 1;
}

YCMD:ahelp(playerid, params[], help)
{
    ADMIN_CHECK(ADMIN_LEVEL_HELPER);

    if(help)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "Displays a list of administrative commands.");
        return 1;
    }

    new adminLevel = Admin_GetLevel(playerid);

    SendClientMessage(playerid, COLOR_LIGHTER_BLUE, "============== ADMINISTRATIVE COMMANDS ==============");
    SendClientMessage(playerid, COLOR_WHITE, "Your level: %d", adminLevel);
    SendClientMessage(playerid, COLOR_LIGHTER_BLUE, "======================================================");

    // Level 1 - Helper
    SendClientMessage(playerid, COLOR_YELLOW, "[Helper] /a /admins /check /goto /gethere /spec /specoff");

    if(adminLevel >= ADMIN_LEVEL_SUPPORT)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "[Support] /kick /mute /unmute /freeze /unfreeze /slap /warn");
        SendClientMessage(playerid, COLOR_YELLOW, "[Support] /respawn /setint /setvw /getpos /revive");
    }

    if(adminLevel >= ADMIN_LEVEL_MODERATOR)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "[Moderator] /ban /jail /unjail /clearwarns /clearchat");
        SendClientMessage(playerid, COLOR_YELLOW, "[Moderator] /sethp /setarmour /disarm");
    }

    if(adminLevel >= ADMIN_LEVEL_ADMIN)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "[Admin] /veh /destroyveh /fixveh /flipveh /gotoxyz /tpall");
        SendClientMessage(playerid, COLOR_YELLOW, "[Admin] /setmoney /akill /explode /ag /setadmin");
    }

    if(adminLevel >= ADMIN_LEVEL_HEADADMIN)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "[HeadAdmin] /unban /invisible /god /fly");
    }

    if(adminLevel >= ADMIN_LEVEL_MANAGEMENT)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "[Management] /settime /setweather /gmx");
    }

    SendClientMessage(playerid, COLOR_LIGHTER_BLUE, "======================================================");
    return 1;
}

YCMD:a(playerid, params[], help)
{
    ADMIN_CHECK(ADMIN_LEVEL_HELPER);

    if(help)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "Sends a message to the admin chat.");
        return 1;
    }

    if(isnull(params))
    {
        return SendClientMessage(playerid, COLOR_GRAY, "Usage: /a [message]");
    }

    new rankName[32];
    Admin_GetRankName(pGlobal[playerid][glo_admin], rankName);

    new msg[256];
    format(msg, sizeof(msg), "[ADMIN] %s %s: %s",
        rankName, pInfo[playerid][player_name], params);

    // Send to all admins
    foreach(new i : Player)
    {
        if(Admin_HasAccess(i, ADMIN_LEVEL_HELPER))
        {
            SendClientMessage(i, COLOR_LIGHTRED, msg);
        }
    }

    return 1;
}

YCMD:aduty(playerid, params[], help)
{
    ADMIN_CHECK(ADMIN_LEVEL_SUPPORT);

    if(help)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "Enables/disables administrative duty.");
        return 1;
    }

    if(pInfo[playerid][player_admin_duty])
    {
        pInfo[playerid][player_admin_duty] = false;
        SetPlayerColor(playerid, COLOR_WHITE);
        SendClientMessage(playerid, COLOR_YELLOW, "Disabled administrative duty.");

        Admin_LogAction(playerid, "Disabled admin duty");
    }
    else
    {
        pInfo[playerid][player_admin_duty] = true;
        SetPlayerColor(playerid, COLOR_ADMIN);
        SendClientMessage(playerid, COLOR_YELLOW, "Enabled administrative duty.");

        Admin_LogAction(playerid, "Enabled admin duty");
    }

    return 1;
}

// ===========================================================================
// COMMANDS - PLAYERS
// ===========================================================================

YCMD:kick(playerid, params[], help)
{
    ADMIN_CHECK(ADMIN_LEVEL_SUPPORT);

    if(help)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "Kicks a player from the server.");
        return 1;
    }

    new targetid, reason[128];

    if(sscanf(params, "us[128]", targetid, reason))
    {
        return SendClientMessage(playerid, COLOR_GRAY, "Usage: /kick [ID/Nick] [reason]");
    }

    if(!IsPlayerConnected(targetid))
    {
        return SendClientMessage(playerid, COLOR_RED, "Player is not connected!");
    }

    // Check if the target has a higher level
    if(Admin_GetLevel(targetid) >= Admin_GetLevel(playerid) && targetid != playerid)
    {
        return SendClientMessage(playerid, COLOR_RED, "You cannot kick an admin with the same or higher level!");
    }

    // Announcement
    new msg[256];
    format(msg, sizeof(msg), "{FF6B6B}[ADMIN] %s was kicked by %s. Reason: %s",
        pInfo[targetid][player_name], pInfo[playerid][player_name], reason);
    SendClientMessageToAll(COLOR_WHITE, msg);

    // Log
    new logMsg[256];
    format(logMsg, sizeof(logMsg), "Kick: %s - Reason: %s", pInfo[targetid][player_name], reason);
    Admin_LogAction(playerid, logMsg);

    // Kick
    Player_Kick(targetid, playerid, reason);

    return 1;
}

YCMD:ban(playerid, params[], help)
{
    ADMIN_CHECK(ADMIN_LEVEL_MODERATOR);

    if(help)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "Bans a player on the server.");
        return 1;
    }

    new targetid, duration, reason[128];

    if(sscanf(params, "uds[128]", targetid, duration, reason))
    {
        SendClientMessage(playerid, COLOR_GRAY, "Usage: /ban [ID/Nick] [duration in minutes (0=perm)] [reason]");
        return 1;
    }

    if(!IsPlayerConnected(targetid))
    {
        return SendClientMessage(playerid, COLOR_RED, "Player is not connected!");
    }

    // Check level
    if(Admin_GetLevel(targetid) >= Admin_GetLevel(playerid) && targetid != playerid)
    {
        return SendClientMessage(playerid, COLOR_RED, "You cannot ban an admin with the same or higher level!");
    }

    // Announcement
    new msg[256];
    if(duration > 0)
    {
        new durationStr[32];
        FormatDuration(duration * 60, durationStr, sizeof(durationStr), false);

        format(msg, sizeof(msg), "{FF0000}[ADMIN] %s was banned by %s for %s. Reason: %s",
            pInfo[targetid][player_name], pInfo[playerid][player_name], durationStr, reason);
    }
    else
    {
        format(msg, sizeof(msg), "{FF0000}[ADMIN] %s was permanently banned by %s. Reason: %s",
            pInfo[targetid][player_name], pInfo[playerid][player_name], reason);
    }
    SendClientMessageToAll(COLOR_WHITE, msg);

    // Log
    new logMsg[256];
    format(logMsg, sizeof(logMsg), "Ban: %s - Duration: %d min - Reason: %s",
        pInfo[targetid][player_name], duration, reason);
    Admin_LogAction(playerid, logMsg);

    // Ban
    Player_Ban(targetid, playerid, reason, duration * 60);

    return 1;
}

YCMD:goto(playerid, params[], help)
{
    ADMIN_CHECK(ADMIN_LEVEL_SUPPORT);

    if(help)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "Teleports you to a player.");
        return 1;
    }

    new targetid;

    if(sscanf(params, "u", targetid))
    {
        return SendClientMessage(playerid, COLOR_GRAY, "Usage: /goto [ID/Nick]");
    }

    if(!IsPlayerConnected(targetid))
    {
        return SendClientMessage(playerid, COLOR_RED, "Player is not connected!");
    }

    if(targetid == playerid)
    {
        return SendClientMessage(playerid, COLOR_RED, "You cannot teleport to yourself!");
    }

    new Float:x, Float:y, Float:z;
    GetPlayerPos(targetid, x, y, z);

    SetPlayerInterior(playerid, GetPlayerInterior(targetid));
    SetPlayerVirtualWorld(playerid, GetPlayerVirtualWorld(targetid));
    SetPlayerPos(playerid, x + 1.0, y, z);

    new msg[128];
    format(msg, sizeof(msg), "Teleported to player %s.", pInfo[targetid][player_name]);
    SendClientMessage(playerid, COLOR_GREEN, msg);

    return 1;
}

YCMD:gethere(playerid, params[], help)
{
    ADMIN_CHECK(ADMIN_LEVEL_SUPPORT);

    if(help)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "Teleports a player to you.");
        return 1;
    }

    new targetid;

    if(sscanf(params, "u", targetid))
    {
        return SendClientMessage(playerid, COLOR_GRAY, "Usage: /gethere [ID/Nick]");
    }

    if(!IsPlayerConnected(targetid))
    {
        return SendClientMessage(playerid, COLOR_RED, "Player is not connected!");
    }

    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);

    SetPlayerInterior(targetid, GetPlayerInterior(playerid));
    SetPlayerVirtualWorld(targetid, GetPlayerVirtualWorld(playerid));
    SetPlayerPos(targetid, x + 1.0, y, z);

    new msg[128];
    format(msg, sizeof(msg), "Teleported player %s to you.", pInfo[targetid][player_name]);
    SendClientMessage(playerid, COLOR_GREEN, msg);

    format(msg, sizeof(msg), "You have been teleported to admin %s.", pInfo[playerid][player_name]);
    SendClientMessage(targetid, COLOR_YELLOW, msg);

    return 1;
}

YCMD:spec(playerid, params[], help)
{
    ADMIN_CHECK(ADMIN_LEVEL_SUPPORT);

    if(help)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "Spectates a player.");
        return 1;
    }

    if(isnull(params))
    {
        // Disable spectate
        if(pInfo[playerid][player_spectating] != INVALID_PLAYER_ID)
        {
            TogglePlayerSpectating(playerid, false);
            pInfo[playerid][player_spectating] = INVALID_PLAYER_ID;
            SendClientMessage(playerid, COLOR_GREEN, "Stopped spectating.");
        }
        else
        {
            SendClientMessage(playerid, COLOR_GRAY, "Usage: /spec [ID/Nick] or /spec (without parameters to disable) ");
        }
        return 1;
    }

    new targetid;

    if(sscanf(params, "u", targetid))
    {
        return SendClientMessage(playerid, COLOR_GRAY, "Usage: /spec [ID/Nick]");
    }

    if(!IsPlayerConnected(targetid))
    {
        return SendClientMessage(playerid, COLOR_RED, "Player is not connected!");
    }

    if(targetid == playerid)
    {
        return SendClientMessage(playerid, COLOR_RED, "You cannot spectate yourself!");
    }

    // Save position before spectate
    if(pInfo[playerid][player_spectating] == INVALID_PLAYER_ID)
    {
        GetPlayerPos(playerid, pInfo[playerid][player_pos_x],
                     pInfo[playerid][player_pos_y], pInfo[playerid][player_pos_z]);
        pInfo[playerid][player_interior] = GetPlayerInterior(playerid);
        pInfo[playerid][player_vw] = GetPlayerVirtualWorld(playerid);
    }

    TogglePlayerSpectating(playerid, true);

    if(IsPlayerInAnyVehicle(targetid))
    {
        PlayerSpectateVehicle(playerid, GetPlayerVehicleID(targetid));
    }
    else
    {
        PlayerSpectatePlayer(playerid, targetid);
    }

    SetPlayerInterior(playerid, GetPlayerInterior(targetid));
    SetPlayerVirtualWorld(playerid, GetPlayerVirtualWorld(targetid));

    pInfo[playerid][player_spectating] = targetid;

    new msg[128];
    format(msg, sizeof(msg), "You are now spectating player %s. Use /spec to stop.", pInfo[targetid][player_name]);
    SendClientMessage(playerid, COLOR_GREEN, msg);

    return 1;
}

YCMD:freeze(playerid, params[], help)
{
    ADMIN_CHECK(ADMIN_LEVEL_SUPPORT);

    if(help)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "Freezes/unfreezes a player.");
        return 1;
    }

    new targetid;

    if(sscanf(params, "u", targetid))
    {
        return SendClientMessage(playerid, COLOR_GRAY, "Usage: /freeze [ID/Nick]");
    }

    if(!IsPlayerConnected(targetid))
    {
        return SendClientMessage(playerid, COLOR_RED, "Player is not connected!");
    }

    if(pInfo[targetid][player_freezed])
    {
        TogglePlayerControllable(targetid, true);
        pInfo[targetid][player_freezed] = false;

        SendClientMessage(playerid, COLOR_GREEN, "Unfroze the player.");
        SendClientMessage(targetid, COLOR_YELLOW, "You have been unfrozen by the admin.");
    }
    else
    {
        TogglePlayerControllable(targetid, false);
        pInfo[targetid][player_freezed] = true;

        SendClientMessage(playerid, COLOR_GREEN, "Froze the player.");
        SendClientMessage(targetid, COLOR_YELLOW, "You have been frozen by the admin.");
    }

    return 1;
}

// ===========================================================================
// COMMANDS - SERVER
// ===========================================================================

YCMD:announce(playerid, params[], help)
{
    ADMIN_CHECK(ADMIN_LEVEL_MODERATOR);

    if(help)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "Sends an announcement to all players.");
        return 1;
    }

    if(isnull(params))
    {
        return SendClientMessage(playerid, COLOR_GRAY, "Usage: /announce [message]");
    }

    new msg[256];
    format(msg, sizeof(msg), "{FF6B6B}[ANNOUNCEMENT] %s", params);
    SendClientMessageToAll(COLOR_WHITE, msg);

    GameTextForAll(params, 5000, 4);

    return 1;
}

YCMD:giveweapon(playerid, params[], help)
{
    ADMIN_CHECK(ADMIN_LEVEL_ADMIN);

    if(help)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "Gives a weapon to a player.");
        return 1;
    }

    new targetid, weaponid, ammo;

    if(sscanf(params, "udd", targetid, weaponid, ammo))
    {
        return SendClientMessage(playerid, COLOR_GRAY, "Usage: /giveweapon [ID/Nick] [weapon ID] [ammo]");
    }

    if(!IsPlayerConnected(targetid))
    {
        return SendClientMessage(playerid, COLOR_RED, "Player is not connected!");
    }

    if(weaponid < 0 || weaponid > 46)
    {
        return SendClientMessage(playerid, COLOR_RED, "Invalid weapon ID!");
    }

    Player_GiveWeapon(targetid, weaponid, ammo);

    new weaponName[32];
    GetWeaponName(weaponid, weaponName, sizeof(weaponName));

    new msg[128];
    format(msg, sizeof(msg), "Gave player %s weapon %s with %d ammo.",
        pInfo[targetid][player_name], weaponName, ammo);
    SendClientMessage(playerid, COLOR_GREEN, msg);

    format(msg, sizeof(msg), "You have received a weapon %s with %d ammo from the admin.",
        weaponName, ammo);
    SendClientMessage(targetid, COLOR_YELLOW, msg);

    // Log
    new logMsg[128];
    format(logMsg, sizeof(logMsg), "Gave weapon: %s -> %s (%s x%d)",
        pInfo[playerid][player_name], pInfo[targetid][player_name], weaponName, ammo);
    Admin_LogAction(playerid, logMsg);

    return 1;
}

YCMD:givemoney(playerid, params[], help)
{
    ADMIN_CHECK(ADMIN_LEVEL_ADMIN);

    if(help)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "Gives money to a player.");
        return 1;
    }

    new targetid, amount;

    if(sscanf(params, "ud", targetid, amount))
    {
        return SendClientMessage(playerid, COLOR_GRAY, "Usage: /givemoney [ID/Nick] [amount]");
    }

    if(!IsPlayerConnected(targetid))
    {
        return SendClientMessage(playerid, COLOR_RED, "Player is not connected!");
    }

    Player_GiveMoney(targetid, amount);

    new msg[128];
    format(msg, sizeof(msg), "Gave player %s %s$.",
        pInfo[targetid][player_name], FormatMoney(amount));
    SendClientMessage(playerid, COLOR_GREEN, msg);

    format(msg, sizeof(msg), "You have received %s$ from the admin.",
        FormatMoney(amount));
    SendClientMessage(targetid, COLOR_YELLOW, msg);

    // Log
    new logMsg[128];
    format(logMsg, sizeof(logMsg), "Gave money: %s -> %s (%s$)",
        pInfo[playerid][player_name], pInfo[targetid][player_name], FormatMoney(amount));
    Admin_LogAction(playerid, logMsg);

    return 1;
}

YCMD:sethealth(playerid, params[], help)
{
    ADMIN_CHECK(ADMIN_LEVEL_MODERATOR);

    if(help)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "Sets the health of a player.");
        return 1;
    }

    new targetid, Float:health;

    if(sscanf(params, "uf", targetid, health))
    {
        return SendClientMessage(playerid, COLOR_GRAY, "Usage: /sethealth [ID/Nick] [health]");
    }

    if(!IsPlayerConnected(targetid))
    {
        return SendClientMessage(playerid, COLOR_RED, "Player is not connected!");
    }

    SetPlayerHealth(targetid, health);

    new msg[128];
    format(msg, sizeof(msg), "Set the health of player %s to %.0f.",
        pInfo[targetid][player_name], health);
    SendClientMessage(playerid, COLOR_GREEN, msg);

    return 1;
}

// Backwards-compatible alias: /sethp
YCMD:sethp(playerid, params[], help)
{
    ADMIN_CHECK(ADMIN_LEVEL_MODERATOR);

    if(help)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "Sets the health of a player.");
        return 1;
    }

    new targetid, Float:health;

    if(sscanf(params, "uf", targetid, health))
    {
        return SendClientMessage(playerid, COLOR_GRAY, "Usage: /sethp [ID/Nick] [health]");
    }

    if(!IsPlayerConnected(targetid))
    {
        return SendClientMessage(playerid, COLOR_RED, "Player is not connected!");
    }

    SetPlayerHealth(targetid, health);

    new msg[128];
    format(msg, sizeof(msg), "Set the health of player %s to %.0f.", pInfo[targetid][player_name], health);
    SendClientMessage(playerid, COLOR_GREEN, msg);

    return 1;
}

YCMD:setskin(playerid, params[], help)
{
    ADMIN_CHECK(ADMIN_LEVEL_MODERATOR);

    if(help)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "Sets the skin of a player.");
        return 1;
    }

    new targetid, skinid;

    if(sscanf(params, "ud", targetid, skinid))
    {
        return SendClientMessage(playerid, COLOR_GRAY, "Usage: /setskin [ID/Nick] [skin ID]");
    }

    if(!IsPlayerConnected(targetid))
    {
        return SendClientMessage(playerid, COLOR_RED, "Player is not connected!");
    }

    if(skinid < 0 || skinid > 311)
    {
        return SendClientMessage(playerid, COLOR_RED, "Invalid skin ID (0-311)!");
    }

    SetPlayerSkin(targetid, skinid);
    pInfo[targetid][player_skin] = skinid;

    new msg[128];
    format(msg, sizeof(msg), "Set the skin of player %s to %d.",
        pInfo[targetid][player_name], skinid);
    SendClientMessage(playerid, COLOR_GREEN, msg);

    return 1;
}

// ===========================================================================
// COMMANDS - VEHICLES
// ===========================================================================

YCMD:veh(playerid, params[], help)
{
    ADMIN_CHECK(ADMIN_LEVEL_MODERATOR);

    if(help)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "Creates a vehicle.");
        return 1;
    }

    new modelid, color1 = -1, color2 = -1;

    if(sscanf(params, "dD(-1)D(-1)", modelid, color1, color2))
    {
        return SendClientMessage(playerid, COLOR_GRAY, "Usage: /veh [model] [color1] [color2]");
    }

    if(modelid < 400 || modelid > 611)
    {
        return SendClientMessage(playerid, COLOR_RED, "Invalid vehicle model (400-611)!");
    }

    new Float:x, Float:y, Float:z, Float:a;
    GetPlayerPos(playerid, x, y, z);
    GetPlayerFacingAngle(playerid, a);

    new vehicleid = CreateVehicle(modelid, x + 3.0, y, z, a, color1, color2, -1);

    if(vehicleid == INVALID_VEHICLE_ID)
    {
        return SendClientMessage(playerid, COLOR_RED, "Cannot create vehicle!");
    }

    PutPlayerInVehicle(playerid, vehicleid, 0);

    new msg[128];
    format(msg, sizeof(msg), "You have created a vehicle with model %d (ID: %d).", modelid, vehicleid);
    SendClientMessage(playerid, COLOR_GREEN, msg);

    Admin_LogAction(playerid, "Created a vehicle");

    return 1;
}

YCMD:destroyveh(playerid, params[], help)
{
    ADMIN_CHECK(ADMIN_LEVEL_MODERATOR);

    if(help)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "Deletes a vehicle.");
        return 1;
    }

    new vehicleid = -1;

    if(isnull(params))
    {
        // Remove the vehicle they are in
        if(!IsPlayerInAnyVehicle(playerid))
        {
            return SendClientMessage(playerid, COLOR_GRAY, "Usage: /destroyveh [vehicle ID] or enter the vehicle");
        }
        vehicleid = GetPlayerVehicleID(playerid);
    }
    else
    {
        if(sscanf(params, "%d", vehicleid) != 1)
        {
            return SendClientMessage(playerid, COLOR_GRAY, "Usage: /destroyveh [vehicle ID]");
        }
    }

    DestroyVehicle(vehicleid);

    new msg[128];
    format(msg, sizeof(msg), "You have removed the vehicle ID: %d.", vehicleid);
    SendClientMessage(playerid, COLOR_GREEN, msg);

    return 1;
}

YCMD:fixveh(playerid, params[], help)
{
    ADMIN_CHECK(ADMIN_LEVEL_SUPPORT);

    if(help)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "Repairs a vehicle.");
        return 1;
    }

    new vehicleid;

    if(isnull(params))
    {
        if(!IsPlayerInAnyVehicle(playerid))
        {
            return SendClientMessage(playerid, COLOR_GRAY, "Usage: /fixveh [vehicle ID] or enter the vehicle");
        }
        vehicleid = GetPlayerVehicleID(playerid);
    }
    else
    {
        if(sscanf(params, "%d", vehicleid) != 1)
        {
            return SendClientMessage(playerid, COLOR_GRAY, "Usage: /fixveh [vehicle ID]");
        }
    }

    RepairVehicle(vehicleid);
    SendClientMessage(playerid, COLOR_GREEN, "You have repaired the vehicle.");

    return 1;
}


YCMD:flipveh(playerid, params[], help)
{
    ADMIN_CHECK(ADMIN_LEVEL_SUPPORT);

    if(help)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "Flips a vehicle.");
        return 1;
    }

    if(!IsPlayerInAnyVehicle(playerid))
    {
        return SendClientMessage(playerid, COLOR_RED, "You must be in a vehicle!");
    }

    new vehicleid = GetPlayerVehicleID(playerid);
    new Float:x, Float:y, Float:z, Float:a;

    GetVehiclePos(vehicleid, x, y, z);
    GetVehicleZAngle(vehicleid, a);
    SetVehiclePos(vehicleid, x, y, z + 0.5);
    SetVehicleZAngle(vehicleid, a);

    SendClientMessage(playerid, COLOR_GREEN, "You have flipped the vehicle.");

    return 1;
}

// ===========================================================================
// COMMANDS - ADVANCED TELEPORTATION
// ===========================================================================

YCMD:gotoxyz(playerid, params[], help)
{
    ADMIN_CHECK(ADMIN_LEVEL_MODERATOR);

    if(help)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "Teleports to the specified coordinates.");
        return 1;
    }

    new Float:x, Float:y, Float:z;

    if(sscanf(params, "fff", x, y, z))
    {
        return SendClientMessage(playerid, COLOR_GRAY, "Usage: /gotoxyz [X] [Y] [Z]");
    }

    SetPlayerPos(playerid, x, y, z);

    new msg[128];
    format(msg, sizeof(msg), "You have teleported to: %.2f, %.2f, %.2f", x, y, z);
    SendClientMessage(playerid, COLOR_GREEN, msg);

    return 1;
}

YCMD:setint(playerid, params[], help)
{
    ADMIN_CHECK(ADMIN_LEVEL_MODERATOR);

    if(help)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "Sets the interior.");
        return 1;
    }

    new interior, targetid = playerid;

    if(sscanf(params, "dU(-1)", interior, targetid))
    {
        return SendClientMessage(playerid, COLOR_GRAY, "Usage: /setint [interior] [Player ID - optional]");
    }

    if(targetid == -1) targetid = playerid;

    if(!IsPlayerConnected(targetid))
    {
        return SendClientMessage(playerid, COLOR_RED, "Player is not connected!");
    }

    SetPlayerInterior(targetid, interior);

    new msg[128];
    if(targetid == playerid)
    {
        format(msg, sizeof(msg), "You have set your interior to %d.", interior);
    }
    else
    {
        format(msg, sizeof(msg), "You have set the interior of player %s to %d.", pInfo[targetid][player_name], interior);
    }
    SendClientMessage(playerid, COLOR_GREEN, msg);

    return 1;
}

YCMD:setvw(playerid, params[], help)
{
    ADMIN_CHECK(ADMIN_LEVEL_MODERATOR);

    if(help)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "Sets the virtual world.");
        return 1;
    }

    new vw, targetid = playerid;

    if(sscanf(params, "dU(-1)", vw, targetid))
    {
        return SendClientMessage(playerid, COLOR_GRAY, "Usage: /setvw [VW] [Player ID - optional]");
    }

    if(targetid == -1) targetid = playerid;

    if(!IsPlayerConnected(targetid))
    {
        return SendClientMessage(playerid, COLOR_RED, "Player is not connected!");
    }

    SetPlayerVirtualWorld(targetid, vw);

    new msg[128];
    if(targetid == playerid)
    {
        format(msg, sizeof(msg), "You have set your VW to %d.", vw);
    }
    else
    {
        format(msg, sizeof(msg), "You have set the VW of player %s to %d.", pInfo[targetid][player_name], vw);
    }
    SendClientMessage(playerid, COLOR_GREEN, msg);

    return 1;
}

YCMD:tpall(playerid, params[], help)
{
    ADMIN_CHECK(ADMIN_LEVEL_HEADADMIN);

    if(help)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "Teleports all players to you.");
        return 1;
    }

    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);

    new count = 0;
    foreach(new i : Player)
    {
        if(i != playerid)
        {
            SetPlayerInterior(i, GetPlayerInterior(playerid));
            SetPlayerVirtualWorld(i, GetPlayerVirtualWorld(playerid));
            SetPlayerPos(i, x + random(5), y + random(5), z);
            count++;
        }
    }

    new msg[128];
    format(msg, sizeof(msg), "You have teleported %d players to you.", count);
    SendClientMessage(playerid, COLOR_GREEN, msg);

    SendClientMessageToAll(COLOR_YELLOW, "[ADMIN] All players have been teleported!");

    Admin_LogAction(playerid, "Teleported all players");

    return 1;
}

// ===========================================================================
// COMMANDS - PLAYER MANAGEMENT
// ===========================================================================

YCMD:slap(playerid, params[], help)
{
    ADMIN_CHECK(ADMIN_LEVEL_SUPPORT);

    if(help)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "Slaps a player up.");
        return 1;
    }

    new targetid;

    if(sscanf(params, "u", targetid))
    {
        return SendClientMessage(playerid, COLOR_GRAY, "Usage: /slap [ID/Nick]");
    }

    if(!IsPlayerConnected(targetid))
    {
        return SendClientMessage(playerid, COLOR_RED, "Player is not connected!");
    }

    new Float:x, Float:y, Float:z;
    GetPlayerPos(targetid, x, y, z);
    SetPlayerPos(targetid, x, y, z + 5.0);

    new Float:health;
    GetPlayerHealth(targetid, health);
    SetPlayerHealth(targetid, health - 10.0);

    new msg[128];
    format(msg, sizeof(msg), "You slapped player %s.", pInfo[targetid][player_name]);
    SendClientMessage(playerid, COLOR_GREEN, msg);

    SendClientMessage(targetid, COLOR_RED, "You have been slapped by an admin!");

    return 1;
}

YCMD:mute(playerid, params[], help)
{
    ADMIN_CHECK(ADMIN_LEVEL_SUPPORT);

    if(help)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "Mutes a player.");
        return 1;
    }

    new targetid, duration, reason[64];

    if(sscanf(params, "udS(Brak powodu)[64]", targetid, duration, reason))
    {
        return SendClientMessage(playerid, COLOR_GRAY, "Usage: /mute [ID/Nick] [duration in minutes] [reason]");
    }

    if(!IsPlayerConnected(targetid))
    {
        return SendClientMessage(playerid, COLOR_RED, "Player is not connected!");
    }

    pInfo[targetid][player_muted] = true;
    pInfo[targetid][player_mute_time] = gettime() + (duration * 60);

    new msg[256];
    format(msg, sizeof(msg), "{FF6B6B}[ADMIN] %s has been muted by %s for %d minutes. Reason: %s",
        pInfo[targetid][player_name], pInfo[playerid][player_name], duration, reason);
    SendClientMessageToAll(COLOR_WHITE, msg);

    new logMsg[128];
    format(logMsg, sizeof(logMsg), "Mute: %s - %d min - %s", pInfo[targetid][player_name], duration, reason);
    Admin_LogAction(playerid, logMsg);

    return 1;
}

YCMD:unmute(playerid, params[], help)
{
    ADMIN_CHECK(ADMIN_LEVEL_SUPPORT);

    if(help)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "Unmutes a player.");
        return 1;
    }

    new targetid;

    if(sscanf(params, "u", targetid))
    {
        return SendClientMessage(playerid, COLOR_GRAY, "Usage: /unmute [ID/Nick]");
    }

    if(!IsPlayerConnected(targetid))
    {
        return SendClientMessage(playerid, COLOR_RED, "Player is not connected!");
    }

    pInfo[targetid][player_muted] = false;
    pInfo[targetid][player_mute_time] = 0;

    new msg[128];
    format(msg, sizeof(msg), "You have unmuted player %s.", pInfo[targetid][player_name]);
    SendClientMessage(playerid, COLOR_GREEN, msg);

    SendClientMessage(targetid, COLOR_YELLOW, "You have been unmuted by the admin.");

    return 1;
}

YCMD:jail(playerid, params[], help)
{
    ADMIN_CHECK(ADMIN_LEVEL_SUPPORT);

    if(help)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "Puts a player in admin jail.");
        return 1;
    }

    new targetid, duration, reason[64];

    if(sscanf(params, "udS(Brak powodu)[64]", targetid, duration, reason))
    {
        return SendClientMessage(playerid, COLOR_GRAY, "Usage: /jail [ID/Nick] [duration in minutes] [reason]");
    }

    if(!IsPlayerConnected(targetid))
    {
        return SendClientMessage(playerid, COLOR_RED, "Player is not connected!");
    }

    // Admin jail position
    SetPlayerPos(targetid, 197.6661, 173.8179, 1003.0234);
    SetPlayerInterior(targetid, 3);
    SetPlayerVirtualWorld(targetid, targetid + 1000);

    pInfo[targetid][player_jailed] = true;
    pInfo[targetid][player_jail_time] = gettime() + (duration * 60);

    TogglePlayerControllable(targetid, false);

    new msg[256];
    format(msg, sizeof(msg), "{FF6B6B}[ADMIN] %s has been jailed by %s for %d minutes. Reason: %s",
        pInfo[targetid][player_name], pInfo[playerid][player_name], duration, reason);
    SendClientMessageToAll(COLOR_WHITE, msg);

    new logMsg[128];
    format(logMsg, sizeof(logMsg), "Jail: %s - %d min - %s", pInfo[targetid][player_name], duration, reason);
    Admin_LogAction(playerid, logMsg);

    return 1;
}

YCMD:unjail(playerid, params[], help)
{
    ADMIN_CHECK(ADMIN_LEVEL_SUPPORT);

    if(help)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "Releases a player from admin jail.");
        return 1;
    }

    new targetid;

    if(sscanf(params, "u", targetid))
    {
        return SendClientMessage(playerid, COLOR_GRAY, "Usage: /unjail [ID/Nick]");
    }

    if(!IsPlayerConnected(targetid))
    {
        return SendClientMessage(playerid, COLOR_RED, "Player is not connected!");
    }

    if(!pInfo[targetid][player_jailed])
    {
        return SendClientMessage(playerid, COLOR_RED, "Player is not in jail!");
    }

    // Teleport to spawn
    SetPlayerPos(targetid, Setting[setting_spawn_pos_x], Setting[setting_spawn_pos_y], Setting[setting_spawn_pos_z]);
    SetPlayerInterior(targetid, 0);
    SetPlayerVirtualWorld(targetid, 0);

    pInfo[targetid][player_jailed] = false;
    pInfo[targetid][player_jail_time] = 0;

    TogglePlayerControllable(targetid, true);

    new msg[128];
    format(msg, sizeof(msg), "You have released player %s from jail.", pInfo[targetid][player_name]);
    SendClientMessage(playerid, COLOR_GREEN, msg);

    SendClientMessage(targetid, COLOR_YELLOW, "You have been released from jail by the admin.");

    return 1;
}

YCMD:warn(playerid, params[], help)
{
    ADMIN_CHECK(ADMIN_LEVEL_SUPPORT);

    if(help)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "Gives a warning to a player.");
        return 1;
    }

    new targetid, reason[128];

    if(sscanf(params, "us[128]", targetid, reason))
    {
        return SendClientMessage(playerid, COLOR_GRAY, "Usage: /warn [ID/Nick] [reason]");
    }

    if(!IsPlayerConnected(targetid))
    {
        return SendClientMessage(playerid, COLOR_RED, "Player is not connected!");
    }

    pInfo[targetid][player_warnings]++;

    new msg[256];
    format(msg, sizeof(msg), "{FF6B6B}[ADMIN] %s received a warning from %s. Reason: %s (%d/3)",
        pInfo[targetid][player_name], pInfo[playerid][player_name], reason, pInfo[targetid][player_warnings]);
    SendClientMessageToAll(COLOR_WHITE, msg);

    // Automatic ban after 3 warnings
    if(pInfo[targetid][player_warnings] >= 3)
    {
        format(msg, sizeof(msg), "{FF0000}[ADMIN] %s has been automatically banned for receiving 3 warnings!",
            pInfo[targetid][player_name]);
        SendClientMessageToAll(COLOR_WHITE, msg);

        Player_Ban(targetid, -1, "Automatic ban - 3 warnings", 1440 * 60); // 24h
    }

    new logMsg[128];
    format(logMsg, sizeof(logMsg), "Warn: %s - %s", pInfo[targetid][player_name], reason);
    Admin_LogAction(playerid, logMsg);

    return 1;
}

YCMD:clearwarns(playerid, params[], help)
{
    ADMIN_CHECK(ADMIN_LEVEL_MODERATOR);

    if(help)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "Clears warnings of a player.");
        return 1;
    }

    new targetid;

    if(sscanf(params, "u", targetid))
    {
        return SendClientMessage(playerid, COLOR_GRAY, "Usage: /clearwarns [ID/Nick]");
    }

    if(!IsPlayerConnected(targetid))
    {
        return SendClientMessage(playerid, COLOR_RED, "Player is not connected!");
    }

    pInfo[targetid][player_warnings] = 0;

    new msg[128];
    format(msg, sizeof(msg), "You have cleared the warnings of player %s.", pInfo[targetid][player_name]);
    SendClientMessage(playerid, COLOR_GREEN, msg);

    SendClientMessage(targetid, COLOR_YELLOW, "Your warnings have been cleared by the admin.");

    return 1;
}

// ===========================================================================
// COMMANDS - INFORMATION
// ===========================================================================

YCMD:check(playerid, params[], help)
{
    ADMIN_CHECK(ADMIN_LEVEL_HELPER);

    if(help)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "Checks information about a player.");
        return 1;
    }

    new targetid;

    if(sscanf(params, "u", targetid))
    {
        return SendClientMessage(playerid, COLOR_GRAY, "Usage: /check [ID/Nick]");
    }

    if(!IsPlayerConnected(targetid))
    {
        return SendClientMessage(playerid, COLOR_RED, "Player is not connected!");
    }

    new Float:hp, Float:armour;
    GetPlayerHealth(targetid, hp);
    GetPlayerArmour(targetid, armour);

    new Float:x, Float:y, Float:z;
    GetPlayerPos(targetid, x, y, z);

    SendClientMessage(playerid, COLOR_LIGHTER_BLUE, "========== PLAYER INFORMATION ==========");

    new msg[256];
    format(msg, sizeof(msg), "Nick: %s | ID: %d | UID: %d",
        pInfo[targetid][player_name], targetid, pInfo[targetid][player_id]);
    SendClientMessage(playerid, COLOR_WHITE, msg);

    format(msg, sizeof(msg), "HP: %.0f | Armour: %.0f | Skin: %d | Money: %s$",
        hp, armour, pInfo[targetid][player_skin], FormatMoney(pInfo[targetid][player_money]));
    SendClientMessage(playerid, COLOR_WHITE, msg);

    format(msg, sizeof(msg), "Position: %.2f, %.2f, %.2f | Interior: %d | VW: %d",
        x, y, z, GetPlayerInterior(targetid), GetPlayerVirtualWorld(targetid));
    SendClientMessage(playerid, COLOR_WHITE, msg);

    new adminRank[32];
    Admin_GetRankName(pGlobal[targetid][glo_admin], adminRank);

    format(msg, sizeof(msg), "Admin: %s (%d) | Warnings: %d/3 | Muted: %s | Jailed: %s",
        adminRank, pGlobal[targetid][glo_admin], pInfo[targetid][player_warnings],
        pInfo[targetid][player_muted] ? "Yes" : "No",
        pInfo[targetid][player_jailed] ? "Yes" : "No");
    SendClientMessage(playerid, COLOR_WHITE, msg);

    format(msg, sizeof(msg), "Ping: %d | FPS: %d", GetPlayerPing(targetid), GetPlayerFPS(targetid));
    SendClientMessage(playerid, COLOR_WHITE, msg);

    SendClientMessage(playerid, COLOR_LIGHTER_BLUE, "==========================================");

    return 1;
}

YCMD:admins(playerid, params[], help)
{
    ADMIN_CHECK(ADMIN_LEVEL_HELPER);

    if(help)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "Displays the list of online admins.");
        return 1;
    }

    SendClientMessage(playerid, COLOR_LIGHTER_BLUE, "========== ONLINE ADMINISTRATORS ==========");

    new count = 0;
    foreach(new i : Player)
    {
        if(pGlobal[i][glo_admin] > 0)
        {
            new rankName[32];
            Admin_GetRankName(pGlobal[i][glo_admin], rankName);

            new msg[128];
            format(msg, sizeof(msg), "%s (%d) - %s %s",
                pInfo[i][player_name], i, rankName,
                pInfo[i][player_admin_duty] ? "{00FF00}[ON DUTY]" : "{FF0000}[OFF DUTY]");
            SendClientMessage(playerid, COLOR_WHITE, msg);
            count++;
        }
    }

    if(count == 0)
    {
        SendClientMessage(playerid, COLOR_GRAY, "No administrators online.");
    }

    SendClientMessage(playerid, COLOR_LIGHTER_BLUE, "=============================================");

    return 1;
}

YCMD:players(playerid, params[], help)
{
    ADMIN_CHECK(ADMIN_LEVEL_HELPER);

    if(help)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "Displays the list of online players.");
        return 1;
    }

    SendClientMessage(playerid, COLOR_LIGHTER_BLUE, "========== ONLINE PLAYERS ==========");

    new count = 0;
    new msg[256];
    new players[512] = "";

    foreach(new i : Player)
    {
        if(strlen(players) > 400)
        {
            SendClientMessage(playerid, COLOR_WHITE, players);
            players[0] = '\0';
        }

        format(msg, sizeof(msg), "%s(%d), ", pInfo[i][player_name], i);
        strcat(players, msg);
        count++;
    }

    if(strlen(players) > 0)
    {
        SendClientMessage(playerid, COLOR_WHITE, players);
    }

    format(msg, sizeof(msg), "Total players online: %d", count);
    SendClientMessage(playerid, COLOR_GREEN, msg);

    SendClientMessage(playerid, COLOR_LIGHTER_BLUE, "====================================");

    return 1;
}

YCMD:getpos(playerid, params[], help)
{
    ADMIN_CHECK(ADMIN_LEVEL_HELPER);

    if(help)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "Shows the current position.");
        return 1;
    }

    new Float:x, Float:y, Float:z, Float:a;
    GetPlayerPos(playerid, x, y, z);
    GetPlayerFacingAngle(playerid, a);

    new msg[256];
    format(msg, sizeof(msg), "Position: X=%.4f, Y=%.4f, Z=%.4f, A=%.4f | Interior: %d | VW: %d",
        x, y, z, a, GetPlayerInterior(playerid), GetPlayerVirtualWorld(playerid));
    SendClientMessage(playerid, COLOR_GREEN, msg);

    // Copy to clipboard (in console)
    printf("Player %s position: %.4f, %.4f, %.4f, %.4f", pInfo[playerid][player_name], x, y, z, a);

    return 1;
}

// ===========================================================================
// COMMANDS - SERVER ADVANCED
// ===========================================================================

YCMD:setarmour(playerid, params[], help)
{
    ADMIN_CHECK(ADMIN_LEVEL_MODERATOR);

    if(help)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "Sets the armor of a player.");
        return 1;
    }

    new targetid, Float:armour;

    if(sscanf(params, "uf", targetid, armour))
    {
        return SendClientMessage(playerid, COLOR_GRAY, "Usage: /setarmour [ID/Nick] [armor]");
    }

    if(!IsPlayerConnected(targetid))
    {
        return SendClientMessage(playerid, COLOR_RED, "Player is not connected!");
    }

    SetPlayerArmour(targetid, armour);

    new msg[128];
    format(msg, sizeof(msg), "Set the armor of player %s to %.0f.",
        pInfo[targetid][player_name], armour);
    SendClientMessage(playerid, COLOR_GREEN, msg);

    return 1;
}

YCMD:setmoney(playerid, params[], help)
{
    ADMIN_CHECK(ADMIN_LEVEL_ADMIN);

    if(help)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "Sets the money of a player.");
        return 1;
    }

    new targetid, amount;

    if(sscanf(params, "ud", targetid, amount))
    {
        return SendClientMessage(playerid, COLOR_GRAY, "Usage: /setmoney [ID/Nick] [amount]");
    }

    if(!IsPlayerConnected(targetid))
    {
        return SendClientMessage(playerid, COLOR_RED, "Player is not connected!");
    }

    pInfo[targetid][player_money] = amount;
    ResetPlayerMoney(targetid);
    GivePlayerMoney(targetid, amount);

    new msg[128];
    format(msg, sizeof(msg), "Set the money of player %s to %s$.",
        pInfo[targetid][player_name], FormatMoney(amount));
    SendClientMessage(playerid, COLOR_GREEN, msg);

    new logMsg[128];
    format(logMsg, sizeof(logMsg), "Set money: %s = %s$", pInfo[targetid][player_name], FormatMoney(amount));
    Admin_LogAction(playerid, logMsg);

    return 1;
}

YCMD:clearchat(playerid, params[], help)
{
    ADMIN_CHECK(ADMIN_LEVEL_MODERATOR);

    if(help)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "Clears the chat.");
        return 1;
    }

    for(new i = 0; i < 50; i++)
    {
        SendClientMessageToAll(COLOR_WHITE, " ");
    }

    new msg[128];
    format(msg, sizeof(msg), "{FF6B6B}[ADMIN] %s cleared the chat.", pInfo[playerid][player_name]);
    SendClientMessageToAll(COLOR_WHITE, msg);

    return 1;
}

YCMD:disarm(playerid, params[], help)
{
    ADMIN_CHECK(ADMIN_LEVEL_MODERATOR);

    if(help)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "Removes all weapons from a player.");
        return 1;
    }

    new targetid;

    if(sscanf(params, "u", targetid))
    {
        return SendClientMessage(playerid, COLOR_GRAY, "Usage: /disarm [ID/Nick]");
    }

    if(!IsPlayerConnected(targetid))
    {
        return SendClientMessage(playerid, COLOR_RED, "Player is not connected!");
    }

    ResetPlayerWeapons(targetid);

    new msg[128];
    format(msg, sizeof(msg), "You have removed all weapons from player %s.", pInfo[targetid][player_name]);
    SendClientMessage(playerid, COLOR_GREEN, msg);

    SendClientMessage(targetid, COLOR_YELLOW, "The administrator has removed all your weapons.");

    return 1;
}

YCMD:revive(playerid, params[], help)
{
    ADMIN_CHECK(ADMIN_LEVEL_MODERATOR);

    if(help)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "Revives a player.");
        return 1;
    }

    new targetid;

    if(sscanf(params, "u", targetid))
    {
        return SendClientMessage(playerid, COLOR_GRAY, "Usage: /revive [ID/Nick]");
    }

    if(!IsPlayerConnected(targetid))
    {
        return SendClientMessage(playerid, COLOR_RED, "Player is not connected!");
    }

    SetPlayerHealth(targetid, 100.0);

    new msg[128];
    format(msg, sizeof(msg), "You have revived player %s.", pInfo[targetid][player_name]);
    SendClientMessage(playerid, COLOR_GREEN, msg);

    SendClientMessage(targetid, COLOR_GREEN, "You have been revived by the admin!");

    return 1;
}

YCMD:akill(playerid, params[], help)
{
    ADMIN_CHECK(ADMIN_LEVEL_MODERATOR);

    if(help)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "Kills a player.");
        return 1;
    }

    new targetid;

    if(sscanf(params, "u", targetid))
    {
        return SendClientMessage(playerid, COLOR_GRAY, "Usage: /akill [ID/Nick]");
    }

    if(!IsPlayerConnected(targetid))
    {
        return SendClientMessage(playerid, COLOR_RED, "Player is not connected!");
    }

    SetPlayerHealth(targetid, 0.0);

    new msg[128];
    format(msg, sizeof(msg), "You have killed player %s.", pInfo[targetid][player_name]);
    SendClientMessage(playerid, COLOR_GREEN, msg);

    Admin_LogAction(playerid, "Killed a player");

    return 1;
}

YCMD:explode(playerid, params[], help)
{
    ADMIN_CHECK(ADMIN_LEVEL_ADMIN);

    if(help)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "Blows up a player.");
        return 1;
    }

    new targetid;

    if(sscanf(params, "u", targetid))
    {
        return SendClientMessage(playerid, COLOR_GRAY, "Usage: /explode [ID/Nick]");
    }

    if(!IsPlayerConnected(targetid))
    {
        return SendClientMessage(playerid, COLOR_RED, "Player is not connected!");
    }

    new Float:x, Float:y, Float:z;
    GetPlayerPos(targetid, x, y, z);
    CreateExplosion(x, y, z, 7, 10.0);

    new msg[128];
    format(msg, sizeof(msg), "You have blown up player %s.", pInfo[targetid][player_name]);
    SendClientMessage(playerid, COLOR_GREEN, msg);

    return 1;
}

// ===========================================================================
// COMMANDS - ADMIN LEVEL MANAGEMENT
// ===========================================================================

YCMD:setadmin(playerid, params[], help)
{
    ADMIN_CHECK(ADMIN_LEVEL_MANAGEMENT);

    if(help)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "Sets the admin level of a player.");
        return 1;
    }

    new targetid, level;

    if(sscanf(params, "ud", targetid, level))
    {
        SendClientMessage(playerid, COLOR_GRAY, "Usage: /setadmin [ID/Nick] [level 0-6]");
        SendClientMessage(playerid, COLOR_GRAY, "Levels: 0-Player, 1-Helper, 2-Support, 3-Mod, 4-Admin, 5-HeadAdmin, 6-Management");
        return 1;
    }

    if(!IsPlayerConnected(targetid))
    {
        return SendClientMessage(playerid, COLOR_RED, "Player is not connected!");
    }

    if(level < 0 || level > ADMIN_LEVEL_MANAGEMENT)
    {
        return SendClientMessage(playerid, COLOR_RED, "Invalid admin level!");
    }

    pGlobal[targetid][glo_admin] = level;

    // Save to database
    new query[256];
    mysql_format(mySQLconnection, query, sizeof(query),
        "UPDATE `global_accounts` SET `admin`=%d WHERE `uid`=%d",
        level, pInfo[targetid][player_uid]);
    mysql_tquery(mySQLconnection, query);

    new rankName[32];
    Admin_GetRankName(level, rankName);

    new msg[128];
    format(msg, sizeof(msg), "You have set the admin level of player %s to %s (%d).",
        pInfo[targetid][player_name], rankName, level);
    SendClientMessage(playerid, COLOR_GREEN, msg);

    format(msg, sizeof(msg), "Your admin level has been changed to %s (%d).", rankName, level);
    SendClientMessage(targetid, COLOR_YELLOW, msg);

    new logMsg[128];
    format(logMsg, sizeof(logMsg), "SetAdmin: %s = %s (%d)", pInfo[targetid][player_name], rankName, level);
    Admin_LogAction(playerid, logMsg);

    return 1;
}

YCMD:unban(playerid, params[], help)
{
    ADMIN_CHECK(ADMIN_LEVEL_ADMIN);

    if(help)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "Unbans a player (by name).");
        return 1;
    }

    new targetName[MAX_PLAYER_NAME + 1];

    if(sscanf(params, "s[" # MAX_PLAYER_NAME "]", targetName))
    {
        return SendClientMessage(playerid, COLOR_GRAY, "Usage: /unban [player name]");
    }

    new query[256];
    new escapedName[MAX_PLAYER_NAME * 2];
    mysql_escape_string(targetName, escapedName, mySQLconnection, sizeof(escapedName));

    mysql_format(mySQLconnection, query, sizeof(query),
        "UPDATE `bans` SET `active`=0 WHERE `name`='%s' AND `active`=1", escapedName);
    mysql_tquery(mySQLconnection, query);

    new msg[128];
    format(msg, sizeof(msg), "You have unbanned player %s.", targetName);
    SendClientMessage(playerid, COLOR_GREEN, msg);

    new logMsg[128];
    format(logMsg, sizeof(logMsg), "Unban: %s", targetName);
    Admin_LogAction(playerid, logMsg);

    return 1;
}

// ===========================================================================
// COMMANDS - WEATHER/TIME
// ===========================================================================

YCMD:settime(playerid, params[], help)
{
    ADMIN_CHECK(ADMIN_LEVEL_ADMIN);

    if(help)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "Sets the time on the server.");
        return 1;
    }

    new hour;

    if(sscanf(params, "%d", hour) != 1)
    {
        return SendClientMessage(playerid, COLOR_GRAY, "Usage: /settime [hour 0-23]");
    }

    if(hour < 0 || hour > 23)
    {
        return SendClientMessage(playerid, COLOR_RED, "Invalid hour!");
    }

    SetWorldTime(hour);

    new msg[128];
    format(msg, sizeof(msg), "{FF6B6B}[ADMIN] %s set the time to %d:00.", pInfo[playerid][player_name], hour);
    SendClientMessageToAll(COLOR_WHITE, msg);

    return 1;
}

YCMD:setweather(playerid, params[], help)
{
    ADMIN_CHECK(ADMIN_LEVEL_ADMIN);

    if(help)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "Sets the weather on the server.");
        return 1;
    }

    new weather;

    if(sscanf(params, "%d", weather) != 1)
    {
        return SendClientMessage(playerid, COLOR_GRAY, "Usage: /setweather [weather ID 0-45]");
    }

    if(weather < 0 || weather > 45)
    {
        return SendClientMessage(playerid, COLOR_RED, "Invalid weather ID!");
    }

    SetWeather(weather);

    new msg[128];
    format(msg, sizeof(msg), "{FF6B6B}[ADMIN] %s set the weather to ID %d.", pInfo[playerid][player_name], weather);
    SendClientMessageToAll(COLOR_WHITE, msg);

    return 1;
}

// ===========================================================================
// COMMANDS - INVISIBLE/GOD MODE
// ===========================================================================

YCMD:invisible(playerid, params[], help)
{
    ADMIN_CHECK(ADMIN_LEVEL_ADMIN);

    if(help)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "Enables/disables invisibility.");
        return 1;
    }

    if(pInfo[playerid][player_invisible])
    {
        pInfo[playerid][player_invisible] = false;

        foreach(new i : Player)
        {
            ShowPlayerNameTagForPlayer(i, playerid, true);
        }
        SetPlayerColor(playerid, COLOR_WHITE);

        SendClientMessage(playerid, COLOR_GREEN, "Invisibility disabled.");
    }
    else
    {
        pInfo[playerid][player_invisible] = true;

        foreach(new i : Player)
        {
            ShowPlayerNameTagForPlayer(i, playerid, false);
        }
        SetPlayerColor(playerid, 0x00000000);

        SendClientMessage(playerid, COLOR_GREEN, "Invisibility enabled.");
    }

    return 1;
}

YCMD:god(playerid, params[], help)
{
    ADMIN_CHECK(ADMIN_LEVEL_ADMIN);

    if(help)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "Enables/disables god mode.");
        return 1;
    }

    if(pInfo[playerid][player_godmode])
    {
        pInfo[playerid][player_godmode] = false;
        SetPlayerHealth(playerid, 100.0);
        SendClientMessage(playerid, COLOR_GREEN, "God mode disabled.");
    }
    else
    {
        pInfo[playerid][player_godmode] = true;
        SetPlayerHealth(playerid, 99999.0);
        SendClientMessage(playerid, COLOR_GREEN, "God mode enabled.");
    }

    return 1;
}

// ===========================================================================
// COMMANDS - NOCLIP/FLY
// ===========================================================================

YCMD:fly(playerid, params[], help)
{
    ADMIN_CHECK(ADMIN_LEVEL_ADMIN);

    if(help)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "Enables/disables fly mode (noclip).");
        return 1;
    }

    if(pInfo[playerid][player_noclip])
    {
        pInfo[playerid][player_noclip] = false;

        new Float:x, Float:y, Float:z;
        GetPlayerPos(playerid, x, y, z);
        SetPlayerPos(playerid, x, y, z);

        SendClientMessage(playerid, COLOR_GREEN, "Fly mode disabled.");
    }
    else
    {
        pInfo[playerid][player_noclip] = true;
        SendClientMessage(playerid, COLOR_GREEN, "Fly mode enabled. Use arrows and space/CTRL.");
    }

    return 1;
}

// ===========================================================================
// COMMANDS - ADMINISTRATIVE HELP
// ===========================================================================

YCMD:acmds(playerid, params[], help)
{
    ADMIN_CHECK(ADMIN_LEVEL_HELPER);

    if(help)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "Displays a list of administrative commands.");
        return 1;
    }

    new level = Admin_GetLevel(playerid);

    SendClientMessage(playerid, COLOR_LIGHTER_BLUE, "========== ADMINISTRATIVE COMMANDS ==========");

    // Helper (1)
    SendClientMessage(playerid, COLOR_GREEN, "[Helper] /a, /check, /admins, /players, /getpos, /acmds");

    if(level >= ADMIN_LEVEL_SUPPORT)
    {
        SendClientMessage(playerid, COLOR_GREEN, "[Support] /kick, /goto, /gethere, /spec, /freeze, /slap");
        SendClientMessage(playerid, COLOR_GREEN, "[Support] /mute, /unmute, /jail, /unjail, /warn, /fixveh, /flipveh, /aduty");
    }

    if(level >= ADMIN_LEVEL_MODERATOR)
    {
        SendClientMessage(playerid, COLOR_GREEN, "[Moderator] /ban, /announce, /sethealth, /setskin, /setarmour");
        SendClientMessage(playerid, COLOR_GREEN, "[Moderator] /clearwarns, /clearchat, /disarm, /revive, /akill");
        SendClientMessage(playerid, COLOR_GREEN, "[Moderator] /veh, /destroyveh, /gotoxyz, /setint, /setvw");
    }

    if(level >= ADMIN_LEVEL_ADMIN)
    {
        SendClientMessage(playerid, COLOR_GREEN, "[Admin] /giveweapon, /givemoney, /setmoney, /explode, /unban");
        SendClientMessage(playerid, COLOR_GREEN, "[Admin] /settime, /setweather, /invisible, /god, /fly");
        SendClientMessage(playerid, COLOR_GREEN, "[Admin] /ag - Group management");
    }

    if(level >= ADMIN_LEVEL_HEADADMIN)
    {
        SendClientMessage(playerid, COLOR_GREEN, "[HeadAdmin] /tpall");
    }

    if(level >= ADMIN_LEVEL_MANAGEMENT)
    {
        SendClientMessage(playerid, COLOR_GREEN, "[Management] /setadmin");
    }

    SendClientMessage(playerid, COLOR_LIGHTER_BLUE, "==============================================");

    return 1;
}

// ===========================================================================
// HELPER FUNCTIONS
// ===========================================================================

/**
 * Gets the FPS of a player (approximate)
 */
stock GetPlayerFPS(playerid)
{
    return pInfo[playerid][player_fps];
}

// ===========================================================================
// COMMANDS FOR GROUP MANAGEMENT - /ag
// ===========================================================================

/**
 * Gets the name of the group type
 */
stock Admin_GetGroupTypeName(type, output[], size = 32)
{
    switch(type)
    {
        case 0: strcopy(output, "None", size);
        case 1: strcopy(output, "Police", size);
        case 2: strcopy(output, "Medics", size);
        case 3: strcopy(output, "Government", size);
        case 4: strcopy(output, "Gang", size);
        case 5: strcopy(output, "Mafia", size);
        case 6: strcopy(output, "Media", size);
        case 7: strcopy(output, "Transport", size);
        case 8: strcopy(output, "Mechanics", size);
        case 9: strcopy(output, "Business", size);
        default: strcopy(output, "Other", size);
    }
}

// Forward declaration
forward Admin_GroupsCmd(playerid, params[], help);

YCMD:agroups(playerid, params[], help)
{
    return Admin_GroupsCmd(playerid, params, help);
}

YCMD:ag(playerid, params[], help)
{
    return Admin_GroupsCmd(playerid, params, help);
}

public Admin_GroupsCmd(playerid, params[], help)
{
    ADMIN_CHECK(ADMIN_LEVEL_ADMIN);

    if(help)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "Group management on the server.");
        return 1;
    }

    new subcmd[32], subparams[128];

    if(sscanf(params, "s[32]S()[128]", subcmd, subparams))
    {
        SendClientMessage(playerid, COLOR_LIGHTER_BLUE, "========== GROUP MANAGEMENT /ag ===========");
        SendClientMessage(playerid, COLOR_WHITE, "/ag create [name] - Creates a new group");
        SendClientMessage(playerid, COLOR_WHITE, "/ag delete [ID] - Deletes a group");
        SendClientMessage(playerid, COLOR_WHITE, "/ag info [ID] - Information about the group");
        SendClientMessage(playerid, COLOR_WHITE, "/ag list - List of all groups");
        SendClientMessage(playerid, COLOR_WHITE, "/ag name [ID] [new name] - Changes the group name");
        SendClientMessage(playerid, COLOR_WHITE, "/ag tag [ID] [new tag] - Changes the group tag");
        SendClientMessage(playerid, COLOR_WHITE, "/ag type [ID] - Group type (Police, Gang, Taxi...) [GUI]");
        SendClientMessage(playerid, COLOR_WHITE, "/ag flags [ID] - Group permissions (handcuff, MDT...) [GUI]");
        SendClientMessage(playerid, COLOR_WHITE, "/ag color [ID] [hex color] - Changes the group color");
        SendClientMessage(playerid, COLOR_WHITE, "/ag leader [ID] [player] - Sets the group leader");
        SendClientMessage(playerid, COLOR_WHITE, "/ag bank [ID] [amount] - Sets the group account balance");
        SendClientMessage(playerid, COLOR_WHITE, "/ag add [group ID] [player] - Adds a player to the group");
        SendClientMessage(playerid, COLOR_WHITE, "/ag remove [group ID] [player] - Removes a player from the group");
        SendClientMessage(playerid, COLOR_WHITE, "/ag perm [group ID] [player] [perm] - Sets player permissions");
        SendClientMessage(playerid, COLOR_WHITE, "/ag rank [ID] [0-9] [name] - Sets the rank name");
        SendClientMessage(playerid, COLOR_LIGHTER_BLUE, "==============================================");
        return 1;
    }

    // /ag create [name]
    if(!strcmp(subcmd, "create", true) || !strcmp(subcmd, "stworz", true))
    {
        if(isnull(subparams))
        {
            return SendClientMessage(playerid, COLOR_GRAY, "Usage: /ag create [group name]");
        }

        // Find free group ID
        new groupId = -1;
        for(new i = 1; i < MAX_GROUPS; i++)
        {
            if(!gInfo[i][group_created])
            {
                groupId = i;
                break;
            }
        }

        if(groupId == -1)
        {
            return SendClientMessage(playerid, COLOR_RED, "No free slots for groups!");
        }

        // Set basic data
        gInfo[groupId][group_created] = true;
        strcopy(gInfo[groupId][group_name], subparams, 64);
        strcopy(gInfo[groupId][group_tag], "GROUP", 15);
        gInfo[groupId][group_type] = 0;
        gInfo[groupId][group_color] = 0xFFFFFFFF;
        gInfo[groupId][group_bank] = 0;
        gInfo[groupId][group_leader] = 0;
        gInfo[groupId][group_max_members] = 50;
        gInfo[groupId][group_flags] = 0;

        // Default rank names
        strcopy(gInfo[groupId][group_rank0], "Recruit", 32);
        strcopy(gInfo[groupId][group_rank1], "Member", 32);
        strcopy(gInfo[groupId][group_rank2], "Senior Member", 32);
        strcopy(gInfo[groupId][group_rank3], "Veteran", 32);
        strcopy(gInfo[groupId][group_rank4], "Specialist", 32);
        strcopy(gInfo[groupId][group_rank5], "Officer", 32);
        strcopy(gInfo[groupId][group_rank6], "Senior Officer", 32);
        strcopy(gInfo[groupId][group_rank7], "Captain", 32);
        strcopy(gInfo[groupId][group_rank8], "Deputy", 32);
        strcopy(gInfo[groupId][group_rank9], "Leader", 32);

        Iter_Add(Groups, groupId);

        // Save to database
        new query[2048];
        new escapedName[129];
        mysql_escape_string(subparams, escapedName, mySQLconnection, sizeof(escapedName));

        mysql_format(mySQLconnection, query, sizeof(query),
            "INSERT INTO `groups` (`group_id`, `group_name`, `group_tag`, `group_type`, `group_color`, `group_bank`, `group_leader`, `group_max_members`, `group_flags`, \
             `group_rank0`, `group_rank1`, `group_rank2`, `group_rank3`, `group_rank4`, `group_rank5`, `group_rank6`, `group_rank7`, `group_rank8`, `group_rank9`) \
             VALUES (%d, '%s', 'GRUPA', 0, -1, 0, 0, 50, 0, 'Rekrut', 'Członek', 'Starszy Członek', 'Weteran', 'Specjalista', 'Oficer', 'Starszy Oficer', 'Kapitan', 'Zastępca', 'Lider')",
            groupId, escapedName);
        mysql_tquery(mySQLconnection, query);


        new msg[128];
        format(msg, sizeof(msg), "Created group '%s' with ID: %d", subparams, groupId);
        SendClientMessage(playerid, COLOR_GREEN, msg);

        Admin_LogAction(playerid, msg);
        return 1;
    }

    // /ag delete [ID]
    if(!strcmp(subcmd, "delete", true) || !strcmp(subcmd, "usun", true))
    {
        new groupId;
        if(sscanf(subparams, "%d", groupId) != 1)
        {
            return SendClientMessage(playerid, COLOR_GRAY, "Usage: /ag delete [group ID]");
        }

        if(groupId <= 0 || groupId >= MAX_GROUPS || !gInfo[groupId][group_created])
        {
            return SendClientMessage(playerid, COLOR_RED, "Group with the given ID does not exist!");
        }

        new groupName[64];
        strcopy(groupName, gInfo[groupId][group_name], sizeof(groupName));

        // Remove all members
        foreach(new i : Player)
        {
            if(Player_IsInGroup(i, groupId))
            {
                Group_RemoveMember(i, groupId);
            }
        }

        // Remove from iterator
        Iter_Remove(Groups, groupId);
        gInfo[groupId][group_created] = false;

        // Delete from database
        new query[128];
        mysql_format(mySQLconnection, query, sizeof(query),
            "DELETE FROM `groups` WHERE `group_id` = %d", groupId);
        mysql_tquery(mySQLconnection, query);

        new msg[128];
        format(msg, sizeof(msg), "Deleted group '%s' (ID: %d)", groupName, groupId);
        SendClientMessage(playerid, COLOR_GREEN, msg);

        Admin_LogAction(playerid, msg);
        return 1;
    }

    // /ag info [ID]
    if(!strcmp(subcmd, "info", true))
    {
        new groupId;
        if(sscanf(subparams, "%d", groupId) != 1)
        {
            return SendClientMessage(playerid, COLOR_GRAY, "Usage: /ag info [group ID]");
        }

        if(groupId <= 0 || groupId >= MAX_GROUPS || !gInfo[groupId][group_created])
        {
            return SendClientMessage(playerid, COLOR_RED, "Group with the given ID does not exist!");
        }

        new typeName[32];
        Admin_GetGroupTypeName(gInfo[groupId][group_type], typeName);

        SendClientMessage(playerid, COLOR_LIGHTER_BLUE, "========== GROUP INFORMATION ==========");

        new msg[256];
        format(msg, sizeof(msg), "ID: %d | Name: %s | Tag: [%s]",
            groupId, gInfo[groupId][group_name], gInfo[groupId][group_tag]);
        SendClientMessage(playerid, COLOR_WHITE, msg);

        format(msg, sizeof(msg), "Type: %s (%d) | Color: 0x%08X | Leader UID: %d",
            typeName, gInfo[groupId][group_type], gInfo[groupId][group_color], gInfo[groupId][group_leader]);
        SendClientMessage(playerid, COLOR_WHITE, msg);

        format(msg, sizeof(msg), "Bank: %s$ | Max members: %d | Flags: %d",
            FormatMoney(gInfo[groupId][group_bank]), gInfo[groupId][group_max_members], gInfo[groupId][group_flags]);
        SendClientMessage(playerid, COLOR_WHITE, msg);

        format(msg, sizeof(msg), "Online members: %d", Group_GetOnlineMembers(groupId));
        SendClientMessage(playerid, COLOR_WHITE, msg);

        SendClientMessage(playerid, COLOR_YELLOW, "Ranks:");
        format(msg, sizeof(msg), "0: %s | 1: %s | 2: %s | 3: %s | 4: %s",
            gInfo[groupId][group_rank0], gInfo[groupId][group_rank1],
            gInfo[groupId][group_rank2], gInfo[groupId][group_rank3], gInfo[groupId][group_rank4]);
        SendClientMessage(playerid, COLOR_WHITE, msg);

        format(msg, sizeof(msg), "5: %s | 6: %s | 7: %s | 8: %s | 9: %s",
            gInfo[groupId][group_rank5], gInfo[groupId][group_rank6],
            gInfo[groupId][group_rank7], gInfo[groupId][group_rank8], gInfo[groupId][group_rank9]);
        SendClientMessage(playerid, COLOR_WHITE, msg);

        SendClientMessage(playerid, COLOR_LIGHTER_BLUE, "==========================================");
        return 1;
    }

    // /ag list
    if(!strcmp(subcmd, "list", true) || !strcmp(subcmd, "lista", true))
    {
        SendClientMessage(playerid, COLOR_LIGHTER_BLUE, "========== GROUPS LIST ==========");

        new count = 0;
        foreach(new i : Groups)
        {
            new typeName[32];
            Admin_GetGroupTypeName(gInfo[i][group_type], typeName);

            new msg[256];
            format(msg, sizeof(msg), "[%d] %s [%s] - Type: %s, Online: %d",
                i, gInfo[i][group_name], gInfo[i][group_tag], typeName, Group_GetOnlineMembers(i));
            SendClientMessage(playerid, gInfo[i][group_color], msg);
            count++;
        }

        if(count == 0)
        {
            SendClientMessage(playerid, COLOR_GRAY, "No groups on the server.");
        }

        new msg[64];
        format(msg, sizeof(msg), "Total groups: %d", count);
        SendClientMessage(playerid, COLOR_GREEN, msg);
        SendClientMessage(playerid, COLOR_LIGHTER_BLUE, "==================================");
        return 1;
    }

    // /ag name [ID] [new name]
    if(!strcmp(subcmd, "name", true) || !strcmp(subcmd, "nazwa", true))
    {
        new groupId, newName[64];
        if(sscanf(subparams, "ds[64]", groupId, newName))
        {
            return SendClientMessage(playerid, COLOR_GRAY, "Usage: /ag name [group ID] [new name]");
        }

        if(groupId <= 0 || groupId >= MAX_GROUPS || !gInfo[groupId][group_created])
        {
            return SendClientMessage(playerid, COLOR_RED, "Group with the given ID does not exist!");
        }

        strcopy(gInfo[groupId][group_name], newName, 64);
        Group_Save(groupId);

        new msg[128];
        format(msg, sizeof(msg), "Changed the name of group %d to: %s", groupId, newName);
        SendClientMessage(playerid, COLOR_GREEN, msg);
        Admin_LogAction(playerid, msg);
        return 1;
    }

    // /ag tag [ID] [new tag]
    if(!strcmp(subcmd, "tag", true))
    {
        new groupId, newTag[15];
        if(sscanf(subparams, "ds[15]", groupId, newTag))
        {
            return SendClientMessage(playerid, COLOR_GRAY, "Usage: /ag tag [group ID] [new tag]");
        }

        if(groupId <= 0 || groupId >= MAX_GROUPS || !gInfo[groupId][group_created])
        {
            return SendClientMessage(playerid, COLOR_RED, "Group with the given ID does not exist!");
        }

        strcopy(gInfo[groupId][group_tag], newTag, 15);
        Group_Save(groupId);

        new msg[128];
        format(msg, sizeof(msg), "Changed the tag of group %d to: [%s]", groupId, newTag);
        SendClientMessage(playerid, COLOR_GREEN, msg);
        Admin_LogAction(playerid, msg);
        return 1;
    }

    // /ag type [ID] - GUI with group types
    if(!strcmp(subcmd, "type", true) || !strcmp(subcmd, "typ", true))
    {
        new groupId;
        if(sscanf(subparams, "%d", groupId) != 1)
        {
            return SendClientMessage(playerid, COLOR_GRAY, "Usage: /ag type [group ID]");
        }

        if(groupId <= 0 || groupId >= MAX_GROUPS || !gInfo[groupId][group_created])
        {
            return SendClientMessage(playerid, COLOR_RED, "Group with the given ID does not exist!");
        }

        pInfo[playerid][player_dialog_tmp1] = groupId;
        Admin_ShowTypeDialog(playerid, groupId);
        return 1;
    }

    // /ag color [ID] [hex color]
    if(!strcmp(subcmd, "color", true) || !strcmp(subcmd, "kolor", true))
    {
        new groupId, colorStr[16];
        if(sscanf(subparams, "ds[16]", groupId, colorStr))
        {
            return SendClientMessage(playerid, COLOR_GRAY, "Usage: /ag color [group ID] [hex color e.g. FF0000FF]");
        }

        if(groupId <= 0 || groupId >= MAX_GROUPS || !gInfo[groupId][group_created])
        {
            return SendClientMessage(playerid, COLOR_RED, "Group with the given ID does not exist!");
        }

        // Convert hex to int
        new color;
        sscanf(colorStr, "h", color);

        gInfo[groupId][group_color] = color;
        Group_Save(groupId);

        new msg[128];
        format(msg, sizeof(msg), "Changed the color of group %d to: 0x%08X", groupId, color);
        SendClientMessage(playerid, COLOR_GREEN, msg);
        Admin_LogAction(playerid, msg);
        return 1;
    }

    // /ag flags [ID] - GUI with permissions
    if(!strcmp(subcmd, "flags", true) || !strcmp(subcmd, "flagi", true) || !strcmp(subcmd, "uprawnienia", true) || !strcmp(subcmd, "abilities", true))
    {
        new groupId;
        if(sscanf(subparams, "%d", groupId) != 1)
        {
            return SendClientMessage(playerid, COLOR_GRAY, "Usage: /ag flags [group ID]");
        }

        if(groupId <= 0 || groupId >= MAX_GROUPS || !gInfo[groupId][group_created])
        {
            return SendClientMessage(playerid, COLOR_RED, "Group with the given ID does not exist!");
        }

        pInfo[playerid][player_dialog_tmp1] = groupId;
        Admin_ShowAbilitiesDialog(playerid, groupId);
        return 1;
    }

    // /ag leader [ID] [player]
    if(!strcmp(subcmd, "leader", true) || !strcmp(subcmd, "lider", true))
    {
        new groupId, targetid;
        if(sscanf(subparams, "du", groupId, targetid))
        {
            return SendClientMessage(playerid, COLOR_GRAY, "Usage: /ag leader [group ID] [ID/Nick player]");
        }

        if(groupId <= 0 || groupId >= MAX_GROUPS || !gInfo[groupId][group_created])
        {
            return SendClientMessage(playerid, COLOR_RED, "Group with the given ID does not exist!");
        }

        if(!IsPlayerConnected(targetid) || !pInfo[targetid][player_logged])
        {
            return SendClientMessage(playerid, COLOR_RED, "Player is not logged in.");
        }

        // Add player to group if not a member
        if(!Player_IsInGroup(targetid, groupId))
        {
            Group_AddMember(targetid, groupId, GROUP_PERM_LEADER, 0);
        }
        else
        {
            Group_SetMemberPermissions(targetid, groupId, GROUP_PERM_LEADER);
        }

        gInfo[groupId][group_leader] = pInfo[targetid][player_uid];
        Group_Save(groupId);

        new msg[128];
        format(msg, sizeof(msg), "Set player %s as the leader of group %s",
            pInfo[targetid][player_name], gInfo[groupId][group_name]);
        SendClientMessage(playerid, COLOR_GREEN, msg);

        format(msg, sizeof(msg), "You have been appointed as the leader of group %s!", gInfo[groupId][group_name]);
        SendClientMessage(targetid, COLOR_YELLOW, msg);

        Admin_LogAction(playerid, msg);
        return 1;
    }

    // /ag bank [ID] [amount]
    if(!strcmp(subcmd, "bank", true))
    {
        new groupId, amount;
        if(sscanf(subparams, "dd", groupId, amount))
        {
            return SendClientMessage(playerid, COLOR_GRAY, "Usage: /ag bank [group ID] [amount]");
        }

        if(groupId <= 0 || groupId >= MAX_GROUPS || !gInfo[groupId][group_created])
        {
            return SendClientMessage(playerid, COLOR_RED, "Group with the given ID does not exist!");
        }

        gInfo[groupId][group_bank] = amount;
        Group_Save(groupId);

        new msg[128];
        format(msg, sizeof(msg), "Set the bank of group %d to: %s$", groupId, FormatMoney(amount));
        SendClientMessage(playerid, COLOR_GREEN, msg);
        Admin_LogAction(playerid, msg);
        return 1;
    }

    // /ag add [group ID] [player]
    if(!strcmp(subcmd, "add", true) || !strcmp(subcmd, "dodaj", true))
    {
        new groupId, targetid;
        if(sscanf(subparams, "du", groupId, targetid))
        {
            return SendClientMessage(playerid, COLOR_GRAY, "Usage: /ag add [group ID] [ID/Nick player]");
        }

        if(groupId <= 0 || groupId >= MAX_GROUPS || !gInfo[groupId][group_created])
        {
            return SendClientMessage(playerid, COLOR_RED, "Group with the given ID does not exist!");
        }

        if(!IsPlayerConnected(targetid) || !pInfo[targetid][player_logged])
        {
            return SendClientMessage(playerid, COLOR_RED, "Player is not logged in.");
        }

        if(Player_IsInGroup(targetid, groupId))
        {
            return SendClientMessage(playerid, COLOR_RED, "Player is already a member of this group!");
        }

        Group_AddMember(targetid, groupId, GROUP_PERM_NONE, 0);

        new msg[128];
        format(msg, sizeof(msg), "Added player %s to group %s",
            pInfo[targetid][player_name], gInfo[groupId][group_name]);
        SendClientMessage(playerid, COLOR_GREEN, msg);
        Admin_LogAction(playerid, msg);
        return 1;
    }

    // /ag remove [group ID] [player]
    if(!strcmp(subcmd, "remove", true) || !strcmp(subcmd, "wyrzuc", true))
    {
        new groupId, targetid;
        if(sscanf(subparams, "du", groupId, targetid))
        {
            return SendClientMessage(playerid, COLOR_GRAY, "Usage: /ag remove [group ID] [ID/Nick player]");
        }

        if(groupId <= 0 || groupId >= MAX_GROUPS || !gInfo[groupId][group_created])
        {
            return SendClientMessage(playerid, COLOR_RED, "Group with the given ID does not exist!");
        }

        if(!IsPlayerConnected(targetid) || !pInfo[targetid][player_logged])
        {
            return SendClientMessage(playerid, COLOR_RED, "Player is not logged in.");
        }

        if(!Player_IsInGroup(targetid, groupId))
        {
            return SendClientMessage(playerid, COLOR_RED, "Player is not a member of this group!");
        }

        Group_RemoveMember(targetid, groupId);

        new msg[128];
        format(msg, sizeof(msg), "Removed player %s from group %s",
            pInfo[targetid][player_name], gInfo[groupId][group_name]);
        SendClientMessage(playerid, COLOR_GREEN, msg);
        Admin_LogAction(playerid, msg);
        return 1;
    }

    // /ag perm [ID grupy] [gracz] [perm]
    if(!strcmp(subcmd, "perm", true) || !strcmp(subcmd, "uprawnienia", true))
    {
        new groupId, targetid, perm;
        if(sscanf(subparams, "dud", groupId, targetid, perm))
        {
            SendClientMessage(playerid, COLOR_GRAY, "Usage: /ag perm [group ID] [ID/Nick player] [permissions]");
            SendClientMessage(playerid, COLOR_GRAY, "Permissions: 1-Invite, 2-Kick, 4-Rank, 8-Bank, 16-Vehicles, 32-Doors, 64-Leader, 128-Deputy, 256-Duty");
            return 1;
        }

        if(groupId <= 0 || groupId >= MAX_GROUPS || !gInfo[groupId][group_created])
        {
            return SendClientMessage(playerid, COLOR_RED, "Group with the given ID does not exist!");
        }

        if(!IsPlayerConnected(targetid) || !pInfo[targetid][player_logged])
        {
            return SendClientMessage(playerid, COLOR_RED, "Player is not logged in.");
        }

        if(!Player_IsInGroup(targetid, groupId))
        {
            return SendClientMessage(playerid, COLOR_RED, "Player is not a member of this group!");
        }

        Group_SetMemberPermissions(targetid, groupId, perm);

        new msg[128];
        format(msg, sizeof(msg), "Set the permissions of player %s in group %s to: %d",
            pInfo[targetid][player_name], gInfo[groupId][group_name], perm);
        SendClientMessage(playerid, COLOR_GREEN, msg);
        Admin_LogAction(playerid, msg);

        return 1;
    }

    // /ag rank [ID] [0-9] [name]
    if(!strcmp(subcmd, "rank", true) || !strcmp(subcmd, "ranga", true))
    {
        new groupId, rankId, rankName[32];
        if(sscanf(subparams, "dds[32]", groupId, rankId, rankName))
        {
            return SendClientMessage(playerid, COLOR_GRAY, "Usage: /ag rank [group ID] [rank 0-9] [rank name]");
        }

        if(groupId <= 0 || groupId >= MAX_GROUPS || !gInfo[groupId][group_created])
        {
            return SendClientMessage(playerid, COLOR_RED, "Group with the given ID does not exist!");
        }

        if(rankId < 0 || rankId > 9)
        {
            return SendClientMessage(playerid, COLOR_RED, "Rank must be in the range 0-9!");
        }

        switch(rankId)
        {
            case 0: strcopy(gInfo[groupId][group_rank0], rankName, 32);
            case 1: strcopy(gInfo[groupId][group_rank1], rankName, 32);
            case 2: strcopy(gInfo[groupId][group_rank2], rankName, 32);
            case 3: strcopy(gInfo[groupId][group_rank3], rankName, 32);
            case 4: strcopy(gInfo[groupId][group_rank4], rankName, 32);
            case 5: strcopy(gInfo[groupId][group_rank5], rankName, 32);
            case 6: strcopy(gInfo[groupId][group_rank6], rankName, 32);
            case 7: strcopy(gInfo[groupId][group_rank7], rankName, 32);
            case 8: strcopy(gInfo[groupId][group_rank8], rankName, 32);
            case 9: strcopy(gInfo[groupId][group_rank9], rankName, 32);
        }

        Group_Save(groupId);

        new msg[128];
        format(msg, sizeof(msg), "Changed the name of rank %d in group %d to: %s", rankId, groupId, rankName);
        SendClientMessage(playerid, COLOR_GREEN, msg);
        Admin_LogAction(playerid, msg);
        return 1;
    }

    // Unknown subcommand
    SendClientMessage(playerid, COLOR_RED, "Unknown subcommand. Type /ag to see help.");
    return 1;
}

/**
 * Shows the dialog for group type
 */
stock Admin_ShowTypeDialog(playerid, groupId)
{
    new dialog[1024];
    new flags = gInfo[groupId][group_flags];
    new title[64];

    format(title, sizeof(title), "Group Type: %s", gInfo[groupId][group_name]);

    for(new i = 0; i < sizeof(Admin_GroupFlags); i++)
    {
        if(flags & Admin_GroupFlags[i][0])
        {
            format(dialog, sizeof(dialog), "%s{00FF00}[X] %s\n", dialog, Admin_GroupFlagNames[i]);
        }
        else
        {
            format(dialog, sizeof(dialog), "%s{FF0000}[ ] %s\n", dialog, Admin_GroupFlagNames[i]);
        }
    }

    ShowPlayerDialog(playerid, DIALOG_AG_TYPES, DIALOG_STYLE_LIST, title, dialog, "Select", "Close");
    return 1;
}

/**
 * Shows the dialog with group permissions
 */
stock Admin_ShowAbilitiesDialog(playerid, groupId)
{
    new dialog[1536];
    new abilities = gInfo[groupId][group_abilities];
    new title[64];

    format(title, sizeof(title), "Group Permissions: %s", gInfo[groupId][group_name]);

    for(new i = 0; i < sizeof(Admin_GroupAbilities); i++)
    {
        if(abilities & Admin_GroupAbilities[i][0])
        {
            format(dialog, sizeof(dialog), "%s{00FF00}[X] %s\n", dialog, Admin_GroupAbilityNames[i]);
        }
        else
        {
            format(dialog, sizeof(dialog), "%s{FF0000}[ ] %s\n", dialog, Admin_GroupAbilityNames[i]);
        }
    }

    ShowPlayerDialog(playerid, DIALOG_AG_ABILITIES, DIALOG_STYLE_LIST, title, dialog, "Select", "Close");
    return 1;
}

/**
 * Handler for the group type dialog
 */
stock Admin_HandleTypeDialog(playerid, response, listitem)
{
    if(!response)
    {
        return 1;
    }

    new groupId = pInfo[playerid][player_dialog_tmp1];

    if(!Group_Exists(groupId))
    {
        SendClientMessage(playerid, COLOR_RED, "Group does not exist!");
        return 1;
    }

    if(listitem < 0 || listitem >= sizeof(Admin_GroupFlags))
    {
        return 1;
    }

    new flag = Admin_GroupFlags[listitem][0];
    new msg[128];

    // Toggle flag
    if(gInfo[groupId][group_flags] & flag)
    {
        gInfo[groupId][group_flags] &= ~flag;
        format(msg, sizeof(msg), "Removed type '%s' from group %s", Admin_GroupFlagNames[listitem], gInfo[groupId][group_name]);
    }
    else
    {
        gInfo[groupId][group_flags] |= flag;
        format(msg, sizeof(msg), "Added type '%s' to group %s", Admin_GroupFlagNames[listitem], gInfo[groupId][group_name]);
    }

    Group_Save(groupId);
    SendClientMessage(playerid, COLOR_GREEN, msg);
    Admin_LogAction(playerid, msg);

    // Refresh dialog
    Admin_ShowTypeDialog(playerid, groupId);
    return 1;
}

/**
 * Handler for the group permissions dialog
 */
stock Admin_HandleAbilitiesDialog(playerid, response, listitem)
{
    if(!response)
    {
        return 1;
    }

    new groupId = pInfo[playerid][player_dialog_tmp1];

    if(!Group_Exists(groupId))
    {
        SendClientMessage(playerid, COLOR_RED, "Group does not exist!");
        return 1;
    }

    if(listitem < 0 || listitem >= sizeof(Admin_GroupAbilities))
    {
        return 1;
    }

    new ability = Admin_GroupAbilities[listitem][0];
    new msg[128];

    // Toggle ability
    if(gInfo[groupId][group_abilities] & ability)
    {
        gInfo[groupId][group_abilities] &= ~ability;
        format(msg, sizeof(msg), "Removed permission '%s' from group %s", Admin_GroupAbilityNames[listitem], gInfo[groupId][group_name]);
    }
    else
    {
        gInfo[groupId][group_abilities] |= ability;
        format(msg, sizeof(msg), "Added permission '%s' to group %s", Admin_GroupAbilityNames[listitem], gInfo[groupId][group_name]);
    }

    Group_Save(groupId);
    SendClientMessage(playerid, COLOR_GREEN, msg);
    Admin_LogAction(playerid, msg);

    // Refresh dialog
    Admin_ShowAbilitiesDialog(playerid, groupId);
    return 1;
}

// ===========================================================================
// ITEM MANAGEMENT COMMANDS (/ap)
// ===========================================================================

#define DIALOG_AP_MENU          3500
#define DIALOG_AP_GIVE          3501
#define DIALOG_AP_GIVE_TYPE     3502
#define DIALOG_AP_GIVE_VALUE    3503
#define DIALOG_AP_GIVE_NAME     3504
#define DIALOG_AP_CHECK         3505
#define DIALOG_AP_REMOVE        3506

static g_APTargetPlayer[MAX_PLAYERS];
static g_APItemType[MAX_PLAYERS];
static g_APItemValue1[MAX_PLAYERS];
static g_APItemValue2[MAX_PLAYERS];

/**
 * /ap - Admin Panel - Items management
 */
YCMD:ap(playerid, params[], help)
{
    ADMIN_CHECK(ADMIN_LEVEL_MODERATOR);

    if(isnull(params))
    {
        SendClientMessage(playerid, COLOR_GREY, "Usage: /ap [give/check/remove/list]");
        SendClientMessage(playerid, COLOR_GREY, "  give [id] - Give an item to a player");
        SendClientMessage(playerid, COLOR_GREY, "  check [id] - Check a player's inventory");
        SendClientMessage(playerid, COLOR_GREY, "  remove [id] [slot] - Remove an item from a player");
        SendClientMessage(playerid, COLOR_GREY, "  list - List of item types");
        return 1;
    }

    new option[16], subparams[64];
    if(sscanf(params, "s[16]S()[64]", option, subparams))
    {
        return 1;
    }

    // /ap give [id]
    if(!strcmp(option, "give", true) || !strcmp(option, "daj", true))
    {
        new targetid;
        if(sscanf(subparams, "u", targetid))
        {
            SendClientMessage(playerid, COLOR_GREY, "Usage: /ap give [player id]");
            return 1;
        }

        if(!IsPlayerConnected(targetid) || !pInfo[targetid][player_logged])
        {
            SendClientMessage(playerid, COLOR_ERROR, "Player is not logged in.");
            return 1;
        }

        g_APTargetPlayer[playerid] = targetid;
        AP_ShowItemTypeDialog(playerid);
        return 1;
    }

    // /ap check [id]
    if(!strcmp(option, "check", true) || !strcmp(option, "sprawdz", true))
    {
        new targetid;
        if(sscanf(subparams, "u", targetid))
        {
            SendClientMessage(playerid, COLOR_GREY, "Usage: /ap check [player id]");
            return 1;
        }

        if(!IsPlayerConnected(targetid) || !pInfo[targetid][player_logged])
        {
            SendClientMessage(playerid, COLOR_ERROR, "Player is not logged in.");
            return 1;
        }

        AP_ShowPlayerItems(playerid, targetid);
        return 1;
    }

    // /ap remove [id] [slot]
    if(!strcmp(option, "remove", true) || !strcmp(option, "usun", true))
    {
        new targetid, slot;
        if(sscanf(subparams, "ui", targetid, slot))
        {
            SendClientMessage(playerid, COLOR_GREY, "Usage: /ap remove [player id] [slot]");
            return 1;
        }

        if(!IsPlayerConnected(targetid) || !pInfo[targetid][player_logged])
        {
            SendClientMessage(playerid, COLOR_ERROR, "Player is not logged in.");
            return 1;
        }

        if(slot < 0 || slot >= MAX_PLAYER_ITEMS)
        {
            SendClientMessage(playerid, COLOR_ERROR, "Invalid slot (0-%d).", MAX_PLAYER_ITEMS - 1);
            return 1;
        }

        if(pItem[targetid][slot][item_id] == 0)
        {
            SendClientMessage(playerid, COLOR_ERROR, "This slot is empty.");
            return 1;
        }

        new itemName[ITEM_NAME_MAX];
        strcopy(itemName, pItem[targetid][slot][item_name], ITEM_NAME_MAX);

        Item_Remove(targetid, slot);

        new msg[128];
        format(msg, sizeof(msg), "Removed item '%s' from slot %d of player %s.",
            itemName, slot, Player_RPName(targetid));
        SendClientMessage(playerid, COLOR_SUCCESS, msg);

        format(msg, sizeof(msg), "The administrator %s removed your item: %s",
            Player_RPName(playerid), itemName);
        SendClientMessage(targetid, COLOR_YELLOW, msg);

        Admin_LogAction(playerid, msg);

        return 1;
    }

    // /ap list - List of item types
    if(!strcmp(option, "list", true) || !strcmp(option, "lista", true))
    {
        SendClientMessage(playerid, COLOR_INFO, "=== ITEM TYPES ===");
        SendClientMessage(playerid, COLOR_GREY, "1-4: Documents (ID, Driver's license, Gun permit, Health card)");
        SendClientMessage(playerid, COLOR_GREY, "10-13: Food (Food, Drink, Alcohol, Drug)");
        SendClientMessage(playerid, COLOR_GREY, "20-22: Medical (Bandage, First aid kit, Painkillers)");
        SendClientMessage(playerid, COLOR_GREY, "30-36: Tools (Phone, Radio, GPS, Camera, Fishing rod, Rope, Lockpick)");
        SendClientMessage(playerid, COLOR_GREY, "50-52: Weapon (Gun, Ammo, Vest)");
        SendClientMessage(playerid, COLOR_GREY, "60-62: Vehicles (Keys, Canister, Repair kit)");
        SendClientMessage(playerid, COLOR_GREY, "70-71: Keys (Door key, Safe key)");
        SendClientMessage(playerid, COLOR_GREY, "80-81: Materials (Materials, Drug package)");
        SendClientMessage(playerid, COLOR_GREY, "90-93: Other (Mask, Paint, Bag, Boombox)");
        return 1;
    }

    SendClientMessage(playerid, COLOR_ERROR, "Unknown option. Use: /ap give/check/remove/list");
    return 1;
}

/**
 * Shows the dialog for item type selection
 */
stock AP_ShowItemTypeDialog(playerid)
{
    new content[2048];
    strcat(content, "=== DOCUMENTS ===\n");
    strcat(content, "1. ID Card\n");
    strcat(content, "2. Driver's License\n");
    strcat(content, "3. Gun Permit\n");
    strcat(content, "4. Health Card\n");
    strcat(content, "=== FOOD ===\n");
    strcat(content, "10. Food\n");
    strcat(content, "11. Drink\n");
    strcat(content, "12. Alcohol\n");
    strcat(content, "13. Drugs\n");
    strcat(content, "=== MEDICAL ===\n");
    strcat(content, "20. Bandage\n");
    strcat(content, "21. First Aid Kit\n");
    strcat(content, "22. Painkillers\n");
    strcat(content, "=== TOOLS ===\n");
    strcat(content, "30. Phone\n");
    strcat(content, "31. Walkie-talkie\n");
    strcat(content, "32. GPS\n");
    strcat(content, "33. Camera\n");
    strcat(content, "34. Fishing Rod\n");
    strcat(content, "35. Rope\n");
    strcat(content, "36. Lockpick\n");
    strcat(content, "=== WEAPON ===\n");
    strcat(content, "50. Gun (value1=weaponid, value2=ammo)\n");
    strcat(content, "51. Ammo (value1=weaponid, value2=amount)\n");
    strcat(content, "52. Vest (value1=armor)\n");

    ShowPlayerDialog(playerid, DIALOG_AP_GIVE_TYPE, DIALOG_STYLE_INPUT,
        "Enter the item type", content, "Next", "Cancel");
}

/**
 * Shows a player's inventory to the admin
 */
stock AP_ShowPlayerItems(playerid, targetid)
{
    new content[2048];
    new header[64];
    format(header, sizeof(header), "Inventory of player %s", Player_RPName(targetid));

    new count = 0;
    for(new i = 0; i < MAX_PLAYER_ITEMS; i++)
    {
        if(pItem[targetid][i][item_id] != 0)
        {
            new line[128];
            format(line, sizeof(line), "[%d] %s (Type: %d, V1: %d, V2: %d)\n",
                i,
                pItem[targetid][i][item_name],
                pItem[targetid][i][item_type],
                pItem[targetid][i][item_value],
                pItem[targetid][i][item_value2]);
            strcat(content, line);
            count++;
        }
    }

    if(count == 0)
    {
        strcat(content, "Player has no items.");
    }

    ShowPlayerDialog(playerid, DIALOG_AP_CHECK, DIALOG_STYLE_MSGBOX, header, content, "OK", "");
}

/**
 * Handles the /ap dialogs
 */
hook OnDialogResponse@AP(playerid, dialogid, response, listitem, inputtext[])
{
    switch(dialogid)
    {
        case DIALOG_AP_GIVE_TYPE:
        {
            if(!response)
            {
                g_APTargetPlayer[playerid] = INVALID_PLAYER_ID;
                return Y_HOOKS_BREAK_RETURN_1;
            }

            new itemType = strval(inputtext);
            if(itemType <= 0 || itemType >= _:ITEM_TYPE_MAX)
            {
                SendClientMessage(playerid, COLOR_ERROR, "Invalid item type!");
                AP_ShowItemTypeDialog(playerid);
                return Y_HOOKS_BREAK_RETURN_1;
            }

            g_APItemType[playerid] = itemType;

            ShowPlayerDialog(playerid, DIALOG_AP_GIVE_VALUE, DIALOG_STYLE_INPUT,
                "Enter the item values",
                "Enter value1 and value2 separated by space\n\nExample: 24 100 (for weapon: weaponid ammo)\nOr: 50 0 (for first aid kit: HP 0)",
                "Next", "Cancel");

            return Y_HOOKS_BREAK_RETURN_1;
        }

        case DIALOG_AP_GIVE_VALUE:
        {
            if(!response)
            {
                g_APTargetPlayer[playerid] = INVALID_PLAYER_ID;
                return Y_HOOKS_BREAK_RETURN_1;
            }

            new value1, value2;
            if(sscanf(inputtext, "iI(0)", value1, value2))
            {
                value1 = strval(inputtext);
                value2 = 0;
            }

            g_APItemValue1[playerid] = value1;
            g_APItemValue2[playerid] = value2;

            ShowPlayerDialog(playerid, DIALOG_AP_GIVE_NAME, DIALOG_STYLE_INPUT,
                "Enter the item name",
                "Enter the item name (or leave blank for default)",
                "Create", "Cancel");

            return Y_HOOKS_BREAK_RETURN_1;
        }

        case DIALOG_AP_GIVE_NAME:
        {
            if(!response)
            {
                g_APTargetPlayer[playerid] = INVALID_PLAYER_ID;
                return Y_HOOKS_BREAK_RETURN_1;
            }

            new targetid = g_APTargetPlayer[playerid];

            if(!IsPlayerConnected(targetid) || !pInfo[targetid][player_logged])
            {
                SendClientMessage(playerid, COLOR_ERROR, "Player has disconnected.");
                return Y_HOOKS_BREAK_RETURN_1;
            }

            // If creating a phone and no number supplied, generate and assign one
            if(g_APItemType[playerid] == ITEM_TYPE_PHONE)
            {
                if(g_APItemValue2[playerid] == 0)
                {
                    new owner_uid = pInfo[targetid][player_uid];
                    new generated = Phone_GenerateFreeNumber(owner_uid);
                    if(generated == 0)
                    {
                        SendClientMessage(playerid, COLOR_ERROR, "Failed to generate a free phone number. No numbers available.");
                        return Y_HOOKS_BREAK_RETURN_1;
                    }
                    g_APItemValue2[playerid] = generated;
                }
                else
                {
                    // If admin supplied a number, ensure it's not already owned
                    new supplied = g_APItemValue2[playerid];
                    if(supplied < PHONE_MIN_NUMBER || supplied > PHONE_MAX_NUMBER)
                    {
                        SendClientMessage(playerid, COLOR_ERROR, "Supplied phone number is out of allowed range.");
                        return Y_HOOKS_BREAK_RETURN_1;
                    }
                    if(gPhoneOwnerUidByNumber[supplied] != 0)
                    {
                        SendClientMessage(playerid, COLOR_ERROR, "That phone number is already assigned to someone.");
                        return Y_HOOKS_BREAK_RETURN_1;
                    }
                    // Ensure DB row exists and set owner
                    Phone_EnsureNumberExists(supplied);
                    Phone_AssignNumberToOwner(pInfo[targetid][player_uid], supplied);
                }
            }

            new slot = Item_CreateForPlayer(
                targetid,
                E_ITEM_TYPE:g_APItemType[playerid],
                g_APItemValue1[playerid],
                g_APItemValue2[playerid],
                inputtext
            );

            if(slot == -1)
            {
                SendClientMessage(playerid, COLOR_ERROR, "Failed to create the item (inventory full?).");
                return Y_HOOKS_BREAK_RETURN_1;
            }

            new msg[128];
            format(msg, sizeof(msg), "Created item '%s' (type: %d, v1: %d, v2: %d) for %s [slot %d]",
                pItem[targetid][slot][item_name],
                g_APItemType[playerid],
                g_APItemValue1[playerid],
                g_APItemValue2[playerid],
                Player_RPName(targetid),
                slot);
            SendClientMessage(playerid, COLOR_SUCCESS, msg);

            format(msg, sizeof(msg), "You have received an item from the administrator: %s",
                pItem[targetid][slot][item_name]);
            SendClientMessage(targetid, COLOR_YELLOW, msg);

            Admin_LogAction(playerid, msg);

            g_APTargetPlayer[playerid] = INVALID_PLAYER_ID;
            return Y_HOOKS_BREAK_RETURN_1;
        }
    }

    return 1;
}

// ===========================================================================
// COMMANDS FOR VEHICLE MANAGEMENT (/av)
// ===========================================================================

#define DIALOG_AV_TUNING        3510
#define DIALOG_AV_ASSIGN        3511
#define DIALOG_AV_COLOR         3512

static g_AVTargetVehicle[MAX_PLAYERS];

/**
 * /av - Admin Vehicles management
 */
YCMD:av(playerid, params[], help)
{
    ADMIN_CHECK(ADMIN_LEVEL_MODERATOR);

    if(isnull(params))
    {
        SendClientMessage(playerid, COLOR_GREY, "Usage: /av [option] [parameters]");
        SendClientMessage(playerid, COLOR_GREY, "  create [model] - Create a vehicle (you can specify name or ID)");
        SendClientMessage(playerid, COLOR_GREY, "  destroy - Delete the vehicle you are in");
        SendClientMessage(playerid, COLOR_GREY, "  repair - Repair the vehicle");
        SendClientMessage(playerid, COLOR_GREY, "  fuel [amount] - Set fuel (0-100)");
        SendClientMessage(playerid, COLOR_GREY, "  color [color1] [color2] - Change colors");
        SendClientMessage(playerid, COLOR_GREY, "  tune - Tuning menu");
        SendClientMessage(playerid, COLOR_GREY, "  assign [player/group] [id] - Assign the vehicle");
        SendClientMessage(playerid, COLOR_GREY, "  park - Save the current position as spawn");
        SendClientMessage(playerid, COLOR_GREY, "  respawn - Respawn the vehicle");
        SendClientMessage(playerid, COLOR_GREY, "  lock/unlock - Lock/Unlock");
        SendClientMessage(playerid, COLOR_GREY, "  info - Information about the vehicle");
        return 1;
    }

    new option[16], subparams[64];
    if(sscanf(params, "s[16]S()[64]", option, subparams))
    {
        return 1;
    }

    // /av create [model]
    if(!strcmp(option, "create", true) || !strcmp(option, "stworz", true))
    {
        new modelid;

        // Check if it's a number or a name
        if(sscanf(subparams, "i", modelid))
        {
            // Not a number - search by name
            modelid = AV_GetModelByName(subparams);

            if(modelid == 0)
            {
                SendClientMessage(playerid, COLOR_ERROR, "Vehicle with the given name not found.");
                SendClientMessage(playerid, COLOR_GREY, "Usage: /av create [model ID or name]");
                return 1;
            }
        }

        if(modelid < 400 || modelid > 611)
        {
            SendClientMessage(playerid, COLOR_ERROR, "Invalid vehicle model (400-611).");
            return 1;
        }

        new Float:x, Float:y, Float:z, Float:angle;
        GetPlayerPos(playerid, x, y, z);
        GetPlayerFacingAngle(playerid, angle);

        // Create vehicle next to the player
        x += 3.0 * floatsin(-angle, degrees);
        y += 3.0 * floatcos(-angle, degrees);

        new vehicleid = Vehicle_Create(modelid, x, y, z, angle, -1, -1, OWNER_SERVER, 0);

        if(vehicleid == INVALID_VEHICLE_ID)
        {
            SendClientMessage(playerid, COLOR_ERROR, "Failed to create the vehicle.");
            return 1;
        }

        new modelName[32], msg[128];
        GetVehicleModelName(modelid, modelName);
        format(msg, sizeof(msg), "You have created a vehicle: %s (model: %d, veh ID: %d)", modelName, modelid, vehicleid);
        SendClientMessage(playerid, COLOR_SUCCESS, msg);

        Admin_LogAction(playerid, "Created a vehicle");

        return 1;
    }

    // /av destroy
    if(!strcmp(option, "destroy", true) || !strcmp(option, "usun", true))
    {
        new vehicleid = GetPlayerVehicleID(playerid);

        if(vehicleid == 0)
        {
            // Check the vehicle in front of the player
            vehicleid = AV_GetNearestVehicle(playerid, 5.0);

            if(vehicleid == INVALID_VEHICLE_ID)
            {
                SendClientMessage(playerid, COLOR_ERROR, "You must be in a vehicle or close to it.");
                return 1;
            }
        }

        new slot = Vehicle_GetSlot(vehicleid);
        new modelName[32], msg[128];
        GetVehicleModelName(GetVehicleModel(vehicleid), modelName);

        if(slot != -1)
        {
            Vehicle_Destroy(slot);
        }
        else
        {
            DestroyVehicle(vehicleid);
        }

        format(msg, sizeof(msg), "You have removed the vehicle: %s (ID: %d)", modelName, vehicleid);
        SendClientMessage(playerid, COLOR_GREEN, msg);

        Admin_LogAction(playerid, msg);
        return 1;
    }

    // /av repair
    if(!strcmp(option, "repair", true) || !strcmp(option, "napraw", true))
    {
        new vehicleid = GetPlayerVehicleID(playerid);

        if(vehicleid == 0)
        {
            vehicleid = AV_GetNearestVehicle(playerid, 5.0);

            if(vehicleid == INVALID_VEHICLE_ID)
            {
                SendClientMessage(playerid, COLOR_ERROR, "You must be in a vehicle or close to it.");
                return 1;
            }
        }

        Vehicle_Repair(vehicleid);

        new modelName[32], msg[128];
        GetVehicleModelName(GetVehicleModel(vehicleid), modelName);
        format(msg, sizeof(msg), "Repaired the vehicle: %s", modelName);
        SendClientMessage(playerid, COLOR_GREEN, msg);
        return 1;
    }

    // /av fuel [amount]
    if(!strcmp(option, "fuel", true) || !strcmp(option, "paliwo", true))
    {
        new fuel;
        if(sscanf(subparams, "i", fuel))
        {
            SendClientMessage(playerid, COLOR_GREY, "Usage: /av fuel [0-100]");
            return 1;
        }

        new vehicleid = GetPlayerVehicleID(playerid);

        if(vehicleid == 0)
        {
            vehicleid = AV_GetNearestVehicle(playerid, 5.0);

            if(vehicleid == INVALID_VEHICLE_ID)
            {
                SendClientMessage(playerid, COLOR_ERROR, "You must be in a vehicle or close to it.");
                return 1;
            }
        }

        Vehicle_SetFuel(vehicleid, clamp(fuel, 0, 100));

        new msg[64];
        format(msg, sizeof(msg), "Set fuel to: %d%%", clamp(fuel, 0, 100));
        SendClientMessage(playerid, COLOR_SUCCESS, msg);
        return 1;
    }

    // /av color [color1] [color2]
    if(!strcmp(option, "color", true) || !strcmp(option, "kolor", true))
    {
        new color1, color2;
        if(sscanf(subparams, "ii", color1, color2))
        {
            SendClientMessage(playerid, COLOR_GREY, "Usage: /av color [color1] [color2]");
            return 1;
        }

        new vehicleid = GetPlayerVehicleID(playerid);

        if(vehicleid == 0)
        {
            vehicleid = AV_GetNearestVehicle(playerid, 5.0);

            if(vehicleid == INVALID_VEHICLE_ID)
            {
                SendClientMessage(playerid, COLOR_ERROR, "You must be in a vehicle or close to it.");
                return 1;
            }
        }

        ChangeVehicleColor(vehicleid, color1, color2);

        new slot = Vehicle_GetSlot(vehicleid);
        if(slot != -1)
        {
            vInfo[slot][veh_color1] = color1;
            vInfo[slot][veh_color2] = color2;
        }

        new msg[64];
        format(msg, sizeof(msg), "Changed colors to: %d, %d", color1, color2);
        SendClientMessage(playerid, COLOR_SUCCESS, msg);
        return 1;
    }

    // /av tune
    if(!strcmp(option, "tune", true) || !strcmp(option, "tuning", true))
    {
        new vehicleid = GetPlayerVehicleID(playerid);

        if(vehicleid == 0)
        {
            SendClientMessage(playerid, COLOR_ERROR, "You must be in a vehicle.");
            return 1;
        }

        g_AVTargetVehicle[playerid] = vehicleid;
        AV_ShowTuningDialog(playerid);
        return 1;
    }

    // /av assign [player/group] [id]
    if(!strcmp(option, "assign", true) || !strcmp(option, "przypisz", true))
    {
        new ownerType[16], ownerId;
        if(sscanf(subparams, "s[16]i", ownerType, ownerId))
        {
            SendClientMessage(playerid, COLOR_GREY, "Usage: /av assign [player/group/server] [id]");
            return 1;
        }

        new vehicleid = GetPlayerVehicleID(playerid);

        if(vehicleid == 0)
        {
            vehicleid = AV_GetNearestVehicle(playerid, 5.0);

            if(vehicleid == INVALID_VEHICLE_ID)
            {
                SendClientMessage(playerid, COLOR_ERROR, "You must be in a vehicle or close to it.");
                return 1;
            }
        }

        new slot = Vehicle_GetSlot(vehicleid);

        if(slot == -1)
        {
            SendClientMessage(playerid, COLOR_ERROR, "This is not a server vehicle.");
            return 1;
        }

        new E_VEHICLE_OWNER_TYPE:newOwnerType;
        new msg[128];

        if(!strcmp(ownerType, "player", true) || !strcmp(ownerType, "gracz", true))
        {
            if(!IsPlayerConnected(ownerId) || !pInfo[ownerId][player_logged])
            {
                SendClientMessage(playerid, COLOR_ERROR, "Player is not logged in.");
                return 1;
            }

            newOwnerType = OWNER_PLAYER;
            vInfo[slot][veh_owner_type] = _:newOwnerType;
            vInfo[slot][veh_owner] = pInfo[ownerId][player_id];

            format(msg, sizeof(msg), "Assigned the vehicle to player: %s (UID: %d)",
                Player_RPName(ownerId), pInfo[ownerId][player_id]);
        }
        else if(!strcmp(ownerType, "group", true) || !strcmp(ownerType, "grupa", true))
        {
            newOwnerType = OWNER_GROUP;
            vInfo[slot][veh_owner_type] = _:newOwnerType;
            vInfo[slot][veh_owner] = ownerId;

            format(msg, sizeof(msg), "Assigned the vehicle to group ID: %d", ownerId);
        }
        else if(!strcmp(ownerType, "server", true) || !strcmp(ownerType, "serwer", true))
        {
            newOwnerType = OWNER_SERVER;
            vInfo[slot][veh_owner_type] = _:newOwnerType;
            vInfo[slot][veh_owner] = 0;

            format(msg, sizeof(msg), "Vehicle set as server-owned");
        }
        else
        {
            SendClientMessage(playerid, COLOR_ERROR, "Invalid type: player/group/server");
            return 1;
        }

        SendClientMessage(playerid, COLOR_SUCCESS, msg);
        Admin_LogAction(playerid, msg);
        return 1;
    }

    // /av park
    if(!strcmp(option, "park", true))
    {
        new vehicleid = GetPlayerVehicleID(playerid);

        if(vehicleid == 0)
        {
            SendClientMessage(playerid, COLOR_ERROR, "You must be in a vehicle.");
            return 1;
        }

        new Float:x, Float:y, Float:z, Float:angle;
        GetVehiclePos(vehicleid, x, y, z);
        GetVehicleZAngle(vehicleid, angle);

        Vehicle_SetSpawnPos(vehicleid, x, y, z, angle);

        SendClientMessage(playerid, COLOR_SUCCESS, "Saved the current position as the vehicle spawn.");
        return 1;
    }

    // /av respawn
    if(!strcmp(option, "respawn", true))
    {
        new vehicleid = GetPlayerVehicleID(playerid);

        if(vehicleid == 0)
        {
            vehicleid = AV_GetNearestVehicle(playerid, 5.0);

            if(vehicleid == INVALID_VEHICLE_ID)
            {
                SendClientMessage(playerid, COLOR_ERROR, "You must be in a vehicle or close to it.");
                return 1;
            }
        }

        SetVehicleToRespawn(vehicleid);
        SendClientMessage(playerid, COLOR_SUCCESS, "The vehicle has been respawned.");
        return 1;
    }

    // /av lock
    if(!strcmp(option, "lock", true) || !strcmp(option, "zablokuj", true))
    {
        new vehicleid = GetPlayerVehicleID(playerid);

        if(vehicleid == 0)
        {
            vehicleid = AV_GetNearestVehicle(playerid, 5.0);

            if(vehicleid == INVALID_VEHICLE_ID)
            {
                SendClientMessage(playerid, COLOR_ERROR, "You must be in a vehicle or close to it.");
                return 1;
            }
        }

        Vehicle_SetLocked(vehicleid, true);
        SendClientMessage(playerid, COLOR_SUCCESS, "The vehicle has been locked.");
        return 1;
    }

    // /av unlock
    if(!strcmp(option, "unlock", true) || !strcmp(option, "odblokuj", true))
    {
        new vehicleid = GetPlayerVehicleID(playerid);

        if(vehicleid == 0)
        {
            vehicleid = AV_GetNearestVehicle(playerid, 5.0);

            if(vehicleid == INVALID_VEHICLE_ID)
            {
                SendClientMessage(playerid, COLOR_ERROR, "You must be in a vehicle or close to it.");
                return 1;
            }
        }

        Vehicle_SetLocked(vehicleid, false);
        SendClientMessage(playerid, COLOR_SUCCESS, "The vehicle has been unlocked.");
        return 1;
    }

    // /av info
    if(!strcmp(option, "info", true))
    {
        new vehicleid = GetPlayerVehicleID(playerid);

        if(vehicleid == 0)
        {
            vehicleid = AV_GetNearestVehicle(playerid, 5.0);

            if(vehicleid == INVALID_VEHICLE_ID)
            {
                SendClientMessage(playerid, COLOR_ERROR, "You must be in a vehicle or close to it.");
                return 1;
            }
        }

        new modelName[32], msg[128];
        GetVehicleModelName(GetVehicleModel(vehicleid), modelName);

        new slot = Vehicle_GetSlot(vehicleid);

        SendClientMessage(playerid, COLOR_YELLOW, "=== Vehicle Information ===");
        format(msg, sizeof(msg), "Model: %s (%d) | Vehicle ID: %d", modelName, GetVehicleModel(vehicleid), vehicleid);
        SendClientMessage(playerid, COLOR_WHITE, msg);

        if(slot != -1)
        {
            new ownerTypeName[16];
            switch(E_VEHICLE_OWNER_TYPE:vInfo[slot][veh_owner_type])
            {
                case OWNER_NONE: ownerTypeName = "None";
                case OWNER_PLAYER: ownerTypeName = "Player";
                case OWNER_GROUP: ownerTypeName = "Group";
                case OWNER_SERVER: ownerTypeName = "Server";
            }

            format(msg, sizeof(msg), "Slot: %d | Owner: %s (ID: %d)",
                slot, ownerTypeName, vInfo[slot][veh_owner]);
            SendClientMessage(playerid, COLOR_WHITE, msg);

            format(msg, sizeof(msg), "Fuel: %d%% | Mileage: %.1f km | HP: %.0f",
                vInfo[slot][veh_fuel], vInfo[slot][veh_mileage], vInfo[slot][veh_health]);
            SendClientMessage(playerid, COLOR_WHITE, msg);

            format(msg, sizeof(msg), "Colors: %d, %d | Locked: %s",
                vInfo[slot][veh_color1], vInfo[slot][veh_color2],
                vInfo[slot][veh_locked] ? "Yes" : "No");
            SendClientMessage(playerid, COLOR_WHITE, msg);
        }
        else
        {
            SendClientMessage(playerid, COLOR_GREY, "This is not a server vehicle (no data in the system).");
        }

        return 1;
    }

    SendClientMessage(playerid, COLOR_ERROR, "Unknown option. Use /av without parameters for help.");
    return 1;
}

/**
 * Searches for a vehicle by model name
 */
stock AV_GetModelByName(const name[])
{
    static const vehicleNames[][] = {
        "Landstalker", "Bravura", "Buffalo", "Linerunner", "Perennial", "Sentinel", "Dumper", "Firetruck",
        "Trashmaster", "Stretch", "Manana", "Infernus", "Voodoo", "Pony", "Mule", "Cheetah", "Ambulance",
        "Leviathan", "Moonbeam", "Esperanto", "Taxi", "Washington", "Bobcat", "Whoopee", "Injection",
        "Hunter", "Premier", "Enforcer", "Securicar", "Banshee", "Predator", "Bus", "Rhino", "Barracks",
        "Hotknife", "Trailer", "Previon", "Coach", "Cabbie", "Stallion", "Rumpo", "Bandit", "Romero",
        "Packer", "Monster", "Admiral", "Squalo", "Seasparrow", "Pizzaboy", "Tram", "Trailer", "Turismo",
        "Speeder", "Reefer", "Tropic", "Flatbed", "Yankee", "Caddy", "Solair", "Berkley", "Skimmer",
        "PCJ", "Faggio", "Freeway", "Baron", "Raider", "Glendale", "Oceanic", "Sanchez", "Sparrow",
        "Patriot", "Quad", "Coastguard", "Dinghy", "Hermes", "Sabre", "Rustler", "ZR350", "Walton", "Regina",
        "Comet", "BMX", "Burrito", "Camper", "Marquis", "Baggage", "Dozer", "Maverick", "Chopper",
        "Rancher", "FBI", "Virgo", "Greenwood", "Jetmax", "Hotring", "Sandking", "Blista",
        "Maverick", "Boxville", "Benson", "Mesa", "Goblin", "Hotring", "Hotring",
        "Bloodring", "Rancher", "SuperGT", "Elegant", "Journey", "Bike", "Mountain", "Beagle",
        "Cropduster", "Stuntplane", "Tanker", "Roadtrain", "Nebula", "Majestic", "Buccaneer", "Shamal",
        "Hydra", "FCR", "NRG", "HPV", "Cement", "Towtruck", "Fortune", "Cadrona", "FBITruck",
        "Willard", "Forklift", "Tractor", "Combine", "Feltzer", "Remington", "Slamvan", "Blade", "Freight",
        "Streak", "Vortex", "Vincent", "Bullet", "Clover", "Sadler", "Firetruck", "Hustler", "Intruder", "Primo",
        "Cargobob", "Tampa", "Sunrise", "Merit", "Utility", "Nevada", "Yosemite", "Windsor", "Monster",
        "Monster", "Uranus", "Jester", "Sultan", "Stratum", "Elegy", "Raindance", "Tiger", "Flash", "Tahoma",
        "Savanna", "Bandito", "Freight", "Trailer", "Kart", "Mower", "Dune", "Sweeper", "Broadway", "Tornado",
        "AT400", "DFT30", "Huntley", "Stafford", "BF400", "Newsvan", "Tug", "Trailer", "Emperor", "Wayfarer",
        "Euros", "Hotdog", "Club", "Trailer", "Trailer", "Andromada", "Dodo", "RCCam", "Launch", "Police",
        "Police", "Police", "Ranger", "Picador", "SWAT", "Alpha", "Phoenix", "Glendale", "Sadler",
        "Luggage", "Luggage", "Stairs", "Boxville", "Tiller", "Utility"
    };

    for(new i = 0; i < sizeof(vehicleNames); i++)
    {
        if(strfind(vehicleNames[i], name, true) != -1 || strfind(name, vehicleNames[i], true) != -1)
        {
            return 400 + i;
        }
    }

    return 0;
}

/**
 * Finds the nearest vehicle
 */
stock AV_GetNearestVehicle(playerid, Float:maxRange)
{
    new Float:px, Float:py, Float:pz;
    GetPlayerPos(playerid, px, py, pz);

    new Float:minDist = maxRange;
    new nearestVeh = INVALID_VEHICLE_ID;

    for(new i = 1; i < MAX_VEHICLES; i++)
    {
        if(!IsValidVehicle(i)) continue;

        new Float:vx, Float:vy, Float:vz;
        GetVehiclePos(i, vx, vy, vz);

        new Float:dist = GetDistanceBetweenPoints3D(px, py, pz, vx, vy, vz);

        if(dist < minDist)
        {
            minDist = dist;
            nearestVeh = i;
        }
    }

    return nearestVeh;
}

/**
 * Shows the tuning dialog
 */
stock AV_ShowTuningDialog(playerid)
{
    ShowPlayerDialog(playerid, DIALOG_AV_TUNING, DIALOG_STYLE_LIST,
        "Vehicle Tuning",
        "Nitro\nHydraulics\nWheels\nSpoiler\nExhaust\nHood\nRoof\nSide Skirts\nFront Bumper\nRear Bumper",
        "Select", "Cancel");
}

/**
 * Adds a tuning component to the vehicle
 */
stock AV_AddTuningComponent(vehicleid, category)
{
    // Basic components for each category
    switch(category)
    {
        case 0: // Nitro
            AddVehicleComponent(vehicleid, 1010); // Nitro x10
        case 1: // Hydraulics
            AddVehicleComponent(vehicleid, 1087); // Hydraulics
        case 2: // Wheels
            AddVehicleComponent(vehicleid, 1025); // Offroad wheels
        case 3: // Spoiler
            AddVehicleComponent(vehicleid, 1000); // Pro spoiler
        case 4: // Exhaust
            AddVehicleComponent(vehicleid, 1018); // Alien exhaust
        case 5: // Hood
            AddVehicleComponent(vehicleid, 1011); // Race scoop
        case 6: // Roof
            AddVehicleComponent(vehicleid, 1006); // Roof scoop
        case 7: // Side Skirts
            AddVehicleComponent(vehicleid, 1007); // Right alien sideskirt
        case 8: // Front Bumper
            AddVehicleComponent(vehicleid, 1003); // Alien front bumper
        case 9: // Rear Bumper
            AddVehicleComponent(vehicleid, 1002); // Alien rear bumper
    }
}

/**
 * Hook OnDialogResponse for /av
 */
hook OnDialogResponse@AV(playerid, dialogid, response, listitem, inputtext[])
{
    switch(dialogid)
    {
        case DIALOG_AV_TUNING:
        {
            if(!response) return Y_HOOKS_BREAK_RETURN_1;

            new vehicleid = g_AVTargetVehicle[playerid];

            if(vehicleid == 0 || !IsValidVehicle(vehicleid))
            {
                SendClientMessage(playerid, COLOR_ERROR, "The vehicle no longer exists.");
                return Y_HOOKS_BREAK_RETURN_1;
            }

            AV_AddTuningComponent(vehicleid, listitem);

            new categoryNames[][] = {
                "Nitro", "Hydraulics", "Wheels", "Spoiler", "Exhaust",
                "Hood", "Roof", "Side Skirts", "Front Bumper", "Rear Bumper"
            };

            new msg[64];
            format(msg, sizeof(msg), "Added component: %s", categoryNames[listitem]);
            SendClientMessage(playerid, COLOR_SUCCESS, msg);

            // Show dialog again
            AV_ShowTuningDialog(playerid);
            return Y_HOOKS_BREAK_RETURN_1;
        }
    }

    return 1;
}

// ===========================================================================
// COMMAND /ad - DOOR MANAGEMENT
// ===========================================================================

forward Admin_DoorsCmd(playerid, params[], help);

YCMD:adoors(playerid, params[], help)
{
    return Admin_DoorsCmd(playerid, params, help);
}

YCMD:ad(playerid, params[], help)
{
    return Admin_DoorsCmd(playerid, params, help);
}

public Admin_DoorsCmd(playerid, params[], help)
{
    ADMIN_CHECK(ADMIN_LEVEL_ADMIN);

    if(help)
    {
        SendClientMessage(playerid, COLOR_YELLOW, "Door management on the server.");
        return 1;
    }

    new subcmd[32], subparams[128];

    if(sscanf(params, "s[32]S()[128]", subcmd, subparams))
    {
        SendClientMessage(playerid, COLOR_LIGHTER_BLUE, "========== DOOR MANAGEMENT /ad ===========");
        SendClientMessage(playerid, COLOR_WHITE, "/ad create [name] - Creates new doors at your position");
        SendClientMessage(playerid, COLOR_WHITE, "/ad delete [ID] - Deletes doors");
        SendClientMessage(playerid, COLOR_WHITE, "/ad info [ID] - Information about the doors");
        SendClientMessage(playerid, COLOR_WHITE, "/ad list - List of all doors");
        SendClientMessage(playerid, COLOR_WHITE, "/ad name [ID] [new name] - Changes the door name");
        SendClientMessage(playerid, COLOR_WHITE, "/ad type [ID] [0-3] - Door type (0=pub, 1=priv, 2=group, 3=lock)");
        SendClientMessage(playerid, COLOR_WHITE, "/ad owner [ID] [player] - Sets the owner (player)");
        SendClientMessage(playerid, COLOR_WHITE, "/ad group [ID] [group ID] - Assigns to a group");
        SendClientMessage(playerid, COLOR_WHITE, "/ad close/open [ID] - Closes/opens the door");
        SendClientMessage(playerid, COLOR_WHITE, "/ad bank [ID] [amount] - Sets the door account balance");
        SendClientMessage(playerid, COLOR_WHITE, "/ad pickup [ID] [model] - Changes the pickup model");
        SendClientMessage(playerid, COLOR_WHITE, "/ad ext [ID] - Sets the external position to yours");
        SendClientMessage(playerid, COLOR_WHITE, "/ad int [ID] - Sets the internal position to yours");
        SendClientMessage(playerid, COLOR_WHITE, "/ad goto [ID] - Teleports to the door");
        SendClientMessage(playerid, COLOR_WHITE, "/ad nearest - Shows info about the nearest door");
        SendClientMessage(playerid, COLOR_LIGHTER_BLUE, "==============================================");
        return 1;
    }

    // /ad create [name]
    if(!strcmp(subcmd, "create", true) || !strcmp(subcmd, "stworz", true))
    {
        if(isnull(subparams))
        {
            return SendClientMessage(playerid, COLOR_GRAY, "Usage: /ad create [door name]");
        }

        // Find free door ID
        new doorId = -1;
        for(new i = 1; i < MAX_DOORS; i++)
        {
            if(!dInfo[i][door_created])
            {
                doorId = i;
                break;
            }
        }

        if(doorId == -1)
        {
            return SendClientMessage(playerid, COLOR_RED, "No free slots for doors!");
        }

        // Get player position
        new Float:x, Float:y, Float:z, Float:a;
        GetPlayerPos(playerid, x, y, z);
        GetPlayerFacingAngle(playerid, a);
        new interior = GetPlayerInterior(playerid);
        new vw = GetPlayerVirtualWorld(playerid);

        // Set basic data
        dInfo[doorId][door_created] = true;
        strcopy(dInfo[doorId][door_name], subparams, 64);
        dInfo[doorId][door_type] = _:DOOR_TYPE_PUBLIC;
        dInfo[doorId][door_owner_type] = _:DOOR_OWNER_SERVER;
        dInfo[doorId][door_owner] = 0;
        dInfo[doorId][door_locked] = false;
        dInfo[doorId][door_pickup_model] = 1318;
        dInfo[doorId][door_bank] = 0;

        // External position (current player position)
        dInfo[doorId][door_ext_x] = x;
        dInfo[doorId][door_ext_y] = y;
        dInfo[doorId][door_ext_z] = z;
        dInfo[doorId][door_ext_a] = a;
        dInfo[doorId][door_ext_interior] = interior;
        dInfo[doorId][door_ext_vw] = vw;

        // Internal position (default same as external)
        dInfo[doorId][door_int_x] = x;
        dInfo[doorId][door_int_y] = y;
        dInfo[doorId][door_int_z] = z;
        dInfo[doorId][door_int_a] = a;
        dInfo[doorId][door_int_interior] = 0;
        dInfo[doorId][door_int_vw] = doorId + 1000;

        // Default additional fields
        dInfo[doorId][door_objects_limit] = 0;
        dInfo[doorId][door_auto_closing] = false;
        dInfo[doorId][door_car_crosing] = false;
        dInfo[doorId][door_payment] = 0;
        dInfo[doorId][door_map_icon] = 0;
        dInfo[doorId][door_closed] = false;
        dInfo[doorId][door_rentable] = false;
        dInfo[doorId][door_rent] = 0;
        dInfo[doorId][door_surface] = 0.0;
        dInfo[doorId][door_time] = 0;
        dInfo[doorId][door_access] = 0;
        dInfo[doorId][door_destroyed] = 0;
        dInfo[doorId][door_burned] = 0;
        dInfo[doorId][door_meters] = 0;
        dInfo[doorId][door_audio][0] = 0;
        dInfo[doorId][door_area] = 0;
        dInfo[doorId][door_demolition] = 0;

        // Add to iterator
        Iter_Add(Doors, doorId);

        // Create visual elements
        Door_CreateVisuals(doorId);

        // Save to database
        new query[1024];
        new escapedName[129];
        mysql_escape_string(subparams, escapedName, mySQLconnection, sizeof(escapedName));

        mysql_format(mySQLconnection, query, sizeof(query),
            "INSERT INTO `doors` (`door_id`, `door_name`, `door_type`, `door_owner_type`, `door_owner`, `door_locked`, `door_pickup`, `door_bank`, \
             `door_ext_x`, `door_ext_y`, `door_ext_z`, `door_ext_a`, `door_ext_interior`, `door_ext_vw`, \
             `door_int_x`, `door_int_y`, `door_int_z`, `door_int_a`, `door_int_interior`, `door_int_vw`, \
             `door_objects_limit`, `door_auto_closing`, `door_car_crosing`, `door_payment`, `door_map_icon`, `door_closed`, `door_rentable`, `door_rent`, `door_surface`, `door_time`, `door_access`, `door_destroyed`, `door_burned`, `door_meters`, `door_audio`, `door_area`, `door_demolition`) \
             VALUES (%d, '%s', 0, 3, 0, 0, 1318, 0, %f, %f, %f, %f, %d, %d, %f, %f, %f, %f, %d, %d, %d, %d, %d, %d, %d, %d, %d, %f, %d, %d, %d, %d, %d, '%s', %d, %d)",
            doorId, escapedName, x, y, z, a, interior, vw, x, y, z, a, 0, doorId+1000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "", 0, 0);
        mysql_tquery(mySQLconnection, query);

        new msg[128];
        format(msg, sizeof(msg), "Created doors '%s' with ID: %d", subparams, doorId);
        SendClientMessage(playerid, COLOR_GREEN, msg);
        SendClientMessage(playerid, COLOR_YELLOW, "Set the internal position with the command /ad wewn %d", doorId);

        Admin_LogAction(playerid, msg);
        return 1;
    }

    // /ad delete [ID]
    if(!strcmp(subcmd, "delete", true) || !strcmp(subcmd, "usun", true))
    {
        new doorId;
        if(sscanf(subparams, "%d", doorId) != 1)
        {
            return SendClientMessage(playerid, COLOR_GRAY, "Usage: /ad delete [door ID]");
        }

        if(doorId <= 0 || doorId >= MAX_DOORS || !dInfo[doorId][door_created])
        {
            return SendClientMessage(playerid, COLOR_RED, "Doors with the given ID do not exist!");
        }

        new doorName[64];
        strcopy(doorName, dInfo[doorId][door_name], sizeof(doorName));

        // Remove visual elements
        Door_DestroyVisuals(doorId);

        // Remove from iterator
        Iter_Remove(Doors, doorId);
        dInfo[doorId][door_created] = false;

        // Delete from database
        new query[128];
        mysql_format(mySQLconnection, query, sizeof(query),
            "DELETE FROM `doors` WHERE `door_id` = %d", doorId);
        mysql_tquery(mySQLconnection, query);

        new msg[128];
        format(msg, sizeof(msg), "Deleted doors '%s' (ID: %d)", doorName, doorId);
        SendClientMessage(playerid, COLOR_GREEN, msg);

        Admin_LogAction(playerid, msg);
        return 1;
    }

    // /ad info [ID]
    if(!strcmp(subcmd, "info", true))
    {
        new doorId;
        if(sscanf(subparams, "%d", doorId) != 1)
        {
            return SendClientMessage(playerid, COLOR_GRAY, "Usage: /ad info [door ID]");
        }

        if(doorId <= 0 || doorId >= MAX_DOORS || !dInfo[doorId][door_created])
        {
            return SendClientMessage(playerid, COLOR_RED, "Doors with the given ID do not exist!");
        }

        new typeName[32], ownerTypeName[32];
        AD_GetDoorTypeName(dInfo[doorId][door_type], typeName);
        AD_GetOwnerTypeName(dInfo[doorId][door_owner_type], ownerTypeName);

        SendClientMessage(playerid, COLOR_LIGHTER_BLUE, "========== DOOR INFORMATION ==========");

        new msg[256];
        format(msg, sizeof(msg), "ID: %d | Name: %s", doorId, dInfo[doorId][door_name]);
        SendClientMessage(playerid, COLOR_WHITE, msg);

        format(msg, sizeof(msg), "Type: %s | Owner: %s (ID: %d) | Status: %s",
            typeName, ownerTypeName, dInfo[doorId][door_owner],
            dInfo[doorId][door_locked] ? "{FF0000}Locked" : "{00FF00}Open");
        SendClientMessage(playerid, COLOR_WHITE, msg);

        format(msg, sizeof(msg), "External: %.2f, %.2f, %.2f (INT: %d, VW: %d)",
            dInfo[doorId][door_ext_x], dInfo[doorId][door_ext_y], dInfo[doorId][door_ext_z],
            dInfo[doorId][door_ext_interior], dInfo[doorId][door_ext_vw]);
        SendClientMessage(playerid, COLOR_GRAY, msg);

        format(msg, sizeof(msg), "Internal: %.2f, %.2f, %.2f (INT: %d)",
            dInfo[doorId][door_int_x], dInfo[doorId][door_int_y], dInfo[doorId][door_int_z],
            dInfo[doorId][door_int_interior]);
        SendClientMessage(playerid, COLOR_GRAY, msg);

        SendClientMessage(playerid, COLOR_LIGHTER_BLUE, "============================================");
        return 1;
    }

    // /ad list
    if(!strcmp(subcmd, "list", true) || !strcmp(subcmd, "lista", true))
    {
        SendClientMessage(playerid, COLOR_LIGHTER_BLUE, "========== DOORS LIST ==========");

        new count = 0;
        foreach(new i : Doors)
        {
            new typeName[32];
            AD_GetDoorTypeName(dInfo[i][door_type], typeName);

            new msg[256];
            format(msg, sizeof(msg), "[%d] %s - Type: %s, Status: %s",
                i, dInfo[i][door_name], typeName,
                dInfo[i][door_locked] ? "{FF0000}Locked" : "{00FF00}Open");
            SendClientMessage(playerid, COLOR_WHITE, msg);
            count++;

            if(count >= 20)
            {
                SendClientMessage(playerid, COLOR_GRAY, "... displayed the first 20 doors");
                break;
            }
        }

        if(count == 0)
        {
            SendClientMessage(playerid, COLOR_GRAY, "No doors on the server.");
        }

        new msg[64];
        format(msg, sizeof(msg), "Total doors: %d", Iter_Count(Doors));
        SendClientMessage(playerid, COLOR_GREEN, msg);
        SendClientMessage(playerid, COLOR_LIGHTER_BLUE, "==================================");
        return 1;
    }

    // /ad name [ID] [new name]
    if(!strcmp(subcmd, "name", true) || !strcmp(subcmd, "nazwa", true))
    {
        new doorId, newName[64];
        if(sscanf(subparams, "ds[64]", doorId, newName))
        {
            return SendClientMessage(playerid, COLOR_GRAY, "Usage: /ad name [door ID] [new name]");
        }

        if(doorId <= 0 || doorId >= MAX_DOORS || !dInfo[doorId][door_created])
        {
            return SendClientMessage(playerid, COLOR_RED, "Doors with the given ID do not exist!");
        }

        strcopy(dInfo[doorId][door_name], newName, 64);
        Door_UpdateLabels(doorId);
        Door_Save(doorId);

        new msg[128];
        format(msg, sizeof(msg), "Changed the name of doors %d to: %s", doorId, newName);
        SendClientMessage(playerid, COLOR_GREEN, msg);
        Admin_LogAction(playerid, msg);
        return 1;
    }

    // /ad type [ID] [0-3]
    if(!strcmp(subcmd, "type", true) || !strcmp(subcmd, "typ", true))
    {
        new doorId, doorType;
        if(sscanf(subparams, "%d %d", doorId, doorType) != 2)
        {
            SendClientMessage(playerid, COLOR_GRAY, "Usage: /ad type [door ID] [type 0-3]");
            SendClientMessage(playerid, COLOR_GRAY, "Types: 0=Public, 1=Private, 2=Group, 3=Locked");
            return 1;
        }

        if(doorId <= 0 || doorId >= MAX_DOORS || !dInfo[doorId][door_created])
        {
            return SendClientMessage(playerid, COLOR_RED, "Doors with the given ID do not exist!");
        }

        if(doorType < 0 || doorType > 3)
        {
            return SendClientMessage(playerid, COLOR_RED, "Invalid type! (0-3)");
        }

        dInfo[doorId][door_type] = doorType;
        Door_Save(doorId);

        new typeName[32];
        AD_GetDoorTypeName(doorType, typeName);

        new msg[128];
        format(msg, sizeof(msg), "Changed the type of doors %d to: %s", doorId, typeName);
        SendClientMessage(playerid, COLOR_GREEN, msg);
        Admin_LogAction(playerid, msg);
        return 1;
    }

    // /ad owner [ID] [player]
    if(!strcmp(subcmd, "owner", true) || !strcmp(subcmd, "wlasciciel", true))
    {
        new doorId, targetid;
        if(sscanf(subparams, "%d %d", doorId, targetid) != 2)
        {
            return SendClientMessage(playerid, COLOR_GRAY, "Usage: /ad owner [door ID] [ID/Nick player]");
        }

        if(doorId <= 0 || doorId >= MAX_DOORS || !dInfo[doorId][door_created])
        {
            return SendClientMessage(playerid, COLOR_RED, "Doors with the given ID do not exist!");
        }

        if(!IsPlayerConnected(targetid) || !pInfo[targetid][player_logged])
        {
            return SendClientMessage(playerid, COLOR_RED, "Player is not logged in.");
        }

        dInfo[doorId][door_owner_type] = _:DOOR_OWNER_PLAYER;
        dInfo[doorId][door_owner] = pInfo[targetid][player_id];
        Door_Save(doorId);

        new msg[128];
        format(msg, sizeof(msg), "Set player %s as the owner of doors %s (ID: %d)",
            pInfo[targetid][player_name], dInfo[doorId][door_name], doorId);
        SendClientMessage(playerid, COLOR_GREEN, msg);

        format(msg, sizeof(msg), "You have been set as the owner of doors: %s!", dInfo[doorId][door_name]);
        SendClientMessage(targetid, COLOR_YELLOW, msg);

        Admin_LogAction(playerid, msg);
        return 1;
    }

    // /ad group [ID] [ID grupy]
    if(!strcmp(subcmd, "group", true) || !strcmp(subcmd, "grupa", true))
    {
        new doorId, groupId;
        if(sscanf(subparams, "%d %d", doorId, groupId) != 2)
        {
            return SendClientMessage(playerid, COLOR_GRAY, "Usage: /ad group [door ID] [group ID]");
        }

        if(doorId <= 0 || doorId >= MAX_DOORS || !dInfo[doorId][door_created])
        {
            return SendClientMessage(playerid, COLOR_RED, "Doors with the given ID do not exist!");
        }

        if(!Group_Exists(groupId))
        {
            return SendClientMessage(playerid, COLOR_RED, "Group with the given ID does not exist!");
        }

        dInfo[doorId][door_owner_type] = _:DOOR_OWNER_GROUP;
        dInfo[doorId][door_owner] = groupId;
        dInfo[doorId][door_type] = _:DOOR_TYPE_GROUP;
        Door_Save(doorId);

        new msg[128];
        format(msg, sizeof(msg), "Assigned doors %s (ID: %d) to group %s (ID: %d)",
            dInfo[doorId][door_name], doorId, gInfo[groupId][group_name], groupId);
        SendClientMessage(playerid, COLOR_GREEN, msg);

        Admin_LogAction(playerid, msg);
        return 1;
    }

    // /ad close [ID]
    if(!strcmp(subcmd, "close", true) || !strcmp(subcmd, "zamknij", true))
    {
        new doorId;
        if(sscanf(subparams, "%d", doorId) != 1)
        {
            return SendClientMessage(playerid, COLOR_GRAY, "Usage: /ad close [ID drzwi]");
        }

        if(doorId <= 0 || doorId >= MAX_DOORS || !dInfo[doorId][door_created])
        {
            return SendClientMessage(playerid, COLOR_RED, "Drzwi o podanym ID nie istnieją!");
        }

        dInfo[doorId][door_locked] = true;
        Door_UpdateLabels(doorId);
        Door_Save(doorId);

        new msg[128];
        format(msg, sizeof(msg), "Drzwi %s (ID: %d) zostały zamknięte",
            dInfo[doorId][door_name], doorId);
        SendClientMessage(playerid, COLOR_GREEN, msg);

        Admin_LogAction(playerid, msg);
        return 1;
    }

    // /ad open [ID]
    if(!strcmp(subcmd, "open", true) || !strcmp(subcmd, "otworz", true))
    {
        new doorId;
        if(sscanf(subparams, "%d", doorId) != 1)
        {
            return SendClientMessage(playerid, COLOR_GRAY, "Usage: /ad open [ID drzwi]");
        }

        if(doorId <= 0 || doorId >= MAX_DOORS || !dInfo[doorId][door_created])
        {
            return SendClientMessage(playerid, COLOR_RED, "Drzwi o podanym ID nie istnieją!");
        }

        dInfo[doorId][door_locked] = false;
        Door_UpdateLabels(doorId);
        Door_Save(doorId);

        new msg[128];
        format(msg, sizeof(msg), "Drzwi %s (ID: %d) zostały otwarte",
            dInfo[doorId][door_name], doorId);
        SendClientMessage(playerid, COLOR_GREEN, msg);

        Admin_LogAction(playerid, msg);
        return 1;
    }

    // /ad bank [ID] [kwota]
    if(!strcmp(subcmd, "bank", true))
    {
        new doorId, amount;
        if(sscanf(subparams, "dd", doorId, amount))
        {
            return SendClientMessage(playerid, COLOR_GRAY, "Usage: /ad bank [door ID] [amount]");
        }

        if(doorId <= 0 || doorId >= MAX_DOORS || !dInfo[doorId][door_created])
        {
            return SendClientMessage(playerid, COLOR_RED, "Drzwi o podanym ID nie istnieją!");
        }

        dInfo[doorId][door_bank] = amount;
        Door_Save(doorId);

        new msg[128];
        format(msg, sizeof(msg), "Set the bank of doors %d to: %s$", doorId, FormatMoney(amount));
        SendClientMessage(playerid, COLOR_GREEN, msg);
        Admin_LogAction(playerid, msg);
        return 1;
    }

    // /ad pickup [ID] [model]
    if(!strcmp(subcmd, "pickup", true))
    {
        new doorId, model;
        if(sscanf(subparams, "dd", doorId, model))
        {
            return SendClientMessage(playerid, COLOR_GRAY, "Usage: /ad pickup [door ID] [pickup model]");
        }

        if(doorId <= 0 || doorId >= MAX_DOORS || !dInfo[doorId][door_created])
        {
            return SendClientMessage(playerid, COLOR_RED, "Drzwi o podanym ID nie istnieją!");
        }

        dInfo[doorId][door_pickup_model] = model;

        // Refresh visual elements
        Door_DestroyVisuals(doorId);
        Door_CreateVisuals(doorId);
        Door_Save(doorId);

        new msg[128];
        format(msg, sizeof(msg), "Changed the pickup model of doors %d to: %d", doorId, model);
        SendClientMessage(playerid, COLOR_GREEN, msg);
        Admin_LogAction(playerid, msg);
        return 1;
    }

    // /ad ext [ID]
    if(!strcmp(subcmd, "ext", true) || !strcmp(subcmd, "zewn", true) || !strcmp(subcmd, "external", true))
    {
        new doorId;
        if(sscanf(subparams, "%d", doorId) != 1)
        {
            return SendClientMessage(playerid, COLOR_GRAY, "Usage: /ad ext [door ID]");
        }

        if(doorId <= 0 || doorId >= MAX_DOORS || !dInfo[doorId][door_created])
        {
            return SendClientMessage(playerid, COLOR_RED, "Drzwi o podanym ID nie istnieją!");
        }

        new Float:x, Float:y, Float:z, Float:a;
        GetPlayerPos(playerid, x, y, z);
        GetPlayerFacingAngle(playerid, a);
        new interior = GetPlayerInterior(playerid);
        new vw = GetPlayerVirtualWorld(playerid);

        dInfo[doorId][door_ext_x] = x;
        dInfo[doorId][door_ext_y] = y;
        dInfo[doorId][door_ext_z] = z;
        dInfo[doorId][door_ext_a] = a;
        dInfo[doorId][door_ext_interior] = interior;
        dInfo[doorId][door_ext_vw] = vw;

        // Refresh visual elements
        Door_DestroyVisuals(doorId);
        Door_CreateVisuals(doorId);
        Door_Save(doorId);

        new msg[128];
        format(msg, sizeof(msg), "Set the external position of doors %d to your position", doorId);
        SendClientMessage(playerid, COLOR_GREEN, msg);
        Admin_LogAction(playerid, msg);
        return 1;
    }

    // /ad int [ID]
    if(!strcmp(subcmd, "int", true) || !strcmp(subcmd, "wewn", true) || !strcmp(subcmd, "internal", true))
    {
        new doorId;
        if(sscanf(subparams, "%d", doorId) != 1)
        {
            return SendClientMessage(playerid, COLOR_GRAY, "Usage: /ad int [door ID]");
        }

        if(doorId <= 0 || doorId >= MAX_DOORS || !dInfo[doorId][door_created])
        {
            return SendClientMessage(playerid, COLOR_RED, "Drzwi o podanym ID nie istnieją!");
        }

        new Float:x, Float:y, Float:z, Float:a;
        GetPlayerPos(playerid, x, y, z);
        GetPlayerFacingAngle(playerid, a);
        new interior = GetPlayerInterior(playerid);

        dInfo[doorId][door_int_x] = x;
        dInfo[doorId][door_int_y] = y;
        dInfo[doorId][door_int_z] = z;
        dInfo[doorId][door_int_a] = a;
        dInfo[doorId][door_int_interior] = interior;

        // Refresh visual elements
        Door_DestroyVisuals(doorId);
        Door_CreateVisuals(doorId);
        Door_Save(doorId);

        new msg[128];
        format(msg, sizeof(msg), "Set the internal position of doors %d to your position", doorId);
        SendClientMessage(playerid, COLOR_GREEN, msg);
        Admin_LogAction(playerid, msg);
        return 1;
    }

    // /ad goto [ID]
    if(!strcmp(subcmd, "tp", true) || !strcmp(subcmd, "goto", true))
    {
        new doorId;
        if(sscanf(subparams, "%d", doorId) != 1)
        {
            return SendClientMessage(playerid, COLOR_GRAY, "Usage: /ad goto [ID drzwi]");
        }

        if(doorId <= 0 || doorId >= MAX_DOORS || !dInfo[doorId][door_created])
        {
            return SendClientMessage(playerid, COLOR_RED, "Drzwi o podanym ID nie istnieją!");
        }

        SetPlayerPos(playerid, dInfo[doorId][door_ext_x], dInfo[doorId][door_ext_y], dInfo[doorId][door_ext_z]);
        SetPlayerFacingAngle(playerid, dInfo[doorId][door_ext_a]);
        SetPlayerInterior(playerid, dInfo[doorId][door_ext_interior]);
        SetPlayerVirtualWorld(playerid, dInfo[doorId][door_ext_vw]);

        new msg[128];
        format(msg, sizeof(msg), "Teleported to doors: %s (ID: %d)", dInfo[doorId][door_name], doorId);
        SendClientMessage(playerid, COLOR_GREEN, msg);
        return 1;
    }

    // /ad nearest
    if(!strcmp(subcmd, "nearest", true) || !strcmp(subcmd, "najblizsze", true) || !strcmp(subcmd, "near", true))
    {
        new doorId = Door_FindNearestExternal(playerid);

        if(doorId == -1)
        {
            return SendClientMessage(playerid, COLOR_RED, "No doors found nearby!");
        }

        // Show info about these doors
        new typeName[32], ownerTypeName[32];
        AD_GetDoorTypeName(dInfo[doorId][door_type], typeName);
        AD_GetOwnerTypeName(dInfo[doorId][door_owner_type], ownerTypeName);

        SendClientMessage(playerid, COLOR_LIGHTER_BLUE, "========== NEAREST DOORS ==========");

        new msg[256];
        format(msg, sizeof(msg), "ID: %d | Name: %s", doorId, dInfo[doorId][door_name]);
        SendClientMessage(playerid, COLOR_WHITE, msg);

        format(msg, sizeof(msg), "Type: %s | Owner: %s (ID: %d) | Status: %s",
            typeName, ownerTypeName, dInfo[doorId][door_owner],
            dInfo[doorId][door_locked] ? "{FF0000}Locked" : "{00FF00}Open");
        SendClientMessage(playerid, COLOR_WHITE, msg);

        SendClientMessage(playerid, COLOR_LIGHTER_BLUE, "=======================================");
        return 1;
    }

    // Unknown subcommand
    SendClientMessage(playerid, COLOR_RED, "Unknown subcommand. Type /ad to see the list.");
    return 1;
}

/**
 * Gets the name of the door type
 */
stock AD_GetDoorTypeName(doorType, output[], size = 32)
{
    switch(doorType)
    {
        case 0: strcopy(output, "Public", size);
        case 1: strcopy(output, "Private", size);
        case 2: strcopy(output, "Group", size);
        case 3: strcopy(output, "Locked", size);
        default: strcopy(output, "Unknown", size);
    }
}

/**
 * Gets the name of the owner type
 */
stock AD_GetOwnerTypeName(ownerType, output[], size = 32)
{
    switch(ownerType)
    {
        case 0: strcopy(output, "None", size);
        case 1: strcopy(output, "Player", size);
        case 2: strcopy(output, "Group", size);
        case 3: strcopy(output, "Server", size);
        default: strcopy(output, "Unknown", size);
    }
}

// ===========================================================================
// EOF
// ===========================================================================


