/*
 * =============================================================================
 *  CONFIG - Konfiguracja serwera
 * =============================================================================
 *
 *  Modu??: core/config.inc
 *  Opis: ??adowanie i zarz??dzanie konfiguracj?? serwera
 *
 *  ZMIANY W REFAKTORZE:
 *  - Usuni??to przestarza??y dini, zamieniono na natywny parser INI
 *  - Dodano walidacj?? konfiguracji
 *  - Dodano domy??lne warto??ci fallback
 *
 * =============================================================================
 */

// Wyciszenie warningów tag mismatch
#pragma warning disable 213

#if defined _config_included
    #endinput
#endif
#define _config_included

// ===========================================================================
// STRUKTURA KONFIGURACJI
// ===========================================================================

enum e_settings
{
    // MySQL
    setting_mysql_hostname[32],
    setting_mysql_username[32],
    setting_mysql_password[64],
    setting_mysql_database[32],
    setting_mysql_port,

    // Serwer
    setting_server_name[64],
    setting_server_website[64],
    setting_server_maxplayers,

    // Gameplay
    setting_spawn_money,
    Float:setting_spawn_health,
    setting_payday_interval,
    setting_start_money,
    setting_welfare_amount,

    // Spawn position
    Float:setting_spawn_pos_x,
    Float:setting_spawn_pos_y,
    Float:setting_spawn_pos_z,
    Float:setting_spawn_pos_a,

    // Bezpieczeństwo
    setting_max_login_attempts,
    setting_kick_delay,
    setting_anticheat_enabled
}
new Setting[e_settings];

// ===========================================================================
// ??ADOWANIE KONFIGURACJI
// ===========================================================================

/**
 * ??aduje konfiguracj?? z pliku INI
 * Je??li plik nie istnieje, tworzy domy??ln?? konfiguracj??
 */
static ConfigPath[128];

stock LoadConfiguration()
{
    new loadStart = GetTickCount();

    // Ustal ścieżkę - sprawdź gdzie jest config
    strcopy(ConfigPath, CONFIG_DEFAULT_PATH, sizeof(ConfigPath));

    if(!fexist(ConfigPath))
    {
        // Sprawdź ścieżkę zapasową
        if(fexist(CONFIG_FALLBACK_PATH))
        {
            strcopy(ConfigPath, CONFIG_FALLBACK_PATH, sizeof(ConfigPath));
            printf("[Config] Używam zapasowej ścieżki: %s", ConfigPath);
        }
        else
        {
            // Brak pliku - użyj domyślnych wartości
            printf("[Config] Brak pliku konfiguracji - używam domyślnych wartości");
            SetDefaultConfiguration();
            return 0;
        }
    }
    else
    {
        printf("[Config] Używam głównej ścieżki: %s", ConfigPath);
    }

    // Otwórz plik
    new File:file = fopen(ConfigPath, io_read);
    if(!file)
    {
        printf("[BŁĄD] Nie można otworzyć pliku konfiguracji (%s)!", ConfigPath);
        SetDefaultConfiguration();
        return 0;
    }

    new line[256], key[64], value[128];
    new section[32];
    strcopy(section, "", 32);

    while(fread(file, line))
    {
        // Usuń białe znaki i newline
        StripNewLine(line);

        // Pomiń puste linie i komentarze
        if(strlen(line) < 3 || line[0] == ';' || line[0] == '#')
        {
            continue;
        }

        // Parsuj sekcję [section]
        if(line[0] == '[' && line[strlen(line)-1] == ']')
        {
            strmid(section, line, 1, strlen(line)-1);
            continue;
        }

        // Parsuj linię key=value (toleruje spacje i cudzysłowy)
        if(sscanf(line, "s[64] =s[128]", key, value))
        {
            continue;
        }

        TrimWhitespace(key);
        TrimWhitespace(value);
        StripQuotes(value);

        // Przypisz wartości - spawn position z sekcji gameplay
        if(!strcmp(section, "gameplay", true))
        {
            if(!strcmp(key, "spawn_pos_x", true))
            {
                Setting[setting_spawn_pos_x] = floatstr(value);
            }
            else if(!strcmp(key, "spawn_pos_y", true))
            {
                Setting[setting_spawn_pos_y] = floatstr(value);
            }
            else if(!strcmp(key, "spawn_pos_z", true))
            {
                Setting[setting_spawn_pos_z] = floatstr(value);
            }
            else if(!strcmp(key, "spawn_pos_a", true))
            {
                Setting[setting_spawn_pos_a] = floatstr(value);
            }
            else if(!strcmp(key, "start_money", true))
            {
                Setting[setting_start_money] = strval(value);
            }
            else if(!strcmp(key, "max_login_attempts", true))
            {
                Setting[setting_max_login_attempts] = strval(value);
            }
        }
        // MySQL sekcja
        else if(!strcmp(section, "mysql", true))
        {
            if(!strcmp(key, "host", true))
            {
                strcopy(Setting[setting_mysql_hostname], value, 32);
            }
            else if(!strcmp(key, "user", true))
            {
                strcopy(Setting[setting_mysql_username], value, 32);
            }
            else if(!strcmp(key, "password", true))
            {
                strcopy(Setting[setting_mysql_password], value, 64);
            }
            else if(!strcmp(key, "database", true))
            {
                strcopy(Setting[setting_mysql_database], value, 32);
            }
            else if(!strcmp(key, "port", true))
            {
                Setting[setting_mysql_port] = strval(value);
            }
        }
        // Server sekcja
        else if(!strcmp(section, "server", true))
        {
            if(!strcmp(key, "name", true))
            {
                strcopy(Setting[setting_server_name], value, 64);
            }
        }
        // Anticheat sekcja
        else if(!strcmp(section, "anticheat", true))
        {
            if(!strcmp(key, "enabled", true))
            {
                Setting[setting_anticheat_enabled] = _:ParseBool(value);
            }
        }
    }

    fclose(file);

    // Walidacja
    ValidateConfiguration();

    // Inicjalizacja element??w gry
    InitializeGameElements();

    printf("[Config] Config Loaded [czas: %d ms]", GetTickCount() - loadStart);
    return 1;
}

