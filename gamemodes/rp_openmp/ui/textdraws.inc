/*
 * =============================================================================
 *  TEXTDRAWS - System GUI Textdraw
 * =============================================================================
 *
 *  Modu??: ui/textdraws.inc
 *  Opis: Centralne zarz??dzanie textdrawami HUD
 *
 *  ZMIANY W REFAKTORZE:
 *  - Zast??piono stare textdrawy nowoczesnymi rozwi??zaniami
 *  - Dodano system HUD per-gracz
 *  - Optymalizacja aktualizacji
 *  - Obs??uga ukrywania/pokazywania
 *
 * =============================================================================
 */

// Wyciszenie warningów tag mismatch dla PlayerText
#pragma warning disable 213

#if defined _textdraws_included
    #endinput
#endif
#define _textdraws_included

// ===========================================================================
// STA??E
// ===========================================================================

// Sloty HUD
enum E_HUD_SLOT
{
    HUD_LOGO,
    HUD_CLOCK,
    HUD_DATE,
    HUD_MONEY,
    HUD_HEALTH,
    HUD_HUNGER,
    HUD_THIRST,
    HUD_BW,
    HUD_SPEEDO_BG,
    HUD_SPEEDO_SPEED,
    HUD_SPEEDO_FUEL,
    HUD_SPEEDO_ENGINE,
    HUD_SPEEDO_LIGHTS,
    HUD_NOTIFY,
    E_HUD_MAX
};

// Flagi HUD
#define HUD_FLAG_HIDDEN         (1 << 0)
#define HUD_FLAG_SPEEDO         (1 << 1)
#define HUD_FLAG_NOTIFY         (1 << 2)
#define HUD_FLAG_MINIMAL        (1 << 3)

// ===========================================================================
// ZMIENNE GLOBALNE
// ===========================================================================

// Globalne textdrawy
static Text:g_GlobalTextdraws[16];

// Textdrawy per-gracz
static PlayerText:g_PlayerHUD[MAX_PLAYERS][E_HUD_MAX];

// Flagi HUD gracza
static g_HUDFlags[MAX_PLAYERS];

// Timer powiadomie??
static g_NotifyTimer[MAX_PLAYERS];
// Global timer to update HUD (clock)
static g_HUDUpdateTimer = 0;
// Minute-aligned timer to update clock exactly on minute boundaries
static g_HUDMinuteTimer = 0;
// Cache of last minute shown per player to avoid redundant updates
static g_HUDPrevMinute[MAX_PLAYERS] = {-1, ...};

// ===========================================================================
// ===========================================================================
// INICJALIZACJA
// ===========================================================================

/**
 * Tworzy globalne textdrawy (wywo??ywane przy OnGameModeInit)
 */
stock Textdraws_CreateGlobal()
{
    // Logo serwera (globalny)
    g_GlobalTextdraws[0] = TextDrawCreate(548.0, 3.0, SETTING_SERVER_NAME);
    TextDrawFont(g_GlobalTextdraws[0], 2);
    TextDrawLetterSize(g_GlobalTextdraws[0], 0.27, 1.0);
    TextDrawColor(g_GlobalTextdraws[0], 0xFFFFFFFF);
    TextDrawSetOutline(g_GlobalTextdraws[0], 1);
    TextDrawSetShadow(g_GlobalTextdraws[0], 0);
    TextDrawAlignment(g_GlobalTextdraws[0], 3);

    // Start global HUD update timer (1s tick, clock updates only when minute changes)
    if(g_HUDUpdateTimer == 0)
    {
        // 1,000 ms = 1 second
        g_HUDUpdateTimer = SetTimerEx("HUD_GlobalTimer", 1000, true);
    }

    // Schedule a minute-aligned timer so clocks update exactly on the minute boundary.
    if(g_HUDMinuteTimer == 0)
    {
        new hour, minute, second; gettime(hour, minute, second);
        new ms_to_next = (60 - second) * 1000;
        if(ms_to_next <= 0) ms_to_next = 1000; // fallback
        g_HUDMinuteTimer = SetTimerEx("HUD_MinuteTimer_OneShot", ms_to_next, false);
        printf("[HUD] Scheduled minute timer in %d ms", ms_to_next);
    }

    return 1;
}

/**
 * Usuwa globalne textdrawy (wywo??ywane przy OnGameModeExit)
 */
stock Textdraws_DestroyGlobal()
{
    for(new i = 0; i < sizeof(g_GlobalTextdraws); i++)
    {
        if(g_GlobalTextdraws[i] != Text:INVALID_TEXT_DRAW)
        {
            TextDrawDestroy(g_GlobalTextdraws[i]);
            g_GlobalTextdraws[i] = Text:INVALID_TEXT_DRAW;
        }
    }

    // Kill global HUD timer
    if(g_HUDUpdateTimer != 0)
    {
        KillTimer(g_HUDUpdateTimer);
        g_HUDUpdateTimer = 0;
    }

    return 1;
}

/**
 * Tworzy HUD dla gracza
 * @param playerid ID gracza
 */
