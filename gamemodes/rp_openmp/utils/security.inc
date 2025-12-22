/*
 * =============================================================================
 *  SECURITY - Funkcje bezpiecze??stwa
 * =============================================================================
 *  
 *  Modu??: utils/security.inc
 *  Opis: Funkcje walidacji danych, anti-cheat i bezpiecze??stwa
 *
 *  ZMIANY W REFAKTORZE:
 *  - Zast??piono MD5 na bcrypt/SHA256
 *  - Dodano walidacj?? danych wej??ciowych
 *  - Dodano ochron?? przed exploitami
 *  - Poprawiono obs??ug?? kick??w/ban??w
 *
 * =============================================================================
 */

#if defined _security_included
    #endinput
#endif
#define _security_included

// ===========================================================================
// STA??E BEZPIECZE??STWA
// ===========================================================================

#define SECURITY_KICK_DELAY         500     // Op????nienie przed kickiem (ms)
#define SECURITY_BAN_DELAY          1000    // Op????nienie przed banem (ms)
#define SECURITY_MAX_WARNINGS       3       // Max ostrze??e?? przed kickiem

// Typy ban??w
#define BAN_TYPE_PERMANENT          0
#define BAN_TYPE_TEMPORARY          1
#define BAN_TYPE_IP                 2

// ===========================================================================
// HASZOWANIE HASE??
// ===========================================================================

/**
 * Haszuje hasło używając bcrypt (asynchronicznie)
 * @param playerid ID gracza
 * @param password Hasło do zhaszowania
 * @param callback Nazwa callbacku do wywołania po zhashowaniu
 */
stock HashPasswordAsync(playerid, const password[], const callback[] = "OnPasswordHashed")
{
    bcrypt_hash(playerid, callback, password, BCRYPT_COST);
}

/**
 * Weryfikuje hasło z hashem (asynchronicznie)
 * @param playerid ID gracza
 * @param password Wprowadzone hasło
 * @param hash Zapisany hash
 * @param callback Nazwa callbacku do wywołania po weryfikacji
 */
stock VerifyPasswordAsync(playerid, const password[], const hash[], const callback[] = "OnPasswordVerified")
{
    bcrypt_verify(playerid, callback, password, hash);
}

// Callback dla weryfikacji hasła
forward OnPasswordVerified(playerid, bool:success);

// Callback dla hashowania hasła
forward OnPasswordHashed(playerid);

// ===========================================================================
// BEZPIECZNY KICK/BAN
// ===========================================================================

static g_KickTimer[MAX_PLAYERS] = {0, ...};
static g_KickReason[MAX_PLAYERS][128];
static g_KickAdmin[MAX_PLAYERS] = {INVALID_PLAYER_ID, ...};

/**
 * Kickuje gracza z op????nieniem (bezpiecznie)
 * @param playerid ID gracza
 * @param adminid ID admina (lub -1 dla systemu)
 * @param reason Pow??d kicka
 */
stock Player_Kick(playerid, adminid, const reason[])
{
    if(!IsPlayerConnected(playerid))
    {
        return 0;
    }
    
    // Zapisz pow??d i admina
    strcopy(g_KickReason[playerid], reason, 128);
    g_KickAdmin[playerid] = adminid;
    
    // Wy??wietl pow??d graczowi
    SendGuiInformation(playerid, "Zosta??e?? wyrzucony", reason);
    
    // Zaloguj
    new adminName[MAX_PLAYER_NAME + 1];
    if(adminid != -1 && IsPlayerConnected(adminid))
    {
        GetPlayerName(adminid, adminName, sizeof(adminName));
    }
    else
    {
        strcopy(adminName, "System", sizeof(adminName));
    }
    
    printf("[Security] KICK: %s przez %s - %s", 
        pInfo[playerid][player_name], adminName, reason);
    
    // Wykonaj kick z op????nieniem
    g_KickTimer[playerid] = SetTimerEx("Security_ExecuteKick", SECURITY_KICK_DELAY, false, "d", playerid);
    
    return 1;
}

forward Security_ExecuteKick(playerid);
public Security_ExecuteKick(playerid)
{
    g_KickTimer[playerid] = 0;
    Kick(playerid);
    return 1;
}

/**
 * Banuje gracza
 * @param playerid ID gracza
 * @param adminid ID admina
 * @param reason Pow??d bana
 * @param duration Czas trwania (0 = permanentny)
 */
