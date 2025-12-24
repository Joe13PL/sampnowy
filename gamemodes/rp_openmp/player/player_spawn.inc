/*
 * =============================================================================
 *  PLAYER SPAWN - System spawnu gracza
 * =============================================================================
 *
 *  Modu??: player/player_spawn.inc
 *  Opis: Obs??uga spawnu gracza po zalogowaniu i ??mierci
 *
 *  ZMIANY W REFAKTORZE:
 *  - Zoptymalizowano logik?? wyboru spawnu
 *  - Dodano obs??ug?? spawn point??w z drzwi
 *  - Poprawiono ustawianie skina i broni
 *
 * =============================================================================
 */

#if defined _player_spawn_included
    #endinput
#endif
#define _player_spawn_included

// ===========================================================================
// STA??E
// ===========================================================================

#define SPAWN_FREEZE_TIME       500         // Czas zamrożenia po spawnie (ms)
#define SPAWN_FADE_TIME         500         // Czas fade in (ms)

// Typy spawnu
enum E_SPAWN_TYPE
{
    SPAWN_DEFAULT,          // Domyślny spawn serwera
    SPAWN_LAST_POSITION,    // Ostatnia pozycja gracza
    SPAWN_HOSPITAL          // Szpital po śmierci
};

// ===========================================================================
// ZMIENNE
// ===========================================================================

static E_SPAWN_TYPE:g_SpawnType[MAX_PLAYERS];
static bool:g_FirstSpawn[MAX_PLAYERS] = {true, ...};
static bool:g_SpawnUseLastPosition[MAX_PLAYERS] = {true, ...};

// Zabezpiecza przed invalid/custom skinem bez załadowanego modelu
static stock Player_GetValidSkin(playerid)
{
    new skin = pInfo[playerid][player_skin];

    if(skin < 0 || skin > 311)
    {
        skin = 26; // prosty domyślny skin
        pInfo[playerid][player_skin] = skin;
    }

    return skin;
}

stock Player_SetSpawnPreferenceUseLast(playerid, bool:useLast)
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }

    g_SpawnUseLastPosition[playerid] = useLast;
    return 1;
}

// Pozycje szpitali
static const Float:g_HospitalSpawns[][4] = {
    {1172.6313, -1323.5852, 15.4063, 270.0},    // LS All Saints
    {2034.5967, -1409.1166, 17.1406, 135.0},    // LS County
    {1607.5580, 1820.9547, 10.8203, 180.0},     // LV
    {-2655.0466, 640.1796, 14.4531, 180.0}      // SF Medical
};

// Pozycje wi??zienia
static const Float:g_JailSpawns[][4] = {
    {264.3963, 77.5989, 1001.0391, 270.0}       // LSPD Jail
};

// ===========================================================================
// G????WNE FUNKCJE
// ===========================================================================

/**
 * Spawnuje gracza po zalogowaniu
 * @param playerid ID gracza
 */
stock Player_Spawn(playerid)
{
    if(!IsValidPlayerId(playerid) || !pInfo[playerid][player_loaded])
    {
        printf("[Player_Spawn] BLOCKED - playerid=%d loaded=%d", playerid, pInfo[playerid][player_loaded]);
        return 0;
    }

    // Określ typ spawnu (zachowaj szpitalny jeśli ustawiony)
    if(g_SpawnType[playerid] != SPAWN_HOSPITAL)
    {
        g_SpawnType[playerid] = Player_DetermineSpawnType(playerid);
    }

    // Pobierz pozycję spawnu
    new Float:x, Float:y, Float:z, Float:a;
    new interior, vw;
    Player_GetSpawnPosition(playerid, x, y, z, a, interior, vw);
    Player_SanitizeSpawn(x, y, z, a, interior, vw);

    // Ustaw skin
    new skin = Player_GetValidSkin(playerid);

    // Zabezpiecz Z
    if(z < 1.0) z = 10.0;

    printf("[Player_Spawn] Spawning player %d at %.2f, %.2f, %.2f skin=%d", playerid, x, y, z, skin);

    // Ustaw spawn info
    SetSpawnInfo(playerid, NO_TEAM, skin, x, y, z, a, 0, 0, 0, 0, 0, 0);

    // BEZPOŚREDNI SPAWN - bez spectator tricku
    // Najpierw upewnij się że gracz NIE jest w spectator
    TogglePlayerSpectating(playerid, false);

    // Teraz wywołaj SpawnPlayer - to wyzwoli OnPlayerSpawn
    SpawnPlayer(playerid);

    // Ukryj menu wyboru modeli z opóźnieniem (może pozostać z poprzedniej sesji/spectating)
    SetTimerEx("Player_HideMenuAfterSpawn", 500, false, "i", playerid);

    return 1;
}

