/*
 * =============================================================================
 *  PLAYER AUTH - System kont i postaci
 * =============================================================================
 *  
 *  Modul: player/player_auth.inc
 *  Opis: System kont z wieloma postaciami, logowanie, rejestracja
 *
 *  NOWY SYSTEM:
 *  - Konto glowne (account) - logowanie
 *  - Postacie (characters) - max 1 dla non-premium, max 3 dla premium
 *  - Tworzenie postaci z wyborem plci i skina
 *
 * =============================================================================
 */

// Wyciszenie warningów tag mismatch
#pragma warning disable 213

#if defined _player_auth_included
    #endinput
#endif
#define _player_auth_included

// Forward: skin picker callback storage handled in skin_selection.inc


// ===========================================================================
// STAlE
// ===========================================================================

#define MAX_LOGIN_ATTEMPTS          3       // Max prob logowania
#define LOGIN_TIMEOUT              180      // Timeout logowania (sekundy) - 3 minuty
#define MIN_PASSWORD_LENGTH         6       // Min dlugosc hasla
#define MAX_PASSWORD_LENGTH        32       // Max dlugosc hasla

#define MAX_CHARACTERS_NORMAL       1       // Max postaci dla non-premium
#define MAX_CHARACTERS_PREMIUM      3       // Max postaci dla premium

#define GENDER_MALE                 0
#define GENDER_FEMALE               1

// Stany autoryzacji
enum E_AUTH_STATE
{
    AUTH_STATE_NONE,
    AUTH_STATE_CHECKING,
    AUTH_STATE_LOGIN,
    AUTH_STATE_REGISTER,
    AUTH_STATE_REGISTER_CONFIRM,
    AUTH_STATE_CHARACTER_SELECT,
    AUTH_STATE_CHARACTER_CREATE_NAME,
    AUTH_STATE_CHARACTER_CREATE_GENDER,
    AUTH_STATE_CHARACTER_CREATE_SKIN,
    AUTH_STATE_LOGGED
};

// Dialogi autoryzacji
#define DIALOG_AUTH_LOGIN           1500
#define DIALOG_AUTH_REGISTER        DIALOG_REGISTER
#define DIALOG_AUTH_REGISTER_CONFIRM DIALOG_REGISTER_CONFIRM
#define DIALOG_AUTH_CHAR_SELECT     1503
#define DIALOG_AUTH_CHAR_NAME       1504
#define DIALOG_AUTH_CHAR_GENDER     1505
#define DIALOG_AUTH_CHAR_SKIN       1506
#define DIALOG_AUTH_SPAWN_SELECT    1507

// ===========================================================================
// STRUKTURA KONTA
// ===========================================================================

enum e_account_info
{
    acc_id,
    acc_username[MAX_PLAYER_NAME],
    acc_password[256],
    acc_email[64],
    acc_ip[16],
    acc_admin_level,
    acc_vip_level,
    acc_vip_expire,
    acc_banned,
    acc_ban_reason[128],
    acc_warns,
    acc_created_at,
    acc_last_login,
    acc_total_playtime
};

// Dane tymczasowej postaci (podczas tworzenia)
enum e_temp_character
{
    temp_name[MAX_PLAYER_NAME],
    temp_gender,
    temp_skin
};

// ===========================================================================
// ZMIENNE
// ===========================================================================

static g_AccountData[MAX_PLAYERS][e_account_info];
static E_AUTH_STATE:g_AuthState[MAX_PLAYERS];
static g_LoginAttempts[MAX_PLAYERS];
static g_LoginTimer[MAX_PLAYERS];
static g_TempPassword[MAX_PLAYERS][MAX_PASSWORD_LENGTH + 1];
static g_TempCharacter[MAX_PLAYERS][e_temp_character];
static g_CharacterList[MAX_PLAYERS][MAX_CHARACTERS_PREMIUM][64]; // Lista nazw postaci
static g_CharacterCount[MAX_PLAYERS];
static g_CharacterIds[MAX_PLAYERS][MAX_CHARACTERS_PREMIUM]; // ID postaci
static g_PendingCharacterIndex[MAX_PLAYERS] = {-1, ...};
static g_PendingPassword[MAX_PLAYERS][MAX_PASSWORD_LENGTH + 1]; // Hasło czekające na bcrypt hash

// ===========================================================================
// FUNKCJE GloWNE
// ===========================================================================

/**
 * Resetuje dane konta gracza
 */
static stock Auth_ResetAccountData(playerid)
{
    g_AccountData[playerid][acc_id] = 0;
    g_AccountData[playerid][acc_username][0] = EOS;
    g_AccountData[playerid][acc_password][0] = EOS;
    g_AccountData[playerid][acc_email][0] = EOS;
    g_AccountData[playerid][acc_ip][0] = EOS;
    g_AccountData[playerid][acc_admin_level] = 0;
    g_AccountData[playerid][acc_vip_level] = 0;
    g_AccountData[playerid][acc_vip_expire] = 0;
    g_AccountData[playerid][acc_banned] = 0;
    g_AccountData[playerid][acc_ban_reason][0] = EOS;
    g_AccountData[playerid][acc_warns] = 0;
    g_AccountData[playerid][acc_created_at] = 0;
    g_AccountData[playerid][acc_last_login] = 0;
    g_AccountData[playerid][acc_total_playtime] = 0;
    
    for(new i = 0; i < MAX_CHARACTERS_PREMIUM; i++)
    {
        g_CharacterList[playerid][i][0] = EOS;
        g_CharacterIds[playerid][i] = 0;
    }
}

/**
 * Rozpoczyna proces autoryzacji gracza
 * @param playerid ID gracza
 */