stock HUD_Create(playerid)
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }

    // === ZEGAR ===
    // Removed clock HUD (handled via date) - reserve slot as invalid
    g_PlayerHUD[playerid][HUD_CLOCK] = PlayerText:INVALID_TEXT_DRAW; // clock removed

    // === DATA ===
    // Nudged slightly to the left for spacing
    new td_date = CreatePlayerTextDraw(playerid, 625.0, 44.0, "01.01.2024");
    if(td_date == 0)
    {
        g_PlayerHUD[playerid][HUD_DATE] = PlayerText:INVALID_TEXT_DRAW;
        printf("[HUD_CREATE] pid=%d date create FAILED (td=0)", playerid);
    }
    else
    {
        g_PlayerHUD[playerid][HUD_DATE] = td_date;
        PlayerTextDrawFont(playerid, g_PlayerHUD[playerid][HUD_DATE], 2);
        PlayerTextDrawLetterSize(playerid, g_PlayerHUD[playerid][HUD_DATE], 0.2, 0.8);
        PlayerTextDrawColour(playerid, g_PlayerHUD[playerid][HUD_DATE], 0xCCCCCCFF);
        PlayerTextDrawSetOutline(playerid, g_PlayerHUD[playerid][HUD_DATE], 1);
        PlayerTextDrawAlignment(playerid, g_PlayerHUD[playerid][HUD_DATE], 3);
    }

    // Debug: log created player textdraw ids
    printf("[HUD_CREATE] pid=%d clock_td=%d date_td=%d", playerid, g_PlayerHUD[playerid][HUD_CLOCK], g_PlayerHUD[playerid][HUD_DATE]);

    // Ensure the clock is initialized for this player
    HUD_UpdateClockOnly(playerid);

    // If minute timer is not scheduled yet, schedule a one-shot aligned to the next minute
    if(g_HUDMinuteTimer == 0)
    {
        new hour, minute, second; gettime(hour, minute, second);
        new ms_to_next = (60 - second) * 1000;
        if(ms_to_next <= 0) ms_to_next = 1000;
        g_HUDMinuteTimer = SetTimerEx("HUD_MinuteTimer_OneShot", ms_to_next, false);
        printf("[HUD_CREATE] Scheduled minute timer in %d ms", ms_to_next);
    }

    // === PIENIĄDZE ===
    new td_money = CreatePlayerTextDraw(playerid, 635.0, 60.0, "0");
    if(td_money == 0)
    {
        g_PlayerHUD[playerid][HUD_MONEY] = PlayerText:INVALID_TEXT_DRAW;
        printf("[HUD_CREATE] pid=%d money create FAILED (td=0)", playerid);
    }
    else
    {
        g_PlayerHUD[playerid][HUD_MONEY] = td_money;
        PlayerTextDrawFont(playerid, g_PlayerHUD[playerid][HUD_MONEY], 2);
        PlayerTextDrawLetterSize(playerid, g_PlayerHUD[playerid][HUD_MONEY], 0.35, 1.4);
        PlayerTextDrawColour(playerid, g_PlayerHUD[playerid][HUD_MONEY], 0x2ECC71FF);
        PlayerTextDrawSetOutline(playerid, g_PlayerHUD[playerid][HUD_MONEY], 1);
        PlayerTextDrawAlignment(playerid, g_PlayerHUD[playerid][HUD_MONEY], 3);
    }

    // === PASEK ZDROWIA ===
    new td_health = CreatePlayerTextDraw(playerid, 635.0, 80.0, "HP: 100");
    if(td_health == 0)
    {
        g_PlayerHUD[playerid][HUD_HEALTH] = PlayerText:INVALID_TEXT_DRAW;
        printf("[HUD_CREATE] pid=%d health create FAILED (td=0)", playerid);
    }
    else
    {
        g_PlayerHUD[playerid][HUD_HEALTH] = td_health;
        PlayerTextDrawFont(playerid, g_PlayerHUD[playerid][HUD_HEALTH], 2);
        PlayerTextDrawLetterSize(playerid, g_PlayerHUD[playerid][HUD_HEALTH], 0.2, 0.8);
        PlayerTextDrawColour(playerid, g_PlayerHUD[playerid][HUD_HEALTH], 0xE74C3CFF);
        PlayerTextDrawSetOutline(playerid, g_PlayerHUD[playerid][HUD_HEALTH], 1);
        PlayerTextDrawAlignment(playerid, g_PlayerHUD[playerid][HUD_HEALTH], 3);
    }

    // === THIRST ===
    new td_hunger = CreatePlayerTextDraw(playerid, 635.0, 92.0, "~y~Hunger: 100%");
    if(td_hunger == 0)
    {
        g_PlayerHUD[playerid][HUD_HUNGER] = PlayerText:INVALID_TEXT_DRAW;
        printf("[HUD_CREATE] pid=%d hunger create FAILED (td=0)", playerid);
    }
    else
    {
        g_PlayerHUD[playerid][HUD_HUNGER] = td_hunger;
        PlayerTextDrawFont(playerid, g_PlayerHUD[playerid][HUD_HUNGER], 2);
        PlayerTextDrawLetterSize(playerid, g_PlayerHUD[playerid][HUD_HUNGER], 0.2, 0.8);
        PlayerTextDrawColour(playerid, g_PlayerHUD[playerid][HUD_HUNGER], 0xF39C12FF);
        PlayerTextDrawSetOutline(playerid, g_PlayerHUD[playerid][HUD_HUNGER], 1);
        PlayerTextDrawAlignment(playerid, g_PlayerHUD[playerid][HUD_HUNGER], 3);
    }

    // === THIRST ===
    new td_thirst = CreatePlayerTextDraw(playerid, 635.0, 104.0, "~b~Thirst: 100%");
    if(td_thirst == 0)
    {
        g_PlayerHUD[playerid][HUD_THIRST] = PlayerText:INVALID_TEXT_DRAW;
        printf("[HUD_CREATE] pid=%d thirst create FAILED (td=0)", playerid);
    }
    else
    {
        g_PlayerHUD[playerid][HUD_THIRST] = td_thirst;
        PlayerTextDrawFont(playerid, g_PlayerHUD[playerid][HUD_THIRST], 2);
        PlayerTextDrawLetterSize(playerid, g_PlayerHUD[playerid][HUD_THIRST], 0.2, 0.8);
        PlayerTextDrawColour(playerid, g_PlayerHUD[playerid][HUD_THIRST], 0x3498DBFF);
        PlayerTextDrawSetOutline(playerid, g_PlayerHUD[playerid][HUD_THIRST], 1);
        PlayerTextDrawAlignment(playerid, g_PlayerHUD[playerid][HUD_THIRST], 3);
    }

    // === BITMASK WOUND (BW) ===
    g_PlayerHUD[playerid][HUD_BW] = CreatePlayerTextDraw(playerid, 320.0, 400.0, "~r~YOU ARE WOUNDED~n~~w~Wait for medical assistance");
    PlayerTextDrawFont(playerid, g_PlayerHUD[playerid][HUD_BW], 2);
    PlayerTextDrawLetterSize(playerid, g_PlayerHUD[playerid][HUD_BW], 0.35, 1.5);
    PlayerTextDrawColour(playerid, g_PlayerHUD[playerid][HUD_BW], 0xFFFFFFFF);
    PlayerTextDrawSetOutline(playerid, g_PlayerHUD[playerid][HUD_BW], 1);
    PlayerTextDrawAlignment(playerid, g_PlayerHUD[playerid][HUD_BW], 2);

    // === PR??DKO??CIOMIERZ - T??O ===
    g_PlayerHUD[playerid][HUD_SPEEDO_BG] = CreatePlayerTextDraw(playerid, 500.0, 350.0, "_");
    PlayerTextDrawLetterSize(playerid, g_PlayerHUD[playerid][HUD_SPEEDO_BG], 0.0, 7.5);
    PlayerTextDrawTextSize(playerid, g_PlayerHUD[playerid][HUD_SPEEDO_BG], 635.0, 0.0);
    PlayerTextDrawUseBox(playerid, g_PlayerHUD[playerid][HUD_SPEEDO_BG], 1);
    PlayerTextDrawBoxColour(playerid, g_PlayerHUD[playerid][HUD_SPEEDO_BG], 0x00000099);

    // === PR??DKO??CIOMIERZ - PR??DKO???? ===
    g_PlayerHUD[playerid][HUD_SPEEDO_SPEED] = CreatePlayerTextDraw(playerid, 565.0, 355.0, "0 km/h");
    PlayerTextDrawFont(playerid, g_PlayerHUD[playerid][HUD_SPEEDO_SPEED], 2);
    PlayerTextDrawLetterSize(playerid, g_PlayerHUD[playerid][HUD_SPEEDO_SPEED], 0.4, 1.8);
    PlayerTextDrawColour(playerid, g_PlayerHUD[playerid][HUD_SPEEDO_SPEED], 0xFFFFFFFF);
    PlayerTextDrawSetOutline(playerid, g_PlayerHUD[playerid][HUD_SPEEDO_SPEED], 1);
    PlayerTextDrawAlignment(playerid, g_PlayerHUD[playerid][HUD_SPEEDO_SPEED], 2);

    // === PRĘDKOŚCIOMIERZ - PALIWO ===
    g_PlayerHUD[playerid][HUD_SPEEDO_FUEL] = CreatePlayerTextDraw(playerid, 510.0, 375.0, "Fuel: 100%");
    PlayerTextDrawFont(playerid, g_PlayerHUD[playerid][HUD_SPEEDO_FUEL], 2);
    PlayerTextDrawLetterSize(playerid, g_PlayerHUD[playerid][HUD_SPEEDO_FUEL], 0.2, 0.8);
    PlayerTextDrawColour(playerid, g_PlayerHUD[playerid][HUD_SPEEDO_FUEL], 0xF39C12FF);
    PlayerTextDrawSetOutline(playerid, g_PlayerHUD[playerid][HUD_SPEEDO_FUEL], 1);

    // === PR??DKO??CIOMIERZ - SILNIK ===
    g_PlayerHUD[playerid][HUD_SPEEDO_ENGINE] = CreatePlayerTextDraw(playerid, 510.0, 390.0, "Engine: ~r~OFF");
    PlayerTextDrawFont(playerid, g_PlayerHUD[playerid][HUD_SPEEDO_ENGINE], 2);
    PlayerTextDrawLetterSize(playerid, g_PlayerHUD[playerid][HUD_SPEEDO_ENGINE], 0.2, 0.8);
    PlayerTextDrawColour(playerid, g_PlayerHUD[playerid][HUD_SPEEDO_ENGINE], 0xFFFFFFFF);
    PlayerTextDrawSetOutline(playerid, g_PlayerHUD[playerid][HUD_SPEEDO_ENGINE], 1);

    // === PRĘDKOŚCIOMIERZ - ŚWIATŁA ===
    g_PlayerHUD[playerid][HUD_SPEEDO_LIGHTS] = CreatePlayerTextDraw(playerid, 510.0, 405.0, "Lights: ~r~OFF");
    PlayerTextDrawFont(playerid, g_PlayerHUD[playerid][HUD_SPEEDO_LIGHTS], 2);
    PlayerTextDrawLetterSize(playerid, g_PlayerHUD[playerid][HUD_SPEEDO_LIGHTS], 0.2, 0.8);
    PlayerTextDrawColour(playerid, g_PlayerHUD[playerid][HUD_SPEEDO_LIGHTS], 0xFFFFFFFF);
    PlayerTextDrawSetOutline(playerid, g_PlayerHUD[playerid][HUD_SPEEDO_LIGHTS], 1);

    // === POWIADOMIENIE ===
    g_PlayerHUD[playerid][HUD_NOTIFY] = CreatePlayerTextDraw(playerid, 320.0, 140.0, " ");
    PlayerTextDrawFont(playerid, g_PlayerHUD[playerid][HUD_NOTIFY], 2);
    PlayerTextDrawLetterSize(playerid, g_PlayerHUD[playerid][HUD_NOTIFY], 0.25, 1.0);
    PlayerTextDrawColour(playerid, g_PlayerHUD[playerid][HUD_NOTIFY], 0xFFFFFFFF);
    PlayerTextDrawSetOutline(playerid, g_PlayerHUD[playerid][HUD_NOTIFY], 1);
    PlayerTextDrawAlignment(playerid, g_PlayerHUD[playerid][HUD_NOTIFY], 2);
    PlayerTextDrawTextSize(playerid, g_PlayerHUD[playerid][HUD_NOTIFY], 400.0, 0.0);

    // Inicjalizacja flag
    g_HUDFlags[playerid] = 0;
    g_NotifyTimer[playerid] = 0;

    // Do not show HUD elements here - show them only after the player has fully spawned
    return 1;
}

