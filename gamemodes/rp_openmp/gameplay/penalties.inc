/*
 * =============================================================================
 *  PENALTIES - System kar i wi??zienia
 * =============================================================================
 *  
 *  Modu??: gameplay/penalties.inc
 *  Opis: System kar (areszt, wi??zienie, mandaty, punkty karne)
 *
 *  ZMIANY W REFAKTORZE:
 *  - Przechowywanie kar w MySQL
 *  - Timer-based processing
 *  - Historia kar gracza
 *
 * =============================================================================
 */

#if defined _penalties_included
    #endinput
#endif
#define _penalties_included

// ===========================================================================
// TYPY KAR
// ===========================================================================

enum E_PENALTY_TYPE
{
    PENALTY_NONE = 0,
    PENALTY_WARN,           // Ostrze??enie
    PENALTY_MUTE,           // Mute czatu
    PENALTY_JAIL,           // Wi??zienie IC
    PENALTY_AJ,             // Admin Jail
    PENALTY_BAN,            // Ban
    PENALTY_KICK,           // Kick
    PENALTY_TICKET,         // Mandat
    PENALTY_PENALTY_POINTS, // Punkty karne
    E_PENALTY_MAX
};

// ===========================================================================
// LOKALIZACJE WI??ZIENIA
// ===========================================================================

enum e_jail_location
{
    Float:jail_x,
    Float:jail_y,
    Float:jail_z,
    Float:jail_angle,
    jail_interior,
    jail_world,
    jail_name[32]
};

static g_JailLocations[][e_jail_location] = {
    // LSPD
    {264.1543, 77.3751, 1001.0391, 270.0, 6, 0, "LSPD"},
    {263.8326, 81.9644, 1001.0391, 270.0, 6, 0, "LSPD"},
    {263.8325, 85.4221, 1001.0391, 270.0, 6, 0, "LSPD"},
    
    // SFPD
    {215.7969, 110.0234, 999.0156, 0.0, 10, 0, "SFPD"},
    {219.8281, 110.0234, 999.0156, 0.0, 10, 0, "SFPD"},
    
    // LVPD
    {198.0938, 158.8047, 1003.0234, 180.0, 3, 0, "LVPD"},
    {193.7344, 158.8047, 1003.0234, 180.0, 3, 0, "LVPD"},
    
    // Wi??zienie stanowe
    {-2653.6187, 1321.4988, 13.1489, 180.0, 0, 0, "Wiezienie Stanowe"}
};

// ===========================================================================
// HISTORIA KAR
// ===========================================================================

enum e_penalty_record
{
    penalty_id,
    E_PENALTY_TYPE:penalty_type,
    penalty_player_id,
    penalty_admin_id,
    penalty_reason[128],
    penalty_duration,
    penalty_timestamp,
    penalty_expired
};

// ===========================================================================
// ZMIENNE
// ===========================================================================

static g_PlayerPenaltyPoints[MAX_PLAYERS];
static g_PlayerJailTime[MAX_PLAYERS];
static g_PlayerMuteTime[MAX_PLAYERS];
static g_PlayerAJTime[MAX_PLAYERS];

// ===========================================================================
// FUNKCJE G????WNE
// ===========================================================================

/**
 * Daje kar?? graczowi
 * @param playerid ID gracza
 * @param adminid ID admina (lub INVALID_PLAYER_ID dla systemu)
 * @param penaltyType Typ kary
 * @param duration Czas trwania (sekundy lub warto????)
 * @param reason Pow??d
 */
stock Penalty_Give(playerid, adminid, E_PENALTY_TYPE:penaltyType, duration, const reason[])
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }
    
    new adminName[MAX_PLAYER_NAME] = "System";
    if(IsValidPlayerId(adminid) && pInfo[adminid][player_logged])
    {
        GetPlayerName(adminid, adminName, sizeof(adminName));
    }
    
    switch(penaltyType)
    {
        case PENALTY_WARN:
        {
            return Penalty_Warn(playerid, adminid, reason);
        }
        
        case PENALTY_MUTE:
        {
            return Penalty_Mute(playerid, adminid, duration, reason);
        }
        
        case PENALTY_JAIL:
        {
            return Penalty_Jail(playerid, adminid, duration, reason);
        }
        
        case PENALTY_AJ:
        {
            return Penalty_AdminJail(playerid, adminid, duration, reason);
        }
        
        case PENALTY_BAN:
        {
            return Penalty_Ban(playerid, adminid, duration, reason);
        }
        
        case PENALTY_KICK:
        {
            return Penalty_Kick(playerid, adminid, reason);
        }
        
        case PENALTY_TICKET:
        {
            return Penalty_Ticket(playerid, adminid, duration, reason);
        }
        
        case PENALTY_PENALTY_POINTS:
        {
            return Penalty_AddPoints(playerid, adminid, duration, reason);
        }
    }
    
    return 0;
}

