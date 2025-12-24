/*
 * =============================================================================
 *  PLAYER DATA - System danych gracza
 * =============================================================================
 *
 *  Modu??: player/player_data.inc
 *  Opis: Obs??uga danych gracza, cache i synchronizacja
 *
 *  ZMIANY W REFAKTORZE:
 *  - Scentralizowano wszystkie operacje na danych gracza
 *  - Dodano cache dla cz??sto u??ywanych danych
 *  - Zastosowano prepared statements MySQL
 *  - Poprawiono obs??ug?? b????d??w
 *
 * =============================================================================
 */

#if defined _player_data_included
    #endinput
#endif
#define _player_data_included

// ===========================================================================
// STA??E
// ===========================================================================

#define PLAYER_DATA_SAVE_INTERVAL   300000  // Auto-save co 5 minut

// Maximum amount of money a player may have to avoid overflows
#define MAX_PLAYER_MONEY            1000000000

// Tryby wczytywania
enum E_PLAYER_LOAD_MODE
{
    LOAD_MODE_FULL,         // Pe??ne wczytanie
    LOAD_MODE_QUICK,        // Szybkie (tylko podstawowe)
    LOAD_MODE_REFRESH       // Od??wie??enie z bazy
};

// ===========================================================================
// FUNKCJE INICJALIZACJI
// ===========================================================================

/**
 * Inicjalizuje dane gracza przy po????czeniu
 * @param playerid ID gracza
 */
stock Player_InitData(playerid)
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }

    // Resetuj struktury danych gracza
    Player_ResetData(playerid);

    // Pobierz podstawowe informacje
    GetPlayerName(playerid, pInfo[playerid][player_name], MAX_PLAYER_NAME);
    GetPlayerIp(playerid, pInfo[playerid][player_ip], 20);
    gpci(playerid, pInfo[playerid][player_serial], 128);

    // Ustaw domy??lne warto??ci
    pInfo[playerid][player_spawn_armor] = 0.0;
    pInfo[playerid][player_money] = 0;
    pInfo[playerid][player_logged] = false;
    pInfo[playerid][player_spawned] = false;
    pInfo[playerid][player_afk] = false;
    pInfo[playerid][player_admin_duty] = false;

    // Inicjalizuj timery/flagi
    pInfo[playerid][player_connect_time] = gettime();
    pInfo[playerid][player_last_activity] = gettime();
    pInfo[playerid][player_y_flood] = 0;

    return 1;
}

/**
 * Resetuje wszystkie dane gracza do warto??ci domy??lnych
 * @param playerid ID gracza
 */
stock Player_ResetData(playerid)
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }

    // Resetuj g????wn?? struktur??
    static const emptyInfo[e_players_info];
    pInfo[playerid] = emptyInfo;

    // Resetuj dane globalne konta
    static const emptyGlobal[e_global_account_info];
    pGlobal[playerid] = emptyGlobal;

    // Resetuj bronie
    for(new i = 0; i < 13; i++)
    {
        pWeapon[playerid][i][pw_id] = 0;
        pWeapon[playerid][i][pw_ammo] = 0;
        pWeapon[playerid][i][pw_visible] = true;
    }

    // Resetuj dodatkowe flagi
    SetPlayerColor(playerid, COLOR_WHITE);

    return 1;
}

/**
 * Czy??ci dane gracza przy roz????czeniu
 * @param playerid ID gracza
 */
stock Player_ClearData(playerid)
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }

    // Anuluj aktywne dialogi
    ShowPlayerDialog(playerid, -1, DIALOG_STYLE_MSGBOX, " ", " ", " ", "");

    // Usu?? z iteratora
    Iter_Remove(Player, playerid);

    // Wyczy???? dane
    Player_ResetData(playerid);

    return 1;
}

// ===========================================================================
// FUNKCJE DOST??PU DO DANYCH
// ===========================================================================

/**
 * Pobiera nick gracza (cached)
 * @param playerid ID gracza
 * @param output Bufor wyj??ciowy
 * @param size Rozmiar bufora
 * @return 1 je??li sukces
 */
stock Player_GetName(playerid, output[], size = MAX_PLAYER_NAME + 1)
{
    if(!IsValidPlayerId(playerid))
    {
        output[0] = EOS;
        return 0;
    }

    // U??ywamy cache zamiast natywnej funkcji
    strcopy(output, pInfo[playerid][player_name], size);
    return 1;
}

/**
 * Pobiera sformatowany nick RP
 * @param playerid ID gracza
 * @param output Bufor wyj??ciowy
 * @param size Rozmiar bufora
 * @return 1 je??li sukces
 */