/**
 * Usuwa HUD gracza
 * @param playerid ID gracza
 */
stock HUD_Destroy(playerid)
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }

    for(new E_HUD_SLOT:i = HUD_LOGO; i < E_HUD_MAX; i++)
    {
        if(g_PlayerHUD[playerid][i] != PlayerText:INVALID_TEXT_DRAW)
        {
            PlayerTextDrawDestroy(playerid, g_PlayerHUD[playerid][i]);
            g_PlayerHUD[playerid][i] = PlayerText:INVALID_TEXT_DRAW;
        }
    }

    if(g_NotifyTimer[playerid] != 0)
    {
        KillTimer(g_NotifyTimer[playerid]);
        g_NotifyTimer[playerid] = 0;
    }

    return 1;
}

// Global timer callback to update HUDs (refresh clock every second)
stock HUD_GlobalTimer()
{
    // Global tick; call HUD_Update on visible players
    for(new i = 0; i < MAX_PLAYERS; i++)
    {
        if(IsPlayerConnected(i) && !(g_HUDFlags[i] & HUD_FLAG_HIDDEN))
        {
            HUD_Update(i);
        }
    }

    return 1;
}

// One-shot timer that fires at the next minute boundary; it will set up a repeating 60s timer afterwards
forward HUD_MinuteTimer_OneShot();
public HUD_MinuteTimer_OneShot()
{
    // One-shot triggered at next minute boundary. Do a clock-only update and then schedule repeating timer.
    for(new i = 0; i < MAX_PLAYERS; i++) if(IsPlayerConnected(i) && !(g_HUDFlags[i] & HUD_FLAG_HIDDEN)) HUD_UpdateClockOnly(i);

    // Schedule repeating timer every 60 seconds
    if(g_HUDMinuteTimer != 0)
    {
        KillTimer(g_HUDMinuteTimer);
        g_HUDMinuteTimer = 0;
    }
    g_HUDMinuteTimer = SetTimerEx("HUD_MinuteTimer", 60000, true);
    printf("[HUD_MINUTE] Repeating minute timer scheduled (60000ms)");
    return 1;
}