forward Player_HideMenuAfterSpawn(playerid);
public Player_HideMenuAfterSpawn(playerid)
{
    if(!IsPlayerConnected(playerid)) return 0;
    HideModelSelectionMenu(playerid);
    return 1;
}

forward Player_PostSpawn(playerid, Float:x, Float:y, Float:z);
public Player_PostSpawn(playerid, Float:x, Float:y, Float:z)
{
    if(IsValidPlayerId(playerid) && pInfo[playerid][player_logged])
    {
        // Ustaw pozycję na ziemi jeśli jesteśmy na zewnątrz
        if(GetPlayerInterior(playerid) == 0)
        {
            SetPlayerPosFindZ(playerid, x, y, z);
        }
        else
        {
            SetPlayerPos(playerid, x, y, z);
        }
    }
    return 1;
}

/**
 * Okre??la typ spawnu dla gracza
 * @param playerid ID gracza
 * @return Typ spawnu
 */
static stock E_SPAWN_TYPE:Player_DetermineSpawnType(playerid)
{
    if(!g_SpawnUseLastPosition[playerid])
    {
        return SPAWN_DEFAULT;
    }

    // Sprawdź czy ma zapisaną poprawną pozycję (nie zero)
    // Jeśli DB ma (0,0,0) to gracz nigdy tam nie był - użyj domyślnego
    if(pInfo[playerid][player_pos_x] != 0.0 &&
       pInfo[playerid][player_pos_y] != 0.0 &&
       pInfo[playerid][player_pos_z] != 0.0)
    {
        // Dodatkowa walidacja - jeśli pozycja jest poza mapą, fallback na default
        new Float:dist_sq = (pInfo[playerid][player_pos_x] * pInfo[playerid][player_pos_x]) +
                            (pInfo[playerid][player_pos_y] * pInfo[playerid][player_pos_y]);
        if(dist_sq < 9000000.0 && pInfo[playerid][player_pos_z] > -50.0 && pInfo[playerid][player_pos_z] < 800.0)
        {
            return SPAWN_LAST_POSITION;
        }
    }

    // Domy??lny spawn
    return SPAWN_DEFAULT;
}

/**
 * Pobiera pozycj?? spawnu dla gracza
 * @param playerid ID gracza
 * @param x, y, z, a Wsp????rz??dne (output)
 * @param interior Interior (output)
 * @param vw Virtual World (output)
 */