stock Player_GetRPName(playerid, output[], size = MAX_PLAYER_NAME + 1)
{
    if(!IsValidPlayerId(playerid))
    {
        output[0] = EOS;
        return 0;
    }

    // Pobierz nick i zamie?? _ na spacj??
    strcopy(output, pInfo[playerid][player_name], size);

    for(new i = 0; output[i] != EOS; i++)
    {
        if(output[i] == '_')
        {
            output[i] = ' ';
        }
    }

    return 1;
}

/**
 * Pobiera imi?? RP gracza jako zwracany string (wersja uproszczona)
 * @param playerid ID gracza
 * @return Sformatowane imi?? RP
 */
stock Player_RPName(playerid)
{
    static output[MAX_PLAYER_NAME + 1];

    if(!IsValidPlayerId(playerid))
    {
        output[0] = EOS;
        return output;
    }

    // Pobierz nick i zamie?? _ na spacj??
    strcopy(output, pInfo[playerid][player_name], sizeof(output));

    for(new i = 0; output[i] != EOS; i++)
    {
        if(output[i] == '_')
        {
            output[i] = ' ';
        }
    }

    return output;
}

/**
 * Pobiera IP gracza (cached)
 * @param playerid ID gracza
 * @param output Bufor wyj??ciowy
 * @param size Rozmiar bufora
 * @return 1 je??li sukces
 */
stock Player_GetIP(playerid, output[], size = 20)
{
    if(!IsValidPlayerId(playerid))
    {
        output[0] = EOS;
        return 0;
    }

    strcopy(output, pInfo[playerid][player_ip], size);
    return 1;
}

// ===========================================================================
// FUNKCJE STATUSU
// ===========================================================================

/**
 * Sprawdza czy gracz jest zalogowany
 * @param playerid ID gracza
 * @return true je??li zalogowany
 */
stock bool:Player_IsLoggedIn(playerid)
{
    if(!IsValidPlayerId(playerid) || !IsPlayerConnected(playerid))
    {
        return false;
    }

    return pInfo[playerid][player_logged];
}

/**
 * Sprawdza czy gracz jest zespawnowany
 * @param playerid ID gracza
 * @return true je??li zespawnowany
 */
stock bool:Player_IsSpawned(playerid)
{
    if(!IsValidPlayerId(playerid))
    {
        return false;
    }

    return pInfo[playerid][player_spawned];
}

/**
 * Sprawdza czy gracz jest AFK
 * @param playerid ID gracza
 * @return true je??li AFK
 */
stock bool:Player_IsAFK(playerid)
{
    if(!IsValidPlayerId(playerid))
    {
        return false;
    }

    return pInfo[playerid][player_afk];
}

/**
 * Ustawia status AFK gracza
 * @param playerid ID gracza
 * @param afk Stan AFK
 */
stock Player_SetAFK(playerid, bool:afk)
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }

    pInfo[playerid][player_afk] = afk;

    if(afk)
    {
        pInfo[playerid][player_afk_time] = gettime();
    }

    return 1;
}

/**
 * Pobiera czas gry gracza
 * @param playerid ID gracza
 * @return Czas gry w minutach
 */
stock Player_GetPlayTime(playerid)
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }

    return pInfo[playerid][player_gameTime] +
           ((gettime() - pInfo[playerid][player_connect_time]) / 60);
}

/**
 * Aktualizuje czas ostatniej aktywno??ci
 * @param playerid ID gracza
 */
stock Player_UpdateActivity(playerid)
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }

    pInfo[playerid][player_last_activity] = gettime();

    // Je??li by?? AFK, wy????cz
    if(pInfo[playerid][player_afk])
    {
        Player_SetAFK(playerid, false);
    }

    return 1;
}

// ===========================================================================
// FUNKCJE PIENI??DZY
// ===========================================================================

/**
 * Pobiera ilo???? pieni??dzy gracza
 * @param playerid ID gracza
 * @return Ilo???? pieni??dzy
 */
stock Player_GetMoney(playerid)
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }

    return pInfo[playerid][player_money];
}

/**
 * Ustawia pieni??dze gracza
 * @param playerid ID gracza
 * @param amount Ilo????
 */
stock Player_SetMoney(playerid, amount)
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }

    // Clamp amount to a safe range
    amount = clamp(amount, 0, MAX_PLAYER_MONEY);

    pInfo[playerid][player_money] = amount;

    // Mark data as modified so auto-save will persist the change
    Player_MarkDataModified(playerid);

    // Synchronizuj z klientem
    ResetPlayerMoney(playerid);
    GivePlayerMoney(playerid, amount);

    return 1;
}