forward HUD_MinuteTimer();
public HUD_MinuteTimer()
{
    for(new i = 0; i < MAX_PLAYERS; i++) if(IsPlayerConnected(i) && !(g_HUDFlags[i] & HUD_FLAG_HIDDEN)) HUD_UpdateClockOnly(i);
    return 1;
}

/**
 * Ensure the date textdraw is created for a player. Creates it on-demand after spawn.
 * @return 1 if exists or created, 0 if creation failed
 */
stock HUD_EnsureDateCreated(playerid)
{
    if(!IsValidPlayerId(playerid)) return 0;

    if(g_PlayerHUD[playerid][HUD_DATE] != PlayerText:INVALID_TEXT_DRAW && g_PlayerHUD[playerid][HUD_DATE] != 0)
    {
        return 1; // already valid
    }

    // Try to create now (player is spawned when this is called)
    new td = CreatePlayerTextDraw(playerid, 625.0, 44.0, "01.01.2024");
    if(td == 0)
    {
        printf("[HUD_CREATE] pid=%d date create FAILED on-demand (td=0)", playerid);
        g_PlayerHUD[playerid][HUD_DATE] = PlayerText:INVALID_TEXT_DRAW;
        return 0;
    }

    g_PlayerHUD[playerid][HUD_DATE] = td;
    PlayerTextDrawFont(playerid, g_PlayerHUD[playerid][HUD_DATE], 2);
    PlayerTextDrawLetterSize(playerid, g_PlayerHUD[playerid][HUD_DATE], 0.2, 0.8);
    PlayerTextDrawColour(playerid, g_PlayerHUD[playerid][HUD_DATE], 0xCCCCCCFF);
    PlayerTextDrawSetOutline(playerid, g_PlayerHUD[playerid][HUD_DATE], 1);
    PlayerTextDrawAlignment(playerid, g_PlayerHUD[playerid][HUD_DATE], 3);

    printf("[HUD_CREATE] pid=%d date created on-demand td=%d", playerid, g_PlayerHUD[playerid][HUD_DATE]);
    return 1;
}

/**
 * Ensure the clock textdraw is created for a player. Creates it on-demand after spawn.
 * @return 1 if exists or created, 0 if creation failed
 */
