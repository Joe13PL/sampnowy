/*
 * Car Shop UI (eSelection-based)
 * - /carshop opens a category selector (SUV / Sports / Casual)
 * - selecting a category shows cars from that category with prices
 * - selecting a car shows a confirmation dialog (purchase flow not implemented)
 */

#if defined _car_shop_included
    #endinput
#endif
#define _car_shop_included

#include "ui/eSelection.inc"

forward CarShop_OnCategorySelected(playerid, selection);
forward CarShop_OnVehicleSelected(playerid, selection);

enum E_CAR_CAT
{
    CAR_CAT_SUV,
    CAR_CAT_SPORTS,
    CAR_CAT_CASUAL,
    CAR_CAT_MAX
};

#define CARSHOP_MODEL_MENU 5100

// Sample car catalog (In future: load from DB / config)
#define CARSHOP_MAX_PER_CAT 8

// Per-category vehicle names and prices (static prototype)
new CarShop_SUV_Names[][] = { "Landstalker", "Perennial" };
new CarShop_SUV_Prices[] = { 18000, 15000 };
new CarShop_SUV_Models[] = { 400, 404 };

new CarShop_SPORTS_Names[][] = { "Banshee", "Infernus" };
new CarShop_SPORTS_Prices[] = { 55000, 120000 };
new CarShop_SPORTS_Models[] = { 429, 411 };

new CarShop_CASUAL_Names[][] = { "Sentinel", "Solair" };
new CarShop_CASUAL_Prices[] = { 8000, 12000 };
new CarShop_CASUAL_Models[] = { 405, 458 };

new CarShop_VehicleCount[CAR_CAT_MAX] = { 2, 2, 2 };

// Simple per-player dialog state
new g_CarShop_PlayerSelection[MAX_PLAYERS];
new g_CarShop_PlayerCategory[MAX_PLAYERS];

// Command to open car shop categories
YCMD:carshop(playerid, params[], help)
{
    CMD_GUARDS();

    // Build a simple category selection using eSelection
    new title[64];
    strcopy(title, "Car Shop - Categories", sizeof(title));
    new items[3][64];
    strcopy(items[0], "SUVs", 64);
    strcopy(items[1], "Sports Cars", 64);
    strcopy(items[2], "Casual Cars", 64);

    // Build dialog body with categories (one per line)
    new body[128];
    format(body, sizeof(body), "SUVs\nSports Cars\nCasual Cars");
    ShowPlayerDialog(playerid, DIALOG_CARSHOP_CATEGORIES, DIALOG_STYLE_LIST, title, body, "Select", "Close");
    return 1;
}

// Callback when category selected
public CarShop_OnCategorySelected(playerid, selection)
{
    if(selection < 0) return 0;

    // Debug log
    printf("[DBG] CarShop_OnCategorySelected: pid=%d selection=%d\n", playerid, selection);

    // Map selection to category
    new cat = selection;
    g_CarShop_PlayerCategory[playerid] = cat;

    // Build a PawnPlus list with model previews
    new List:menu = list_new();
    new label[96];

    if(cat == CAR_CAT_SUV)
    {
        strcopy(label, "SUVs - Select Vehicle", sizeof(label));
        for(new i = 0; i < CarShop_VehicleCount[CAR_CAT_SUV]; i++)
        {
            new line[96];
            format(line, sizeof(line), "%s - $%d", CarShop_SUV_Names[i], CarShop_SUV_Prices[i]);
            AddModelMenuItem(menu, CarShop_SUV_Models[i], line);
        }
    }
    else if(cat == CAR_CAT_SPORTS)
    {
        strcopy(label, "Sports Cars - Select Vehicle", sizeof(label));
        for(new i = 0; i < CarShop_VehicleCount[CAR_CAT_SPORTS]; i++)
        {
            new line[96];
            format(line, sizeof(line), "%s - $%d", CarShop_SPORTS_Names[i], CarShop_SPORTS_Prices[i]);
            AddModelMenuItem(menu, CarShop_SPORTS_Models[i], line);
        }
    }
    else
    {
        strcopy(label, "Casual Cars - Select Vehicle", sizeof(label));
        for(new i = 0; i < CarShop_VehicleCount[CAR_CAT_CASUAL]; i++)
        {
            new line[96];
            format(line, sizeof(line), "%s - $%d", CarShop_CASUAL_Names[i], CarShop_CASUAL_Prices[i]);
            AddModelMenuItem(menu, CarShop_CASUAL_Models[i], line);
        }
    }

    ShowModelSelectionMenu(playerid, label, CARSHOP_MODEL_MENU, menu);
    return 1;
}

