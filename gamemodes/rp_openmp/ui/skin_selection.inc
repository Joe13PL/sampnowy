#if defined _skin_selection_included
    #endinput
#endif
#define _skin_selection_included

// Wrapper on top of eSelection for skin choosing.
// Stores per-player callback name to invoke after selection or cancel.

#define MODEL_SELECTION_SKIN_MENU (5000)

static g_SkinSelectCallback[MAX_PLAYERS][32];

stock SkinSelection_Show(playerid, const callback[])
{
    if(!IsPlayerConnected(playerid))
    {
        return 0;
    }

    strcopy(g_SkinSelectCallback[playerid], callback, sizeof g_SkinSelectCallback[]);

    new List:skins = list_new();
    new label[24];

    for(new i; i < 312; i++)
    {
        format(label, sizeof(label), "Skin %d", i);
        AddModelMenuItem(skins, i, label);
    }

    ShowModelSelectionMenu(playerid, "Skin Selection", MODEL_SELECTION_SKIN_MENU, skins);
    return 1;
}

public OnModelSelectionResponse(playerid, extraid, index, modelid, response)
{
    if(extraid == MODEL_SELECTION_SKIN_MENU)
    {
        // response == MODEL_RESPONSE_SELECT (1) when a model is clicked, 0 when canceled
        new chosen = (response == MODEL_RESPONSE_SELECT) ? modelid : -1;

        if(g_SkinSelectCallback[playerid][0])
        {
            CallLocalFunction(g_SkinSelectCallback[playerid], "ii", playerid, chosen);
        }

        g_SkinSelectCallback[playerid][0] = EOS;
        return 1;
    }

    #if !defined CARSHOP_MODEL_MENU
        #define CARSHOP_MODEL_MENU 5100
    #endif

    if(extraid == CARSHOP_MODEL_MENU)
    {
        // Only proceed on a selection
        if(response != MODEL_RESPONSE_SELECT) return 1;

        // Debug logging: show which model was clicked
        printf("[DBG] OnModelSelectionResponse (CARSHOP_MODEL_MENU): pid=%d index=%d modelid=%d response=%d\n", playerid, index, modelid, response);

        // Hide the model selection menu first to prevent the click from being interpreted as a dialog response
        HideModelSelectionMenu(playerid);

        new idx = index; // index within the current list

        new name[64]; new price;
        new foundCat;
        if(!CarShop_FindByModel(modelid, foundCat, idx))
        {
            SendClientMessage(playerid, COLOR_ERROR, "Invalid car selection.");
            return 1;
        }
        if(!CarShop_GetVehicleInfo(foundCat, idx, name, sizeof(name), price, modelid))
        {
            SendClientMessage(playerid, COLOR_ERROR, "Invalid car selection.");
            return 1;
        }

        new msg[128];
        format(msg, sizeof(msg), "Do you want to buy %s for $%d?", name, price);
        new data[32]; format(data, sizeof(data), "%d;%d", foundCat, idx);

        // Hide menu and show confirmation after a short delay to avoid click-race
        HideModelSelectionMenu(playerid);
        printf("[DBG] Scheduling delayed confirm for pid=%d data='%s'\n", playerid, data);
        ShowConfirmDialogExDelayed(playerid, "carshop_buy", "Confirm Purchase", msg, data, 250);
        return 1;
    }

    return 0;
}