stock Player_GetSpawnPosition(playerid, &Float:x, &Float:y, &Float:z, &Float:a, &interior, &vw)
{
    switch(g_SpawnType[playerid])
    {
        case SPAWN_LAST_POSITION:
        {
            x = pInfo[playerid][player_pos_x];
            y = pInfo[playerid][player_pos_y];
            z = pInfo[playerid][player_pos_z];
            a = pInfo[playerid][player_pos_a];
            interior = pInfo[playerid][player_interior];
            vw = pInfo[playerid][player_vw];

            // WALIDACJA - jeśli DB ma zerowe koordynaty, fallback
            if(x == 0.0 && y == 0.0 && z == 0.0)
            {
                g_SpawnType[playerid] = SPAWN_DEFAULT;
                Player_GetSpawnPosition(playerid, x, y, z, a, interior, vw);
            }
        }

        case SPAWN_HOSPITAL:
        {
            // Znajd?? najbli??szy szpital
            new bestIdx = 0;
            new Float:bestDist = 99999.0;

            for(new i = 0; i < sizeof(g_HospitalSpawns); i++)
            {
                new Float:dist = GetDistanceBetweenPoints2D(
                    pInfo[playerid][player_pos_x], pInfo[playerid][player_pos_y],
                    g_HospitalSpawns[i][0], g_HospitalSpawns[i][1]
                );

                if(dist < bestDist)
                {
                    bestDist = dist;
                    bestIdx = i;
                }
            }

            x = g_HospitalSpawns[bestIdx][0];
            y = g_HospitalSpawns[bestIdx][1];
            z = g_HospitalSpawns[bestIdx][2];
            a = g_HospitalSpawns[bestIdx][3];
            interior = 0;
            vw = 0;
        }

        default: // SPAWN_DEFAULT
        {
            // Twardy fallback jeśli konfiguracja nie załadowała współrzędnych (0,0,0)
            if(Setting[setting_spawn_pos_x] == 0.0 && Setting[setting_spawn_pos_y] == 0.0 && Setting[setting_spawn_pos_z] == 0.0)
            {
                Setting[setting_spawn_pos_x] = 1607.5580;
                Setting[setting_spawn_pos_y] = 1820.9547;
                Setting[setting_spawn_pos_z] = 10.8203;
                Setting[setting_spawn_pos_a] = 180.0;
            }
            x = Setting[setting_spawn_pos_x];
            y = Setting[setting_spawn_pos_y];
            z = Setting[setting_spawn_pos_z];
            a = Setting[setting_spawn_pos_a];
            interior = 0;
            vw = 0;
        }
    }
}

// W razie zapisanej złej pozycji (poza mapą) przerzuć na domyślny spawn
static stock Player_SanitizeSpawn(&Float:x, &Float:y, &Float:z, &Float:a, &interior, &vw)
{
     // Odległość od środka zbyt duża, z poza zakresem, lub zapisane okolice (0,0)
     if(floatsqroot(x * x + y * y) > 10000.0 || z > 2000.0 || z < -50.0 ||
         (floatsqroot(x * x + y * y) < 100.0 && z < 50.0))
    {
        // Fallback na domyślny spawn
        if(Setting[setting_spawn_pos_x] == 0.0 && Setting[setting_spawn_pos_y] == 0.0 && Setting[setting_spawn_pos_z] == 0.0)
        {
            Setting[setting_spawn_pos_x] = 1607.5580;
            Setting[setting_spawn_pos_y] = 1820.9547;
            Setting[setting_spawn_pos_z] = 10.8203;
            Setting[setting_spawn_pos_a] = 180.0;
        }
        x = Setting[setting_spawn_pos_x];
        y = Setting[setting_spawn_pos_y];
        z = Setting[setting_spawn_pos_z];
        a = Setting[setting_spawn_pos_a];
        interior = 0;
        vw = 0;
    }
}

// ===========================================================================
// HOOK - OnPlayerSpawn
// ===========================================================================