stock HUD_EnsureClockCreated(playerid)
{
    if(!IsValidPlayerId(playerid)) return 0;

    if(g_PlayerHUD[playerid][HUD_CLOCK] != PlayerText:INVALID_TEXT_DRAW && g_PlayerHUD[playerid][HUD_CLOCK] != 0)
    {
        return 1; // already valid
    }

    // Try to create now (player is spawned when this is called)
    new td = CreatePlayerTextDraw(playerid, 635.0, 18.0, "00:00");
    if(td == 0)
    {
        printf("[HUD_CREATE] pid=%d clock create FAILED on-demand (td=0)", playerid);
        g_PlayerHUD[playerid][HUD_CLOCK] = PlayerText:INVALID_TEXT_DRAW;
        return 0;
    }

    g_PlayerHUD[playerid][HUD_CLOCK] = td;
    PlayerTextDrawFont(playerid, g_PlayerHUD[playerid][HUD_CLOCK], 2);
    PlayerTextDrawLetterSize(playerid, g_PlayerHUD[playerid][HUD_CLOCK], 0.5, 2.0);
    PlayerTextDrawColour(playerid, g_PlayerHUD[playerid][HUD_CLOCK], 0xFFFFFFFF);
    PlayerTextDrawSetOutline(playerid, g_PlayerHUD[playerid][HUD_CLOCK], 1);
    PlayerTextDrawAlignment(playerid, g_PlayerHUD[playerid][HUD_CLOCK], 3);

    printf("[HUD_CREATE] pid=%d clock created on-demand td=%d", playerid, g_PlayerHUD[playerid][HUD_CLOCK]);
    return 1;
}


// ===========================================================================
// WY??WIETLANIE HUD
// ===========================================================================

/**
 * Pokazuje HUD gracza
 * @param playerid ID gracza
 */
stock HUD_Show(playerid)
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }

    // Only show HUD when player is fully logged in and spawned
    if(!pInfo[playerid][player_logged] || !Player_IsSpawned(playerid))
    {
        printf("[HUD] HUD_Show skipped for pid=%d - logged=%d spawned=%d", playerid, pInfo[playerid][player_logged], Player_IsSpawned(playerid));
        return 0;
    }

    // Pokaż globalne
    TextDrawShowForPlayer(playerid, g_GlobalTextdraws[0]);

    // Pokaż podstawowe elementy (ensure clock + date exist and show)
    // Ensure clock exists (create on-demand if necessary)
    if(g_PlayerHUD[playerid][HUD_CLOCK] == PlayerText:INVALID_TEXT_DRAW || g_PlayerHUD[playerid][HUD_CLOCK] == 0)
    {
        if(HUD_EnsureClockCreated(playerid))
        {
            new ok_clock = PlayerTextDrawShow(playerid, g_PlayerHUD[playerid][HUD_CLOCK]);
            printf("[HUD] HUD_Show: clock created on-demand and shown td=%d ok=%d for pid=%d", g_PlayerHUD[playerid][HUD_CLOCK], ok_clock, playerid);
        }
        else
        {
            printf("[HUD] HUD_Show: clock textdraw INVALID (td=%d) for pid=%d", g_PlayerHUD[playerid][HUD_CLOCK], playerid);
        }
    }
    else
    {
        new ok_clock = PlayerTextDrawShow(playerid, g_PlayerHUD[playerid][HUD_CLOCK]);
        printf("[HUD] HUD_Show: clock_td=%d show_ok=%d for pid=%d", g_PlayerHUD[playerid][HUD_CLOCK], ok_clock, playerid);
    }

    // Ensure date exists (create on-demand if necessary)
    if(g_PlayerHUD[playerid][HUD_DATE] == PlayerText:INVALID_TEXT_DRAW || g_PlayerHUD[playerid][HUD_DATE] == 0)
    {
        if(HUD_EnsureDateCreated(playerid))
        {
            new ok_date2 = PlayerTextDrawShow(playerid, g_PlayerHUD[playerid][HUD_DATE]);
            printf("[HUD] HUD_Show: date created on-demand and shown td=%d ok=%d for pid=%d", g_PlayerHUD[playerid][HUD_DATE], ok_date2, playerid);
        }
        else
        {
            printf("[HUD] HUD_Show: date textdraw INVALID (td=%d) for pid=%d", g_PlayerHUD[playerid][HUD_DATE], playerid);
        }
    }
    else
    {
        new ok_date = PlayerTextDrawShow(playerid, g_PlayerHUD[playerid][HUD_DATE]);
        printf("[HUD] HUD_Show: date_td=%d show_ok=%d for pid=%d", g_PlayerHUD[playerid][HUD_DATE], ok_date, playerid);
    }

    new ok_hunger = PlayerTextDrawShow(playerid, g_PlayerHUD[playerid][HUD_HUNGER]);
    new ok_thirst = PlayerTextDrawShow(playerid, g_PlayerHUD[playerid][HUD_THIRST]);
    printf("[HUD] HUD_Show: hunger_td=%d ok=%d, thirst_td=%d ok=%d", g_PlayerHUD[playerid][HUD_HUNGER], ok_hunger, g_PlayerHUD[playerid][HUD_THIRST], ok_thirst);

    g_HUDFlags[playerid] &= ~HUD_FLAG_HIDDEN;

    // Aktualizuj warto??ci
    HUD_Update(playerid);

    // Debug: log show event for troubleshooting clock disappearance
    printf("[HUD] HUD_Show called for pid=%d, flags=%d", playerid, g_HUDFlags[playerid]);

    return 1;
}