stock Auth_Start(playerid)
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }
    
    // Resetuj stan
    g_AuthState[playerid] = AUTH_STATE_CHECKING;
    g_LoginAttempts[playerid] = 0;
    g_TempPassword[playerid][0] = EOS;
    g_CharacterCount[playerid] = 0;
    
    // Wyczysc dane konta
    Auth_ResetAccountData(playerid);
    
    // Sprawdz czy konto istnieje
    Auth_CheckAccount(playerid);
    
    // Ustaw timeout
    g_LoginTimer[playerid] = SetTimerEx("Auth_Timeout", LOGIN_TIMEOUT * 1000, false, "i", playerid);
    
    return 1;
}

/**
 * Sprawdza czy konto gracza istnieje w bazie
 * @param playerid ID gracza
 */
static stock Auth_CheckAccount(playerid)
{
    new query[256];
    new name[MAX_PLAYER_NAME + 1];
    
    GetPlayerName(playerid, name, sizeof(name));
    
    mysql_format(mySQLconnection, query, sizeof(query),
        "SELECT * FROM `users` WHERE `username` = '%e' LIMIT 1", name);
    
    mysql_tquery(mySQLconnection, query, "Auth_OnAccountCheck", "i", playerid);
}

forward Auth_OnAccountCheck(playerid);
public Auth_OnAccountCheck(playerid)
{
    if(!IsPlayerConnected(playerid))
    {
        return 0;
    }
    
    if(cache_num_rows() > 0)
    {
        // Konto istnieje - wczytaj dane
        cache_get_value_int(0, "id", g_AccountData[playerid][acc_id]);
        cache_get_value(0, "username", g_AccountData[playerid][acc_username], MAX_PLAYER_NAME);
        cache_get_value(0, "password", g_AccountData[playerid][acc_password], 256);
        cache_get_value_int(0, "admin_level", g_AccountData[playerid][acc_admin_level]);
        cache_get_value_int(0, "vip_level", g_AccountData[playerid][acc_vip_level]);
        cache_get_value_int(0, "vip_expire", g_AccountData[playerid][acc_vip_expire]);
        cache_get_value_int(0, "banned", g_AccountData[playerid][acc_banned]);
        cache_get_value_int(0, "warns", g_AccountData[playerid][acc_warns]);
        cache_get_value_int(0, "created_at", g_AccountData[playerid][acc_created_at]);
        cache_get_value_int(0, "last_login", g_AccountData[playerid][acc_last_login]);
        cache_get_value_int(0, "total_playtime", g_AccountData[playerid][acc_total_playtime]);
        
        // Sprawdz ban
        if(g_AccountData[playerid][acc_banned])
        {
            Auth_HandleBan(playerid);
            return 0;
        }
        
        // Pokaz dialog logowania
        g_AuthState[playerid] = AUTH_STATE_LOGIN;
        Auth_ShowLoginDialog(playerid, false);
    }
    else
    {
        // Nowe konto - rejestracja
        g_AuthState[playerid] = AUTH_STATE_REGISTER;
        Auth_ShowRegisterDialog(playerid);
    }
    
    return 1;
}

// ===========================================================================
// LOGOWANIE
// ===========================================================================

/**
 * Wyswietla dialog logowania
 */
stock Auth_ShowLoginDialog(playerid, bool:wrongPassword = false)
{
    new name[MAX_PLAYER_NAME];
    GetPlayerName(playerid, name, sizeof(name));
    
    new caption[64];
    format(caption, sizeof(caption), "Logowanie - %s", name);
    
    new info[512];
    if(wrongPassword)
    {
        format(info, sizeof(info), "{FF6B6B}Nieprawidlowe haslo!{FFFFFF}\nPozostalo prob: %d/%d\n\nWprowadz haslo do swojego konta.\n\n{AAAAAA}Zapomniales hasla? Odwiedz strone serwera.", MAX_LOGIN_ATTEMPTS - g_LoginAttempts[playerid], MAX_LOGIN_ATTEMPTS);
    }
    else
    {
        format(info, sizeof(info), "{FFFFFF}Witaj ponownie na serwerze!\n\nWprowadz haslo do swojego konta.\n\n{AAAAAA}Konto: {FFFFFF}%s", name);
    }
    
    ShowPlayerDialog(playerid, DIALOG_AUTH_LOGIN, DIALOG_STYLE_PASSWORD, caption, info, "Zaloguj", "Wyjdz");
}

/**
 * Weryfikuje haslo logowania
 */
stock Auth_VerifyPassword(playerid, const password[])
{
    if(strlen(password) < MIN_PASSWORD_LENGTH)
    {
        g_LoginAttempts[playerid]++;
        
        if(g_LoginAttempts[playerid] >= MAX_LOGIN_ATTEMPTS)
        {
            Player_Kick(playerid, -1, "Przekroczono limit prob logowania");
            return 0;
        }
        
        Auth_ShowLoginDialog(playerid, true);
        return 0;
    }
    
    // Weryfikuj hasło asynchronicznie przez bcrypt
    bcrypt_verify(playerid, "Auth_OnPasswordVerified", password, g_AccountData[playerid][acc_password]);
    
    return 1;
}

forward Auth_OnPasswordVerified(playerid, bool:success);
public Auth_OnPasswordVerified(playerid, bool:success)
{
    if(!IsPlayerConnected(playerid))
    {
        return 0;
    }
    
    if(success)
    {
        // Hasło poprawne - załaduj listę postaci
        Auth_LoadCharacterList(playerid);
    }
    else
    {
        g_LoginAttempts[playerid]++;
        
        if(g_LoginAttempts[playerid] >= MAX_LOGIN_ATTEMPTS)
        {
            Player_Kick(playerid, -1, "Przekroczono limit prob logowania");
            return 0;
        }
        
        Auth_ShowLoginDialog(playerid, true);
    }
    
    return 1;
}

// ===========================================================================
// REJESTRACJA KONTA
// ===========================================================================

/**
 * Wyswietla dialog rejestracji
 */