hook OnPlayerSpawn@Spawn(playerid)
{
    if(!IsValidPlayerId(playerid) || !pInfo[playerid][player_logged])
    {
        return 0;
    }

    // Pobierz pozycj?? spawnu
    new Float:x, Float:y, Float:z, Float:a;
    new interior, vw;

    Player_GetSpawnPosition(playerid, x, y, z, a, interior, vw);
    Player_SanitizeSpawn(x, y, z, a, interior, vw);

    // NATYCHMIAST ustaw pozycj?? na wypadek że SetSpawnInfo nie zadziałał
    if(interior == 0)
    {
        SetPlayerPosFindZ(playerid, x, y, z);
    }
    else
    {
        SetPlayerPos(playerid, x, y, z);
    }
    SetPlayerFacingAngle(playerid, a);
    SetPlayerInterior(playerid, interior);
    SetPlayerVirtualWorld(playerid, vw);

    printf("[Spawn] Gracz %d spawned at %.2f, %.2f, %.2f (VW:%d, INT:%d)",
        playerid, x, y, z, vw, interior);

    // Ustaw zdrowie i pancerz
    // Jeśli to pierwszy spawn po połączeniu LUB mamy zapisany pancerz > 0, przywróć go.
    if(g_FirstSpawn[playerid] || pInfo[playerid][player_spawn_armor] > 0.0)
    {
        SetPlayerHealth(playerid, pInfo[playerid][player_health]);
        SetPlayerArmour(playerid, pInfo[playerid][player_spawn_armor]);
        // Keep internal cache consistent as well
        pInfo[playerid][player_armour] = pInfo[playerid][player_spawn_armor];
        pInfo[playerid][player_armour_changed] = true;
        printf("[Spawn] pid=%d applied saved armour=%.2f (cached=%.2f)", playerid, pInfo[playerid][player_spawn_armor], pInfo[playerid][player_armour]);
        // Verify after short delay because some client-side/resync events can reset armour
        SetTimerEx("Player_VerifyArmour", 500, false, "d", playerid);
        g_FirstSpawn[playerid] = false;
    }
    else
    {
        // Po śmierci lub brak zapisanego pancerza - pełne zdrowie, brak pancerza
        SetPlayerHealth(playerid, 100.0);
        SetPlayerArmour(playerid, 0.0);
    }

    // Ustaw skin
    SetPlayerSkin(playerid, Player_GetValidSkin(playerid));

    // Przywr???? bronie
    Player_RefreshWeapons(playerid);

    // Synchronizuj pieni??dze
    ResetPlayerMoney(playerid);
    GivePlayerMoney(playerid, pInfo[playerid][player_money]);

    // Zamro?? na chwil??
    TogglePlayerControllable(playerid, false);
    SetTimerEx("Player_Unfreeze", SPAWN_FREEZE_TIME, false, "d", playerid);

    // Ustaw kamer??
    SetCameraBehindPlayer(playerid);

    // Oznacz jako zespawnowany
    pInfo[playerid][player_spawned] = true;

    // NATYCHMIAST sprawdzaj czy gracz nie spawuje w złym miejscu
    SetTimerEx("Player_FixSpawn", 100, false, "d", playerid);
    SetTimerEx("Player_FixSpawn", 500, false, "d", playerid);
    SetTimerEx("Player_FixSpawn", 2000, false, "d", playerid);

    // Efekt fade in
    Player_FadeFromBlack(playerid, SPAWN_FADE_TIME);

    // Wiadomo???? powitalna przy pierwszym spawnie
    if(g_FirstSpawn[playerid])
    {
        SendClientMessage(playerid, COLOR_WHITE, "");
        SendClientMessage(playerid, COLOR_YELLOW, "??????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????");
        SendClientMessage(playerid, COLOR_WHITE, " Witaj na serwerze! U??yj /pomoc aby uzyska?? list?? komend.");
        SendClientMessage(playerid, COLOR_YELLOW, "??????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????");
        SendClientMessage(playerid, COLOR_WHITE, "");
    }

    return 1;
}

forward Player_Unfreeze(playerid);
public Player_Unfreeze(playerid)
{
    if(IsPlayerConnected(playerid))
    {
        TogglePlayerControllable(playerid, true);
    }
    return 1;
}

