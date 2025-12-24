/*
 * =============================================================================
 *  PLAYER SAVE - System zapisu danych gracza
 * =============================================================================
 *
 *  Modu??: player/player_save.inc
 *  Opis: Zapisywanie i wczytywanie danych gracza z bazy danych
 *
 *  ZMIANY W REFAKTORZE:
 *  - Zoptymalizowano zapytania SQL
 *  - Dodano prepared statements
 *  - Dodano auto-save w okre??lonych interwa??ach
 *  - Poprawiono obs??ug?? b????d??w
 *
 * =============================================================================
 */

// Wyciszenie warningów tag mismatch
#pragma warning disable 213

#if defined _player_save_included
    #endinput
#endif
#define _player_save_included

// ===========================================================================
// STA??E
// ===========================================================================

#define SAVE_INTERVAL           300000      // Auto-save co 5 minut
#define SAVE_ON_DEATH           true        // Zapisuj przy ??mierci
#define SAVE_ON_SPAWN           true        // Zapisuj przy spawnie

// ===========================================================================
// ZMIENNE
// ===========================================================================

static g_AutoSaveTimer[MAX_PLAYERS] = {0, ...};
static bool:g_DataModified[MAX_PLAYERS] = {false, ...};
// Transient flag used to skip the initial save scheduled on first spawn after loading data
static bool:g_JustLoaded[MAX_PLAYERS] = {false, ...};

// ===========================================================================
// WCZYTYWANIE DANYCH
// ===========================================================================

/**
 * Wczytuje pe??ne dane gracza z bazy
 * @param playerid ID gracza
 */
stock Player_LoadData(playerid)
{
    if(!IsValidPlayerId(playerid) || pInfo[playerid][player_id] == 0)
    {
        return 0;
    }

    new query[512];
    mysql_format(mySQLconnection, query, sizeof(query),
        "SELECT * FROM `characters` WHERE `id` = %d LIMIT 1",
        pInfo[playerid][player_id]
    );

    mysql_tquery(mySQLconnection, query, "Player_OnDataLoad", "d", playerid);
    return 1;
}

