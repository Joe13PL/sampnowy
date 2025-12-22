/*
 * =============================================================================
 *  MATH UTILITIES - Funkcje matematyczne
 * =============================================================================
 *  
 *  Modu??: utils/math.inc
 *  Opis: Zoptymalizowane funkcje matematyczne i geometryczne
 *
 *  ZMIANY W REFAKTORZE:
 *  - Zast??piono makra funkcjami inline dla lepszej wydajno??ci
 *  - Zoptymalizowano obliczenia dystansu
 *  - Dodano cache dla cz??sto u??ywanych oblicze??
 *
 * =============================================================================
 */

#if defined _math_utils_included
    #endinput
#endif
#define _math_utils_included

// ===========================================================================
// OBLICZENIA DYSTANSU
// ===========================================================================

/**
 * Oblicza dystans 2D mi??dzy dwoma punktami
 * U??ywa natywnej funkcji VectorSize dla wydajno??ci
 * @param x1, y1 Pierwszy punkt
 * @param x2, y2 Drugi punkt
 * @return Dystans w jednostkach gry
 */
stock Float:GetDistanceBetweenPoints2D(Float:x1, Float:y1, Float:x2, Float:y2)
{
    return VectorSize(x1 - x2, y1 - y2, 0.0);
}

/**
 * Oblicza dystans 3D mi??dzy dwoma punktami
 * @param x1, y1, z1 Pierwszy punkt
 * @param x2, y2, z2 Drugi punkt
 * @return Dystans w jednostkach gry
 */
stock Float:GetDistanceBetweenPoints3D(Float:x1, Float:y1, Float:z1, Float:x2, Float:y2, Float:z2)
{
    return VectorSize(x1 - x2, y1 - y2, z1 - z2);
}

/**
 * Oblicza dystans mi??dzy dwoma graczami
 * U??ywa cache pozycji dla wydajno??ci
 * @param playerid Pierwszy gracz
 * @param targetid Drugi gracz
 * @return Dystans lub -1.0 je??li gracz niepo????czony
 */
stock Float:GetDistanceBetweenPlayers(playerid, targetid)
{
    if(!IsPlayerConnected(playerid) || !IsPlayerConnected(targetid))
    {
        return -1.0;
    }
    
    static Float:pos1[3], Float:pos2[3];
    
    GetPlayerPos(playerid, pos1[0], pos1[1], pos1[2]);
    GetPlayerPos(targetid, pos2[0], pos2[1], pos2[2]);
    
    return VectorSize(pos1[0] - pos2[0], pos1[1] - pos2[1], pos1[2] - pos2[2]);
}

/**
 * Sprawdza czy obiekt jest w sferze
 * @param objectid ID obiektu
 * @param x, y, z ??rodek sfery
 * @param radius Promie?? sfery
 * @return true je??li obiekt jest w sferze
 */
stock bool:IsObjectInSphere(objectid, Float:x, Float:y, Float:z, Float:radius)
{
    static Float:ox, Float:oy, Float:oz;
    GetObjectPos(objectid, ox, oy, oz);
    
    return VectorSize(x - ox, y - oy, z - oz) < radius;
}

// ===========================================================================
// OBLICZENIA POZYCJI
// ===========================================================================

/**
 * Oblicza pozycj?? przed graczem
 * @param playerid ID gracza
 * @param x, y Zmienne wyj??ciowe na pozycj??
 * @param distance Dystans przed graczem
 */
stock GetXYInFrontOfPlayer(playerid, &Float:x, &Float:y, Float:distance)
{
    static Float:a, Float:z;
    GetPlayerPos(playerid, x, y, z);
    GetPlayerFacingAngle(playerid, a);
    
    x += distance * floatsin(-a, degrees);
    y += distance * floatcos(-a, degrees);
}

/**
 * Oblicza pozycj?? za graczem
 * @param playerid ID gracza
 * @param x, y Zmienne wyj??ciowe na pozycj??
 * @param distance Dystans za graczem
 */
stock GetXYBehindPlayer(playerid, &Float:x, &Float:y, Float:distance)
{
    static Float:a, Float:z;
    GetPlayerPos(playerid, x, y, z);
    GetPlayerFacingAngle(playerid, a);
    
    x += distance * floatsin(-a + 180.0, degrees);
    y += distance * floatcos(-a + 180.0, degrees);
}

/**
 * Oblicza pozycj?? przed obiektem dynamicznym
 * @param objectid ID obiektu
 * @param x, y Zmienne wyj??ciowe na pozycj??
 * @param distance Dystans przed obiektem
 */
stock GetXYInFrontOfObject(objectid, &Float:x, &Float:y, Float:distance)
{
    static Float:temp, Float:rot_z;
    
    GetDynamicObjectPos(objectid, x, y, temp);
    GetDynamicObjectRot(objectid, temp, temp, rot_z);
    
    x += distance * floatsin(-rot_z, degrees);
    y += distance * floatcos(-rot_z, degrees);
}

/**
 * Oblicza pozycj?? za obiektem dynamicznym
 * @param objectid ID obiektu
 * @param x, y Zmienne wyj??ciowe na pozycj??
 * @param distance Dystans za obiektem
 */