/**
 * Dodaje/odejmuje pieni??dze gracza
 * @param playerid ID gracza
 * @param amount Ilo???? (ujemna = odejmij)
 * @return true je??li sukces
 */
stock bool:Player_GiveMoney(playerid, amount)
{
    if(!IsValidPlayerId(playerid))
    {
        return false;
    }

    new newMoney = pInfo[playerid][player_money] + amount;

    // Sprawdź czy gracz ma wystarczająco środków oraz czy nie przekraczamy maksymalnej wartości
    if(newMoney < 0)
    {
        return false;
    }

    if(newMoney > MAX_PLAYER_MONEY)
    {
        // Prevent overflow and invalid states
        printf("[Player_GiveMoney] Rejecting update: pid=%d would exceed MAX_PLAYER_MONEY (attempted=%d)", playerid, newMoney);
        return false;
    }

    pInfo[playerid][player_money] = newMoney;

    // Mark data modified so it will be persisted on auto-save/disconnect
    Player_MarkDataModified(playerid);

    // Synchronizuj z klientem
    ResetPlayerMoney(playerid);
    GivePlayerMoney(playerid, newMoney);

    return true;
}

/**
 * Sprawdza czy gracz ma wystarczaj??co pieni??dzy
 * @param playerid ID gracza
 * @param amount Wymagana ilo????
 * @return true je??li ma wystarczaj??co
 */
stock bool:Player_HasMoney(playerid, amount)
{
    if(!IsValidPlayerId(playerid))
    {
        return false;
    }

    return pInfo[playerid][player_money] >= amount;
}

// ===========================================================================
// FUNKCJE ZDROWIA I PANCERZA
// ===========================================================================

/**
 * Pobiera zdrowie gracza (cached)
 * @param playerid ID gracza
 * @return Zdrowie
 */
stock Float:Player_GetHealth(playerid)
{
    if(!IsValidPlayerId(playerid))
    {
        return 0.0;
    }

    new Float:health;
    GetPlayerHealth(playerid, health);
    return health;
}

/**
 * Ustawia zdrowie gracza
 * @param playerid ID gracza
 * @param health Zdrowie (0.0-100.0)
 */
stock Player_SetHealth(playerid, Float:health)
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }

    health = clamp(floatround(health), 0, 100);
    SetPlayerHealth(playerid, float(floatround(health)));
    return 1;
}

/**
 * Dodaje zdrowie gracza
 * @param playerid ID gracza
 * @param amount Ilo???? (mo??e by?? ujemna)
 */
stock Player_GiveHealth(playerid, Float:amount)
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }

    new Float:currentHealth;
    GetPlayerHealth(playerid, currentHealth);

    new Float:newHealth = float(clamp(floatround(currentHealth + amount), 0, 100));
    SetPlayerHealth(playerid, newHealth);

    return 1;
}

/**
 * Pobiera pancerz gracza
 * @param playerid ID gracza
 * @return Pancerz
 */
stock Float:Player_GetArmour(playerid)
{
    if(!IsValidPlayerId(playerid))
    {
        return 0.0;
    }

    new Float:armour;
    GetPlayerArmour(playerid, armour);
    return armour;
}

/**
 * Ustawia pancerz gracza
 * @param playerid ID gracza
 * @param armour Pancerz (0.0-100.0)
 */
stock Player_SetArmour(playerid, Float:armour)
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }

    armour = Clamp(armour, 0.0, 100.0);
    SetPlayerArmour(playerid, armour);
    return 1;
}

// ===========================================================================
// FUNKCJE POZYCJI
// ===========================================================================

/**
 * Pobiera pozycj?? gracza
 * @param playerid ID gracza
 * @param x, y, z Wsp????rz??dne (output)
 * @return 1 je??li sukces
 */
stock Player_GetPosition(playerid, &Float:x, &Float:y, &Float:z)
{
    if(!IsValidPlayerId(playerid))
    {
        x = 0.0;
        y = 0.0;
        z = 0.0;
        return 0;
    }

    GetPlayerPos(playerid, x, y, z);
    return 1;
}

/**
 * Ustawia pozycj?? gracza z zachowaniem interior i VW
 * @param playerid ID gracza
 * @param x, y, z Wsp????rz??dne
 * @param angle Kierunek (opcjonalny)
 */
stock Player_SetPosition(playerid, Float:x, Float:y, Float:z, Float:angle = -1.0)
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }

    SetPlayerPos(playerid, x, y, z);

    if(angle >= 0.0)
    {
        SetPlayerFacingAngle(playerid, angle);
    }

    return 1;
}