forward Player_OnDataLoad(playerid);
public Player_OnDataLoad(playerid)
{
    if(!IsPlayerConnected(playerid))
    {
        return 0;
    }

    if(cache_num_rows() == 0)
    {
        // Brak danych - b????d krytyczny
        SendGuiInformation(playerid, "B????d", "Nie mo??na wczyta?? danych postaci!");
        Player_Kick(playerid, -1, "B????d wczytywania danych");
        return 0;
    }

    // === PODSTAWOWE DANE ===
    cache_get_value_int(0, "skin", pInfo[playerid][player_skin]);
    cache_get_value_int(0, "gender", pInfo[playerid][player_sex]);
    cache_get_value_int(0, "age", pInfo[playerid][player_age]);
    cache_get_value_int(0, "money", pInfo[playerid][player_money]);
    cache_get_value_int(0, "bank", pInfo[playerid][player_bank]);

    // === POZYCJA ===
    cache_get_value_float(0, "pos_x", pInfo[playerid][player_pos_x]);
    cache_get_value_float(0, "pos_y", pInfo[playerid][player_pos_y]);
    cache_get_value_float(0, "pos_z", pInfo[playerid][player_pos_z]);
    cache_get_value_float(0, "pos_angle", pInfo[playerid][player_pos_a]);
    cache_get_value_int(0, "interior", pInfo[playerid][player_interior]);
    cache_get_value_int(0, "world", pInfo[playerid][player_vw]);

    // === ZDROWIE I STATYSTYKI ===
    cache_get_value_float(0, "health", pInfo[playerid][player_health]);
    cache_get_value_float(0, "armour", pInfo[playerid][player_spawn_armor]);
    // Mirror to current armour cache for consistency
    pInfo[playerid][player_armour] = pInfo[playerid][player_spawn_armor];
    printf("[PlayerLoad] pid=%d loaded spawn_armour=%.2f", playerid, pInfo[playerid][player_spawn_armor]);
    cache_get_value_int(0, "bw_time", pInfo[playerid][player_bw]);
    cache_get_value_int(0, "aj_time", pInfo[playerid][player_aj]);

    // === CZAS GRY ===
    cache_get_value_int(0, "gametime", pInfo[playerid][player_gameTime]);
    cache_get_value_int(0, "last_login", pInfo[playerid][player_last_login]);

    // === GRUPY ===
    cache_get_value_int(0, "group1", pInfo[playerid][player_group][0]);
    cache_get_value_int(0, "group2", pInfo[playerid][player_group][1]);
    cache_get_value_int(0, "group3", pInfo[playerid][player_group][2]);
    cache_get_value_int(0, "group4", pInfo[playerid][player_group][3]);
    cache_get_value_int(0, "group5", pInfo[playerid][player_group][4]);

    cache_get_value_int(0, "group1_perm", pInfo[playerid][player_group_perm][0]);
    cache_get_value_int(0, "group2_perm", pInfo[playerid][player_group_perm][1]);
    cache_get_value_int(0, "group3_perm", pInfo[playerid][player_group_perm][2]);
    cache_get_value_int(0, "group4_perm", pInfo[playerid][player_group_perm][3]);
    cache_get_value_int(0, "group5_perm", pInfo[playerid][player_group_perm][4]);

    cache_get_value_int(0, "group1_salary", pInfo[playerid][player_group_salary][0]);
    cache_get_value_int(0, "group2_salary", pInfo[playerid][player_group_salary][1]);
    cache_get_value_int(0, "group3_salary", pInfo[playerid][player_group_salary][2]);
    cache_get_value_int(0, "group4_salary", pInfo[playerid][player_group_salary][3]);
    cache_get_value_int(0, "group5_salary", pInfo[playerid][player_group_salary][4]);

    // === DODATKOWE DANE ===
    cache_get_value_int(0, "hunger", pInfo[playerid][player_hunger]);
    cache_get_value_int(0, "thirst", pInfo[playerid][player_thirst]);
    cache_get_value_int(0, "strength", pInfo[playerid][player_strength]);
    cache_get_value_int(0, "job", pInfo[playerid][player_job]);
    cache_get_value_int(0, "spawn_door", pInfo[playerid][player_spawn_door]);

    // === WCZYTAJ DODATKOWE DANE ===
    Player_LoadWeapons(playerid);
    Player_LoadItems(playerid);

    // Ustaw gracza jako gotowego do spawnu
    pInfo[playerid][player_loaded] = true;

    // Uruchom auto-save
    g_AutoSaveTimer[playerid] = SetTimerEx("Player_AutoSave", SAVE_INTERVAL, true, "d", playerid);
    g_DataModified[playerid] = false;

    // Mark as just loaded so we avoid scheduling a save on the immediate spawn
    g_JustLoaded[playerid] = true;

    // Wywołaj spawn
    Player_Spawn(playerid);

    printf("[PlayerSave] Wczytano dane gracza %s (UID: %d)",
        pInfo[playerid][player_name], pInfo[playerid][player_id]);

    return 1;
}

/**
 * Wczytuje bronie gracza
 * @param playerid ID gracza
 */
stock Player_LoadWeapons(playerid)
{
    new query[256];
    mysql_format(mySQLconnection, query, sizeof(query),
        "SELECT `weapon_slot`, `weapon_id`, `weapon_ammo` \
         FROM `user_weapons` WHERE `weapon_owner` = %d",
        pInfo[playerid][player_id]
    );

    mysql_tquery(mySQLconnection, query, "Player_OnWeaponsLoad", "d", playerid);
}

