/*
 * FORWARDS - Deklaracje forward dla funkcji używanych między modułami
 */

#if defined _forwards_included
    #endinput
#endif
#define _forwards_included

// VEHICLES.INC
forward Vehicle_GetSlot(vehicleid);
forward bool:Vehicle_IsOwner(playerid, vehicleid);
forward Vehicle_SetOwner(slot, playerid);
forward Vehicle_Save(slot);
forward bool:Vehicle_IsEngineOn(vehicleid);
forward Vehicle_SetEngine(vehicleid, bool:state);
forward Vehicle_GetFuel(vehicleid);
forward Vehicle_ConsumeFuel(vehicleid, Float:distance);
forward Vehicle_Respawn(vehicleid);
forward Vehicle_Refuel(vehicleid, amount);
forward Vehicle_GetPlayerVehicles(playerid, output[], maxOutput);
forward Vehicle_SetSpawnPos(vehicleid, Float:x, Float:y, Float:z, Float:a);
forward Vehicle_ToggleLock(vehicleid);
forward Vehicle_SetLocked(vehicleid, bool:locked);
forward bool:Vehicle_IsLocked(vehicleid);
forward bool:Vehicle_IsServerVehicle(vehicleid);

// DOORS.INC
forward Door_GetNearest(playerid, Float:range);

// UTILS/TIME.INC
forward GetTimestamp();
forward FormatDuration(seconds, output[], size, bool:showSeconds);

// PLAYER/PLAYER_COMMANDS.INC
forward cmd_inv(playerid);