// ===========================================================================
// OSTRZE??ENIE
// ===========================================================================

/**
 * Daje ostrze??enie graczowi
 */
stock Penalty_Warn(playerid, adminid, const reason[])
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }
    
    pInfo[playerid][player_warns]++;
    
    new adminName[MAX_PLAYER_NAME] = "System";
    if(IsValidPlayerId(adminid))
    {
        GetPlayerName(adminid, adminName, sizeof(adminName));
    }
    
    // Zapisz do bazy
    Penalty_Log(playerid, adminid, PENALTY_WARN, 0, reason);
    
    // Powiadom
    new msg[256];
    format(msg, sizeof(msg), 
        "[WARN] Administrator %s da?? ostrze??enie dla %s (%d/3). Pow??d: %s",
        adminName, Player_RPName(playerid), pInfo[playerid][player_warns], reason);
    SendClientMessageToAll(COLOR_ADMIN, msg);
    
    // Sprawd?? liczb?? warn??w
    if(pInfo[playerid][player_warns] >= 3)
    {
        Penalty_Ban(playerid, adminid, 3600, "3 ostrze??enia (automatyczny ban 1h)");
    }
    
    return 1;
}

// ===========================================================================
// MUTE
// ===========================================================================

/**
 * Wycisza gracza
 * @param duration Czas w sekundach
 */
stock Penalty_Mute(playerid, adminid, duration, const reason[])
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }
    
    g_PlayerMuteTime[playerid] = duration;
    pInfo[playerid][player_muted] = 1;
    
    new adminName[MAX_PLAYER_NAME] = "System";
    if(IsValidPlayerId(adminid))
    {
        GetPlayerName(adminid, adminName, sizeof(adminName));
    }
    
    Penalty_Log(playerid, adminid, PENALTY_MUTE, duration, reason);
    
    new msg[256];
    format(msg, sizeof(msg), 
        "[MUTE] %s wyciszy?? %s na %s. Pow??d: %s",
        adminName, Player_RPName(playerid), FormatDuration(duration), reason);
    SendClientMessageToAll(COLOR_ADMIN, msg);
    
    return 1;
}

/**
 * Zdejmuje mute
 */
stock Penalty_Unmute(playerid)
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }
    
    g_PlayerMuteTime[playerid] = 0;
    pInfo[playerid][player_muted] = 0;
    
    SendClientMessage(playerid, COLOR_SUCCESS, "Twoje wyciszenie zosta??o zdj??te.");
    
    return 1;
}

/**
 * Sprawdza czy gracz jest wyciszony
 */
stock bool:Penalty_IsMuted(playerid)
{
    if(!IsValidPlayerId(playerid))
    {
        return false;
    }
    
    return g_PlayerMuteTime[playerid] > 0 || pInfo[playerid][player_muted] != 0;
}

// ===========================================================================
// WI??ZIENIE IC
// ===========================================================================

/**
 * Wsadza gracza do wi??zienia (IC)
 * @param duration Czas w sekundach
 */
stock Penalty_Jail(playerid, adminid, duration, const reason[])
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }
    
    g_PlayerJailTime[playerid] = duration;
    pInfo[playerid][player_jail] = duration;
    
    // Wybierz losow?? cel??
    new cell = random(sizeof(g_JailLocations));
    
    SetPlayerPos(playerid, 
        g_JailLocations[cell][jail_x],
        g_JailLocations[cell][jail_y],
        g_JailLocations[cell][jail_z]);
    SetPlayerFacingAngle(playerid, g_JailLocations[cell][jail_angle]);
    SetPlayerInterior(playerid, g_JailLocations[cell][jail_interior]);
    SetPlayerVirtualWorld(playerid, g_JailLocations[cell][jail_world]);
    
    // Zabierz bro??
    ResetPlayerWeapons(playerid);
    
    new adminName[MAX_PLAYER_NAME] = "System";
    if(IsValidPlayerId(adminid))
    {
        GetPlayerName(adminid, adminName, sizeof(adminName));
    }
    
    Penalty_Log(playerid, adminid, PENALTY_JAIL, duration, reason);
    
    new msg[256];
    format(msg, sizeof(msg), 
        "Zosta??e?? aresztowany przez %s na %s. Pow??d: %s",
        adminName, FormatDuration(duration), reason);
    SendClientMessage(playerid, COLOR_JAIL, msg);
    
    return 1;
}