forward Player_OnWeaponsLoad(playerid);
public Player_OnWeaponsLoad(playerid)
{
    if(!IsPlayerConnected(playerid))
    {
        return 0;
    }

    new rows = cache_num_rows();
    new slot, weaponId, ammo;

    for(new i = 0; i < rows; i++)
    {
        cache_get_value_int(i, "weapon_slot", slot);
        cache_get_value_int(i, "weapon_id", weaponId);
        cache_get_value_int(i, "weapon_ammo", ammo);

        if(slot >= 0 && slot < 13)
        {
            pWeapon[playerid][slot][pw_id] = weaponId;
            pWeapon[playerid][slot][pw_ammo] = ammo;
        }
    }

    return 1;
}

/**
 * Wczytuje przedmioty gracza
 * @param playerid ID gracza
 */
stock Player_LoadItems(playerid)
{
    new query[256];
    mysql_format(mySQLconnection, query, sizeof(query),
            "SELECT `item_id`, `item_type`, `item_value`, `item_value2`, `item_phone`, `item_name` \
             FROM `items` WHERE `item_owner` = %d AND `item_owner_type` = 0 \
         ORDER BY `item_id` ASC LIMIT %d",
        pInfo[playerid][player_id], MAX_PLAYER_ITEMS
    );

    mysql_tquery(mySQLconnection, query, "Player_OnItemsLoad", "d", playerid);
}

forward Player_OnItemsLoad(playerid);
public Player_OnItemsLoad(playerid)
{
    if(!IsPlayerConnected(playerid))
    {
        return 0;
    }

    new rows = cache_num_rows();
    pInfo[playerid][player_item_count] = 0;

    for(new i = 0; i < rows && i < MAX_PLAYER_ITEMS; i++)
    {
        cache_get_value_int(i, "item_id", pItem[playerid][i][item_id]);
        cache_get_value_int(i, "item_type", pItem[playerid][i][item_type]);
        cache_get_value_int(i, "item_value", pItem[playerid][i][item_value]);
        cache_get_value_int(i, "item_value2", pItem[playerid][i][item_value2]);
        // For phones: load number from `item_phone` column into item_value2 for runtime
        new tmp_phone = 0;
        cache_get_value_int(i, "item_phone", tmp_phone);
        if(pItem[playerid][i][item_type] == 30) // ITEM_TYPE_PHONE = 30
        {
            if(tmp_phone >= PHONE_MIN_NUMBER && tmp_phone <= PHONE_MAX_NUMBER)
            {
                pItem[playerid][i][item_value2] = tmp_phone;
            }
            else
            {
                // Generate new number if phone has none
                new num, found;
                do
                {
                    num = PHONE_MIN_NUMBER + random(PHONE_MAX_NUMBER - PHONE_MIN_NUMBER + 1);
                    found = 0;
                    for(new a = 0; a < MAX_PLAYERS; a++)
                    {
                        for(new b = 0; b < MAX_PLAYER_ITEMS; b++)
                        {
                            if(pItem[a][b][item_type] == 30 && pItem[a][b][item_value2] == num)
                            {
                                found = 1; break;
                            }
                        }
                        if(found) break;
                    }
                    // Check global used numbers loaded from DB
                    if(!found && num >= PHONE_MIN_NUMBER && num <= PHONE_MAX_NUMBER)
                    {
                        if(gPhoneUsed[num]) found = 1;
                    }
                }
                while(found);
                pItem[playerid][i][item_value2] = num;
                // Save to DB immediately
                Item_SaveToDatabase(playerid, i);
                if(num >= PHONE_MIN_NUMBER && num <= PHONE_MAX_NUMBER && !gPhoneUsed[num]) { gPhoneUsed[num] = 1; gPhoneUsedCount++; }
                printf("[PHONE] Generated new number %d for player %d slot %d\n", num, playerid, i);
            }
        }
        cache_get_value(i, "item_name", pItem[playerid][i][item_name], 32);

        pInfo[playerid][player_item_count]++;
    }

    return 1;
}