/**
 * Ustawia domy??lne warto??ci konfiguracji
 */
stock SetDefaultConfiguration()
{
    strcopy(Setting[setting_mysql_hostname], "127.0.0.1", 32);
    strcopy(Setting[setting_mysql_username], "root", 32);
    strcopy(Setting[setting_mysql_password], "admin", 64);
    strcopy(Setting[setting_mysql_database], "samptesty", 32);
    Setting[setting_mysql_port] = 3306;

    strcopy(Setting[setting_server_name], "OpenRP Roleplay", 64);
    strcopy(Setting[setting_server_website], "OpenRP.pl", 64);
    Setting[setting_server_maxplayers] = MAX_PLAYERS;

    Setting[setting_spawn_money] = 0;
    Setting[setting_spawn_health] = 100.0;
    Setting[setting_payday_interval] = 3600;

    // Domyślny spawn
    Setting[setting_spawn_pos_x] = 1607.5580;
    Setting[setting_spawn_pos_y] = 1820.9547;
    Setting[setting_spawn_pos_z] = 10.8203;
    Setting[setting_spawn_pos_a] = 180.0;

    Setting[setting_max_login_attempts] = 5;
    Setting[setting_kick_delay] = 500;
    Setting[setting_anticheat_enabled] = 1;
}

/**
 * Waliduje za??adowan?? konfiguracj??
 */
stock ValidateConfiguration()
{
    // MySQL
    if(strlen(Setting[setting_mysql_hostname]) < 1)
    {
        strcopy(Setting[setting_mysql_hostname], "127.0.0.1", 32);
        print("[Config] Ostrze??enie: Brak hostname MySQL, u??yto 127.0.0.1");
    }

    if(Setting[setting_mysql_port] <= 0 || Setting[setting_mysql_port] > 65535)
    {
        Setting[setting_mysql_port] = 3306;
        print("[Config] Ostrze??enie: Nieprawid??owy port MySQL, u??yto 3306");
    }

    // Bezpiecze??stwo
    if(Setting[setting_max_login_attempts] < 1)
    {
        Setting[setting_max_login_attempts] = 5;
    }

    if(Setting[setting_kick_delay] < 100)
    {
        Setting[setting_kick_delay] = 500;
    }

    // Spawn fallback je?li w configu brak wpisu / b??d
    if(Setting[setting_spawn_pos_z] == 0.0)
    {
        Setting[setting_spawn_pos_x] = 1607.5580;
        Setting[setting_spawn_pos_y] = 1820.9547;
        Setting[setting_spawn_pos_z] = 10.8203;
        Setting[setting_spawn_pos_a] = 180.0;
    }
}

