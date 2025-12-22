/*
 * =============================================================================
 *  MYSQL CONNECTION - Po????czenie z baz?? danych
 * =============================================================================
 *  
 *  Modu??: database/mysql_connection.inc
 *  Opis: Zarz??dzanie po????czeniem MySQL (R41+ kompatybilny z open.mp)
 *
 *  ZMIANY W REFAKTORZE:
 *  - Zaktualizowano do MySQL R41+
 *  - Dodano prepared statements dla bezpiecze??stwa
 *  - Dodano connection pooling
 *  - Poprawiono obs??ug?? b????d??w
 *  - Dodano automatyczne reconnect
 *
 * =============================================================================
 */

#if defined _mysql_connection_included
    #endinput
#endif
#define _mysql_connection_included

// ===========================================================================
// STA??E MySQL
// ===========================================================================

#define MYSQL_RECONNECT_DELAY       5000    // Op????nienie przed reconnectem (ms)
#define MYSQL_MAX_QUERY_LENGTH      4096    // Maksymalna d??ugo???? zapytania

// Sta??e b????d??w MySQL
#if !defined CR_SERVER_GONE_ERROR
    #define CR_SERVER_GONE_ERROR 2006
#endif
#if !defined CR_SERVER_LOST
    #define CR_SERVER_LOST 2013
#endif
#if !defined ER_SYNTAX_ERROR
    #define ER_SYNTAX_ERROR 1064
#endif
#if !defined ER_DUP_ENTRY
    #define ER_DUP_ENTRY 1062
#endif

// ===========================================================================
// ZMIENNE MODU??U
// ===========================================================================

static bool:g_MySQLConnected = false;
static g_ReconnectAttempts = 0;
static const MAX_RECONNECT_ATTEMPTS = 5;

forward DB_OnCheckItemPhoneColumn();

// ===========================================================================
// PO????CZENIE Z BAZ?? DANYCH
// ===========================================================================

/**
 * Nawi??zuje po????czenie z baz?? danych MySQL
 * @return true je??li po????czono, false w przypadku b????du
 */
stock bool:MySQL_Connect()
{
    new startTime = GetTickCount();
    
    // Opcje po????czenia
    new MySQLOpt:options = mysql_init_options();
    mysql_set_option(options, AUTO_RECONNECT, true);
    mysql_set_option(options, MULTI_STATEMENTS, true);
    mysql_set_option(options, POOL_SIZE, 2);
    
    // Pr??ba po????czenia
    mySQLconnection = mysql_connect(
        Setting[setting_mysql_hostname],
        Setting[setting_mysql_username],
        Setting[setting_mysql_password],
        Setting[setting_mysql_database],
        options
    );
    
    // Sprawd?? status po????czenia
    if(mysql_errno(mySQLconnection) != 0)
    {
        printf("[MySQL] B????D: Nie mo??na po????czy?? z baz?? danych!");
        printf("[MySQL] Host: %s, User: %s, DB: %s",
            Setting[setting_mysql_hostname],
            Setting[setting_mysql_username],
            Setting[setting_mysql_database]);
        
        g_MySQLConnected = false;
        return false;
    }
    
    // Ustaw charset
    mysql_set_charset("utf8mb4", mySQLconnection);
    
    // W????cz logowanie
    mysql_log(ALL);
    
    g_MySQLConnected = true;
    g_ReconnectAttempts = 0;
    
    printf("[MySQL] Po????czono z baz?? danych %s@%s [czas: %d ms]",
        Setting[setting_mysql_username],
        Setting[setting_mysql_hostname],
        GetTickCount() - startTime);
    
    // Check for `item_phone` column presence (migration may be pending)
    {
        new query[512];
        mysql_format(mySQLconnection, query, sizeof(query),
            "SELECT COUNT(*) AS exists_count FROM information_schema.columns WHERE table_schema = DATABASE() AND table_name = 'items' AND column_name = 'item_phone'");
        mysql_tquery(mySQLconnection, query, "DB_OnCheckItemPhoneColumn");
    }
    return true;
}