/**
 * Ukrywa HUD gracza
 * @param playerid ID gracza
 */
stock HUD_Hide(playerid)
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }

    // Ukryj globalne
    TextDrawHideForPlayer(playerid, g_GlobalTextdraws[0]);

    // Ukryj wszystkie elementy
    for(new E_HUD_SLOT:i = HUD_LOGO; i < E_HUD_MAX; i++)
    {
        if(g_PlayerHUD[playerid][i] != PlayerText:INVALID_TEXT_DRAW)
        {
            PlayerTextDrawHide(playerid, g_PlayerHUD[playerid][i]);
        }
    }

    g_HUDFlags[playerid] |= HUD_FLAG_HIDDEN;

    // Debug: log hide event
    printf("[HUD] HUD_Hide called for pid=%d", playerid);

    return 1;
}

/**
 * Aktualizuje warto??ci HUD
 * @param playerid ID gracza
 */
stock HUD_Update(playerid)
{
    if(!IsValidPlayerId(playerid) || (g_HUDFlags[playerid] & HUD_FLAG_HIDDEN))
    {
        return 0;
    }

    new text[64];

    // Clock (update only on minute change)
    new hour, minute, second;
    gettime(hour, minute, second);
    new set_ok_clock = 0;

    // Log tick and previous minute for debugging
    printf("[HUD_TIMER] pid=%d tick h=%d m=%d prev=%d td=%d", playerid, hour, minute, g_HUDPrevMinute[playerid], g_PlayerHUD[playerid][HUD_CLOCK]);

    // Only update the clock text when the minute changed to avoid redundant updates
    if(g_PlayerHUD[playerid][HUD_CLOCK] == PlayerText:INVALID_TEXT_DRAW || g_PlayerHUD[playerid][HUD_CLOCK] == 0)
    {
        if(HUD_EnsureClockCreated(playerid))
        {
            // Force update on creation by setting prev minute to invalid
            g_HUDPrevMinute[playerid] = -1;
            new textInit[6]; format(textInit, sizeof(textInit), "%02d:%02d", hour, minute);
            set_ok_clock = PlayerTextDrawSetString(playerid, g_PlayerHUD[playerid][HUD_CLOCK], textInit);
            PlayerTextDrawShow(playerid, g_PlayerHUD[playerid][HUD_CLOCK]);
            printf("[HUD_TIMER] pid=%d created clock set='%s' set_ok=%d td=%d", playerid, textInit, set_ok_clock, g_PlayerHUD[playerid][HUD_CLOCK]);
        }
        else
        {
            printf("[HUD_TIMER] pid=%d failed to create clock td=%d", playerid, g_PlayerHUD[playerid][HUD_CLOCK]);
        }
    }
    else
    {
        if(g_HUDPrevMinute[playerid] != minute)
        {
            new textClock[6]; format(textClock, sizeof(textClock), "%02d:%02d", hour, minute);
            set_ok_clock = PlayerTextDrawSetString(playerid, g_PlayerHUD[playerid][HUD_CLOCK], textClock);
            printf("[HUD_TIMER] pid=%d update attempt set='%s' set_ok=%d td=%d", playerid, textClock, set_ok_clock, g_PlayerHUD[playerid][HUD_CLOCK]);
            if(set_ok_clock) g_HUDPrevMinute[playerid] = minute;
            else printf("[HUD_TIMER] pid=%d update FAILED for td=%d", playerid, g_PlayerHUD[playerid][HUD_CLOCK]);
        }
    }
    printf("[HUD_UPDATE] pid=%d clock set_ok=%d td=%d minute=%d", playerid, set_ok_clock, g_PlayerHUD[playerid][HUD_CLOCK], minute);

    // Date - leave this (clock removed)
    new year, month, day;
    getdate(year, month, day);
    format(text, sizeof(text), "%02d.%02d.%d", day, month, year);
    new set_ok_date = 0;
    if(g_PlayerHUD[playerid][HUD_DATE] != PlayerText:INVALID_TEXT_DRAW && g_PlayerHUD[playerid][HUD_DATE] != 0)
    {
        set_ok_date = PlayerTextDrawSetString(playerid, g_PlayerHUD[playerid][HUD_DATE], text);
    }
    else
    {
        // Try to create lazily (player is spawned when updates happen)
        if(HUD_EnsureDateCreated(playerid))
        {
            set_ok_date = PlayerTextDrawSetString(playerid, g_PlayerHUD[playerid][HUD_DATE], text);
            // show it now that it exists
            PlayerTextDrawShow(playerid, g_PlayerHUD[playerid][HUD_DATE]);
        }
    }
    printf("[HUD_UPDATE] pid=%d date='%s' set_ok=%d td=%d", playerid, text, set_ok_date, g_PlayerHUD[playerid][HUD_DATE]);

    // Hunger
    format(text, sizeof(text), "Hunger: %d%%", pInfo[playerid][player_hunger]);
    new set_ok_hunger = 0;
    if(g_PlayerHUD[playerid][HUD_HUNGER] != PlayerText:INVALID_TEXT_DRAW && g_PlayerHUD[playerid][HUD_HUNGER] != 0)
    {
        set_ok_hunger = PlayerTextDrawSetString(playerid, g_PlayerHUD[playerid][HUD_HUNGER], text);
    }
    printf("[HUD_UPDATE] pid=%d hunger='%s' set_ok=%d td=%d", playerid, text, set_ok_hunger, g_PlayerHUD[playerid][HUD_HUNGER]);

    // Pragnienie
    format(text, sizeof(text), "Thirst: %d%%", pInfo[playerid][player_thirst]);
    new set_ok_thirst = 0;
    if(g_PlayerHUD[playerid][HUD_THIRST] != PlayerText:INVALID_TEXT_DRAW && g_PlayerHUD[playerid][HUD_THIRST] != 0)
    {
        set_ok_thirst = PlayerTextDrawSetString(playerid, g_PlayerHUD[playerid][HUD_THIRST], text);
    }
    printf("[HUD_UPDATE] pid=%d thirst='%s' set_ok=%d td=%d", playerid, text, set_ok_thirst, g_PlayerHUD[playerid][HUD_THIRST]);

    return 1;
}