stock Auth_ShowRegisterDialog(playerid)
{
    new name[MAX_PLAYER_NAME];
    GetPlayerName(playerid, name, sizeof(name));
    
    new caption[64];
    format(caption, sizeof(caption), "Rejestracja - %s", name);
    
    new info[512];
    format(info, sizeof(info), "{FFFFFF}Witaj na serwerze!\n\nTwoje konto nie istnieje w bazie danych.\nWprowadz haslo, ktore bedziesz uzywac do logowania.\n\n{AAAAAA}Wymagania hasla:\n{FFFFFF}- Minimum %d znakow\n- Maximum %d znakow\n- Zalecane: duze/male litery i cyfry", MIN_PASSWORD_LENGTH, MAX_PASSWORD_LENGTH);
    
    ShowPlayerDialog(playerid, DIALOG_AUTH_REGISTER, DIALOG_STYLE_PASSWORD, caption, info, "Dalej", "Wyjdz");
}

/**
 * Wyswietla dialog potwierdzenia hasla
 */
stock Auth_ShowRegisterConfirmDialog(playerid)
{
    new info[256] = "{FFFFFF}Powtorz wprowadzone haslo, aby potwierdzic rejestracje.\n\n{AAAAAA}Upewnij sie, ze hasla sa identyczne.";
    
    ShowPlayerDialog(playerid, DIALOG_AUTH_REGISTER_CONFIRM, DIALOG_STYLE_PASSWORD, "Potwierdz haslo", info, "Zarejestruj", "Wroc");
}

/**
 * Tworzy nowe konto
 */
stock Auth_CreateAccount(playerid, const password[])
{
    // Zapisz hasło tymczasowo - bcrypt hashuje asynchronicznie
    strcopy(g_PendingPassword[playerid], password, sizeof(g_PendingPassword[]));
    
    // Hashuj hasło przez bcrypt
    bcrypt_hash(playerid, "Auth_OnPasswordHashed", password, BCRYPT_COST);
}

forward Auth_OnPasswordHashed(playerid);
public Auth_OnPasswordHashed(playerid)
{
    if(!IsPlayerConnected(playerid))
    {
        return 0;
    }
    
    new name[MAX_PLAYER_NAME];
    GetPlayerName(playerid, name, sizeof(name));
    
    new ip[16];
    GetPlayerIp(playerid, ip, sizeof(ip));
    
    // Pobierz hash z bcrypt
    new hashedPassword[BCRYPT_HASH_LENGTH];
    bcrypt_get_hash(hashedPassword);
    
    // Wyczyść tymczasowe hasło
    g_PendingPassword[playerid][0] = EOS;
    
    new query[512];
    mysql_format(mySQLconnection, query, sizeof(query), "INSERT INTO `users` (`username`, `password`, `ip`, `created_at`, `last_login`) VALUES ('%e', '%e', '%e', %d, %d)", name, hashedPassword, ip, gettime(), gettime());
    
    mysql_tquery(mySQLconnection, query, "Auth_OnAccountCreated", "i", playerid);
    
    return 1;
}

forward Auth_OnAccountCreated(playerid);
public Auth_OnAccountCreated(playerid)
{
    if(!IsPlayerConnected(playerid))
    {
        return 0;
    }
    
    g_AccountData[playerid][acc_id] = cache_insert_id();
    
    new name[MAX_PLAYER_NAME];
    GetPlayerName(playerid, name, sizeof(name));
    strcopy(g_AccountData[playerid][acc_username], name, MAX_PLAYER_NAME);
    
    SendClientMessage(playerid, COLOR_SUCCESS, "Konto zostalo utworzone pomyslnie!");
    SendClientMessage(playerid, COLOR_INFO, "Teraz musisz stworzyc swoja pierwsza postac.");
    
    // Przejdz do tworzenia postaci
    g_CharacterCount[playerid] = 0;
    Auth_ShowCharacterNameDialog(playerid);
    
    return 1;
}

// ===========================================================================
// WYBoR POSTACI
// ===========================================================================

/**
 * laduje liste postaci dla konta
 */
stock Auth_LoadCharacterList(playerid)
{
    new query[256];
    mysql_format(mySQLconnection, query, sizeof(query),
        "SELECT `id`, `name`, `level`, `money`, `hours` FROM `characters` WHERE `account_id` = %d ORDER BY `last_save` DESC",
        g_AccountData[playerid][acc_id]);
    
    mysql_tquery(mySQLconnection, query, "Auth_OnCharacterListLoaded", "i", playerid);
}

forward Auth_OnCharacterListLoaded(playerid);
public Auth_OnCharacterListLoaded(playerid)
{
    if(!IsPlayerConnected(playerid))
    {
        return 0;
    }
    
    new rows = cache_num_rows();
    g_CharacterCount[playerid] = rows;
    
    if(rows == 0)
    {
        // Brak postaci - utworz nowa
        SendClientMessage(playerid, COLOR_INFO, "Nie masz zadnej postaci. Stworz swoja pierwsza postac!");
        Auth_ShowCharacterNameDialog(playerid);
        return 1;
    }
    
    // Wczytaj liste postaci
    for(new i = 0; i < rows && i < MAX_CHARACTERS_PREMIUM; i++)
    {
        cache_get_value_int(i, "id", g_CharacterIds[playerid][i]);
        cache_get_value(i, "name", g_CharacterList[playerid][i], 64);
    }
    
    // Pokaz dialog wyboru
    Auth_ShowCharacterSelectDialog(playerid);
    
    return 1;
}

/**
 * Wyswietla dialog wyboru postaci
 */