// Opóźniony fix spawnu - jeśli coś poszło nie tak z pozycją
forward Player_FixSpawn(playerid);
public Player_FixSpawn(playerid)
{
    if(!IsValidPlayerId(playerid) || !pInfo[playerid][player_logged])
    {
        return 1;
    }

    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);

    // Zawsze resetuj jeśli cokolwiek podejrzanego
    new Float:dist_sq = (x * x) + (y * y);

    // Jeśli: blisko (0,0), niebo, pod mapą, poza mapą
    if(dist_sq < 100000.0 && z < 50.0)  // Blisko zera = zły spawn
    {
        new Float:spx = Setting[setting_spawn_pos_x];
        new Float:spy = Setting[setting_spawn_pos_y];
        new Float:spz = Setting[setting_spawn_pos_z];
        new Float:spa = Setting[setting_spawn_pos_a];
        if(spx == 0.0 && spy == 0.0 && spz == 0.0)
        {
            spx = 1607.5580;
            spy = 1820.9547;
            spz = 10.8203;
            spa = 180.0;
        }

        printf("[SpawnFix] NAPRAWA - Gracz %d był na %.2f,%.2f,%.2f -> Reset na %.2f,%.2f,%.2f",
            playerid, x, y, z, spx, spy, spz);

        if(GetPlayerInterior(playerid) == 0)
        {
            SetPlayerPosFindZ(playerid, spx, spy, spz);
        }
        else
        {
            SetPlayerPos(playerid, spx, spy, spz);
        }
        SetPlayerFacingAngle(playerid, spa);
        SetPlayerInterior(playerid, 0);
        SetPlayerVirtualWorld(playerid, 0);
    }
    else if(dist_sq > 9000000.0 || z > 800.0 || z < -50.0)  // Poza mapą
    {
        new Float:spx = Setting[setting_spawn_pos_x];
        new Float:spy = Setting[setting_spawn_pos_y];
        new Float:spz = Setting[setting_spawn_pos_z];
        new Float:spa = Setting[setting_spawn_pos_a];
        if(spx == 0.0 && spy == 0.0 && spz == 0.0)
        {
            spx = 1607.5580;
            spy = 1820.9547;
            spz = 10.8203;
            spa = 180.0;
        }

        printf("[SpawnFix] POZA MAPĄ - Gracz %d był na %.2f,%.2f,%.2f -> Reset",
            playerid, x, y, z);

        if(GetPlayerInterior(playerid) == 0)
        {
            SetPlayerPosFindZ(playerid, spx, spy, spz);
        }
        else
        {
            SetPlayerPos(playerid, spx, spy, spz);
        }
        SetPlayerFacingAngle(playerid, spa);
        SetPlayerInterior(playerid, 0);
        SetPlayerVirtualWorld(playerid, 0);
    }

    return 1;
}

// ===========================================================================
// EFEKTY WIZUALNE
// ===========================================================================

/**
 * Efekt przej??cia z czerni
 * @param playerid ID gracza
 * @param time Czas trwania efektu
 */
stock Player_FadeFromBlack(playerid, time)
{
    // U??ywamy textdrawa z czarnym t??em
    // Lub GameTextForPlayer z pustym tekstem

    // Alternatywnie - flash ekranu
    PlayerPlaySound(playerid, 1056, 0.0, 0.0, 0.0);

    return 1;
}

/**
 * Efekt przej??cia do czerni
 * @param playerid ID gracza
 * @param time Czas trwania efektu
 */
stock Player_FadeToBlack(playerid, time)
{
    PlayerPlaySound(playerid, 1055, 0.0, 0.0, 0.0);
    return 1;
}

// ===========================================================================
// HOOK - OnPlayerDeath
// ===========================================================================

hook OnPlayerDeath@Spawn(playerid, killerid, reason)
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }

    // Zapisz pozycj?? ??mierci
    GetPlayerPos(playerid, pInfo[playerid][player_pos_x],
                 pInfo[playerid][player_pos_y], pInfo[playerid][player_pos_z]);

    // Ustaw spawn na szpital
    g_SpawnType[playerid] = SPAWN_HOSPITAL;

    // Oznacz jako nie??ywy
    pInfo[playerid][player_spawned] = false;

    // Log ??mierci
    if(killerid != INVALID_PLAYER_ID && IsPlayerConnected(killerid))
    {
        new weaponName[32];
        GetWeaponName(reason, weaponName, sizeof(weaponName));

        printf("[Death] %s zosta?? zabity przez %s (bro??: %s)",
            pInfo[playerid][player_name],
            pInfo[killerid][player_name],
            weaponName);
    }
    else
    {
        printf("[Death] %s zmar?? (przyczyna: %d)",
            pInfo[playerid][player_name], reason);
    }

    return 1;
}

// ===========================================================================
// FUNKCJE POMOCNICZE
// ===========================================================================

// Blokuje ekran wyboru klasy po śmierci/logowaniu i od razu spawnuje
hook OnPlayerRequestClass@Spawn(playerid, classid)
{
    if(pInfo[playerid][player_logged])
    {
        // Wymuś pełny flow spawnu (SetSpawnInfo + post-fix) zamiast domyślnego wyboru klasy
        Player_Spawn(playerid);
        return 0;
    }
    return 1;
}

/**
 * Respawnuje gracza w okre??lonym miejscu
 * @param playerid ID gracza
 * @param spawnType Typ spawnu
 */