/**
 * Teleportuje gracza z pe??nymi ustawieniami
 * @param playerid ID gracza
 * @param x, y, z Wsp????rz??dne
 * @param angle Kierunek
 * @param interior Interior ID
 * @param virtualworld VW ID
 */
stock Player_Teleport(playerid, Float:x, Float:y, Float:z, Float:angle, interior, virtualworld)
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }

    // Je??li w poje??dzie, teleportuj pojazd
    if(IsPlayerInAnyVehicle(playerid))
    {
        new vehicleid = GetPlayerVehicleID(playerid);
        SetVehiclePos(vehicleid, x, y, z);
        SetVehicleZAngle(vehicleid, angle);
        LinkVehicleToInterior(vehicleid, interior);
        SetVehicleVirtualWorld(vehicleid, virtualworld);
    }
    else
    {
        SetPlayerPos(playerid, x, y, z);
        SetPlayerFacingAngle(playerid, angle);
    }

    SetPlayerInterior(playerid, interior);
    SetPlayerVirtualWorld(playerid, virtualworld);

    return 1;
}

// ===========================================================================
// FUNKCJE BRONI
// ===========================================================================

/**
 * Daje bro?? graczowi i zapisuje do ekwipunku
 * @param playerid ID gracza
 * @param weaponid ID broni
 * @param ammo Amunicja
 * @return 1 je??li sukces
 */
stock Player_GiveWeapon(playerid, weaponid, ammo)
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }

    // Okre??l slot broni
    new slot = GetWeaponSlot(weaponid);

    if(slot < 0 || slot >= 13)
    {
        return 0;
    }

    // Zapisz do ekwipunku
    pWeapon[playerid][slot][pw_id] = weaponid;
    pWeapon[playerid][slot][pw_ammo] += ammo;

    // Daj bro??
    GivePlayerWeapon(playerid, weaponid, ammo);

    return 1;
}

/**
 * Usuwa bro?? gracza
 * @param playerid ID gracza
 * @param weaponid ID broni
 * @return 1 je??li sukces
 */
stock Player_RemoveWeapon(playerid, weaponid)
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }

    new slot = GetWeaponSlot(weaponid);

    if(slot < 0 || slot >= 13)
    {
        return 0;
    }

    pWeapon[playerid][slot][pw_id] = 0;
    pWeapon[playerid][slot][pw_ammo] = 0;

    // Prze??aduj bronie gracza
    Player_RefreshWeapons(playerid);

    return 1;
}

/**
 * Od??wie??a bronie gracza z ekwipunku
 * @param playerid ID gracza
 */
stock Player_RefreshWeapons(playerid)
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }

    ResetPlayerWeapons(playerid);

    for(new slot = 0; slot < 13; slot++)
    {
        if(pWeapon[playerid][slot][pw_id] > 0 &&
           pWeapon[playerid][slot][pw_ammo] > 0 &&
           pWeapon[playerid][slot][pw_visible])
        {
            GivePlayerWeapon(playerid, pWeapon[playerid][slot][pw_id], pWeapon[playerid][slot][pw_ammo]);
        }
    }

    return 1;
}

// GetWeaponSlot jest zdefiniowany w open.mp lub defines.inc

// ===========================================================================
// FUNKCJE FORMATOWANIA
// ===========================================================================

/**
 * Pobiera sformatowany status gracza
 * @param playerid ID gracza
 * @param output Bufor wyj??ciowy
 * @param size Rozmiar bufora
 */
stock Player_GetStatusString(playerid, output[], size = 64)
{
    if(!IsValidPlayerId(playerid) || !IsPlayerConnected(playerid))
    {
        strcopy(output, "Offline", size);
        return 0;
    }

    if(!pInfo[playerid][player_logged])
    {
        strcopy(output, "Niezalogowany", size);
    }
    else if(pInfo[playerid][player_afk])
    {
        strcopy(output, "AFK", size);
    }
    else if(pInfo[playerid][player_admin_duty])
    {
        strcopy(output, "S??u??ba Admin", size);
    }
    else
    {
        strcopy(output, "Online", size);
    }

    return 1;
}

// ===========================================================================
// HOOKI
// ===========================================================================

hook OnPlayerConnect@PlayerData(playerid)
{
    Player_InitData(playerid);
    Iter_Add(Player, playerid);
    return 1;
}

hook OnPlayerDisconnect@PlayerData(playerid, reason)
{
    Player_ClearData(playerid);
    return 1;
}

hook OnPlayerUpdate@PlayerData(playerid)
{
    Player_UpdateActivity(playerid);
    return 1;
}

// ===========================================================================
// EOF
// ===========================================================================

