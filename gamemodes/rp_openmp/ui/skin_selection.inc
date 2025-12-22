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

    ShowModelSelectionMenu(playerid, "Wyb\243r skina", MODEL_SELECTION_SKIN_MENU, skins);
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

    return 0;
}
