/*
 * =============================================================================
 *  TIMERS - System timer??w
 * =============================================================================
 *  
 *  Modu??: core/timers.inc
 *  Opis: Timery serwera i gracza z wykorzystaniem y_timers
 *
 *  ZMIANY W REFAKTORZE:
 *  - Zast??piono SetTimer/SetTimerEx na y_timers (task/timer)
 *  - Zoptymalizowano cykle przetwarzania
 *  - Unikano wielokrotnych iteracji w tym samym ticku
 *
 * =============================================================================
 */

#if defined _timers_included
    #endinput
#endif
#define _timers_included

// ===========================================================================
// STA??E
// ===========================================================================

#define TIMER_GLOBAL_UPDATE     1000        // Globalny update co 1s
#define TIMER_PLAYER_UPDATE     1000        // Update gracza co 1s
#define TIMER_VEHICLE_UPDATE    5000        // Update pojazdu co 5s
#define TIMER_SPEEDO_UPDATE     200         // Update speedometra co 200ms (płynne odświeżanie)
#define TIMER_PAYDAY_INTERVAL   3600000     // Payday co godzin??
#define TIMER_SAVE_INTERVAL     300000      // Auto-save co 5 min

// ===========================================================================
// GLOBALNY TIMER - SEKUNDOWY
// ===========================================================================

task GlobalSecondTimer[TIMER_GLOBAL_UPDATE]()
{
    // Aktualizuj czas serwera
    new hour, minute, second;
    gettime(hour, minute, second);
    
    // Aktualizuj czas gry (opcjonalnie)
    // SetWorldTime(hour);
    
    // Przetwarzaj graczy
    foreach(new playerid : Player)
    {
        Timer_ProcessPlayer(playerid);
    }
}

/**
 * Przetwarza timery dla pojedynczego gracza
 * @param playerid ID gracza
 */
stock Timer_ProcessPlayer(playerid)
{
    if(!IsPlayerConnected(playerid) || !pInfo[playerid][player_logged])
    {
        return;
    }
    
    // === G????D I PRAGNIENIE ===
    if(pInfo[playerid][player_spawned])
    {
        // Zmniejsz g????d co minut??
        static lastHungerUpdate[MAX_PLAYERS];
        
        if(gettime() - lastHungerUpdate[playerid] >= 60)
        {
            lastHungerUpdate[playerid] = gettime();
            
            if(pInfo[playerid][player_hunger] > 0)
            {
                pInfo[playerid][player_hunger]--;
            }
            
            if(pInfo[playerid][player_thirst] > 0)
            {
                pInfo[playerid][player_thirst]--;
            }
            
            // Efekty g??odu/pragnienia
            if(pInfo[playerid][player_hunger] <= 10)
            {
                SendClientMessage(playerid, COLOR_RED, "Jeste?? bardzo g??odny! Zjedz co??.");
                
                if(pInfo[playerid][player_hunger] == 0)
                {
                    Player_GiveHealth(playerid, -5.0);
                }
            }
            
            if(pInfo[playerid][player_thirst] <= 10)
            {
                SendClientMessage(playerid, COLOR_RED, "Jeste?? spragniony! Napij si?? czego??.");
                
                if(pInfo[playerid][player_thirst] == 0)
                {
                    Player_GiveHealth(playerid, -5.0);
                }
            }
        }
    }
    
    // === SPRAWDZENIE AFK ===
    static lastActivityCheck[MAX_PLAYERS];
    
    if(gettime() - pInfo[playerid][player_last_activity] > 180) // 3 minuty bez aktywno??ci
    {
        if(!pInfo[playerid][player_afk])
        {
            Player_SetAFK(playerid, true);
            SendClientMessage(playerid, COLOR_YELLOW, "Zosta??e?? oznaczony jako AFK.");
        }
    }
    
    // === BW (Brutalnie Powalony) ===
    if(pInfo[playerid][player_bw] > 0)
    {
        pInfo[playerid][player_bw]--;
        
        if(pInfo[playerid][player_bw] == 0)
        {
            SendClientMessage(playerid, COLOR_GREEN, "Obudzi??e?? si?? po pobiciu.");
            TogglePlayerControllable(playerid, true);
        }
        else
        {
            // Co 30 sekund informacja
            if(pInfo[playerid][player_bw] % 30 == 0)
            {
                new msg[64];
                format(msg, sizeof(msg), "BW pozosta??o: %d sekund", pInfo[playerid][player_bw]);
                SendClientMessage(playerid, COLOR_GRAY, msg);
            }
        }
    }
    
    // === ARESZT (AJ) ===
    if(pInfo[playerid][player_aj] > 0)
    {
        static lastAJUpdate[MAX_PLAYERS];
        
        if(gettime() - lastAJUpdate[playerid] >= 60) // Co minut??
        {
            lastAJUpdate[playerid] = gettime();
            pInfo[playerid][player_aj]--;
            
            if(pInfo[playerid][player_aj] == 0)
            {
                // Zwolnij z wi??zienia
                SendClientMessage(playerid, COLOR_GREEN, "Twoja kara dobieg??a ko??ca. Zosta??e?? zwolniony.");
                Player_Respawn(playerid, SPAWN_DEFAULT);
            }
            else
            {
                new msg[64];
                format(msg, sizeof(msg), "Pozosta??o %d minut kary.", pInfo[playerid][player_aj]);
                SendClientMessage(playerid, COLOR_GRAY, msg);
            }
        }
    }
    
    // === UPOJENIE ALKOHOLOWE ===
    if(pInfo[playerid][player_drunk] > 0)
    {
        pInfo[playerid][player_drunk] -= 10;
        
        if(pInfo[playerid][player_drunk] <= 0)
        {
            pInfo[playerid][player_drunk] = 0;
            SetPlayerDrunkLevel(playerid, 0);
        }
        else
        {
            SetPlayerDrunkLevel(playerid, pInfo[playerid][player_drunk]);
        }
    }
}