/**
 * Wypuszcza gracza z wi??zienia
 */
stock Penalty_Unjail(playerid)
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }
    
    g_PlayerJailTime[playerid] = 0;
    pInfo[playerid][player_jail] = 0;
    
    // Teleportuj przed komend??
    SetPlayerPos(playerid, 1545.2649, -1675.4849, 13.5625);
    SetPlayerInterior(playerid, 0);
    SetPlayerVirtualWorld(playerid, 0);
    
    SendClientMessage(playerid, COLOR_SUCCESS, "Zosta??e?? wypuszczony z wi??zienia.");
    
    return 1;
}

/**
 * Sprawdza czy gracz jest w wi??zieniu
 */
stock bool:Penalty_IsJailed(playerid)
{
    if(!IsValidPlayerId(playerid))
    {
        return false;
    }
    
    return g_PlayerJailTime[playerid] > 0 || pInfo[playerid][player_jail] > 0;
}

// ===========================================================================
// ADMIN JAIL
// ===========================================================================

/**
 * Wsadza gracza do Admin Jail
 */
stock Penalty_AdminJail(playerid, adminid, duration, const reason[])
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }
    
    g_PlayerAJTime[playerid] = duration;
    pInfo[playerid][player_aj] = duration;
    
    // Teleportuj do AJ
    SetPlayerPos(playerid, 198.7645, 161.2795, 1003.0313);
    SetPlayerInterior(playerid, 3);
    SetPlayerVirtualWorld(playerid, playerid + 1000);
    
    TogglePlayerControllable(playerid, 0);
    
    new adminName[MAX_PLAYER_NAME] = "System";
    if(IsValidPlayerId(adminid))
    {
        GetPlayerName(adminid, adminName, sizeof(adminName));
    }
    
    Penalty_Log(playerid, adminid, PENALTY_AJ, duration, reason);
    
    new msg[256];
    format(msg, sizeof(msg), 
        "[AJ] %s da?? AdminJail dla %s na %s. Pow??d: %s",
        adminName, Player_RPName(playerid), FormatDuration(duration), reason);
    SendClientMessageToAll(COLOR_ADMIN, msg);
    
    return 1;
}

/**
 * Wypuszcza gracza z Admin Jail
 */
stock Penalty_UnAdminJail(playerid)
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }
    
    g_PlayerAJTime[playerid] = 0;
    pInfo[playerid][player_aj] = 0;
    
    TogglePlayerControllable(playerid, 1);
    
    // Teleportuj do spawnu
    SetPlayerPos(playerid, 1481.0, -1764.0, 18.7);
    SetPlayerInterior(playerid, 0);
    SetPlayerVirtualWorld(playerid, 0);
    
    SendClientMessage(playerid, COLOR_SUCCESS, "Zosta??e?? wypuszczony z AdminJail.");
    
    return 1;
}

// ===========================================================================
// BAN
// ===========================================================================

/**
 * Banuje gracza
 * @param duration Czas w sekundach (0 = permanentny)
 */