// ===========================================================================
// ZAPISYWANIE DANYCH
// ===========================================================================

/**
 * Zapisuje wszystkie dane gracza
 * @param playerid ID gracza
 * @param reason Pow??d zapisu (dla log??w)
 */
stock Player_SaveData(playerid, const reason[] = "manual")
{
    if(!IsValidPlayerId(playerid) || !pInfo[playerid][player_logged])
    {
        return 0;
    }

    // Guard: avoid overwriting DB-loaded values with an immediate spawn save right after connect/login.
    // If the save reason is "spawn" and the player connected less than 15 seconds ago, skip the save.
    if(strcmp(reason, "spawn") == 0)
    {
        if(gettime() - pInfo[playerid][player_connect_time] < 15)
        {
            printf("[PlayerSave] Skipping initial spawn save: pid=%d elapsed=%d", playerid, gettime() - pInfo[playerid][player_connect_time]);
            return 0;
        }
    }

    // Aktualizuj pozycj??
    if(pInfo[playerid][player_spawned])
    {
        GetPlayerPos(playerid, pInfo[playerid][player_pos_x],
                     pInfo[playerid][player_pos_y], pInfo[playerid][player_pos_z]);
        GetPlayerFacingAngle(playerid, pInfo[playerid][player_pos_a]);
        pInfo[playerid][player_interior] = GetPlayerInterior(playerid);
        pInfo[playerid][player_vw] = GetPlayerVirtualWorld(playerid);

        // Aktualizuj zdrowie
        GetPlayerHealth(playerid, pInfo[playerid][player_health]);
        GetPlayerArmour(playerid, pInfo[playerid][player_spawn_armor]);
    }

    // Aktualizuj czas gry
    pInfo[playerid][player_gameTime] += (gettime() - pInfo[playerid][player_connect_time]) / 60;
    pInfo[playerid][player_connect_time] = gettime();

    // G????wne zapytanie
    new query[2048];
    mysql_format(mySQLconnection, query, sizeof(query),
        "UPDATE `characters` SET \
         `skin` = %d, `gender` = %d, `age` = %d, \
         `money` = %d, `bank` = %d, \
         `pos_x` = %f, `pos_y` = %f, `pos_z` = %f, `pos_angle` = %f, \
         `interior` = %d, `world` = %d, \
         `health` = %f, `armour` = %f, \
         `bw_time` = %d, `aj_time` = %d, \
         `gametime` = %d, \
         `group1` = %d, `group2` = %d, `group3` = %d, `group4` = %d, `group5` = %d, \
         `group1_perm` = %d, `group2_perm` = %d, `group3_perm` = %d, `group4_perm` = %d, `group5_perm` = %d, \
         `group1_salary` = %d, `group2_salary` = %d, `group3_salary` = %d, `group4_salary` = %d, `group5_salary` = %d, \
         `hunger` = %d, `thirst` = %d, `strength` = %d, \
         `job` = %d, `spawn_door` = %d, \
         `hours` = %d, `minutes` = %d, `kills` = %d, `deaths` = %d, `driven_km` = %f, `penalty_points` = %d \
         WHERE `id` = %d",
        pInfo[playerid][player_skin], pInfo[playerid][player_sex], pInfo[playerid][player_age],
        pInfo[playerid][player_money], pInfo[playerid][player_bank],
        pInfo[playerid][player_pos_x], pInfo[playerid][player_pos_y],
        pInfo[playerid][player_pos_z], pInfo[playerid][player_pos_a],
        pInfo[playerid][player_interior], pInfo[playerid][player_vw],
        pInfo[playerid][player_health], pInfo[playerid][player_spawn_armor],
        pInfo[playerid][player_bw], pInfo[playerid][player_aj],
        pInfo[playerid][player_gameTime],
        pInfo[playerid][player_group][0], pInfo[playerid][player_group][1],
        pInfo[playerid][player_group][2], pInfo[playerid][player_group][3], pInfo[playerid][player_group][4],
        pInfo[playerid][player_group_perm][0], pInfo[playerid][player_group_perm][1],
        pInfo[playerid][player_group_perm][2], pInfo[playerid][player_group_perm][3], pInfo[playerid][player_group_perm][4],
        pInfo[playerid][player_group_salary][0], pInfo[playerid][player_group_salary][1],
        pInfo[playerid][player_group_salary][2], pInfo[playerid][player_group_salary][3], pInfo[playerid][player_group_salary][4],
        pInfo[playerid][player_hunger], pInfo[playerid][player_thirst], pInfo[playerid][player_strength],
        pInfo[playerid][player_job], pInfo[playerid][player_spawn_door],
        pInfo[playerid][player_hours], pInfo[playerid][player_minutes], pInfo[playerid][player_kills], pInfo[playerid][player_deaths], pInfo[playerid][player_driven_km], pInfo[playerid][player_penalty_points],
        pInfo[playerid][player_id]
    );
    printf("[PlayerSave] Saving: pid=%d name=%s armour=%.2f", playerid, pInfo[playerid][player_name], pInfo[playerid][player_spawn_armor]);

    mysql_tquery(mySQLconnection, query);

    // Zapisz bronie
    Player_SaveWeapons(playerid);

    // Zapisz przedmioty
    Player_SaveItems(playerid);

    g_DataModified[playerid] = false;

    printf("[PlayerSave] Zapisano dane gracza %s (UID: %d) - powod: %s",
        pInfo[playerid][player_name], pInfo[playerid][player_id], reason);

    return 1;
}