stock Auth_ShowCharacterSelectDialog(playerid)
{
    g_AuthState[playerid] = AUTH_STATE_CHARACTER_SELECT;
    
    new content[1024];
    new count = g_CharacterCount[playerid];
    new maxChars = Auth_GetMaxCharacters(playerid);
    
    // Naglowek
    strcopy(content, "Postac\tPoziom\n", sizeof(content));
    
    for(new i = 0; i < count; i++)
    {
        new line[128];
        format(line, sizeof(line), "%s\t-\n", g_CharacterList[playerid][i]);
        strcat(content, line);
    }
    
    // Opcja tworzenia nowej postaci (jesli mozna)
    if(count < maxChars)
    {
        strcat(content, "{2ECC71}+ Stworz nowa postac\t-\n");
    }
    
    new title[64];
    format(title, sizeof(title), "Wybierz postac (%d/%d)", count, maxChars);
    
    ShowPlayerDialog(playerid, DIALOG_AUTH_CHAR_SELECT, DIALOG_STYLE_TABLIST_HEADERS, 
        title, content, "Wybierz", "Wyjdz");
}

stock Auth_ShowSpawnChoiceDialog(playerid)
{
    ShowPlayerDialog(playerid, DIALOG_AUTH_SPAWN_SELECT, DIALOG_STYLE_LIST,
        "Wybierz miejsce spawnu",
        "Ostatnia pozycja (jesli zapisana)\nDomyslny spawn serwera",
        "OK", "Wroc");
}

/**
 * Pobiera maksymalna liczbe postaci dla gracza
 */
stock Auth_GetMaxCharacters(playerid)
{
    // Sprawdz czy ma VIP
    if(g_AccountData[playerid][acc_vip_level] > 0 && 
       g_AccountData[playerid][acc_vip_expire] > gettime())
    {
        return MAX_CHARACTERS_PREMIUM;
    }
    
    return MAX_CHARACTERS_NORMAL;
}

/**
 * Sprawdza czy gracz ma VIP
 */
stock bool:Auth_HasVIP(playerid)
{
    return (g_AccountData[playerid][acc_vip_level] > 0 && 
            g_AccountData[playerid][acc_vip_expire] > gettime());
}

/**
 * Wybiera postac do gry
 */
stock Auth_SelectCharacter(playerid, characterIndex)
{
    if(characterIndex < 0 || characterIndex >= g_CharacterCount[playerid])
    {
        return 0;
    }
    
    new characterId = g_CharacterIds[playerid][characterIndex];
    
    // Wczytaj pelne dane postaci
    Auth_LoadCharacterData(playerid, characterId);
    
    return 1;
}

/**
 * laduje dane wybranej postaci
 */
static stock Auth_LoadCharacterData(playerid, characterId)
{
    new query[256];
    mysql_format(mySQLconnection, query, sizeof(query),
        "SELECT * FROM `characters` WHERE `id` = %d LIMIT 1", characterId);
    
    mysql_tquery(mySQLconnection, query, "Auth_OnCharacterDataLoaded", "i", playerid);
}

forward Auth_OnCharacterDataLoaded(playerid);
public Auth_OnCharacterDataLoaded(playerid)
{
    if(!IsPlayerConnected(playerid))
    {
        return 0;
    }
    
    if(cache_num_rows() == 0)
    {
        SendClientMessage(playerid, COLOR_ERROR, "Blad wczytywania postaci!");
        Auth_ShowCharacterSelectDialog(playerid);
        return 0;
    }
    
    // Wczytaj dane do pInfo
    cache_get_value_int(0, "id", pInfo[playerid][player_uid]);
    pInfo[playerid][player_id] = pInfo[playerid][player_uid];
    cache_get_value_int(0, "account_id", pInfo[playerid][player_account_id]);
    cache_get_value(0, "name", pInfo[playerid][player_name], MAX_PLAYER_NAME);
    cache_get_value_int(0, "skin", pInfo[playerid][player_skin]);
    cache_get_value_int(0, "gender", pInfo[playerid][player_gender]);
    cache_get_value_int(0, "money", pInfo[playerid][player_money]);
    cache_get_value_int(0, "bank", pInfo[playerid][player_bank]);
    cache_get_value_int(0, "level", pInfo[playerid][player_level]);
    cache_get_value_int(0, "respect", pInfo[playerid][player_respect]);
    cache_get_value_int(0, "hours", pInfo[playerid][player_hours]);
    cache_get_value_int(0, "minutes", pInfo[playerid][player_minutes]);
    cache_get_value_float(0, "health", pInfo[playerid][player_health]);
    cache_get_value_float(0, "armour", pInfo[playerid][player_armour]);
    cache_get_value_float(0, "pos_x", pInfo[playerid][player_pos_x]);
    cache_get_value_float(0, "pos_y", pInfo[playerid][player_pos_y]);
    cache_get_value_float(0, "pos_z", pInfo[playerid][player_pos_z]);
    cache_get_value_float(0, "pos_angle", pInfo[playerid][player_pos_a]);
    cache_get_value_int(0, "interior", pInfo[playerid][player_interior]);
    cache_get_value_int(0, "world", pInfo[playerid][player_vw]);
    cache_get_value_int(0, "hunger", pInfo[playerid][player_hunger]);
    cache_get_value_int(0, "thirst", pInfo[playerid][player_thirst]);
    cache_get_value_int(0, "spawns", pInfo[playerid][player_spawns]);
    cache_get_value_int(0, "kills", pInfo[playerid][player_kills]);
    cache_get_value_int(0, "deaths", pInfo[playerid][player_deaths]);
    cache_get_value_float(0, "driven_km", pInfo[playerid][player_driven_km]);
    cache_get_value_int(0, "jail_time", pInfo[playerid][player_jail]);
    cache_get_value_int(0, "aj_time", pInfo[playerid][player_aj]);
    cache_get_value_int(0, "bw_time", pInfo[playerid][player_bw]);
    cache_get_value_int(0, "penalty_points", pInfo[playerid][player_penalty_points]);
    
    // Skopiuj dane admina z konta
    pInfo[playerid][player_admin] = g_AccountData[playerid][acc_admin_level];
    pInfo[playerid][player_vip] = g_AccountData[playerid][acc_vip_level];
    pInfo[playerid][player_warns] = g_AccountData[playerid][acc_warns];
    
    // WAŻNE: Synchronizuj z pGlobal dla komend admina
    pGlobal[playerid][glo_admin] = g_AccountData[playerid][acc_admin_level];
    
    // WAŻNE: Oznacz dane jako załadowane - bez tego Player_Spawn zwróci 0
    pInfo[playerid][player_loaded] = true;
    
    // Zakoncz autoryzacje
    Auth_CompleteLogin(playerid);
    
    return 1;
}