// Returns true and fills name/price/model for a given category/index
stock bool:CarShop_GetVehicleInfo(cat, idx, name[], namesize, &price, &modelid)
{
    if(cat == CAR_CAT_SUV)
    {
        if(idx < 0 || idx >= CarShop_VehicleCount[CAR_CAT_SUV]) return false;
        strcopy(name, CarShop_SUV_Names[idx], namesize);
        price = CarShop_SUV_Prices[idx];
        modelid = CarShop_SUV_Models[idx];
        return true;
    }
    else if(cat == CAR_CAT_SPORTS)
    {
        if(idx < 0 || idx >= CarShop_VehicleCount[CAR_CAT_SPORTS]) return false;
        strcopy(name, CarShop_SPORTS_Names[idx], namesize);
        price = CarShop_SPORTS_Prices[idx];
        modelid = CarShop_SPORTS_Models[idx];
        return true;
    }
    else if(cat == CAR_CAT_CASUAL)
    {
        if(idx < 0 || idx >= CarShop_VehicleCount[CAR_CAT_CASUAL]) return false;
        strcopy(name, CarShop_CASUAL_Names[idx], namesize);
        price = CarShop_CASUAL_Prices[idx];
        modelid = CarShop_CASUAL_Models[idx];
        return true;
    }
    return false;
}

// Find category and index for given model id. Returns true if found.
stock bool:CarShop_FindByModel(modelid, &outCat, &outIdx)
{
    for(new i = 0; i < CarShop_VehicleCount[CAR_CAT_SUV]; i++)
    {
        if(CarShop_SUV_Models[i] == modelid) { outCat = CAR_CAT_SUV; outIdx = i; return true; }
    }
    for(new i = 0; i < CarShop_VehicleCount[CAR_CAT_SPORTS]; i++)
    {
        if(CarShop_SPORTS_Models[i] == modelid) { outCat = CAR_CAT_SPORTS; outIdx = i; return true; }
    }
    for(new i = 0; i < CarShop_VehicleCount[CAR_CAT_CASUAL]; i++)
    {
        if(CarShop_CASUAL_Models[i] == modelid) { outCat = CAR_CAT_CASUAL; outIdx = i; return true; }
    }
    return false;
}

// Small set of parking spots to spawn purchased vehicles. (Prototype)
#define CARSHOP_PARKS_COUNT 3
new Float:CarShop_ParkX[CARSHOP_PARKS_COUNT] = { 1900.0, 1780.0, 1600.0 };
new Float:CarShop_ParkY[CARSHOP_PARKS_COUNT] = { -1400.0, 900.0, 1800.0 };
new Float:CarShop_ParkZ[CARSHOP_PARKS_COUNT] = { 10.0, 10.0, 10.0 };
new Float:CarShop_ParkA[CARSHOP_PARKS_COUNT] = { 0.0, 90.0, 180.0 };

stock bool:CarShop_PickParkingSpot(playerid, &x, &y, &z, &a)
{
    // Try predefined spots first
    for(new i = 0; i < CARSHOP_PARKS_COUNT; i++)
    {
        new Float:px = CarShop_ParkX[i];
        new Float:py = CarShop_ParkY[i];
        new Float:pz = CarShop_ParkZ[i];
        new Float:pa = CarShop_ParkA[i];

        // Check proximity to existing server vehicles (avoid overlaps)
        new ok = 1;
        foreach(new vs : Vehicles)
        {
            if(vInfo[vs][veh_created])
            {
                new Float:vx = vInfo[vs][veh_spawn_x];
                new Float:vy = vInfo[vs][veh_spawn_y];
                if(FloatAbs(vx - px) < 6.0 && FloatAbs(vy - py) < 6.0) { ok = 0; break; }
            }
        }

        if(ok)
        {
            x = px; y = py; z = pz; a = pa;
            return true;
        }
    }

    // Fallback: spawn a few units in front of player
    if(IsPlayerConnected(playerid))
    {
        new Float:px, py, pz;
        GetPlayerPos(playerid, px, py, pz);
        new Float:ang = GetPlayerFacingAngle(playerid);
        new Float:rad = ang * 3.14159265358979323846 / 180.0;
        x = px + floatround(floatcos(rad, false) * 3.0, floatround_floor);
        y = py + floatround(floatsin(rad, false) * 3.0, floatround_floor);
        z = pz;
        a = ang; // already in degrees
        return true;
    }

    return false;
}

public CarShop_OnVehicleSelected(playerid, selection)
{
    if(selection < 0) return 0;

    new cat = g_CarShop_PlayerCategory[playerid];
    g_CarShop_PlayerSelection[playerid] = selection;

    new name[64];
    new price = 0;

    if(cat == CAR_CAT_SUV)
    {
        strcopy(name, CarShop_SUV_Names[selection], sizeof(name));
        price = CarShop_SUV_Prices[selection];
    }
    else if(cat == CAR_CAT_SPORTS)
    {
        strcopy(name, CarShop_SPORTS_Names[selection], sizeof(name));
        price = CarShop_SPORTS_Prices[selection];
    }
    else
    {
        strcopy(name, CarShop_CASUAL_Names[selection], sizeof(name));
        price = CarShop_CASUAL_Prices[selection];
    }

    new msg[128];
    format(msg, sizeof(msg), "Do you want to buy %s for $%d?", name, price);

    new data[32];
    format(data, sizeof(data), "%d;%d", cat, selection);

    // Use the central confirm dialog with a callback so Dialog_HandleConfirm can process purchase
    HideModelSelectionMenu(playerid);
    ShowConfirmDialogExDelayed(playerid, "carshop_buy", "Confirm purchase", msg, data, 250);

    return 1;
}