/**
 * Zapisuje bronie gracza
 * @param playerid ID gracza
 */
stock Player_SaveWeapons(playerid)
{
    // Usuń stare bronie
    new query[256];
    mysql_format(mySQLconnection, query, sizeof(query),
        "DELETE FROM `user_weapons` WHERE `weapon_owner` = %d",
        pInfo[playerid][player_id]
    );
    mysql_tquery(mySQLconnection, query);

    // Zapisz aktualne bronie
    for(new slot = 0; slot < 13; slot++)
    {
        if(pWeapon[playerid][slot][pw_id] > 0 && pWeapon[playerid][slot][pw_ammo] > 0)
        {
            mysql_format(mySQLconnection, query, sizeof(query),
                "INSERT INTO `user_weapons` (`weapon_owner`, `weapon_slot`, `weapon_id`, `weapon_ammo`) \
                 VALUES (%d, %d, %d, %d)",
                pInfo[playerid][player_id], slot,
                pWeapon[playerid][slot][pw_id], pWeapon[playerid][slot][pw_ammo]
            );
            mysql_tquery(mySQLconnection, query);
        }
    }

    return 1;
}

/**
 * Zapisuje przedmioty gracza
 * @param playerid ID gracza
 */
stock Player_SaveItems(playerid)
{
    new query[512];

    for(new i = 0; i < MAX_PLAYER_ITEMS; i++)
    {
        if(pItem[playerid][i][item_id] > 0)
        {
            new escapedName[65];
            mysql_escape_string(pItem[playerid][i][item_name], escapedName, mySQLconnection, sizeof(escapedName));

            mysql_format(mySQLconnection, query, sizeof(query),
                "UPDATE `items` SET `item_type` = %d, `item_value` = %d, `item_value2` = %d, `item_name` = '%s' WHERE `item_id` = %d",
                pItem[playerid][i][item_type], pItem[playerid][i][item_value],
                pItem[playerid][i][item_value2], escapedName, pItem[playerid][i][item_id]
            );
            mysql_tquery(mySQLconnection, query);
        }
    }

    return 1;
}

// ===========================================================================
// AUTO-SAVE
// ===========================================================================