// ===========================================================================
// TWORZENIE POSTACI
// ===========================================================================

/**
 * Wyswietla dialog nazwy postaci
 */
stock Auth_ShowCharacterNameDialog(playerid)
{
    g_AuthState[playerid] = AUTH_STATE_CHARACTER_CREATE_NAME;
    
    new info[512];
    format(info, sizeof(info),
        "{FFFFFF}Wprowadz imie i nazwisko swojej postaci.\n\n{AAAAAA}Wymagania:\n{FFFFFF}- Format: Imie_Nazwisko\n- Tylko litery i podkreslnik\n- Przyklad: Jan_Kowalski\n\n{FF9800}Uwaga: Nazwa musi byc unikalna i realistyczna!");
    
    ShowPlayerDialog(playerid, DIALOG_AUTH_CHAR_NAME, DIALOG_STYLE_INPUT, 
        "Tworzenie postaci - Imie", info, "Dalej", "Anuluj");
}

/**
 * Weryfikuje nazwe postaci
 */
stock Auth_VerifyCharacterName(playerid, const name[])
{
    // Sprawdz dlugosc
    if(strlen(name) < 3 || strlen(name) > 24)
    {
        SendClientMessage(playerid, COLOR_ERROR, "Nazwa musi miec od 3 do 24 znakow!");
        Auth_ShowCharacterNameDialog(playerid);
        return 0;
    }
    
    // Sprawdz format (Imie_Nazwisko)
    if(strfind(name, "_") == -1)
    {
        SendClientMessage(playerid, COLOR_ERROR, "Nazwa musi byc w formacie: Imie_Nazwisko");
        Auth_ShowCharacterNameDialog(playerid);
        return 0;
    }
    
    // Sprawdz dozwolone znaki
    for(new i = 0, len = strlen(name); i < len; i++)
    {
        if(!((name[i] >= 'A' && name[i] <= 'Z') ||
             (name[i] >= 'a' && name[i] <= 'z') ||
             name[i] == '_'))
        {
            SendClientMessage(playerid, COLOR_ERROR, "Nazwa moze zawierac tylko litery i podkreslnik!");
            Auth_ShowCharacterNameDialog(playerid);
            return 0;
        }
    }
    
    // Zapisz tymczasowo i sprawdz w bazie
    strcopy(g_TempCharacter[playerid][temp_name], name, MAX_PLAYER_NAME);
    
    new query[256];
    mysql_format(mySQLconnection, query, sizeof(query),
        "SELECT `id` FROM `characters` WHERE `name` = '%e' LIMIT 1", name);
    
    mysql_tquery(mySQLconnection, query, "Auth_OnCharacterNameCheck", "i", playerid);
    
    return 1;
}

forward Auth_OnCharacterNameCheck(playerid);
public Auth_OnCharacterNameCheck(playerid)
{
    if(!IsPlayerConnected(playerid))
    {
        return 0;
    }
    
    if(cache_num_rows() > 0)
    {
        SendClientMessage(playerid, COLOR_ERROR, "Ta nazwa jest juz zajeta!");
        Auth_ShowCharacterNameDialog(playerid);
        return 0;
    }
    
    // Nazwa dostepna - przejdz do wyboru plci
    Auth_ShowGenderDialog(playerid);
    
    return 1;
}

/**
 * Wyswietla dialog wyboru plci
 */
stock Auth_ShowGenderDialog(playerid)
{
    g_AuthState[playerid] = AUTH_STATE_CHARACTER_CREATE_GENDER;
    
    new info[256] = 
        "{3498DB}? Mezczyzna\n{E91E63}? Kobieta";
    
    ShowPlayerDialog(playerid, DIALOG_AUTH_CHAR_GENDER, DIALOG_STYLE_LIST, 
        "Tworzenie postaci - Plec", info, "Wybierz", "Wroc");
}

/**
 * Ustawia plec i pokazuje wybor skina
 * Gracz pozostaje w spectating - menu skina wyświetlane przed spawnem
 */
stock Auth_SetGender(playerid, gender)
{
    g_TempCharacter[playerid][temp_gender] = gender;
    g_AuthState[playerid] = AUTH_STATE_CHARACTER_CREATE_SKIN;
    
    // Pokaż menu wyboru skina (gracz nadal w spectating)
    SkinSelection_Show(playerid, "Auth_OnSkinSelected");
    
    SendClientMessage(playerid, COLOR_INFO, "Wybierz skin dla swojej postaci.");
}

// ===========================================================================
// PREVIEW MODEL - WYBOR SKINA
// ===========================================================================

// Bufor listy skin??w (generowany raz)
stock ShowSkinPreviewDialog(playerid, gender, const callback[])
{
    // Delegate to eSelection-based picker; callback invoked via OnModelSelectionResponse
    SkinSelection_Show(playerid, callback);
}

/**
 * Callback po wyborze skina (gracz w spectating - przed spawnem)
 */
forward Auth_OnSkinSelected(playerid, skinid);
public Auth_OnSkinSelected(playerid, skinid)
{
    if(g_AuthState[playerid] != AUTH_STATE_CHARACTER_CREATE_SKIN)
    {
        return 0;
    }
    
    if(skinid < 0)
    {
        // Anulowano - pokaż menu ponownie (nie można anulować przy rejestracji)
        SendClientMessage(playerid, COLOR_ERROR, "Musisz wybrac skin aby kontynuowac!");
        SkinSelection_Show(playerid, "Auth_OnSkinSelected");
        return 0;
    }
    
    // Zapisz wybrany skin
    g_TempCharacter[playerid][temp_skin] = skinid;
    
    // Utworz postac w bazie (spawn nastąpi po utworzeniu)
    Auth_CreateCharacter(playerid);
    
    return 1;
}