/**
 * Inicjalizuje elementy gry po za??adowaniu konfiguracji
 */
stock InitializeGameElements()
{
    // Tworzenie labeli dla graczy (cache)
    for(new i = 0; i < MAX_PLAYERS; i++)
    {
        pInfo[i][player_label] = Create3DTextLabel("", 0xFFFFFF60, 0.0, 0.0, 0.0, 12.0, 0, 1);
        pInfo[i][player_description_label] = Create3DTextLabel("", LABEL_DESCRIPTION, 0.0, 0.0, 0.0, 4.0, 0, 1);
    }

    // Ko??o m??y??skie (Ferris Wheel)
    InitializeFerrisWheel();

    // Aktualizacja daty na textdrawie
    UpdateDateTextdraw();

    // Pickupy pracy
    InitializeJobPickups();

    // Ustaw tryb gry
    SetGameModeText("OpenRP BETA");
}

/**
 * Inicjalizuje ko??o m??y??skie
 */
stock InitializeFerrisWheel()
{
    FerrisWheelObjects[10] = CreateObject(18877, 389.7734, -2028.4688, 22, 0, 0, 90, 300);
    FerrisWheelObjects[11] = CreateObject(18878, 389.7734, -2028.4688, 22, 0, 0, 90, 300);

    for(new x = 0; x < sizeof(FerrisWheelObjects) - 2; x++)
    {
        FerrisWheelObjects[x] = CreateObject(18879, 389.7734, -2028.4688, 22, 0, 0, 90, 300);
        AttachObjectToObject(FerrisWheelObjects[x], FerrisWheelObjects[10],
            gFerrisCageOffsets[x][0], gFerrisCageOffsets[x][1], gFerrisCageOffsets[x][2],
            0.0, 0.0, 90, 0);
    }

    SetTimer("RotateFerrisWheel", 3000, false);
}

/**
 * Aktualizuje textdraw z dat??
 */
stock UpdateDateTextdraw()
{
    new year, month, day;
    getdate(year, month, day);

    new str[64];
    format(str, sizeof(str), "~y~OpenRP~w~~n~%02d.%02d.%d", day, month, year);
    TextDrawSetString(Textdraw2, str);
}

/**
 * Inicjalizuje pickupy prac
 */
stock InitializeJobPickups()
{
    job_pickup = CreateDynamicPickup(1210, 2, 1495.9677, -1749.3802, 15.4453);
    Pickup[job_pickup][pickup_type] = PICKUP_TYPE_JOB;

    gov_pickup = CreateDynamicPickup(1210, 2, 1490.9677, -1729.3802, 15.4453);
    Pickup[gov_pickup][pickup_type] = PICKUP_TYPE_GOV;
}

// ===========================================================================
// FUNKCJE POMOCNICZE
// ===========================================================================

/**
 * Usuwa znaki nowej linii z ko??ca stringa
 */
stock StripNewLine(string[])
{
    new len = strlen(string);

    while(len > 0 && (string[len - 1] == '\r' || string[len - 1] == '\n'))
    {
        string[--len] = EOS;
    }

    return len;
}

stock TrimWhitespace(string[])
{
    new start = 0, end = strlen(string);
    while(start < end && (string[start] == ' ' || string[start] == '\t')) start++;
    while(end > start && (string[end - 1] == ' ' || string[end - 1] == '\t')) end--;
    if(start > 0) strdel(string, 0, start);
    string[end - start] = EOS;
    return end - start;
}

stock StripQuotes(string[])
{
    new len = strlen(string);
    if(len >= 2 && string[0] == '"' && string[len - 1] == '"')
    {
        strdel(string, 0, 1);
        string[len - 2] = EOS;
    }
    return strlen(string);
}

stock bool:ParseBool(const string[])
{
    return !strcmp(string, "true", true) || !strcmp(string, "1", true) || !strcmp(string, "yes", true);
}

stock bool:CopyFile(const source[], const dest[])
{
    new File:src = fopen(source, io_read);
    if(!src) return false;

    new File:dst = fopen(dest, io_write);
    if(!dst)
    {
        fclose(src);
        return false;
    }

    new buffer[256];
    while(fread(src, buffer))
    {
        fwrite(dst, buffer);
    }

    fclose(src);
    fclose(dst);
    return true;
}

// ===========================================================================
// EOF
// ===========================================================================