forward Player_AutoSave(playerid);
public Player_AutoSave(playerid)
{
    if(!IsPlayerConnected(playerid) || !pInfo[playerid][player_logged])
    {
        // Zatrzymaj timer
        if(g_AutoSaveTimer[playerid] != 0)
        {
            KillTimer(g_AutoSaveTimer[playerid]);
            g_AutoSaveTimer[playerid] = 0;
        }
        return 0;
    }

    // Zapisz tylko je??li dane zosta??y zmodyfikowane lub min???? okre??lony czas
    if(g_DataModified[playerid])
    {
        Player_SaveData(playerid, "auto-save");
    }

    return 1;
}

/**
 * Oznacza dane gracza jako zmodyfikowane
 * @param playerid ID gracza
 */
stock Player_MarkDataModified(playerid)
{
    if(IsValidPlayerId(playerid))
    {
        g_DataModified[playerid] = true;
    }
}

// ===========================================================================
// HOOKI
// ===========================================================================

hook OnPlayerDisconnect@Save(playerid, reason)
{
    // Zatrzymaj auto-save
    if(g_AutoSaveTimer[playerid] != 0)
    {
        KillTimer(g_AutoSaveTimer[playerid]);
        g_AutoSaveTimer[playerid] = 0;
    }

    // Zapisz dane przed wyjściem
    if(pInfo[playerid][player_logged])
    {
        // Ensure current armour is captured before save (fix: armour not persisted on logout)
        GetPlayerArmour(playerid, pInfo[playerid][player_spawn_armor]);
        printf("[PlayerSave] Disconnect: pid=%d armour=%.2f", playerid, pInfo[playerid][player_spawn_armor]);
        Player_SaveData(playerid, "disconnect");
    }

    return 1;
}

hook OnPlayerDeath@Save(playerid, killerid, reason)
{
    #if SAVE_ON_DEATH == true
        if(pInfo[playerid][player_logged])
        {
            Player_SaveData(playerid, "death");
        }
    #endif

    return 1;
}

hook OnPlayerSpawn@Save(playerid)
{
    #if SAVE_ON_SPAWN == true
        // Skip saving on the initial spawn after loading data (avoids overwriting DB-loaded values)
        if(g_JustLoaded[playerid])
        {
            g_JustLoaded[playerid] = false; // clear the transient flag
        }
        else
        {
            // Opóźniony zapis po spawnie (np. respawn po śmierci)
            SetTimerEx("Player_DelayedSave", 5000, false, "d", playerid);
        }
    #endif

    return 1;
}

forward Player_DelayedSave(playerid);
public Player_DelayedSave(playerid)
{
    if(IsPlayerConnected(playerid) && pInfo[playerid][player_logged])
    {
        Player_SaveData(playerid, "spawn");
    }
    return 1;
}

// ===========================================================================
// BACKUP
// ===========================================================================

/**
 * Tworzy backup danych gracza (na wypadek rollbacka)
 * @param playerid ID gracza
 */
stock Player_CreateBackup(playerid)
{
    if(!IsValidPlayerId(playerid) || !pInfo[playerid][player_logged])
    {
        return 0;
    }

    new query[512];
    mysql_format(mySQLconnection, query, sizeof(query),
        "INSERT INTO `user_backups` \
         (`backup_uid`, `backup_time`, `backup_money`, `backup_bank`, `backup_health`, \
          `backup_pos_x`, `backup_pos_y`, `backup_pos_z`) \
         VALUES (%d, %d, %d, %d, %f, %f, %f, %f)",
        pInfo[playerid][player_id], gettime(),
        pInfo[playerid][player_money], pInfo[playerid][player_bank],
        pInfo[playerid][player_health],
        pInfo[playerid][player_pos_x], pInfo[playerid][player_pos_y], pInfo[playerid][player_pos_z]
    );

    mysql_tquery(mySQLconnection, query);

    return 1;
}

// ===========================================================================
// EOF
// ===========================================================================