// Stare funkcje Auth_HideMenuDelayed usunięte - nie są już potrzebne
// eSelection teraz niszczy PlayerTextDrawy zamiast je ukrywać

// Auth_CreateCharacterAfterSkinSelect i Auth_CompleteLoginNoSpawn usunięte
// Używamy standardowego Auth_CreateCharacter i Auth_CompleteLogin

/**
 * Tworzy postac w bazie danych
 */
stock Auth_CreateCharacter(playerid)
{
    new query[1024];
    
    // Pobierz poczatkowa pozycje spawn z configu
    new Float:spawnX = Setting[setting_spawn_pos_x];
    new Float:spawnY = Setting[setting_spawn_pos_y];
    new Float:spawnZ = Setting[setting_spawn_pos_z];
    new Float:spawnA = Setting[setting_spawn_pos_a];
    
    // Fallback na hardkodowane wartości jeśli coś się nie załadowało z configu
    if(spawnX == 0.0 || spawnY == 0.0 || spawnZ == 0.0)
    {
        spawnX = 1607.5580;
        spawnY = 1820.9547;
        spawnZ = 10.8203;
        spawnA = 180.0;
    }
    
    printf("[Auth] Tworzenie postaci dla %d - Spawn: %.2f, %.2f, %.2f", playerid, spawnX, spawnY, spawnZ);
    
    mysql_format(mySQLconnection, query, sizeof(query),
        "INSERT INTO `characters` (`account_id`, `name`, `gender`, `skin`, `money`, `bank`, `level`, `health`, `armour`, `pos_x`, `pos_y`, `pos_z`, `pos_angle`, `interior`, `world`, `hunger`, `thirst`, `created_at`, `last_save`) VALUES (%d, '%e', %d, %d, %d, 0, 1, 100.0, 0.0, %f, %f, %f, %f, 0, 0, 100, 100, %d, %d)",
        g_AccountData[playerid][acc_id],
        g_TempCharacter[playerid][temp_name],
        g_TempCharacter[playerid][temp_gender],
        g_TempCharacter[playerid][temp_skin],
        Setting[setting_start_money],
        spawnX, spawnY, spawnZ, spawnA,
        gettime(), gettime());
    
    mysql_tquery(mySQLconnection, query, "Auth_OnCharacterCreate", "i", playerid);
}

forward Auth_OnCharacterCreate(playerid);
public Auth_OnCharacterCreate(playerid)
{
    if(!IsPlayerConnected(playerid))
    {
        return 0;
    }
    
    new charId = cache_insert_id();
    
    if(charId == 0)
    {
        SendClientMessage(playerid, COLOR_ERROR, "Blad tworzenia postaci!");
        return 0;
    }
    
    // Ustaw podstawowe dane
    pInfo[playerid][player_uid] = charId;
    pInfo[playerid][player_id] = charId;
    pInfo[playerid][player_account_id] = g_AccountData[playerid][acc_id];
    strcopy(pInfo[playerid][player_name], g_TempCharacter[playerid][temp_name], MAX_PLAYER_NAME);
    pInfo[playerid][player_gender] = g_TempCharacter[playerid][temp_gender];
    pInfo[playerid][player_skin] = g_TempCharacter[playerid][temp_skin];
    pInfo[playerid][player_money] = Setting[setting_start_money];
    pInfo[playerid][player_bank] = 0;
    pInfo[playerid][player_level] = 1;
    pInfo[playerid][player_health] = 100.0;
    pInfo[playerid][player_armour] = 0.0;

    // Użyj tych samych startowych koordynatów co podczas INSERT
    new Float:spawnX = Setting[setting_spawn_pos_x];
    new Float:spawnY = Setting[setting_spawn_pos_y];
    new Float:spawnZ = Setting[setting_spawn_pos_z];
    new Float:spawnA = Setting[setting_spawn_pos_a];
    if(spawnX == 0.0 || spawnY == 0.0 || spawnZ == 0.0)
    {
        spawnX = 1607.5580;
        spawnY = 1820.9547;
        spawnZ = 10.8203;
        spawnA = 180.0;
    }
    pInfo[playerid][player_pos_x] = spawnX;
    pInfo[playerid][player_pos_y] = spawnY;
    pInfo[playerid][player_pos_z] = spawnZ;
    pInfo[playerid][player_pos_a] = spawnA;
    pInfo[playerid][player_interior] = 0;
    pInfo[playerid][player_vw] = 0;
    pInfo[playerid][player_hunger] = 100;
    pInfo[playerid][player_thirst] = 100;
    pInfo[playerid][player_admin] = g_AccountData[playerid][acc_admin_level];
    pInfo[playerid][player_vip] = g_AccountData[playerid][acc_vip_level];
    
    // WAŻNE: Synchronizuj z pGlobal dla komend admina
    pGlobal[playerid][glo_admin] = g_AccountData[playerid][acc_admin_level];
    
    // WAŻNE: Oznacz dane jako załadowane - bez tego Player_Spawn zwróci 0
    pInfo[playerid][player_loaded] = true;
    
    new msg[128];
    format(msg, sizeof(msg), "Postac {2ECC71}%s{FFFFFF} zostala utworzona!", 
        g_TempCharacter[playerid][temp_name]);
    SendClientMessage(playerid, COLOR_WHITE, msg);
    
    // Zakoncz autoryzacje
    Auth_CompleteLogin(playerid);
    
    return 1;
}

// ===========================================================================
// ZAKOnCZENIE AUTORYZACJI
// ===========================================================================

/**
 * Konczy proces logowania
 * @param playerid ID gracza
 */