/**
 * Force direct HUD clock update and return whether direct set succeeded (1) or not (0)
 */
stock HUD_ForceUpdate(playerid)
{
    if(!IsValidPlayerId(playerid)) return 0;
    // Force a full HUD update (clock, hunger, thirst, etc.) and return success.
    return HUD_Update(playerid);
}

/**
 * Update only the clock text for a single player. Used by minute-aligned timer.
 */
stock HUD_UpdateClockOnly(playerid)
{
    if(!IsValidPlayerId(playerid)) return 0;
    if(g_HUDFlags[playerid] & HUD_FLAG_HIDDEN) return 0;

    if(g_PlayerHUD[playerid][HUD_CLOCK] == PlayerText:INVALID_TEXT_DRAW || g_PlayerHUD[playerid][HUD_CLOCK] == 0)
    {
        if(!HUD_EnsureClockCreated(playerid)) return 0;
    }

    new hour, minute, second;
    gettime(hour, minute, second);

    new textClock[6]; format(textClock, sizeof(textClock), "%02d:%02d", hour, minute);
    new ok = PlayerTextDrawSetString(playerid, g_PlayerHUD[playerid][HUD_CLOCK], textClock);
    if(ok) PlayerTextDrawShow(playerid, g_PlayerHUD[playerid][HUD_CLOCK]);

    g_HUDPrevMinute[playerid] = minute;
    return ok;
}

// ===========================================================================
// PR??DKO??CIOMIERZ
// ===========================================================================

/**
 * Pokazuje pr??dko??ciomierz
 * @param playerid ID gracza
 */
stock Speedo_Show(playerid)
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }

    // Bez tla - lepiej wyglada
    // PlayerTextDrawShow(playerid, g_PlayerHUD[playerid][HUD_SPEEDO_BG]);
    PlayerTextDrawShow(playerid, g_PlayerHUD[playerid][HUD_SPEEDO_SPEED]);
    PlayerTextDrawShow(playerid, g_PlayerHUD[playerid][HUD_SPEEDO_FUEL]);
    PlayerTextDrawShow(playerid, g_PlayerHUD[playerid][HUD_SPEEDO_ENGINE]);
    PlayerTextDrawShow(playerid, g_PlayerHUD[playerid][HUD_SPEEDO_LIGHTS]);

    g_HUDFlags[playerid] |= HUD_FLAG_SPEEDO;

    return 1;
}

/**
 * Ukrywa pr??dko??ciomierz
 * @param playerid ID gracza
 */
stock Speedo_Hide(playerid)
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }

    // Bez tla - lepiej wyglada
    // PlayerTextDrawHide(playerid, g_PlayerHUD[playerid][HUD_SPEEDO_BG]);
    PlayerTextDrawHide(playerid, g_PlayerHUD[playerid][HUD_SPEEDO_SPEED]);
    PlayerTextDrawHide(playerid, g_PlayerHUD[playerid][HUD_SPEEDO_FUEL]);
    PlayerTextDrawHide(playerid, g_PlayerHUD[playerid][HUD_SPEEDO_ENGINE]);
    PlayerTextDrawHide(playerid, g_PlayerHUD[playerid][HUD_SPEEDO_LIGHTS]);

    g_HUDFlags[playerid] &= ~HUD_FLAG_SPEEDO;

    return 1;
}

/**
 * Aktualizuje pr??dko??ciomierz
 * @param playerid ID gracza
 * @param vehicleid ID pojazdu
 */
stock Speedo_Update(playerid, vehicleid)
{
    if(!IsValidPlayerId(playerid) || !(g_HUDFlags[playerid] & HUD_FLAG_SPEEDO))
    {
        return 0;
    }

    new text[64];

    // Pr??dko????
    new speed = GetVehicleSpeed(vehicleid);
    format(text, sizeof(text), "%d km/h", speed);
    PlayerTextDrawSetString(playerid, g_PlayerHUD[playerid][HUD_SPEEDO_SPEED], text);

    // Paliwo - sprawdz czy pojazd jest w systemie
    new slot = Vehicle_GetSlot(vehicleid);
    if(slot != -1)
    {
        new fuel = Vehicle_GetFuel(vehicleid);
        format(text, sizeof(text), "Fuel: %d%%", fuel);
    }
    else
    {
        text = "Fuel: ---";
    }
    PlayerTextDrawSetString(playerid, g_PlayerHUD[playerid][HUD_SPEEDO_FUEL], text);

    // Stan silnika - prefer server-side authoritative flags where possible
    // slot already obtained above when checking fuel
    new engine = 0, lights = 0;
    if(slot != -1)
    {
        engine = vInfo[slot][veh_engine];
        lights = vInfo[slot][veh_lights];
    }
    else
    {
        // fallback to native state for non-server vehicles
        new _engine, _lights, alarm, doors, bonnet, boot, objective;
        GetVehicleParamsEx(vehicleid, _engine, _lights, alarm, doors, bonnet, boot, objective);
        engine = _engine; lights = _lights;
    }

    format(text, sizeof(text), "Engine: %s", engine ? "~g~ON" : "~r~OFF");
    Polish_EncodeForClient(text, text, sizeof(text));
    PlayerTextDrawSetString(playerid, g_PlayerHUD[playerid][HUD_SPEEDO_ENGINE], text);

    format(text, sizeof(text), "Lights: %s", lights ? "~g~ON" : "~r~OFF");
    Polish_EncodeForClient(text, text, sizeof(text));
    PlayerTextDrawSetString(playerid, g_PlayerHUD[playerid][HUD_SPEEDO_LIGHTS], text);

    return 1;
}