stock Player_Respawn(playerid, E_SPAWN_TYPE:spawnType = SPAWN_LAST_POSITION)
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }

    // Zapisz aktualn?? pozycj?? je??li spawn last position
    if(spawnType == SPAWN_LAST_POSITION)
    {
        GetPlayerPos(playerid, pInfo[playerid][player_pos_x],
                     pInfo[playerid][player_pos_y], pInfo[playerid][player_pos_z]);
        GetPlayerFacingAngle(playerid, pInfo[playerid][player_pos_a]);
        pInfo[playerid][player_interior] = GetPlayerInterior(playerid);
        pInfo[playerid][player_vw] = GetPlayerVirtualWorld(playerid);
    }

    g_SpawnType[playerid] = spawnType;
    g_FirstSpawn[playerid] = false;

    // Wymu?? respawn
    SpawnPlayer(playerid);

    return 1;
}

/**
 * Teleportuje gracza do szpitala
 * @param playerid ID gracza
 */
stock Player_SendToHospital(playerid)
{
    g_SpawnType[playerid] = SPAWN_HOSPITAL;

    new Float:x, Float:y, Float:z, Float:a;
    new interior, vw;

    Player_GetSpawnPosition(playerid, x, y, z, a, interior, vw);
    Player_Teleport(playerid, x, y, z, a, interior, vw);

    return 1;
}

/**
 * Teleportuje gracza do wi??zienia
 * @param playerid ID gracza
 * @param time Czas kary (minuty)
 */
stock Player_SendToJail(playerid, time)
{
    pInfo[playerid][player_aj] = time;
    g_SpawnType[playerid] = SPAWN_JAIL;

    new Float:x, Float:y, Float:z, Float:a;
    new interior, vw;

    Player_GetSpawnPosition(playerid, x, y, z, a, interior, vw);
    Player_Teleport(playerid, x, y, z, a, interior, vw);

    // Usu?? bronie
    ResetPlayerWeapons(playerid);

    SendGuiInformation(playerid, "Aresztowany",
        "Zosta??e?? aresztowany i osadzony w wi??zieniu.\nCzas kary: %d minut.", time);

    return 1;
}

// ===========================================================================
// HOOKI
// ===========================================================================

hook OnPlayerConnect@Spawn(playerid)
{
    g_FirstSpawn[playerid] = true;
    g_SpawnType[playerid] = SPAWN_DEFAULT;
    g_SpawnUseLastPosition[playerid] = true;
    return 1;
}

hook OnPlayerDisconnect@Spawn(playerid, reason)
{
    // Zapisz pozycj?? przy roz????czeniu
    if(pInfo[playerid][player_spawned])
    {
        GetPlayerPos(playerid, pInfo[playerid][player_pos_x],
                     pInfo[playerid][player_pos_y], pInfo[playerid][player_pos_z]);
        GetPlayerFacingAngle(playerid, pInfo[playerid][player_pos_a]);
        pInfo[playerid][player_interior] = GetPlayerInterior(playerid);
        pInfo[playerid][player_vw] = GetPlayerVirtualWorld(playerid);
    }

    return 1;
}

// Verify applied armour after spawn (timer handler)
forward Player_VerifyArmour(playerid);
public Player_VerifyArmour(playerid)
{
    if(!IsValidPlayerId(playerid) || !pInfo[playerid][player_logged]) return 0;

    new Float:cur;
    GetPlayerArmour(playerid, cur);
    if(pInfo[playerid][player_spawn_armor] > 0.0 && floatround(cur) != floatround(pInfo[playerid][player_spawn_armor]))
    {
        printf("[Spawn] pid=%d verify mismatch cur=%.2f expected=%.2f - reapplying", playerid, cur, pInfo[playerid][player_spawn_armor]);
        SetPlayerArmour(playerid, pInfo[playerid][player_spawn_armor]);
        GetPlayerArmour(playerid, cur);
        printf("[Spawn] pid=%d after reapply cur=%.2f", playerid, cur);
    }
    else
    {
        printf("[Spawn] pid=%d verify ok cur=%.2f expected=%.2f", playerid, cur, pInfo[playerid][player_spawn_armor]);
    }

    return 1;
}

// ===========================================================================
// EOF
// ===========================================================================

