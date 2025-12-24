/*
 * APlace - Simple shop / place creation system
 * - Admin command /aplace to create a shop at player's position
 * - Stores shops in memory (TODO: persist to DB)
 */

#if defined _aplace_included
    #endinput
#endif
#define _aplace_included

new g_ShopCount = 0;
new g_ShopX[256];
new g_ShopY[256];
new g_ShopZ[256];
new g_ShopType[256]; // e.g., 0 = vehicle shop, 1 = clothing, etc.

// Admin command: /aplace create <type>
YCMD:aplace(playerid, params[], help)
{
    CMD_GUARDS();

    if(!IsPlayerAdmin(playerid)) return SendClientMessage(playerid, COLOR_ERROR, "Brak uprawnien.");

    new sub[32];
    if(sscanf(params, "s", sub))
    {
        return SendClientMessage(playerid, COLOR_GREY, "Uzycie: /aplace create <type>");
    }

    if(strcmp(sub, "create", true) == 0)
    {
        new tstr[16];
        if(sscanf(params, "s s", sub, tstr) == 0)
        {
            return SendClientMessage(playerid, COLOR_GREY, "Uzycie: /aplace create <type>");
        }
        new Float:x, Float:y, Float:z;
        GetPlayerPos(playerid, x, y, z);
        new stype = 0;
        if(strcmp(tstr, "vehicle", true) == 0) stype = 0;
        else if(strcmp(tstr, "clothing", true) == 0) stype = 1;
        else if(strcmp(tstr, "general", true) == 0) stype = 2;
        else return SendClientMessage(playerid, COLOR_ERROR, "Nieznany typ. Dostepne: vehicle, clothing, general");

        new id = g_ShopCount++;
        g_ShopX[id] = floatround(x);
        g_ShopY[id] = floatround(y);
        g_ShopZ[id] = floatround(z);
        g_ShopType[id] = stype;

        SendClientMessage(playerid, COLOR_SUCCESS, "Shop created.");
        printf("[APlace] Admin %d created shop id=%d type=%d at %.2f, %.2f, %.2f", playerid, id, stype, x, y, z);
        return 1;
    }

    return SendClientMessage(playerid, COLOR_GREY, "Uzycie: /aplace create <type>");
}