// Force update timer to handle deferred engine state changes on enter
timer Speedo_ForceUpdate[500](playerid, vehicleid)
{
    Speedo_Update(playerid, vehicleid);
    return 1;
}

// ===========================================================================
// STAN BW (BITMASK WOUND)
// ===========================================================================

/**
 * Pokazuje komunikat BW
 * @param playerid ID gracza
 * @param seconds Pozosta??y czas
 */
stock BW_Show(playerid, seconds)
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }

    new text[128];
    format(text, sizeof(text),
        "~r~YOU ARE INJURED~n~~w~Time left: %s~n~Wait for medical assistance",
        FormatDuration(seconds));

    PlayerTextDrawSetString(playerid, g_PlayerHUD[playerid][HUD_BW], text);
    PlayerTextDrawShow(playerid, g_PlayerHUD[playerid][HUD_BW]);

    return 1;
}

/**
 * Ukrywa komunikat BW
 * @param playerid ID gracza
 */
stock BW_Hide(playerid)
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }

    PlayerTextDrawHide(playerid, g_PlayerHUD[playerid][HUD_BW]);

    return 1;
}

// ===========================================================================
// POWIADOMIENIA
// ===========================================================================

/**
 * Wy??wietla powiadomienie na ekranie
 * @param playerid ID gracza
 * @param message Wiadomo????
 * @param duration Czas wy??wietlania (ms)
 * @param color Kolor tekstu
 */
stock ShowNotification(playerid, const message[], duration = 5000, color = 0xFFFFFFFF)
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }

    // Anuluj poprzedni timer
    if(g_NotifyTimer[playerid] != 0)
    {
        KillTimer(g_NotifyTimer[playerid]);
    }

    PlayerTextDrawColour(playerid, g_PlayerHUD[playerid][HUD_NOTIFY], color);
    PlayerTextDrawSetString(playerid, g_PlayerHUD[playerid][HUD_NOTIFY], message);
    PlayerTextDrawShow(playerid, g_PlayerHUD[playerid][HUD_NOTIFY]);

    g_HUDFlags[playerid] |= HUD_FLAG_NOTIFY;

    // Timer do ukrycia
    g_NotifyTimer[playerid] = SetTimerEx("HideNotificationTimer", duration, false, "i", playerid);

    return 1;
}

forward HideNotificationTimer(playerid);
public HideNotificationTimer(playerid)
{
    if(IsValidPlayerId(playerid) && (g_HUDFlags[playerid] & HUD_FLAG_NOTIFY))
    {
        PlayerTextDrawHide(playerid, g_PlayerHUD[playerid][HUD_NOTIFY]);
        g_HUDFlags[playerid] &= ~HUD_FLAG_NOTIFY;
    }

    g_NotifyTimer[playerid] = 0;

    return 1;
}

// ===========================================================================
// HOOKI
// ===========================================================================

hook OnPlayerConnect@Textdraws(playerid)
{
    HUD_Create(playerid);
    return 1;
}

hook OnPlayerDisconnect@Textdraws(playerid, reason)
{
    HUD_Destroy(playerid);
    return 1;
}

hook OnPlayerSpawn@Textdraws(playerid)
{
    // Poka?? HUD po spawnowaniu
    if(pInfo[playerid][player_logged])
    {
        HUD_Show(playerid);
    }

    return 1;
}

static g_DebugSpeedo = 0;

hook OnPlayerStateChange@Textdraws(playerid, newstate, oldstate)
{
    // Pr??dko??ciomierz
    if(newstate == PLAYER_STATE_DRIVER)
    {
        Speedo_Show(playerid);

        if(g_DebugSpeedo)
        {
            new vehicleid = GetPlayerVehicleID(playerid);
            new slot = Vehicle_GetSlot(vehicleid);
            new engine, lights, alarm, doors, bonnet, boot, objective;
            GetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, boot, objective);
            printf("[SpeedoDebug] player=%d vehicle=%d slot=%d nativeEngine=%d nativeLights=%d vInfoEngine=%d vInfoLights=%d\n", playerid, vehicleid, slot, engine, lights, slot != -1 ? vInfo[slot][veh_engine] : -1, slot != -1 ? vInfo[slot][veh_lights] : -1);

            // Force immediate update to avoid initial stale state
            Speedo_Update(playerid, vehicleid);

            // Also schedule a delayed update in case some other deferred logic changes state shortly after enter
            defer Speedo_ForceUpdate[500](playerid, vehicleid);
        }
    }
    else if(oldstate == PLAYER_STATE_DRIVER)
    {
        Speedo_Hide(playerid);

        // Wyłącz tempomat gdy gracz wysiada z pojazdu
        if(g_PlayerCruiseControl[playerid] > 0)
        {
            g_PlayerCruiseControl[playerid] = 0;
            SendClientMessage(playerid, COLOR_WARNING, "Cruise control disabled.");
        }
    }

    return 1;
}

// ===========================================================================
// EOF
// ===========================================================================