// ===========================================================================
// TIMER POJAZD??W
// ===========================================================================

// ===========================================================================
// TIMER SPEEDOMETRA (szybki - 200ms)
// ===========================================================================

task SpeedoUpdateTimer[TIMER_SPEEDO_UPDATE]()
{
    foreach(new playerid : Player)
    {
        if(!pInfo[playerid][player_spawned])
        {
            continue;
        }
        
        new vehicleid = GetPlayerVehicleID(playerid);
        
        if(vehicleid == 0)
        {
            continue;
        }
        
        if(GetPlayerState(playerid) != PLAYER_STATE_DRIVER)
        {
            continue;
        }
        
        // Aktualizuj speedometr
        Speedo_Update(playerid, vehicleid);
        
        // === TEMPOMAT (ogranicznik prędkości) ===
        if(g_PlayerCruiseControl[playerid] > 0)
        {
            new Float:vX, Float:vY, Float:vZ;
            GetVehicleVelocity(vehicleid, vX, vY, vZ);
            
            // Oblicz aktualną prędkość (tylko poziomą, bez Z)
            new Float:horizontalSpeed = floatsqroot(vX * vX + vY * vY);
            new Float:currentSpeed = horizontalSpeed * 180.0;
            new Float:maxSpeed = float(g_PlayerCruiseControl[playerid]);
            
            // Sprawdź czy silnik jest włączony
            new engine, lights, alarm, doors, bonnet, boot, objective;
            GetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, boot, objective);
            
            if(!engine)
            {
                // Silnik wyłączony - wyłącz tempomat
                g_PlayerCruiseControl[playerid] = 0;
                SendClientMessage(playerid, COLOR_WARNING, "Tempomat wyłączony - silnik nie działa.");
            }
            else if(currentSpeed > maxSpeed)
            {
                // Prędkość przekracza limit - ogranicz ją
                new Float:maxVel = maxSpeed / 180.0;
                new Float:ratio = maxVel / horizontalSpeed;
                
                // Skaluj prędkość do limitu
                SetVehicleVelocity(vehicleid, vX * ratio, vY * ratio, vZ);
            }
        }
    }
}

// ===========================================================================
// TIMER POJAZDÓW
// ===========================================================================

task VehicleUpdateTimer[TIMER_VEHICLE_UPDATE]()
{
    foreach(new playerid : Player)
    {
        if(!pInfo[playerid][player_spawned])
        {
            continue;
        }
        
        new vehicleid = GetPlayerVehicleID(playerid);
        
        if(vehicleid == 0)
        {
            continue;
        }
        
        // Sprawd?? czy pojazd jest serwerowy
        new slot = Vehicle_GetSlot(vehicleid);
        
        if(slot == -1)
        {
            continue;
        }
        
        // === ZU??YCIE PALIWA ===
        if(Vehicle_IsEngineOn(vehicleid))
        {
            static Float:lastPos[MAX_PLAYERS][3];
            
            new Float:x, Float:y, Float:z;
            GetVehiclePos(vehicleid, x, y, z);
            
            new Float:dist = GetDistanceBetweenPoints3D(
                lastPos[playerid][0], lastPos[playerid][1], lastPos[playerid][2],
                x, y, z
            );
            
            // Aktualizuj pozycj??
            lastPos[playerid][0] = x;
            lastPos[playerid][1] = y;
            lastPos[playerid][2] = z;
            
            // Zu??yj paliwo
            if(dist > 0.0)
            {
                Vehicle_ConsumeFuel(vehicleid, dist);
            }
            
            // Sprawd?? paliwo
            if(Vehicle_GetFuel(vehicleid) <= 0)
            {
                Vehicle_SetEngine(vehicleid, false);
                SendClientMessage(playerid, COLOR_RED, "Pojazd stan???? - brak paliwa!");
            }
        }
        
        // === AKTUALIZACJA STANU ===
        new Float:health;
        GetVehicleHealth(vehicleid, health);
        vInfo[slot][veh_health] = health;
    }
}

// ===========================================================================
// PAYDAY TIMER
// ===========================================================================