stock Player_Ban(playerid, adminid, const reason[], duration = 0)
{
    if(!IsPlayerConnected(playerid))
    {
        return 0;
    }
    
    // Pobierz dane gracza
    new ip[20], serial[128];
    GetPlayerIp(playerid, ip, sizeof(ip));
    gpci(playerid, serial, sizeof(serial));
    
    // Zapisz do bazy danych
    new expireTime = (duration > 0) ? (gettime() + duration) : 0;
    
    MySQL_ExecuteFormat(
        "INSERT INTO `bans` (ban_player, ban_ip, ban_serial, ban_admin, ban_reason, ban_time, ban_expire) \
         VALUES (%d, '%s', '%s', %d, '%s', %d, %d)",
        Player_GetUID(playerid), ip, serial, 
        (adminid != -1) ? Player_GetUID(adminid) : 0,
        reason, gettime(), expireTime
    );
    
    // Ustaw flag?? bana na koncie
    MySQL_ExecuteFormat(
        "UPDATE `users` SET `banned` = 1, `ban_reason` = '%s' WHERE `id` = %d",
        reason, pGlobal[playerid][global_id]
    );
    
    // Wy??wietl pow??d
    new message[256];
    if(duration > 0)
    {
        new durationStr[32];
        FormatDuration(duration, durationStr, sizeof(durationStr), false);
        format(message, sizeof(message), "Zosta??e?? zbanowany na %s.\nPow??d: %s", durationStr, reason);
    }
    else
    {
        format(message, sizeof(message), "Zosta??e?? permanentnie zbanowany.\nPow??d: %s", reason);
    }
    
    SendGuiInformation(playerid, "Zosta??e?? zbanowany", message);
    
    // Zaloguj
    printf("[Security] BAN: %s (UID: %d) - %s", 
        pInfo[playerid][player_name], Player_GetUID(playerid), reason);
    
    // Kick z op????nieniem
    SetTimerEx("Security_ExecuteKick", SECURITY_BAN_DELAY, false, "d", playerid);
    
    return 1;
}

// ===========================================================================
// WALIDACJA DANYCH
// ===========================================================================

/**
 * Sprawdza czy pozycja jest prawid??owa
 * @param x, y, z Wsp????rz??dne
 * @return true je??li pozycja jest w granicach mapy
 */
stock bool:IsValidPosition(Float:x, Float:y, Float:z)
{
    // Granice mapy SA
    if(x < -20000.0 || x > 20000.0) return false;
    if(y < -20000.0 || y > 20000.0) return false;
    if(z < -100.0 || z > 10000.0) return false;
    
    return true;
}

/**
 * Sprawdza czy ID gracza jest prawid??owe
 * @param playerid ID gracza
 * @return true je??li ID jest prawid??owe
 */
stock bool:IsValidPlayerId(playerid)
{
    return (playerid >= 0 && playerid < MAX_PLAYERS);
}

/**
 * Sprawdza czy gracz jest prawid??owy i po????czony
 * @param playerid ID gracza
 * @return true je??li gracz jest po????czony
 */
stock bool:IsValidPlayer(playerid)
{
    return IsValidPlayerId(playerid) && IsPlayerConnected(playerid) && !IsPlayerNPC(playerid);
}

/**
 * Sprawdza czy ID pojazdu jest prawid??owe
 * @param vehicleid ID pojazdu
 * @return true je??li pojazd jest prawid??owy
 */
stock bool:IsValidVehicleId(vehicleid)
{
    return (vehicleid >= 1 && vehicleid < MAX_VEHICLES && IsValidVehicle(vehicleid));
}

// ===========================================================================
// ANTI-CHEAT
// ===========================================================================

/**
 * Sprawdza gracza pod k??tem podejrzanej aktywno??ci
 * @param playerid ID gracza
 * @param checkType Typ sprawdzenia
 * @return true je??li wykryto cheat
 */