stock Penalty_Ban(playerid, adminid, duration, const reason[])
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }
    
    new adminName[MAX_PLAYER_NAME] = "System";
    if(IsValidPlayerId(adminid))
    {
        GetPlayerName(adminid, adminName, sizeof(adminName));
    }
    
    // Zapisz ban w bazie
    new query[512];
    new ip[16];
    GetPlayerIp(playerid, ip, sizeof(ip));
    
    mysql_format(g_MySQL, query, sizeof(query),
        "INSERT INTO `bans` (`player_id`, `player_name`, `player_ip`, `admin_id`, `admin_name`, `reason`, `duration`, `timestamp`, `expire_time`) \
        VALUES (%d, '%e', '%e', %d, '%e', '%e', %d, %d, %d)",
        pInfo[playerid][player_uid],
        Player_RPName(playerid),
        ip,
        IsValidPlayerId(adminid) ? pInfo[adminid][player_uid] : 0,
        adminName,
        reason,
        duration,
        gettime(),
        duration > 0 ? gettime() + duration : 0);
    
    mysql_query(g_MySQL, query);
    
    Penalty_Log(playerid, adminid, PENALTY_BAN, duration, reason);
    
    // Powiadom
    new msg[256];
    if(duration > 0)
    {
        format(msg, sizeof(msg), 
            "[BAN] %s zbanowa?? %s na %s. Pow??d: %s",
            adminName, Player_RPName(playerid), FormatDuration(duration), reason);
    }
    else
    {
        format(msg, sizeof(msg), 
            "[BAN] %s permanentnie zbanowa?? %s. Pow??d: %s",
            adminName, Player_RPName(playerid), reason);
    }
    SendClientMessageToAll(COLOR_ADMIN, msg);
    
    // Wyrzu?? gracza
    format(msg, sizeof(msg), "Zosta??e?? zbanowany. Pow??d: %s", reason);
    SendClientMessage(playerid, COLOR_ERROR, msg);
    
    SetTimerEx("DelayedKick", 500, false, "i", playerid);
    
    return 1;
}

// ===========================================================================
// KICK
// ===========================================================================

/**
 * Wyrzuca gracza z serwera
 */
stock Penalty_Kick(playerid, adminid, const reason[])
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }
    
    new adminName[MAX_PLAYER_NAME] = "System";
    if(IsValidPlayerId(adminid))
    {
        GetPlayerName(adminid, adminName, sizeof(adminName));
    }
    
    Penalty_Log(playerid, adminid, PENALTY_KICK, 0, reason);
    
    new msg[256];
    format(msg, sizeof(msg), 
        "[KICK] %s wyrzuci?? %s. Pow??d: %s",
        adminName, Player_RPName(playerid), reason);
    SendClientMessageToAll(COLOR_ADMIN, msg);
    
    format(msg, sizeof(msg), "Zosta??e?? wyrzucony. Pow??d: %s", reason);
    SendClientMessage(playerid, COLOR_ERROR, msg);
    
    SetTimerEx("DelayedKick", 500, false, "i", playerid);
    
    return 1;
}

// DelayedKick jest zdefiniowany w player_auth.inc

// ===========================================================================
// MANDATY
// ===========================================================================

/**
 * Daje mandat graczowi
 * @param amount Kwota mandatu
 */
stock Penalty_Ticket(playerid, adminid, amount, const reason[])
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }
    
    // Sprawd?? czy gracz ma pieni??dze
    if(Player_GetMoney(playerid) < amount)
    {
        // Zamie?? na punkty karne
        Penalty_AddPoints(playerid, adminid, 5, "Nieop??acony mandat");
        SendClientMessage(playerid, COLOR_WARNING, "Nie sta?? ci?? na mandat - otrzymujesz punkty karne.");
    }
    else
    {
        Player_GiveMoney(playerid, -amount);
    }
    
    new adminName[MAX_PLAYER_NAME] = "System";
    if(IsValidPlayerId(adminid))
    {
        GetPlayerName(adminid, adminName, sizeof(adminName));
    }
    
    Penalty_Log(playerid, adminid, PENALTY_TICKET, amount, reason);
    
    new msg[256];
    format(msg, sizeof(msg), 
        "Otrzyma??e?? mandat od %s na kwot?? %s$. Pow??d: %s",
        adminName, FormatMoney(amount), reason);
    SendClientMessage(playerid, COLOR_TICKET, msg);
    
    // Wiadomo???? RP
    format(msg, sizeof(msg), "* %s wystawia mandat dla %s.",
        Player_RPName(adminid), Player_RPName(playerid));
    SendLocalMessage(playerid, 15.0, COLOR_ME, msg);
    
    return 1;
}

// ===========================================================================
// PUNKTY KARNE
// ===========================================================================

/**
 * Dodaje punkty karne graczowi
 */