task PaydayTimer[TIMER_PAYDAY_INTERVAL]()
{
    // Wyp??a?? pensje z grup
    foreach(new groupId : Groups)
    {
        Group_PaySalaries(groupId);
    }
    
    // Wyp??a?? zasi??ek dla bezrobotnych
    foreach(new playerid : Player)
    {
        if(!pInfo[playerid][player_logged])
        {
            continue;
        }
        
        // Sprawd?? czy ma prac??
        new bool:hasJob = false;
        
        for(new i = 0; i < MAX_PLAYER_GROUPS; i++)
        {
            if(pInfo[playerid][player_group][i] > 0)
            {
                hasJob = true;
                break;
            }
        }
        
        if(!hasJob)
        {
            // Zasi??ek dla bezrobotnych
            new welfare = Setting[setting_welfare_amount];
            if(welfare > 0)
            {
                Player_GiveMoney(playerid, welfare);
                SendClientMessage(playerid, COLOR_GREEN, 
                    "Otrzyma??e?? zasi??ek dla bezrobotnych. Znajd?? prac??!");
            }
        }
        
        // Aktualizuj czas gry
        pInfo[playerid][player_gameTime] += 60; // Godzina
        
        // Aktualizuj payday counter
        pInfo[playerid][player_paydays]++;
    }
    
    SendClientMessageToAll(COLOR_YELLOW, "[PAYDAY] Wszyscy gracze otrzymali swoje wyp??aty!");
}

// ===========================================================================
// AUTO-SAVE TIMER
// ===========================================================================

task AutoSaveTimer[TIMER_SAVE_INTERVAL]()
{
    new savedCount = 0;
    
    foreach(new playerid : Player)
    {
        if(pInfo[playerid][player_logged])
        {
            Player_SaveData(playerid, "global-autosave");
            savedCount++;
        }
    }
    
    // Zapisz pojazdy
    foreach(new vehSlot : Vehicles)
    {
        Vehicle_Save(vehSlot);
    }
    
    // Zapisz grupy
    foreach(new groupId : Groups)
    {
        Group_Save(groupId);
    }
    
    printf("[AutoSave] Zapisano dane %d graczy, pojazd??w i grup", savedCount);
}

// ===========================================================================
// WEATHER/TIME TIMER
// ===========================================================================

static g_CurrentWeather = 0;
static g_CurrentHour = 12;

task WeatherTimeTimer[60000]() // Co minut??
{
    // Aktualizuj godzin?? w grze
    g_CurrentHour++;
    
    if(g_CurrentHour >= 24)
    {
        g_CurrentHour = 0;
        
        // Nowy dzie?? - mo??e zmieni?? pogod??
        if(random(100) < 30) // 30% szans na zmian?? pogody
        {
            g_CurrentWeather = random(19); // 0-18 pogody
            SetWeather(g_CurrentWeather);
        }
    }
    
    SetWorldTime(g_CurrentHour);
}

// ===========================================================================
// CUSTOM PLAYER TIMERS
// ===========================================================================

/**
 * Uruchamia timer dla gracza
 * @param playerid ID gracza
 * @param callback Nazwa callbacku
 * @param interval Interwa??
 * @param repeating Czy powtarza??
 * @return ID timera
 */
stock Timer_StartForPlayer(playerid, const callback[], interval, bool:repeating = false)
{
    return SetTimerEx(callback, interval, repeating, "d", playerid);
}

/**
 * Zatrzymuje timer gracza
 * @param timerid ID timera
 */
stock Timer_Stop(timerid)
{
    if(timerid != 0)
    {
        KillTimer(timerid);
    }
}

// ===========================================================================
// UTILITY TIMERS
// ===========================================================================

/**
 * Op????nione wykonanie funkcji (defer)
 * @param callback Nazwa callbacku
 * @param delay Op????nienie w ms
 */
stock Timer_Defer(const callback[], delay)
{
    return SetTimer(callback, delay, false);
}

/**
 * Op????nione wykonanie dla gracza
 * @param callback Nazwa callbacku
 * @param delay Op????nienie w ms
 * @param playerid ID gracza
 */
stock Timer_DeferPlayer(const callback[], delay, playerid)
{
    return SetTimerEx(callback, delay, false, "d", playerid);
}

// ===========================================================================
// HOOKS
// ===========================================================================

hook OnPlayerConnect@Timers(playerid)
{
    // Inicjalizacja zmiennych czasowych gracza
    pInfo[playerid][player_connect_time] = gettime();
    pInfo[playerid][player_last_activity] = gettime();
    
    return 1;
}

hook OnPlayerUpdate@Timers(playerid)
{
    // Aktualizuj ostatni?? aktywno????
    pInfo[playerid][player_last_activity] = gettime();
    
    // Wy????cz AFK je??li by??
    if(pInfo[playerid][player_afk])
    {
        Player_SetAFK(playerid, false);
    }
    
    return 1;
}

// ===========================================================================
// EOF
// ===========================================================================