stock Auth_CompleteLogin(playerid)
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }
    
    // Anuluj timer timeout
    if(g_LoginTimer[playerid] != 0)
    {
        KillTimer(g_LoginTimer[playerid]);
        g_LoginTimer[playerid] = 0;
    }
    
    // Zmien stan
    g_AuthState[playerid] = AUTH_STATE_LOGGED;
    pInfo[playerid][player_logged] = true;
    
    // Dodaj do iteratora
    Iter_Add(Player, playerid);
    
    // Zaladuj dodatkowe dane gracza
    Player_LoadItems(playerid);
    Player_LoadWeapons(playerid);
    
    // Zaktualizuj ostatnie logowanie
    new query[256];
        mysql_format(mySQLconnection, query, sizeof(query),
            "UPDATE `users` SET `last_login` = %d WHERE `id` = %d",
        gettime(), g_AccountData[playerid][acc_id]);
    mysql_tquery(mySQLconnection, query);
    
    // Zaktualizuj last_save postaci
    mysql_format(mySQLconnection, query, sizeof(query),
        "UPDATE `characters` SET `last_save` = %d WHERE `id` = %d",
        gettime(), pInfo[playerid][player_uid]);
    mysql_tquery(mySQLconnection, query);
    
    // Spawnuj gracza - użyj Player_Spawn zamiast SpawnPlayer
    // SpawnPlayer wyzwala class selection co powoduje lecenie w chmurach
    Player_Spawn(playerid);
    
    // Ukryj menu wyboru modeli (może zostać widoczne z poprzedniej sesji)
    HideModelSelectionMenu(playerid);
    
    // Pokaz wiadomosc powitalna
    new msg[128];
    format(msg, sizeof(msg), "Witaj, {2ECC71}%s{FFFFFF}! Pomyslnie zalogowano.", 
        Player_RPName(playerid));
    SendClientMessage(playerid, COLOR_WHITE, msg);
    
    // Log
    printf("[Auth] Gracz %s (UID: %d, Account: %d) zalogowany pomyslnie", 
        pInfo[playerid][player_name], pInfo[playerid][player_uid], g_AccountData[playerid][acc_id]);
    
    // Wywolaj callback
    CallLocalFunction("OnPlayerLogin", "i", playerid);
    
    return 1;
}

// Forward callback
forward OnPlayerLogin(playerid);

// ===========================================================================
// OBSlUGA ODPOWIEDZI DIALOGoW
// ===========================================================================

hook OnDialogResponse@Auth(playerid, dialogid, response, listitem, inputtext[])
{
    // Debug trace to verify which dialog IDs are hit at runtime.
    printf("[AUTH DEBUG] pid=%d dialog=%d state=%d resp=%d list=%d", playerid, dialogid, g_AuthState[playerid], response, listitem);

    switch(dialogid)
    {
        case DIALOG_AUTH_LOGIN:
        {
            if(!response)
            {
                Player_Kick(playerid, -1, "Odmowiono logowania");
                return Y_HOOKS_BREAK_RETURN_1;
            }
            
            Auth_VerifyPassword(playerid, inputtext);
            return Y_HOOKS_BREAK_RETURN_1;
        }
        
        case DIALOG_AUTH_REGISTER:
        {
            if(!response)
            {
                Player_Kick(playerid, -1, "Odmowiono rejestracji");
                return Y_HOOKS_BREAK_RETURN_1;
            }
            
            if(strlen(inputtext) < MIN_PASSWORD_LENGTH)
            {
                SendClientMessage(playerid, COLOR_ERROR, "Haslo jest za krotkie!");
                Auth_ShowRegisterDialog(playerid);
                return Y_HOOKS_BREAK_RETURN_1;
            }
            
            if(strlen(inputtext) > MAX_PASSWORD_LENGTH)
            {
                SendClientMessage(playerid, COLOR_ERROR, "Haslo jest za dlugie!");
                Auth_ShowRegisterDialog(playerid);
                return Y_HOOKS_BREAK_RETURN_1;
            }
            
            // Zapisz haslo tymczasowo
            strcopy(g_TempPassword[playerid], inputtext, sizeof(g_TempPassword[]));
            g_AuthState[playerid] = AUTH_STATE_REGISTER_CONFIRM;
            Auth_ShowRegisterConfirmDialog(playerid);
            
            return Y_HOOKS_BREAK_RETURN_1;
        }
        
        case DIALOG_AUTH_REGISTER_CONFIRM:
        {
            if(!response)
            {
                Auth_ShowRegisterDialog(playerid);
                return Y_HOOKS_BREAK_RETURN_1;
            }
            
            if(strcmp(inputtext, g_TempPassword[playerid]) != 0)
            {
                SendClientMessage(playerid, COLOR_ERROR, "Hasla nie sa identyczne!");
                Auth_ShowRegisterDialog(playerid);
                return Y_HOOKS_BREAK_RETURN_1;
            }
            
            // Utworz konto
            Auth_CreateAccount(playerid, g_TempPassword[playerid]);
            g_TempPassword[playerid][0] = EOS;
            
            return Y_HOOKS_BREAK_RETURN_1;
        }
        
        case DIALOG_AUTH_CHAR_SELECT:
        {
            if(!response)
            {
                Player_Kick(playerid, -1, "Odmowiono wyboru postaci");
                return Y_HOOKS_BREAK_RETURN_1;
            }
            
            new count = g_CharacterCount[playerid];
            
            if(listitem < count)
            {
                // Wybrano istniejaca postac - zapytaj o spawn
                g_PendingCharacterIndex[playerid] = listitem;
                Auth_ShowSpawnChoiceDialog(playerid);
            }
            else
            {
                // Tworzenie nowej postaci
                if(count >= Auth_GetMaxCharacters(playerid))
                {
                    SendClientMessage(playerid, COLOR_ERROR, "Osiagnales limit postaci!");
                    Auth_ShowCharacterSelectDialog(playerid);
                }
                else
                {
                    Auth_ShowCharacterNameDialog(playerid);
                }
            }
            
            return Y_HOOKS_BREAK_RETURN_1;
        }

        case DIALOG_AUTH_SPAWN_SELECT:
        {
            if(!response)
            {
                g_PendingCharacterIndex[playerid] = -1;
                Auth_ShowCharacterSelectDialog(playerid);
                return Y_HOOKS_BREAK_RETURN_1;
            }

            new charIndex = g_PendingCharacterIndex[playerid];
            if(charIndex < 0 || charIndex >= g_CharacterCount[playerid])
            {
                Auth_ShowCharacterSelectDialog(playerid);
                return Y_HOOKS_BREAK_RETURN_1;
            }

            switch(listitem)
            {
                case 0: Player_SetSpawnPreferenceUseLast(playerid, true);
                default: Player_SetSpawnPreferenceUseLast(playerid, false);
            }

            Auth_SelectCharacter(playerid, charIndex);
            g_PendingCharacterIndex[playerid] = -1;
            return Y_HOOKS_BREAK_RETURN_1;
        }
        
        case DIALOG_AUTH_CHAR_NAME:
        {
            if(!response)
            {
                if(g_CharacterCount[playerid] > 0)
                {
                    Auth_ShowCharacterSelectDialog(playerid);
                }
                else
                {
                    Player_Kick(playerid, -1, "Odmowiono tworzenia postaci");
                }
                return Y_HOOKS_BREAK_RETURN_1;
            }
            
            Auth_VerifyCharacterName(playerid, inputtext);
            return Y_HOOKS_BREAK_RETURN_1;
        }
        
        case DIALOG_AUTH_CHAR_GENDER:
        {
            if(!response)
            {
                Auth_ShowCharacterNameDialog(playerid);
                return Y_HOOKS_BREAK_RETURN_1;
            }
            
            // 0 = Mezczyzna, 1 = Kobieta
            Auth_SetGender(playerid, listitem);
            return Y_HOOKS_BREAK_RETURN_1;
        }

        // Skin selection now handled via eSelection (see skin_selection.inc)
    }
    
    return 1;
}