stock Penalty_AddPoints(playerid, adminid, points, const reason[])
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }
    
    g_PlayerPenaltyPoints[playerid] += points;
    pInfo[playerid][player_penalty_points] += points;
    
    Penalty_Log(playerid, adminid, PENALTY_PENALTY_POINTS, points, reason);
    
    new msg[256];
    format(msg, sizeof(msg), 
        "Otrzyma??e?? %d punkt??w karnych. ????cznie: %d/24. Pow??d: %s",
        points, pInfo[playerid][player_penalty_points], reason);
    SendClientMessage(playerid, COLOR_WARNING, msg);
    
    // Sprawd?? czy gracz nie ma za du??o punkt??w
    if(pInfo[playerid][player_penalty_points] >= 24)
    {
        // Zatrzymaj prawo jazdy
        pInfo[playerid][player_driving_license] = 0;
        pInfo[playerid][player_penalty_points] = 0;
        g_PlayerPenaltyPoints[playerid] = 0;
        
        SendClientMessage(playerid, COLOR_ERROR, "Utraci??e?? prawo jazdy z powodu nadmiernej ilo??ci punkt??w karnych!");
    }
    
    return 1;
}

/**
 * Usuwa punkty karne
 */
stock Penalty_RemovePoints(playerid, points)
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }
    
    g_PlayerPenaltyPoints[playerid] = max(0, g_PlayerPenaltyPoints[playerid] - points);
    pInfo[playerid][player_penalty_points] = g_PlayerPenaltyPoints[playerid];
    
    return 1;
}

// ===========================================================================
// LOGOWANIE KAR
// ===========================================================================

/**
 * Loguje kar?? do bazy danych
 */
static stock Penalty_Log(playerid, adminid, E_PENALTY_TYPE:penaltyType, value, const reason[])
{
    new query[512];
    
    mysql_format(g_MySQL, query, sizeof(query),
        "INSERT INTO `penalties` (`player_id`, `admin_id`, `type`, `value`, `reason`, `timestamp`) \
        VALUES (%d, %d, %d, %d, '%e', %d)",
        pInfo[playerid][player_uid],
        IsValidPlayerId(adminid) ? pInfo[adminid][player_uid] : 0,
        _:penaltyType,
        value,
        reason,
        gettime());
    
    mysql_query(g_MySQL, query, false);
    
    return 1;
}

// ===========================================================================
// PRZETWARZANIE TIMEROWE
// ===========================================================================

/**
 * Przetwarza aktywne kary gracza (wywo??ywane co sekund??)
 */
stock Penalty_ProcessPlayer(playerid)
{
    if(!IsValidPlayerId(playerid) || !pInfo[playerid][player_logged])
    {
        return 0;
    }
    
    // Mute
    if(g_PlayerMuteTime[playerid] > 0)
    {
        g_PlayerMuteTime[playerid]--;
        
        if(g_PlayerMuteTime[playerid] == 0)
        {
            Penalty_Unmute(playerid);
        }
    }
    
    // Jail
    if(g_PlayerJailTime[playerid] > 0)
    {
        g_PlayerJailTime[playerid]--;
        pInfo[playerid][player_jail] = g_PlayerJailTime[playerid];
        
        if(g_PlayerJailTime[playerid] == 0)
        {
            Penalty_Unjail(playerid);
        }
    }
    
    // Admin Jail
    if(g_PlayerAJTime[playerid] > 0)
    {
        g_PlayerAJTime[playerid]--;
        pInfo[playerid][player_aj] = g_PlayerAJTime[playerid];
        
        if(g_PlayerAJTime[playerid] == 0)
        {
            Penalty_UnAdminJail(playerid);
        }
    }
    
    return 1;
}

// ===========================================================================
// HOOKI
// ===========================================================================

hook OnPlayerConnect@Penalties(playerid)
{
    g_PlayerPenaltyPoints[playerid] = 0;
    g_PlayerJailTime[playerid] = 0;
    g_PlayerMuteTime[playerid] = 0;
    g_PlayerAJTime[playerid] = 0;
    
    return 1;
}

hook OnPlayerText@Penalties(playerid, text[])
{
    if(Penalty_IsMuted(playerid))
    {
        new msg[64];
        format(msg, sizeof(msg), "Jeste?? wyciszony. Pozosta??o: %s", 
            FormatDuration(g_PlayerMuteTime[playerid]));
        SendClientMessage(playerid, COLOR_ERROR, msg);
        
        return Y_HOOKS_BREAK_RETURN_0;
    }
    
    return 1;
}

// ===========================================================================
// EOF
// ===========================================================================