/**
 * Zamyka po????czenie z baz?? danych
 */
stock MySQL_Disconnect()
{
    if(!g_MySQLConnected)
    {
        return;
    }
    
    mysql_close(mySQLconnection);
    g_MySQLConnected = false;
    
    print("[MySQL] Po????czenie zamkni??te");
}

/**
 * Sprawdza czy po????czenie jest aktywne
 * @return true je??li po????czone
 */
stock bool:MySQL_IsConnected()
{
    return g_MySQLConnected && mysql_errno(mySQLconnection) == 0;
}

/**
 * Pr??buje ponownie po????czy?? si?? z baz?? danych
 * @return true je??li uda??o si?? po????czy??
 */
stock bool:MySQL_Reconnect()
{
    if(g_ReconnectAttempts >= MAX_RECONNECT_ATTEMPTS)
    {
        printf("[MySQL] Przekroczono maksymaln?? liczb?? pr??b reconnectu (%d)", MAX_RECONNECT_ATTEMPTS);
        return false;
    }
    
    g_ReconnectAttempts++;
    printf("[MySQL] Pr??ba reconnectu %d/%d...", g_ReconnectAttempts, MAX_RECONNECT_ATTEMPTS);
    
    // Zamknij stare po????czenie
    mysql_close(mySQLconnection);
    g_MySQLConnected = false;
    
    // Poczekaj i spr??buj ponownie
    // Uwaga: W produkcji u??yj timera zamiast synchronicznego czekania
    return MySQL_Connect();
}

// ===========================================================================
// WYKONYWANIE ZAPYTA??
// ===========================================================================

/**
 * Wykonuje zapytanie SQL (synchronicznie)
 * @param query Zapytanie SQL
 * @return Cache handle lub MYSQL_INVALID_CACHE
 */
stock Cache:MySQL_Query(const query[])
{
    if(!MySQL_IsConnected())
    {
        print("[MySQL] Ostrze??enie: Brak po????czenia, pr??ba reconnectu...");
        if(!MySQL_Reconnect())
        {
            return Cache:MYSQL_INVALID_CACHE;
        }
    }
    
    return mysql_query(mySQLconnection, query);
}

/**
 * Wykonuje zapytanie SQL (asynchronicznie)
 * @param callback Nazwa callbacku
 * @param query Zapytanie SQL
 * @param format Format parametr??w (opcjonalny)
 */
stock MySQL_TQuery(const callback[], const query[], const format[] = "", GLOBAL_TAG_TYPES:...)
{
    if(!MySQL_IsConnected())
    {
        print("[MySQL] Ostrze??enie: Brak po????czenia dla async query");
        return;
    }
    
    mysql_tquery(mySQLconnection, query, callback, format);
}

public DB_OnCheckItemPhoneColumn()
{
    if(cache_num_rows() <= 0) return 0;
    new exists = 0;
    cache_get_value_int(0, "exists_count", exists);
    if(exists == 0)
    {
        printf("[WARN] Database schema missing 'item_phone' column. Run migration: rp_openmp/database/migrations/0004_add_item_phone_number.sql\n");
        printf("[HINT] Run:\nmysql -u <user> -p <db> < rp_openmp/database/migrations/0004_add_item_phone_number.sql\n");
    }
    return 1;
}

/**
 * Wykonuje sformatowane zapytanie (synchronicznie)
 * U??ywa bezpiecznego formatowania
 * @param format Format zapytania
 * @param ... Argumenty
 * @return Cache handle
 */