stock bool:AntiCheat_Check(playerid, checkType)
{
    if(!Setting[setting_anticheat_enabled])
    {
        return false;
    }
    
    switch(checkType)
    {
        case 0: // Airbreak check
        {
            static Float:lastPos[MAX_PLAYERS][3];
            static Float:currentPos[3];
            
            GetPlayerPos(playerid, currentPos[0], currentPos[1], currentPos[2]);
            
            // Oblicz dystans od ostatniej pozycji
            new Float:dist = GetDistanceBetweenPoints3D(
                lastPos[playerid][0], lastPos[playerid][1], lastPos[playerid][2],
                currentPos[0], currentPos[1], currentPos[2]
            );
            
            // Zapisz aktualn?? pozycj??
            lastPos[playerid][0] = currentPos[0];
            lastPos[playerid][1] = currentPos[1];
            lastPos[playerid][2] = currentPos[2];
            
            // Sprawd?? czy dystans jest podejrzany
            if(dist > 50.0 && !IsPlayerInAnyVehicle(playerid))
            {
                pInfo[playerid][player_airbrk_warns]++;
                
                if(pInfo[playerid][player_airbrk_warns] >= SECURITY_MAX_WARNINGS)
                {
                    Player_Kick(playerid, -1, "Podejrzana aktywno???? (teleport)");
                    return true;
                }
            }
            else
            {
                // Reset ostrze??e?? przy normalnym ruchu
                if(pInfo[playerid][player_airbrk_warns] > 0)
                {
                    pInfo[playerid][player_airbrk_warns]--;
                }
            }
        }
        
        case 1: // Weapon check
        {
            // Sprawd?? czy gracz ma bro?? kt??rej nie powinien mie??
            for(new slot = 0; slot < 13; slot++)
            {
                new weapon, ammo;
                GetPlayerWeaponData(playerid, slot, weapon, ammo);
                
                if(weapon > 0 && ammo > 0)
                {
                    // Sprawd?? czy gracz ma t?? bro?? w ekwipunku
                    if(pWeapon[playerid][slot][pw_id] != weapon)
                    {
                        // Nielegalna bro??
                        pInfo[playerid][player_cheat_warns]++;
                        
                        // Usu?? bro??
                        ResetPlayerWeapons(playerid);
                        
                        if(pInfo[playerid][player_cheat_warns] >= SECURITY_MAX_WARNINGS)
                        {
                            Player_Kick(playerid, -1, "Nielegalna bro??");
                            return true;
                        }
                    }
                }
            }
        }
        
        case 2: // Money check
        {
            new clientMoney = GetPlayerMoney(playerid);
            new serverMoney = pInfo[playerid][player_money];
            
            if(clientMoney != serverMoney)
            {
                // Synchronizuj pieni??dze
                ResetPlayerMoney(playerid);
                GivePlayerMoney(playerid, serverMoney);
                
                pInfo[playerid][player_cheat_warns]++;
                
                if(pInfo[playerid][player_cheat_warns] >= SECURITY_MAX_WARNINGS)
                {
                    Player_Kick(playerid, -1, "Modyfikacja pieni??dzy");
                    return true;
                }
            }
        }
        
        case 3: // Health check
        {
            static Float:health, Float:armour;
            GetPlayerHealth(playerid, health);
            GetPlayerArmour(playerid, armour);
            
            if(health > 100.0 && !pInfo[playerid][player_admin_duty])
            {
                SetPlayerHealth(playerid, 100.0);
                pInfo[playerid][player_cheat_warns]++;
            }
            
            if(armour > 100.0)
            {
                SetPlayerArmour(playerid, 100.0);
                pInfo[playerid][player_cheat_warns]++;
            }
            
            if(pInfo[playerid][player_cheat_warns] >= SECURITY_MAX_WARNINGS)
            {
                Player_Kick(playerid, -1, "Modyfikacja zdrowia/pancerza");
                return true;
            }
        }
    }
    
    return false;
}

/**
 * Resetuje ostrze??enia anti-cheat dla gracza
 * @param playerid ID gracza
 */
stock AntiCheat_ResetWarnings(playerid)
{
    pInfo[playerid][player_cheat_warns] = 0;
    pInfo[playerid][player_airbrk_warns] = 0;
}

// ===========================================================================
// OCHRONA PRZED FLOOD
// ===========================================================================

static g_LastCommand[MAX_PLAYERS];
static g_CommandCount[MAX_PLAYERS];

/**
 * Sprawdza flood komend
 * @param playerid ID gracza
 * @return true je??li gracz flooduje
 */
stock bool:IsFloodingCommands(playerid)
{
    new currentTime = GetTickCount();
    
    if(currentTime - g_LastCommand[playerid] < 500) // 500ms mi??dzy komendami
    {
        g_CommandCount[playerid]++;
        
        if(g_CommandCount[playerid] >= 5)
        {
            return true;
        }
    }
    else
    {
        g_CommandCount[playerid] = 0;
    }
    
    g_LastCommand[playerid] = currentTime;
    return false;
}

/**
 * Sprawdza flood czatu
 * @param playerid ID gracza
 * @return true je??li gracz flooduje
 */
stock bool:IsFloodingChat(playerid)
{
    new currentTime = GetTickCount();
    
    if(currentTime - pInfo[playerid][player_y_flood] < 1000)
    {
        return true;
    }
    
    pInfo[playerid][player_y_flood] = currentTime;
    return false;
}

// ===========================================================================
// NATIVE GPCI
// ===========================================================================

#if !defined gpci
    native gpci(playerid, serial[], len);
#endif

// ===========================================================================
// POMOCNICZE FUNKCJE
// ===========================================================================

/**
 * Pobiera UID gracza (bezpiecznie)
 * @param playerid ID gracza
 * @return UID gracza lub 0 je??li niezalogowany
 */
stock Player_GetUID(playerid)
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }
    
    return pInfo[playerid][player_id];
}

/**
 * Sprawdza czy gracz jest zalogowany
 * @param playerid ID gracza
 * @return true je??li zalogowany
 */
stock bool:Player_IsLogged(playerid)
{
    if(!IsValidPlayerId(playerid))
    {
        return false;
    }
    
    return pInfo[playerid][player_logged];
}

// ===========================================================================
// EOF
// ===========================================================================