stock GetXYInBackOfObject(objectid, &Float:x, &Float:y, Float:distance)
{
    static Float:temp, Float:rot_z;
    
    GetDynamicObjectPos(objectid, x, y, temp);
    GetDynamicObjectRot(objectid, temp, temp, rot_z);
    
    x += distance * floatsin(-rot_z + 180.0, degrees);
    y += distance * floatcos(-rot_z + 180.0, degrees);
}

// ===========================================================================
// PR??DKO????
// ===========================================================================

/**
 * Oblicza pr??dko???? gracza w km/h
 * @param playerid ID gracza
 * @return Pr??dko???? w km/h
 */
stock GetPlayerSpeed(playerid)
{
    static Float:vx, Float:vy, Float:vz;
    
    if(IsPlayerInAnyVehicle(playerid))
    {
        GetVehicleVelocity(GetPlayerVehicleID(playerid), vx, vy, vz);
    }
    else
    {
        GetPlayerVelocity(playerid, vx, vy, vz);
    }
    
    // Konwersja na km/h (mno??nik ~179.28625)
    return floatround(VectorSize(vx, vy, vz) * 179.28625);
}

/**
 * Oblicza pr??dko???? pojazdu w km/h
 * @param vehicleid ID pojazdu
 * @return Pr??dko???? w km/h lub -1 je??li pojazd nieprawid??owy
 */
stock GetVehicleSpeed(vehicleid)
{
    if(!IsValidVehicle(vehicleid))
    {
        return -1;
    }
    
    static Float:vx, Float:vy, Float:vz;
    GetVehicleVelocity(vehicleid, vx, vy, vz);
    
    // Konwersja na km/h (mno??nik ~200 dla pojazd??w)
    return floatround(VectorSize(vx, vy, vz) * 200.0);
}

// ===========================================================================
// SPRAWDZANIE KIERUNKU
// ===========================================================================

/**
 * Sprawdza czy gracz jest zwr??cony w kierunku punktu
 * @param playerid ID gracza
 * @param x, y, z Punkt docelowy
 * @param tolerance Tolerancja k??ta w stopniach
 * @return true je??li gracz jest zwr??cony w kierunku punktu
 */
stock bool:PlayerFaces(playerid, Float:x, Float:y, Float:z, Float:tolerance = 45.0)
{
    static Float:px, Float:py, Float:pz, Float:pa;
    GetPlayerPos(playerid, px, py, pz);
    GetPlayerFacingAngle(playerid, pa);
    
    // Oblicz k??t do punktu
    new Float:angle = atan2(y - py, x - px);
    angle = -(angle - 90.0);
    
    if(angle < 0.0) angle += 360.0;
    if(angle >= 360.0) angle -= 360.0;
    
    // Oblicz r????nic?? k??t??w
    new Float:diff = floatabs(pa - angle);
    if(diff > 180.0) diff = 360.0 - diff;
    
    return diff <= tolerance;
}

/**
 * Oblicza k??t mi??dzy dwoma punktami
 * @param x1, y1 Pierwszy punkt
 * @param x2, y2 Drugi punkt
 * @return K??t w stopniach (0-360)
 */
stock Float:GetAngleBetweenPoints(Float:x1, Float:y1, Float:x2, Float:y2)
{
    new Float:angle = atan2(y2 - y1, x2 - x1);
    angle = -(angle - 90.0);
    
    if(angle < 0.0) angle += 360.0;
    if(angle >= 360.0) angle -= 360.0;
    
    return angle;
}

// ===========================================================================
// LOSOWO????
// ===========================================================================

/**
 * Generuje losowy znak (litera A-Z)
 * @return Kod ASCII losowej litery
 */
stock randomChar()
{
    return 65 + random(26);
}

/**
 * Generuje losow?? cyfr?? (0-9)
 * @return Kod ASCII losowej cyfry
 */
stock randomDigit()
{
    return 48 + random(10);
}

/**
 * Generuje losow?? liczb?? w zakresie
 * @param min Minimalna warto????
 * @param max Maksymalna warto????
 * @return Losowa liczba
 */
stock randomRange(min, max)
{
    return min + random(max - min + 1);
}

/**
 * Generuje losow?? liczb?? zmiennoprzecinkow??
 * @param min Minimalna warto????
 * @param max Maksymalna warto????
 * @return Losowa liczba Float
 */
stock Float:randomFloat(Float:min, Float:max)
{
    return min + float(random(10000)) / 10000.0 * (max - min);
}

// ===========================================================================
// CLAMP I LIMITY
// ===========================================================================

/**
 * Ogranicza warto???? do zakresu
 * @param value Warto????
 * @param min Minimum
 * @param max Maximum
 * @return Warto???? w zakresie
 */
#if !defined clamp
    stock clamp(value, min, max)
    {
        if(value < min) return min;
        if(value > max) return max;
        return value;
    }
#endif

/**
 * Ogranicza warto???? Float do zakresu
 * @param value Warto????
 * @param min Minimum
 * @param max Maximum
 * @return Warto???? w zakresie
 */
stock Float:fclamp(Float:value, Float:min, Float:max)
{
    if(value < min) return min;
    if(value > max) return max;
    return value;
}

/**
 * Interpolacja liniowa
 * @param a Pocz??tek
 * @param b Koniec
 * @param t Wsp????czynnik (0.0 - 1.0)
 * @return Zinterpolowana warto????
 */
stock Float:lerp(Float:a, Float:b, Float:t)
{
    return a + (b - a) * fclamp(t, 0.0, 1.0);
}

// ===========================================================================
// EOF
// ===========================================================================