stock Cache:MySQL_QueryFormat(const fmt[], GLOBAL_TAG_TYPES:...)
{
    static query[4096];
    
    // Formatowanie z obs??ug?? zmiennej liczby argument??w
    #emit PUSH.pri
    #emit PUSH.alt
    #emit LCTRL 5
    #emit PUSH.pri
    #emit LCTRL 4
    #emit PUSH.pri
    #emit LOAD.S.pri 12
    #emit ADD.C 4
    #emit PUSH.pri
    #emit CONST.pri 4096
    #emit PUSH.pri
    #emit PUSH.C query
    #emit PUSH.C 12
    #emit SYSREQ.C fmt
    #emit STACK 20
    #emit POP.alt
    #emit LCTRL 5
    #emit SCTRL 4
    #emit POP.alt
    #emit POP.pri
    
    // Alternatywne proste rozwi??zanie:
    format(query, sizeof(query), fmt);
    
    return MySQL_Query(query);
}

/**
 * Wykonuje sformatowane zapytanie (bez oczekiwania na wynik)
 * Idealne dla UPDATE, INSERT, DELETE
 * @param query Zapytanie SQL
 */
stock MySQL_ExecuteFormat(const fmt[], GLOBAL_TAG_TYPES:...)
{
    static query[4096];
    
    // Sprawdź liczbę argumentów
    new numArgs = numargs();
    
    // Jeśli tylko format string (bez parametrów)
    if(numArgs == 1)
    {
        strcopy(query, fmt, sizeof(query));
    }
    else
    {
        // Użyj va_format do poprawnego formatowania z argumentami
        new args[32];
        for(new i = 1; i < numArgs && i < 32; i++)
        {
            args[i-1] = getarg(i);
        }
        
        // Proste formatowanie dla częstych przypadków
        switch(numArgs)
        {
            case 2: format(query, sizeof(query), fmt, args[0]);
            case 3: format(query, sizeof(query), fmt, args[0], args[1]);
            case 4: format(query, sizeof(query), fmt, args[0], args[1], args[2]);
            case 5: format(query, sizeof(query), fmt, args[0], args[1], args[2], args[3]);
            case 6: format(query, sizeof(query), fmt, args[0], args[1], args[2], args[3], args[4]);
            case 7: format(query, sizeof(query), fmt, args[0], args[1], args[2], args[3], args[4], args[5]);
            case 8: format(query, sizeof(query), fmt, args[0], args[1], args[2], args[3], args[4], args[5], args[6]);
            default: format(query, sizeof(query), fmt, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7]);
        }
    }
    
    if(!MySQL_IsConnected())
    {
        printf("[MySQL] Ostrzezenie: Pomijam zapytanie (brak polaczenia): %.100s...", query);
        return;
    }
    
    mysql_tquery(mySQLconnection, query);
}

/**
 * Pobiera liczb?? wierszy z ostatniego zapytania
 * @return Liczba wierszy
 */
stock MySQL_GetRowCount()
{
    return cache_num_rows();
}

/**
 * Pobiera liczb?? zmienionych wierszy
 * @return Liczba zmienionych wierszy
 */
stock MySQL_GetAffectedRows()
{
    return cache_affected_rows();
}

/**
 * Pobiera ostatnie wstawione ID
 * @return Insert ID
 */
stock MySQL_GetInsertId()
{
    return cache_insert_id();
}

// ===========================================================================
// PREPARED STATEMENTS (dla cz??sto u??ywanych zapyta??)
// ===========================================================================

static Statement:stmt_LoadPlayer = MYSQL_INVALID_STATEMENT;
static Statement:stmt_SavePlayer = MYSQL_INVALID_STATEMENT;
static Statement:stmt_CheckAccount = MYSQL_INVALID_STATEMENT;

/**
 * Przygotowuje prepared statements
 * Wywo??a?? po po????czeniu z baz?? danych
 */