/**
 * Obsluga bana
 * @param playerid ID gracza
 */
static stock Auth_HandleBan(playerid)
{
    new message[512];
    
    format(message, sizeof(message),
        "{FF6B6B}Twoje konto zostalo zbanowane!\n\n{FFFFFF}Jesli uwazasz, ze ban jest niesluszny,\nskontaktuj sie z administracja na forum serwera.");
    
    ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, 
        "Konto zbanowane", message, "Wyjdz", "");
    
    SetTimerEx("DelayedKick", 3000, false, "i", playerid);
}

// ===========================================================================
// TIMEOUT
// ===========================================================================

forward Auth_Timeout(playerid);
public Auth_Timeout(playerid)
{
    g_LoginTimer[playerid] = 0;
    
    if(IsPlayerConnected(playerid) && g_AuthState[playerid] != AUTH_STATE_LOGGED)
    {
        SendClientMessage(playerid, COLOR_ERROR, "Przekroczono limit czasu na logowanie!");
        Player_Kick(playerid, -1, "Timeout logowania");
    }
    
    return 1;
}

forward DelayedKick(playerid);
public DelayedKick(playerid)
{
    if(IsPlayerConnected(playerid))
    {
        Kick(playerid);
    }
    return 1;
}

// ===========================================================================
// FUNKCJE POMOCNICZE
// ===========================================================================

/**
 * Pobiera stan autoryzacji gracza
 * @param playerid ID gracza
 * @return Stan autoryzacji
 */
stock E_AUTH_STATE:Auth_GetState(playerid)
{
    if(!IsValidPlayerId(playerid))
    {
        return AUTH_STATE_NONE;
    }
    
    return g_AuthState[playerid];
}

/**
 * Sprawdza czy gracz jest zalogowany
 */
stock bool:Auth_IsLogged(playerid)
{
    return g_AuthState[playerid] == AUTH_STATE_LOGGED;
}

/**
 * Sprawdza czy gracz jest w procesie autoryzacji
 * @param playerid ID gracza
 * @return true jesli jest w autoryzacji
 */
stock bool:Auth_IsInProgress(playerid)
{
    return g_AuthState[playerid] != AUTH_STATE_NONE && 
           g_AuthState[playerid] != AUTH_STATE_LOGGED;
}

/**
 * Pobiera ID konta gracza
 */
stock Auth_GetAccountId(playerid)
{
    return g_AccountData[playerid][acc_id];
}

/**
 * Pobiera poziom VIP gracza
 */
stock Auth_GetVIPLevel(playerid)
{
    return g_AccountData[playerid][acc_vip_level];
}

/**
 * Pobiera poziom admina z konta
 */
stock Auth_GetAdminLevel(playerid)
{
    return g_AccountData[playerid][acc_admin_level];
}

// ===========================================================================
// HOOKI
// ===========================================================================

hook OnPlayerConnect@Auth(playerid)
{
    g_AuthState[playerid] = AUTH_STATE_NONE;
    g_LoginAttempts[playerid] = 0;
    g_LoginTimer[playerid] = 0;
    g_TempPassword[playerid][0] = EOS;
    g_CharacterCount[playerid] = 0;
    g_PendingCharacterIndex[playerid] = -1;
    Auth_ResetAccountData(playerid);
    
    return 1;
}

hook OnPlayerDisconnect@Auth(playerid, reason)
{
    // Anuluj timer jesli aktywny
    if(g_LoginTimer[playerid] != 0)
    {
        KillTimer(g_LoginTimer[playerid]);
        g_LoginTimer[playerid] = 0;
    }
    
    g_AuthState[playerid] = AUTH_STATE_NONE;
    g_TempPassword[playerid][0] = EOS;
    g_PendingCharacterIndex[playerid] = -1;
    
    return 1;
}

// ===========================================================================
// EOF
// ===========================================================================




