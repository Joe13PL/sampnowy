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
    g_PlayerHUD[playerid][HUD_CLOCK] = CreatePlayerTextDraw(playerid, 635.0, 22.0, "00:00");
    PlayerTextDrawFont(playerid, g_PlayerHUD[playerid][HUD_CLOCK], 2);
    PlayerTextDrawLetterSize(playerid, g_PlayerHUD[playerid][HUD_CLOCK], 0.5, 2.0);
    PlayerTextDrawColour(playerid, g_PlayerHUD[playerid][HUD_CLOCK], 0xFFFFFFFF);
    PlayerTextDrawSetOutline(playerid, g_PlayerHUD[playerid][HUD_CLOCK], 1);
    PlayerTextDrawAlignment(playerid, g_PlayerHUD[playerid][HUD_CLOCK], 3);

    // === DATA ===
    g_PlayerHUD[playerid][HUD_DATE] = CreatePlayerTextDraw(playerid, 635.0, 42.0, "01.01.2024");
    PlayerTextDrawFont(playerid, g_PlayerHUD[playerid][HUD_DATE], 2);
    PlayerTextDrawLetterSize(playerid, g_PlayerHUD[playerid][HUD_DATE], 0.2, 0.8);
    PlayerTextDrawColour(playerid, g_PlayerHUD[playerid][HUD_DATE], 0xCCCCCCFF);
    PlayerTextDrawSetOutline(playerid, g_PlayerHUD[playerid][HUD_DATE], 1);
    PlayerTextDrawAlignment(playerid, g_PlayerHUD[playerid][HUD_DATE], 3);

    // === PIENI??DZE ===
    g_PlayerHUD[playerid][HUD_MONEY] = CreatePlayerTextDraw(playerid, 635.0, 60.0, "0");
    PlayerTextDrawFont(playerid, g_PlayerHUD[playerid][HUD_MONEY], 2);
    PlayerTextDrawLetterSize(playerid, g_PlayerHUD[playerid][HUD_MONEY], 0.35, 1.4);
    PlayerTextDrawColour(playerid, g_PlayerHUD[playerid][HUD_MONEY], 0x2ECC71FF);
    PlayerTextDrawSetOutline(playerid, g_PlayerHUD[playerid][HUD_MONEY], 1);
    PlayerTextDrawAlignment(playerid, g_PlayerHUD[playerid][HUD_MONEY], 3);

    // === PASEK ZDROWIA ===
    g_PlayerHUD[playerid][HUD_HEALTH] = CreatePlayerTextDraw(playerid, 635.0, 80.0, "HP: 100");
    PlayerTextDrawFont(playerid, g_PlayerHUD[playerid][HUD_HEALTH], 2);
    PlayerTextDrawLetterSize(playerid, g_PlayerHUD[playerid][HUD_HEALTH], 0.2, 0.8);
    PlayerTextDrawColour(playerid, g_PlayerHUD[playerid][HUD_HEALTH], 0xE74C3CFF);
    PlayerTextDrawSetOutline(playerid, g_PlayerHUD[playerid][HUD_HEALTH], 1);
    PlayerTextDrawAlignment(playerid, g_PlayerHUD[playerid][HUD_HEALTH], 3);

    // === G????D ===
    g_PlayerHUD[playerid][HUD_HUNGER] = CreatePlayerTextDraw(playerid, 635.0, 92.0, "~y~Glod: 100%");
    PlayerTextDrawFont(playerid, g_PlayerHUD[playerid][HUD_HUNGER], 2);
    PlayerTextDrawLetterSize(playerid, g_PlayerHUD[playerid][HUD_HUNGER], 0.2, 0.8);
    PlayerTextDrawColour(playerid, g_PlayerHUD[playerid][HUD_HUNGER], 0xF39C12FF);
    PlayerTextDrawSetOutline(playerid, g_PlayerHUD[playerid][HUD_HUNGER], 1);
    PlayerTextDrawAlignment(playerid, g_PlayerHUD[playerid][HUD_HUNGER], 3);

    // === PRAGNIENIE ===
    g_PlayerHUD[playerid][HUD_THIRST] = CreatePlayerTextDraw(playerid, 635.0, 104.0, "~b~Pragnienie: 100%");
    PlayerTextDrawFont(playerid, g_PlayerHUD[playerid][HUD_THIRST], 2);
    PlayerTextDrawLetterSize(playerid, g_PlayerHUD[playerid][HUD_THIRST], 0.2, 0.8);
    PlayerTextDrawColour(playerid, g_PlayerHUD[playerid][HUD_THIRST], 0x3498DBFF);
    PlayerTextDrawSetOutline(playerid, g_PlayerHUD[playerid][HUD_THIRST], 1);
    PlayerTextDrawAlignment(playerid, g_PlayerHUD[playerid][HUD_THIRST], 3);

    // === BITMASK WOUND (BW) ===
    g_PlayerHUD[playerid][HUD_BW] = CreatePlayerTextDraw(playerid, 320.0, 400.0, "~r~JESTES RANNY~n~~w~Poczekaj na pomoc medyczna");
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

    // === PR??DKO??CIOMIERZ - PALIWO ===
    g_PlayerHUD[playerid][HUD_SPEEDO_FUEL] = CreatePlayerTextDraw(playerid, 510.0, 375.0, "Paliwo: 100%");
    PlayerTextDrawFont(playerid, g_PlayerHUD[playerid][HUD_SPEEDO_FUEL], 2);
    PlayerTextDrawLetterSize(playerid, g_PlayerHUD[playerid][HUD_SPEEDO_FUEL], 0.2, 0.8);
    PlayerTextDrawColour(playerid, g_PlayerHUD[playerid][HUD_SPEEDO_FUEL], 0xF39C12FF);
    PlayerTextDrawSetOutline(playerid, g_PlayerHUD[playerid][HUD_SPEEDO_FUEL], 1);

    // === PR??DKO??CIOMIERZ - SILNIK ===
    g_PlayerHUD[playerid][HUD_SPEEDO_ENGINE] = CreatePlayerTextDraw(playerid, 510.0, 390.0, "Silnik: ~r~OFF");
    PlayerTextDrawFont(playerid, g_PlayerHUD[playerid][HUD_SPEEDO_ENGINE], 2);
    PlayerTextDrawLetterSize(playerid, g_PlayerHUD[playerid][HUD_SPEEDO_ENGINE], 0.2, 0.8);
    PlayerTextDrawColour(playerid, g_PlayerHUD[playerid][HUD_SPEEDO_ENGINE], 0xFFFFFFFF);
    PlayerTextDrawSetOutline(playerid, g_PlayerHUD[playerid][HUD_SPEEDO_ENGINE], 1);

    // === PR??DKO??CIOMIERZ - ??WIAT??A ===
    g_PlayerHUD[playerid][HUD_SPEEDO_LIGHTS] = CreatePlayerTextDraw(playerid, 510.0, 405.0, "Światła: ~r~OFF");
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

    // Pokaż globalne
    TextDrawShowForPlayer(playerid, g_GlobalTextdraws[0]);

    // Poka?? podstawowe elementy
    PlayerTextDrawShow(playerid, g_PlayerHUD[playerid][HUD_CLOCK]);
    PlayerTextDrawShow(playerid, g_PlayerHUD[playerid][HUD_DATE]);
    PlayerTextDrawShow(playerid, g_PlayerHUD[playerid][HUD_HUNGER]);
    PlayerTextDrawShow(playerid, g_PlayerHUD[playerid][HUD_THIRST]);

    g_HUDFlags[playerid] &= ~HUD_FLAG_HIDDEN;

    // Aktualizuj warto??ci
    HUD_Update(playerid);

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

    // Zegar
    new hour, minute, second;
    gettime(hour, minute, second);
    format(text, sizeof(text), "%02d:%02d", hour, minute);
    PlayerTextDrawSetString(playerid, g_PlayerHUD[playerid][HUD_CLOCK], text);

    // Data
    new year, month, day;
    getdate(year, month, day);
    format(text, sizeof(text), "%02d.%02d.%d", day, month, year);
    PlayerTextDrawSetString(playerid, g_PlayerHUD[playerid][HUD_DATE], text);

    // G????d
    format(text, sizeof(text), "Glod: %d%%", pInfo[playerid][player_hunger]);
    PlayerTextDrawSetString(playerid, g_PlayerHUD[playerid][HUD_HUNGER], text);

    // Pragnienie
    format(text, sizeof(text), "Pragnienie: %d%%", pInfo[playerid][player_thirst]);
    PlayerTextDrawSetString(playerid, g_PlayerHUD[playerid][HUD_THIRST], text);

    return 1;
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
        format(text, sizeof(text), "Paliwo: %d%%", fuel);
    }
    else
    {
        text = "Paliwo: ---";
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

    format(text, sizeof(text), "Silnik: %s", engine ? "~g~ON" : "~r~OFF");
    Polish_EncodeForClient(text, text, sizeof(text));
    PlayerTextDrawSetString(playerid, g_PlayerHUD[playerid][HUD_SPEEDO_ENGINE], text);

    format(text, sizeof(text), "Swiatla: %s", lights ? "~g~ON" : "~r~OFF");
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
        "~r~JESTES RANNY~n~~w~Pozostalo: %s~n~Poczekaj na pomoc medyczna",
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
            SendClientMessage(playerid, COLOR_WARNING, "Tempomat wyłączony.");
        }
    }

    return 1;
}

// ===========================================================================
// EOF
// ===========================================================================