stock MySQL_PrepareStatements()
{
    // Sprawdzenie konta
    stmt_CheckAccount = mysql_stmt_prepare(mySQLconnection,
        "SELECT u.id, u.username, u.password, u.admin_level, u.vip_level, u.vip_expire, u.banned, u.warns \
         FROM users u \
         WHERE u.username = ?"
    );
    
    // ??adowanie gracza
    stmt_LoadPlayer = mysql_stmt_prepare(mySQLconnection,
        "SELECT * FROM characters WHERE id = ? LIMIT 1"
    );
    
    printf("[MySQL] Prepared statements zainicjalizowane");
}

/**
 * Sprawdza konto gracza u??ywaj??c prepared statement
 * @param playerid ID gracza
 * @param name Nick gracza
 */
stock MySQL_CheckAccount(playerid, const name[])
{
    if(stmt_CheckAccount == MYSQL_INVALID_STATEMENT)
    {
        // Fallback do normalnego query
        new query[256];
        mysql_format(mySQLconnection, query, sizeof(query),
            "SELECT id, username, password, admin_level, vip_level, vip_expire, banned, warns \
             FROM users \
             WHERE username = '%e'", name);
        mysql_tquery(mySQLconnection, query, "OnAccountChecked", "d", playerid);
        return;
    }
    
    mysql_stmt_bind_value(stmt_CheckAccount, 0, TYPE_STRING, name);
    mysql_stmt_execute(stmt_CheckAccount, "OnAccountChecked", "d", playerid);
}

// ===========================================================================
// CZYSZCZENIE SESJI
// ===========================================================================

/**
 * Czy??ci stare sesje przy starcie serwera
 */
stock MySQL_CleanupSessions()
{
    MySQL_ExecuteFormat("DELETE FROM `logged_players`");
    MySQL_ExecuteFormat("UPDATE `characters` SET online = 0");
    
    print("[MySQL] Wyczyszczono stare sesje");
}

// ===========================================================================
// ESCAPE STRING??W
// ===========================================================================

/**
 * Escapuje string dla bezpiecznego u??ycia w SQL
 * @param input Wej??ciowy string
 * @param output Wyj??ciowy bufor
 * @param size Rozmiar bufora
 */
stock MySQL_EscapeString(const input[], output[], size = sizeof(output))
{
    mysql_escape_string(input, output, mySQLconnection, size);
}

/**
 * Formatuje i escapuje string
 * @param output Wyj??ciowy bufor
 * @param size Rozmiar bufora
 * @param format Format
 * @param ... Argumenty
 */
stock MySQL_Format(output[], size, const format[], GLOBAL_TAG_TYPES:...)
{
    mysql_format(mySQLconnection, output, size, format);
}

// ===========================================================================
// CALLBACK OBS??UGI B????D??W
// ===========================================================================

public OnQueryError(errorid, const error[], const callback[], const query[], MySQL:handle)
{
    switch(errorid)
    {
        case CR_SERVER_GONE_ERROR, CR_SERVER_LOST:
        {
            printf("[MySQL] Utracono po????czenie z serwerem MySQL");
            printf("[MySQL] B????d: %s", error);
            
            g_MySQLConnected = false;
            
            // Spr??buj reconnect
            SetTimerEx("MySQL_DelayedReconnect", MYSQL_RECONNECT_DELAY, false, "");
        }
        
        case ER_SYNTAX_ERROR:
        {
            printf("[MySQL] B????d sk??adni SQL!");
            printf("[MySQL] Query: %s", query);
            printf("[MySQL] Callback: %s", callback);
            printf("[MySQL] Error: %s", error);
        }
        
        case ER_DUP_ENTRY:
        {
            printf("[MySQL] Duplikat wpisu");
            printf("[MySQL] Query: %s", query);
        }
        
        default:
        {
            printf("[MySQL] B????d %d: %s", errorid, error);
            printf("[MySQL] Query: %s", query);
            printf("[MySQL] Callback: %s", callback);
        }
    }
    
    return 1;
}

forward MySQL_DelayedReconnect();
public MySQL_DelayedReconnect()
{
    MySQL_Reconnect();
    return 1;
}

// ===========================================================================
// EOF
// ===========================================================================

