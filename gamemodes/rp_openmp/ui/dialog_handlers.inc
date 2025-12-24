/*
 * =============================================================================
 *  DIALOG HANDLERS - Obs??uga odpowiedzi dialog??w
 * =============================================================================
 *
 *  Modu??: ui/dialog_handlers.inc
 *  Opis: Centralna obs??uga odpowiedzi dialog??w
 *
 *  ZMIANY W REFAKTORZE:
 *  - Wykorzystanie y_hooks dla OnDialogResponse
 *  - Modularna obs??uga dialog??w
 *  - Lepsze zarz??dzanie stanami
 *
 * =============================================================================
 */

#if defined _dialog_handlers_included
    #endinput
#endif
#define _dialog_handlers_included

// Localization defaults (fallback)


// ===========================================================================
// G????WNY HOOK DIALOG??W
// ===========================================================================

hook OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
    // Bezpiecze??stwo - usu?? niepo????dane znaki
    new text[256];
    if(inputtext[0] != EOS)
    {
        strcopy(text, inputtext, sizeof(text));
        SanitizeString(text);
    }

    switch(dialogid)
    {
        // =========================================================
        // DIALOGI AUTENTYKACJI
        // =========================================================

        case DIALOG_LOGIN:
        {
            return Dialog_HandleLogin(playerid, response, text);
        }

        // =========================================================
        // DIALOGI GRACZA
        // =========================================================

        case DIALOG_INVENTORY:
        {
            return Dialog_HandleInventory(playerid, response, listitem);
        }

        case DIALOG_ITEM_OPTIONS:
        {
            return Dialog_HandleItemOptions(playerid, response, listitem);
        }

        case DIALOG_ITEM_USE:
        {
            return Dialog_HandleItemUse(playerid, response, listitem);
        }

        case DIALOG_ITEM_GIVE:
        {
            // Debug to catch unexpected invocations during auth flow
            printf("[DIALOG HANDLERS DEBUG] ITEM_GIVE pid=%d resp=%d list=%d", playerid, response, listitem);
            return Dialog_HandleItemGive(playerid, response, text);
        }

        // Phone dialogs
        case DIALOG_PHONE_MENU:
        {
            return Dialog_HandlePhoneMenu(playerid, response, listitem);
        }
        case DIALOG_PHONE_DIAL:
        {
            return Dialog_HandlePhoneDial(playerid, response, text);
        }
        case DIALOG_PHONE_CONTACTS:
        {
            return Dialog_HandlePhoneContacts(playerid, response, listitem);
        }
        case DIALOG_PHONE_CONTACT_OPTIONS:
        {
            return Dialog_HandlePhoneContactOptions(playerid, response, listitem);
        }
        case DIALOG_PHONE_CONTACT_VIEW:
        {
            return Dialog_HandlePhoneContactView(playerid, response);
        }
        case DIALOG_PHONE_CONTACT_EDIT:
        {
            return Dialog_HandlePhoneContactEdit(playerid, response, text);
        }
        case DIALOG_PHONE_CONTACT_ADD:
        {
            return Dialog_HandlePhoneContactAdd(playerid, response, text);
        }
        case DIALOG_PHONE_SMS_MENU:
        {
            return Dialog_HandlePhoneSMSMenu(playerid, response, listitem);
        }
        case DIALOG_PHONE_SMS_LIST:
        {
            return Dialog_HandlePhoneSMSList(playerid, response, listitem);
        }
        case DIALOG_PHONE_SMS_VIEW:
        {
            return Dialog_HandlePhoneSMSView(playerid, response);
        }
        case DIALOG_PHONE_SMS_SEND:
        {
            return Dialog_HandlePhoneSMSSend(playerid, response, text);
        }
        case DIALOG_PHONE_HISTORY:
        {
            return Dialog_HandlePhoneHistory(playerid, response, listitem);
        }
        case DIALOG_PHONE_HISTORY_DETAIL:
        {
            return Dialog_HandlePhoneHistoryDetail(playerid, response);
        }
        case DIALOG_PHONE_ITEM_OPTIONS:
        {
            return Dialog_HandlePhoneItemOptions(playerid, response, listitem);
        }

        // =========================================================
        // DIALOGI GRUP
        // =========================================================

        case DIALOG_GROUPS:
        {
            return Dialog_HandleGroups(playerid, response, listitem);
        }

        case DIALOG_GROUP_OPTIONS:
        {
            return Dialog_HandleGroupOptions(playerid, response, listitem);
        }

        case DIALOG_GROUP_MEMBERS:
        {
            return Dialog_HandleGroupMembers(playerid, response, listitem);
        }

        // =========================================================
        // DIALOGI POJAZD??W
        // =========================================================

        case DIALOG_VEHICLES:
        {
            return Dialog_HandleVehicles(playerid, response, listitem);
        }

        case DIALOG_VEHICLE_OPTIONS:
        {
            return Dialog_HandleVehicleOptions(playerid, response, listitem);
        }

        case DIALOG_CARSHOP_CATEGORIES:
        {
            if(!response) return 1;
            return CarShop_OnCategorySelected(playerid, listitem);
        }

        case DIALOG_CARSHOP_VEHICLES:
        {
            if(!response) return 1;
            return CarShop_OnVehicleSelected(playerid, listitem);
        }

        // =========================================================
        // DIALOGI STATS MENU
        // =========================================================

        case DIALOG_STATS_MENU:
        {
            return Dialog_HandleStatsMenu(playerid, response, listitem);
        }

        case DIALOG_STATS:
        {
            if(response)
            {
                // Wstecz - pokaz menu
                Player_ShowStatsMenu(playerid);
            }
            return 1;
        }

        case DIALOG_PREMIUM_SHOP:
        {
            return Dialog_HandlePremiumShop(playerid, response, listitem);
        }

        case DIALOG_WALKING_STYLE:
        {
            return Dialog_HandleWalkingStyle(playerid, response, listitem);
        }

        case DIALOG_CREATE_GANG:
        {
            return Dialog_HandleCreateGang(playerid, response, listitem, inputtext);
        }

        case DIALOG_CREATE_GANG_CONFIRM:
        {
            return Dialog_HandleCreateGangConfirm(playerid, response);
        }

        // =========================================================
        // DIALOGI ADMINISTRACYJNE
        // =========================================================

        case DIALOG_AG_TYPES:
        {
            return Admin_HandleTypeDialog(playerid, response, listitem);
        }

        case DIALOG_AG_ABILITIES:
        {
            return Admin_HandleAbilitiesDialog(playerid, response, listitem);
        }

        case DIALOG_ADMINS_LIST:
        {
            return Admin_HandleAdminListDialog(playerid, response, listitem);
        }

        case DIALOG_ADMIN_INFO:
        {
            // Message box; nothing special to handle here
            return 1;
        }

        // =========================================================
        // DIALOGI PRACY
        // =========================================================

        case DIALOG_WORK:
        {
            return Dialog_HandleWork(playerid, response, listitem);
        }

        // =========================================================
        // DIALOGI INFORMACYJNE
        // =========================================================

        case DIALOG_INFO:
        {
            // Dialog informacyjny - tylko zamknij
            return 1;
        }

        case DIALOG_CONFIRM:
        {
            return Dialog_HandleConfirm(playerid, response);
        }
    }

    return 0;
}

// ===========================================================================
// HANDLERY AUTENTYKACJI
// ===========================================================================

Dialog_HandleLogin(playerid, response, const inputtext[])
{
    if(!response)
    {
        // Gracz anulowa?? logowanie
        Player_Kick(playerid, -1, "Anulowano logowanie");
        return 1;
    }

    if(strlen(inputtext) < 4)
    {
        SendClientMessage(playerid, COLOR_ERROR, "Hasło musi mieć co najmniej 6 znaków!");
        Auth_ShowRegisterDialog(playerid); // usuń zbędne argumenty
        return 1;
    }

    Auth_VerifyPassword(playerid, inputtext);

    return 1;
}

Dialog_HandleRegister(playerid, response, const inputtext[])
{
    if(!response)
    {
        Player_Kick(playerid, -1, "Anulowano rejestracj??");
        return 1;
    }

    if(strlen(inputtext) < 6)
    {
        SendClientMessage(playerid, COLOR_ERROR, "Has??o musi mie?? co najmniej 6 znak??w!");
        Auth_ShowRegisterDialog(playerid);
        return 1;
    }

    if(strlen(inputtext) > 32)
    {
        SendClientMessage(playerid, COLOR_ERROR, "Has??o mo??e mie?? maksymalnie 32 znaki!");
        Auth_ShowRegisterDialog(playerid);
        return 1;
    }

    // Zapisz has??o tymczasowo
    SetPVarString(playerid, "TempPassword", inputtext);

    // Popro?? o potwierdzenie
    ShowInputDialog(playerid, DIALOG_REGISTER_CONFIRM,
        "Potwierdzenie has??a",
        "Wpisz has??o ponownie w celu potwierdzenia:", true);

    return 1;
}

Dialog_HandleRegisterConfirm(playerid, response, const inputtext[])
{
    if(!response)
    {
        DeletePVar(playerid, "TempPassword");
        Auth_ShowRegisterDialog(playerid);
        return 1;
    }

    new savedPassword[64];
    GetPVarString(playerid, "TempPassword", savedPassword, sizeof(savedPassword));

    if(strcmp(inputtext, savedPassword) != 0)
    {
        SendClientMessage(playerid, COLOR_ERROR, "Has??a nie s?? identyczne!");
        DeletePVar(playerid, "TempPassword");
        Auth_ShowRegisterDialog(playerid);

        return 1;
    }

    // Utw??rz konto
    Auth_CreateAccount(playerid, savedPassword);
    DeletePVar(playerid, "TempPassword");

    return 1;
}

// ===========================================================================
// HANDLERY EKWIPUNKU
// ===========================================================================

Dialog_HandleInventory(playerid, response, listitem)
{
    if(!response)
    {
        return 1;
    }

    // Znajd?? wybrany przedmiot
    new slot = -1;
    new count = 0;

    for(new i = 0; i < MAX_PLAYER_ITEMS; i++)
    {
        if(pItem[playerid][i][item_id] > 0)
        {
            if(count == listitem)
            {
                slot = i;
                break;
            }
            count++;
        }
    }

    if(slot == -1)
    {
        SendClientMessage(playerid, COLOR_ERROR, "Nieprawid??owy przedmiot.");
        return 1;
    }

    Dialog_ShowItemOptions(playerid, slot);

    return 1;
}

Dialog_HandleItemOptions(playerid, response, listitem)
{
    if(!response)
    {
        Dialog_ShowInventory(playerid);
        return 1;
    }

    new data[16];
    Dialog_GetData(playerid, data);
    new slot = strval(data);

    switch(listitem)
    {
        case 0: // U??yj
        {
            Item_Use(playerid, slot);
        }

        case 1: // Oddaj graczowi
        {
            ShowInputDialog(playerid, DIALOG_ITEM_GIVE,
                "Oddaj przedmiot",
                "Wpisz ID gracza, kt??remu chcesz odda?? przedmiot:");
            Dialog_SetData(playerid, data);
        }

        case 2: // Pod??????
        {
            Item_Drop(playerid, slot);
        }

        case 3: // Wyrzu??
        {
            Item_Destroy(playerid, slot);
            SendClientMessage(playerid, COLOR_INFO, "Wyrzuci??e?? przedmiot.");
        }
    }

    return 1;
}

Dialog_HandleItemUse(playerid, response, listitem)
{
    // Obsługa specjalnych opcji użycia przedmiotu
    return 1;
}

Dialog_HandlePhoneDial(playerid, response, const inputtext[])
{
    if(!response)
    {
        return 1; // cancelled
    }

    if(isnull(inputtext))
    {
        printf("[DBG] Dialog_HandlePhoneDial: isnull input for player=%d\n", playerid);
        SendClientMessage(playerid, COLOR_GRAY, "Użycie: wpisz ID gracza");
        return 1;
    }

    new num;
    if(!Phone_ParseNumberString(inputtext, num))
    {
        printf("[DBG] Dialog_HandlePhoneDial: parse failed for player=%d input='%s' len=%d\n", playerid, inputtext, strlen(inputtext));
        SendClientMessage(playerid, COLOR_GRAY, "Użycie: wpisz numer telefonu (np. 12345 lub 012345)");
        return 1;
    }

    // Accept 1-6 digit numbers (allow leading-zero numbers displayed as 6 digits)
        if(num < PHONE_MIN_NUMBER || num > PHONE_MAX_NUMBER)
    {
        SendClientMessage(playerid, COLOR_RED, "Nieprawidlowy numer telefonu.");
        return 1;
    }

    // Start call by number (handles online/offline semantics)
    Phone_StartCallByNumber(playerid, num);
    return 1;
}

Dialog_HandlePhoneMenu(playerid, response, listitem)
{
    if(!response) return 1; // closed

    switch(listitem)
    {
        case 0: { ShowPlayerDialog(playerid, DIALOG_PHONE_DIAL, DIALOG_STYLE_INPUT, "Phone", "Enter a phone number to call:", "Call", "Cancel"); return 1; }
        case 1: { Dialog_ShowPhoneContacts(playerid); return 1; }
        case 2: { Dialog_ShowPhoneSMS(playerid); return 1; }
        case 3: { Dialog_ShowPhoneHistory(playerid); return 1; }
        case 4: { Dialog_ShowPhoneSettings(playerid); return 1; }
        default: { return 1; }
    }
}

Dialog_HandlePhoneContacts(playerid, response, listitem)
{
    if(!response)
    {
        // Right button = Dodaj kontakt (expect phone number now)
        ShowInputDialog(playerid, DIALOG_PHONE_CONTACT_ADD, "Add contact", "Enter the phone number you want to add as a contact:", false);
        return 1;
    }

    // If the user selected the first row (Add new contact), start the add flow
    if(listitem == 0)
    {
        ShowInputDialog(playerid, DIALOG_PHONE_CONTACT_ADD, "Add contact", "Enter the phone number you want to add as a contact:", false);
        return 1;
    }

    if(listitem < 0 || listitem >= g_PhoneContactsCount[playerid]) return 1;

    new contactDbId = g_PhoneContactsList[playerid][listitem];
    new contactNumber = g_PhoneContactsUID[playerid][listitem];

    // Store contact id in dialog data for options
    new data[32]; format(data, sizeof(data), "%d %d", contactDbId, contactNumber);
    Dialog_SetData(playerid, data);

    new options[128] = "Call\nView\nEdit name\nDelete";
    ShowListDialog(playerid, DIALOG_PHONE_CONTACT_OPTIONS, "Opcje kontaktu", options);
    return 1;
}

Dialog_HandlePhoneHistory(playerid, response, listitem)
{
    if(!response) return 1;
    if(listitem < 0 || listitem >= g_PhoneHistoryCount[playerid]) return 1;

    new callId = g_PhoneHistoryList[playerid][listitem];
    new query[256];
    format(query, sizeof(query), "SELECT id, caller_name, callee_name, start_time, answer_time, end_time, duration, result, caller_number, callee_number FROM `phone_calls` WHERE id = %d", callId);
    mysql_tquery(mySQLconnection, query, "Phone_OnHistoryDetailLoaded", "i", playerid);
    return 1;
}

Dialog_HandlePhoneHistoryDetail(playerid, response)
{
    if(!response) return 1;
    new data[64]; Dialog_GetData(playerid, data);
    new other_number;
    if(sscanf(data, "%d", other_number) != 1) return 1;

    // Find connected player by phone number
    foreach(new i : Player)
    {
        new pslot = Item_FindByType(i, ITEM_TYPE_PHONE);
        if(pslot >= 0 && pItem[i][pslot][item_value2] == other_number && pInfo[i][player_logged])
        {
            Phone_Dial(playerid, other_number);
            return 1;
        }
    }

    SendClientMessage(playerid, COLOR_ERROR, "Gracz nie jest online.");
    return 1;
}

Dialog_HandlePhoneContactOptions(playerid, response, listitem)
{
    if(!response) return 1;
    new data[64]; Dialog_GetData(playerid, data);
    new contactDbId, contactNumber;
    if(sscanf(data, "%d %d", contactDbId, contactNumber) != 2) return 1;

    switch(listitem)
    {
        case 0: // Zadzwoń
        {
            // Dial by number regardless of online status (Phone_Dial supports numeric targets)
            Phone_Dial(playerid, contactNumber);
            return 1;
        }
        case 1: // Pokaż
        {
            new query[128]; format(query, sizeof(query), "SELECT id, contact_number, contact_name, created_at FROM `phone_contacts` WHERE id = %d", contactDbId);
            mysql_tquery(mySQLconnection, query, "Phone_OnContactDetailLoaded", "i", playerid);
            return 1;
        }
        case 2: // Edytuj nazwę
        {
            new buf[32]; format(buf, sizeof(buf), "%d", contactDbId);
            Dialog_SetData(playerid, buf);
            ShowInputDialog(playerid, DIALOG_PHONE_CONTACT_EDIT, "Edit contact", "Enter a new display name for the contact:", false);
            return 1;
        }
        case 3: // Usun
        {
            new buf[64]; format(buf, sizeof(buf), "%d", contactDbId);
            ShowConfirmDialogEx(playerid, "phone_contact_delete", "Confirm", "Are you sure you want to delete this contact?", buf);
            return 1;
        }
    }

    return 1;
}

Dialog_HandlePhoneContactView(playerid, response)
{
    if(!response) return 1;
    new data[64]; Dialog_GetData(playerid, data);
    new contactDbId, contactNumber;
    if(sscanf(data, "%d %d", contactDbId, contactNumber) != 2) return 1;

    if (g_PhoneDBHasContactCols) {
        // Left button = Zadzwoń by number
        if(contactNumber == 0) { SendClientMessage(playerid, COLOR_ERROR, "Contact has no assigned number."); return 1; }
        Phone_Dial(playerid, contactNumber);
        return 1;
    } else {
        // Legacy: contactNumber is actually contact UID - find online player by UID and dial their device
        new contactUid = contactNumber;
        new targetPid = -1;
        foreach(new i : Player)
        {
            if(pInfo[i][player_uid] == contactUid && pInfo[i][player_logged])
            {
                new pslot = Item_FindByType(i, ITEM_TYPE_PHONE);
                if(pslot >= 0) { targetPid = i; break; }
            }
        }

        if(targetPid == -1) { SendClientMessage(playerid, COLOR_ERROR, "Player is not available."); return 1; }
        Phone_Dial(playerid, targetPid);
        return 1;
    }
}

Dialog_HandlePhoneContactEdit(playerid, response, const inputtext[])
{
    if(!response) { Dialog_ShowPhoneContacts(playerid); return 1; }
    if(isnull(inputtext) || strlen(inputtext) == 0) { SendClientMessage(playerid, COLOR_ERROR, "Name cannot be empty."); return 1; }

    new data[64]; Dialog_GetData(playerid, data);
    printf("[DBG] Dialog_HandlePhoneContactEdit data='%s' input='%s'\n", data, inputtext);

    new contactDbId, targetNumber;

    // Robust parse: allow extra spaces and tolerate malformed spacing
    new parsed = 0;
    // Try ADD
    new idx = 0; while(data[idx] == ' ') idx++;
    if((data[idx] == 'A' || data[idx] == 'a') && ((data[idx+1] == 'D' || data[idx+1] == 'd') && (data[idx+2] == 'D' || data[idx+2] == 'd')))
    {
        idx += 3; while(data[idx] == ' ') idx++;
        new num = 0; new any = 0; new j = idx;
        while(data[j] != '\0') { if(data[j] >= '0' && data[j] <= '9') { any = 1; num = num*10 + (data[j]-'0'); } else break; j++; }
        if(any) { targetNumber = num; parsed = 1; }
    }

    if(parsed)
    {
        // Add new contact for current phone
        new slot = pInfo[playerid][player_dialog_tmp1];
        if(slot < 0 || pItem[playerid][slot][item_type] != ITEM_TYPE_PHONE)
        {
            slot = Item_FindByType(playerid, ITEM_TYPE_PHONE);
            if(slot < 0) { SendClientMessage(playerid, COLOR_ERROR, "You do not have a phone with you."); return 1; }
            pInfo[playerid][player_dialog_tmp1] = slot;
        }

        new phone_number = pItem[playerid][slot][item_value2];
        if(phone_number == 0) { SendClientMessage(playerid, COLOR_ERROR, "Ten telefon nie ma przypisanego numeru."); return 1; }

        Phone_AddContactForNumber(playerid, targetNumber, inputtext);
        Dialog_ShowPhoneContacts(playerid);
        return 1;
    }

    // Try EDIT
    idx = 0; while(data[idx] == ' ') idx++;
    if((data[idx] == 'E' || data[idx] == 'e') && ((data[idx+1] == 'D' || data[idx+1] == 'd') && (data[idx+2] == 'I' || data[idx+2] == 'i') && (data[idx+3] == 'T' || data[idx+3] == 't')))
    {
        idx += 4; while(data[idx] == ' ') idx++;
        new num2 = 0; new any2 = 0; new j2 = idx;
        while(data[j2] != '\0') { if(data[j2] >= '0' && data[j2] <= '9') { any2 = 1; num2 = num2*10 + (data[j2]-'0'); } else break; j2++; }
        if(any2) { contactDbId = num2; parsed = 1; }
    }

    if(parsed)
    {
        new esc[128]; MySQL_EscapeString(inputtext, esc, sizeof(esc));
        MySQL_ExecuteFormat("UPDATE `phone_contacts` SET `contact_name` = '%s' WHERE id = %d", esc, contactDbId);
        SendClientMessage(playerid, COLOR_SUCCESS, "Contact name updated.");
        Dialog_ShowPhoneContacts(playerid);
        return 1;
    }

    printf("[DBG] Dialog_HandlePhoneContactEdit: failed to parse data='%s'\n", data);
    SendClientMessage(playerid, COLOR_ERROR, "Blad wewnetrzny.");
    Dialog_ShowPhoneContacts(playerid);
    return 1;
}

Dialog_HandlePhoneContactAdd(playerid, response, const inputtext[])
{
    if(!response) { Dialog_ShowPhoneContacts(playerid); return 1; }
    // First step: expect a phone number
    if(isnull(inputtext) || strlen(inputtext) == 0) { SendClientMessage(playerid, COLOR_ERROR, "Enter a phone number."); return 1; }

    // validate numeric phone number (digits only)
    new isNum = 1; new len = strlen(inputtext);
    for(new i = 0; i < len; i++) if(inputtext[i] < '0' || inputtext[i] > '9') { isNum = 0; break; }
    if(!isNum) { SendClientMessage(playerid, COLOR_ERROR, "Enter a valid phone number."); return 1; }

    // parse number
    new parsedNum = 0; for(new j = 0; j < len; j++) parsedNum = parsedNum * 10 + (inputtext[j] - '0');
    if(parsedNum < PHONE_MIN_NUMBER || parsedNum > PHONE_MAX_NUMBER) { SendClientMessage(playerid, COLOR_ERROR, "Nieprawidlowy numer telefonu."); return 1; }

    // store target number with ADD prefix and ask for display name
    new buf[32]; format(buf, sizeof(buf), "ADD %d", parsedNum);
    Dialog_SetData(playerid, buf);
    ShowInputDialog(playerid, DIALOG_PHONE_CONTACT_EDIT, "Add contact - name", "Enter a display name for the contact:", false);
    return 1;
}

Dialog_HandlePhoneItemOptions(playerid, response, listitem)
{
    if(!response) return 1;

    // slot stored in pInfo[playerid][player_dialog_tmp1]
    new slot = pInfo[playerid][player_dialog_tmp1];
    if(slot < 0 || slot >= MAX_PLAYER_ITEMS || pItem[playerid][slot][item_type] != ITEM_TYPE_PHONE) { SendClientMessage(playerid, COLOR_ERROR, "Invalid phone."); return 1; }

    switch(listitem)
    {
        case 0: // Otwórz menu
        {
            Dialog_ShowPhoneMenu(playerid);
            return 1;
        }
        case 1: // Wlacz/Wylacz telefon
        {
            new enabled = pItem[playerid][slot][item_value];
            if(enabled == 0) { pItem[playerid][slot][item_value] = 1; SendClientMessage(playerid, COLOR_SUCCESS, "Phone turned on."); }
            else { pItem[playerid][slot][item_value] = 0; SendClientMessage(playerid, COLOR_WARNING, "Phone turned off."); }
            // Persist change
            Item_SaveToDatabase(playerid, slot);
            return 1;
        }
    }

    return 1;
}

Dialog_HandlePhoneSMSMenu(playerid, response, listitem)
{
    if(!response) return 1;
    switch(listitem)
    {
        case 0: { Dialog_ShowPhoneSMS(playerid); return 1; }
        case 1: { Dialog_SetData(playerid, ""); ShowInputDialog(playerid, DIALOG_PHONE_SMS_SEND, "Nowy SMS - odbiorca (numer)", "Wpisz numer odbiorcy:", false); return 1; }
        default: return 1;
    }
}

Dialog_HandlePhoneSMSList(playerid, response, listitem)
{
    if(!response) return 1;
    if(listitem < 0 || listitem >= g_PhoneSMSCount[playerid]) return 1;

    new smsId = g_PhoneSMSList[playerid][listitem];
    new data[32]; format(data, sizeof(data), "%d", smsId);
    Dialog_SetData(playerid, data);

    new slot = pInfo[playerid][player_dialog_tmp1];
    if(slot < 0 || pItem[playerid][slot][item_type] != ITEM_TYPE_PHONE) slot = Item_FindByType(playerid, ITEM_TYPE_PHONE);
    new myphone = 0; if(slot >= 0) myphone = pItem[playerid][slot][item_value2];
    new query[1024];
    new msgCol[16]; if(g_PhoneSMSUsesBody) strcopy(msgCol, "s.body", sizeof(msgCol)); else strcopy(msgCol, "s.message", sizeof(msgCol));
    format(query, sizeof(query), "SELECT s.id, s.sender_number, s.receiver_number, %s AS message_col, s.timestamp, s.is_read, COALESCE(pc_sender.contact_name, s.sender_number) AS sender_display, COALESCE(pc_receiver.contact_name, s.receiver_number) AS receiver_display, s.sender_number AS sender_phone, s.receiver_number AS receiver_phone FROM `phone_sms` s LEFT JOIN `phone_contacts` pc_sender ON pc_sender.phone_number = %d AND pc_sender.contact_number = s.sender_number LEFT JOIN `phone_contacts` pc_receiver ON pc_receiver.phone_number = %d AND pc_receiver.contact_number = s.receiver_number WHERE s.id = %d", msgCol, myphone, myphone, smsId);
    if(strlen(query) >= sizeof(query) - 1) printf("[WARN] Dialog_HandlePhoneSMSList: query truncated (len=%d, cap=%d)\n", strlen(query), sizeof(query));
    printf("[DBG] Dialog_HandlePhoneSMSList query: %s\n", query);
    mysql_tquery(mySQLconnection, query, "Phone_OnSMSViewLoaded", "i", playerid);
    return 1;
}

Dialog_HandlePhoneSMSView(playerid, response)
{
    // Debug: print current Dialog data to help diagnose non-working buttons
    new curdata[64]; Dialog_GetData(playerid, curdata);
    printf("[DBG] Dialog_HandlePhoneSMSView invoked response=%d data='%s'\n", response, curdata);

    if(!response)
    {
        // Confirm delete
        new data[64]; Dialog_GetData(playerid, data);
        new smsId, sender_number;
        if(sscanf(data, "%d %d", smsId, sender_number) != 2)
        {
            // fallback manual parse (handles weird spacing/encoding)
            new token1[32]; new token2[32]; token1[0] = '\0'; token2[0] = '\0';
            new p = 0; new ti = 0;
            while(data[p] == ' ') p++;
            while(data[p] != '\0' && data[p] != ' ' && ti < sizeof(token1)-1) token1[ti++] = data[p++]; token1[ti] = '\0';
            while(data[p] == ' ') p++;
            ti = 0;
            while(data[p] != '\0' && data[p] != ' ' && ti < sizeof(token2)-1) token2[ti++] = data[p++]; token2[ti] = '\0';
            if(token1[0] != '\0') sscanf(token1, "%d", smsId);
            if(token2[0] != '\0') sscanf(token2, "%d", sender_number);
            if(smsId == 0 || sender_number == 0) { printf("[DBG] Dialog_HandlePhoneSMSView: failed to parse data='%s' for delete\n", data); return 1; }
        }
        new buf[32]; format(buf, sizeof(buf), "%d", smsId);
        ShowConfirmDialogEx(playerid, "phone_sms_delete", "Potwierdz", "Czy na pewno chcesz usunac wiadomosc?", buf);
        return 1;
    }

    // Reply
    new data[64]; Dialog_GetData(playerid, data);
    new smsId = 0; new sender_number = 0;
    // Try robust manual parse (fallback if sscanf fails due to spacing/encoding)
    if(sscanf(data, "%d %d", smsId, sender_number) != 2)
    {
        new token1[32]; new token2[32]; token1[0] = '\0'; token2[0] = '\0';
        new p = 0; new ti = 0;
        // skip leading spaces
        while(data[p] == ' ') p++;
        while(data[p] != '\0' && data[p] != ' ' && ti < sizeof(token1)-1) token1[ti++] = data[p++]; token1[ti] = '\0';
        while(data[p] == ' ') p++;
        ti = 0;
        while(data[p] != '\0' && data[p] != ' ' && ti < sizeof(token2)-1) token2[ti++] = data[p++]; token2[ti] = '\0';
        if(token1[0] != '\0') sscanf(token1, "%d", smsId);
        if(token2[0] != '\0') sscanf(token2, "%d", sender_number);
        printf("[DBG] Dialog_HandlePhoneSMSView manual-parse token1='%s' token2='%s' -> smsId=%d sender_number=%d\n", token1, token2, smsId, sender_number);
        if(smsId == 0 || sender_number == 0) { printf("[DBG] Dialog_HandlePhoneSMSView: failed to parse data='%s' after manual-parse\n", data); return 1; }
    }

    new onlinePid = -1;
    foreach(new i : Player)
    {
        new pslot = Item_FindByType(i, ITEM_TYPE_PHONE);
        if(pslot >= 0 && pItem[i][pslot][item_value2] == sender_number && pInfo[i][player_logged]) { onlinePid = i; break; }
    }

    // store receiver number for send handler
    new buf[32]; format(buf, sizeof(buf), "%d", sender_number);
    Dialog_SetData(playerid, buf);

    ShowInputDialog(playerid, DIALOG_PHONE_SMS_SEND, "Reply", "Enter SMS content:", false);
    return 1;
}

Dialog_HandlePhoneSMSSend(playerid, response, const inputtext[])
{
    if(!response) { Dialog_ShowPhoneSMS(playerid); return 1; }

    new data[64]; Dialog_GetData(playerid, data);

    // Compose step 1: if data does not contain a receiver marker, expect the user to input a phone number
    new tmpNum;
    if(sscanf(data, "%d", tmpNum) != 1)
    {
        // validate numeric phone number (digits only)
        new isNum = 1; new len = strlen(inputtext);
        for(new i = 0; i < len; i++) if(inputtext[i] < '0' || inputtext[i] > '9') { isNum = 0; break; }
        if(!isNum) { SendClientMessage(playerid, COLOR_ERROR, "Wpisz poprawny numer odbiorcy."); return 1; }
        // parse number
        new parsedNum = 0; for(new j = 0; j < len; j++) parsedNum = parsedNum * 10 + (inputtext[j] - '0');
        if(parsedNum < PHONE_MIN_NUMBER || parsedNum > PHONE_MAX_NUMBER) { SendClientMessage(playerid, COLOR_ERROR, "Nieprawidlowy numer telefonu."); return 1; }
        // store and ask for message body
        new buf[32]; format(buf, sizeof(buf), "%d", parsedNum);
        Dialog_SetData(playerid, buf);
        ShowInputDialog(playerid, DIALOG_PHONE_SMS_SEND, "New SMS", "Enter SMS content:", false);
        return 1;
    }

    // Compose step 2: sending message body
    if(isnull(inputtext) || strlen(inputtext) == 0) { SendClientMessage(playerid, COLOR_ERROR, "Wiadomosc nie moze byc pusta."); return 1; }

    new receiver_number;
    if(sscanf(data, "%d", receiver_number) != 1) { SendClientMessage(playerid, COLOR_ERROR, "Nieprawidlowy odbiorca."); return 1; }

    // Send SMS by number
    Phone_SendSMSByNumber(playerid, receiver_number, inputtext);

    Dialog_ShowPhoneSMS(playerid);
    return 1;
}

Dialog_HandleItemGive(playerid, response, const inputtext[])
{
    if(!response)
    {
        Dialog_ShowInventory(playerid);
        return 1;
    }

    new data[16];
    Dialog_GetData(playerid, data);
    new slot = strval(data);

    new targetid = strval(inputtext);

    if(!IsValidPlayerId(targetid) || !pInfo[targetid][player_logged])
    {
        SendClientMessage(playerid, COLOR_ERROR, "Nieprawid??owy gracz.");
        Dialog_ShowInventory(playerid);
        return 1;
    }

    // Utw??rz ofert?? przedmiotu
    Offer_Create(playerid, targetid, OFFER_ITEM, 0, slot, 0);

    return 1;
}

// ===========================================================================
// HANDLERY GRUP
// ===========================================================================

Dialog_HandleGroups(playerid, response, listitem)
{
    if(!response)
    {
        return 1;
    }

    // Znajd?? wybran?? grup??
    new gid = -1;
    new count = 0;

    for(new i = 0; i < MAX_PLAYER_GROUPS; i++)
    {
        new groupId = pInfo[playerid][player_group][i];

        if(groupId > 0 && gInfo[groupId][group_created])
        {
            if(count == listitem)
            {
                gid = groupId;
                break;
            }
            count++;
        }
    }

    if(gid == -1)
    {
        return 1;
    }

    // Zapisz ID grupy
    new data[16];
    format(data, sizeof(data), "%d", gid);
    Dialog_SetData(playerid, data);

    // Poka?? opcje grupy
    new content[256];
    format(content, sizeof(content),
        "Czat grupy\nLista cz??onk??w\nMagazyn grupy\nOpu???? grup??");

    ShowListDialog(playerid, DIALOG_GROUP_OPTIONS,
        gInfo[gid][group_name], content);

    return 1;
}

Dialog_HandleGroupOptions(playerid, response, listitem)
{
    if(!response)
    {
        Dialog_ShowPlayerGroups(playerid);
        return 1;
    }

    new data[16];
    Dialog_GetData(playerid, data);
    new gid = strval(data);

    switch(listitem)
    {
        case 0: // Czat grupy
        {
            // Otw??rz input czatu
            ShowInputDialog(playerid, DIALOG_GROUP_CHAT,
                "Czat grupy",
                "Wpisz wiadomo???? do cz??onk??w grupy:");
            Dialog_SetData(playerid, data);
        }

        case 1: // Lista cz??onk??w
        {
            Dialog_ShowGroupMembers(playerid, gid);
        }

        case 2: // Magazyn
        {
            // Poka?? magazyn grupy
            SendClientMessage(playerid, COLOR_INFO, "System magazynu w budowie.");
        }

        case 3: // Opu???? grup??
        {
            Group_RemoveMember(gid, playerid);
            SendClientMessage(playerid, COLOR_INFO, "Opu??ci??e?? grup??.");
        }
    }

    return 1;
}

Dialog_HandleGroupMembers(playerid, response, listitem)
{
    if(!response)
    {
        Dialog_ShowPlayerGroups(playerid);
    }

    return 1;
}

Dialog_ShowGroupMembers(playerid, groupId)
{
    new content[2048];
    new count = 0;

    strcopy(content, "Gracz\tRanga\tOnline\n", sizeof(content));

    foreach(new i : Player)
    {
        if(Player_IsInGroup(i, groupId))
        {
            new rankName[32];
            Player_GetGroupRankName(i, groupId, rankName);

            new line[128];
            format(line, sizeof(line), "%s\t%s\t%s\n",
                Player_GetRPName(i),
                rankName,
                "Online");

            strcat(content, line);
            count++;
        }
    }

    if(count == 0)
    {
        strcopy(content, "Brak cz??onk??w online.", sizeof(content));
    }

    new title[64];
    format(title, sizeof(title), "Cz??onkowie: %s", gInfo[groupId][group_name]);

    ShowPlayerDialog(playerid, DIALOG_GROUP_MEMBERS, DIALOG_STYLE_TABLIST_HEADERS,
        title, content, "OK", "Wr????");

    return 1;
}

// ===========================================================================
// HANDLERY POJAZD??W
// ===========================================================================

Dialog_HandleVehicles(playerid, response, listitem)
{
    if(!response)
    {
        return 1;
    }

    new vehicles[MAX_VEHICLES];
    new vehicleCount = Vehicle_GetPlayerVehicles(playerid, vehicles);

    if(listitem >= vehicleCount)
    {
        return 1;
    }

    new slot = vehicles[listitem];

    // Zapisz slot
    new data[16];
    format(data, sizeof(data), "%d", slot);
    Dialog_SetData(playerid, data);

    // Poka?? opcje
    new content[256] = "Lokalizuj pojazd\nPrzywo??aj pojazd\nSprzedaj pojazd";

    new title[64];
    new modelName[32];
    GetVehicleModelName(vInfo[slot][veh_model], modelName);
    format(title, sizeof(title), "Opcje: %s", modelName);

    ShowListDialog(playerid, DIALOG_VEHICLE_OPTIONS, title, content);

    return 1;
}

Dialog_HandleVehicleOptions(playerid, response, listitem)
{
    if(!response)
    {
        Dialog_ShowPlayerVehicles(playerid);
        return 1;
    }

    new data[16];
    Dialog_GetData(playerid, data);
    new slot = strval(data);

    switch(listitem)
    {
        case 0: // Lokalizuj
        {
            new Float:x, Float:y, Float:z;
            GetVehiclePos(vInfo[slot][veh_gameid], x, y, z);
            SetPlayerCheckpoint(playerid, x, y, z, 3.0);
            SendClientMessage(playerid, COLOR_INFO, "Pojazd zaznaczony na mapie.");
        }

        case 1: // Przywo??aj
        {
            new Float:x, Float:y, Float:z;
            GetPlayerPos(playerid, x, y, z);
            SetVehiclePos(vInfo[slot][veh_gameid], x + 3.0, y, z);
            SendClientMessage(playerid, COLOR_SUCCESS, "Pojazd przywo??any.");
        }

        case 2: // Sprzedaj
        {
            SendClientMessage(playerid, COLOR_INFO, "Aby sprzeda?? pojazd, znajd?? kupca i u??yj /sprzedajpojazd.");
        }
    }

    return 1;
}

// ===========================================================================
// HANDLERY PRACY
// ===========================================================================

Dialog_HandleWork(playerid, response, listitem)
{
    if(!response)
    {
        return 1;
    }

    // Mapowanie listitem na typ pracy
    new E_WORK_TYPE:workType;

    switch(listitem)
    {
        case 0: workType = WORK_TAXI;
        case 1: workType = WORK_TRUCKER;
        case 2: workType = WORK_BUS;
        case 3: workType = WORK_PIZZA;
        case 4: workType = WORK_GARBAGE;
        case 5: workType = WORK_FARMER;
        case 6: workType = WORK_MINER;
        case 7: workType = WORK_LUMBERJACK;
        case 8: workType = WORK_FISHERMAN;
        default: workType = WORK_NONE;
    }

    if(workType != WORK_NONE)
    {
        Work_Start(playerid, workType);
    }

    return 1;
}

// ===========================================================================
// HANDLERY STATS MENU
// ===========================================================================

enum e_WalkStyle {
    walk_name[32],
    walk_lib[16],
    walk_anim[24]
}

// Style chodzenia
static WalkingStyles[][e_WalkStyle] = {
    {"Normalny", "PED", "WALK_player"},
    {"Gangster", "PED", "WALK_gang1"},
    {"Gangster 2", "PED", "WALK_gang2"},
    {"Sexy", "PED", "WOMAN_walkshopm"},
    {"Pijany", "PED", "WALK_drunk"},
    {"Stary", "PED", "WALK_old"},
    {"Gruby", "PED", "WALK_fat"},
    {"Blindman", "PED", "WALK_civi"},
    {"Szybki", "PED", "sprint_panic"}
};

stock Player_ShowStatsMenu(playerid)
{
    // Sprawdz warunki do zalozenia grupy przestepczej
    new bool:canCreateGang = Player_CanCreateGang(playerid);
    new gangStatus[64];

    if(canCreateGang)
    {
        gangStatus = "{00FF00}[DOSTEPNE]";
    }
    else
    {
        gangStatus = "{FF6600}[WYMAGANIA NIE SPELNIONE]";
    }

    new content[512];
    content[0] = EOS;

    strcat(content, "{FFFFFF}Statystyki postaci\n", sizeof(content));
    strcat(content, "{FFFFFF}Sklep Premium\n", sizeof(content));
    strcat(content, "{FFFFFF}Styl chodzenia (Alt)\n", sizeof(content));
    format(content, sizeof(content), "%s{FFFFFF}Zaloz grupe przestepcza %s", content, gangStatus);

    ShowPlayerDialog(playerid, DIALOG_STATS_MENU, DIALOG_STYLE_LIST,
        "Menu statystyk", content, "Wybierz", "Zamknij");

    return 1;
}

Dialog_HandleStatsMenu(playerid, response, listitem)
{
    if(!response)
    {
        return 1;
    }

    switch(listitem)
    {
        case 0: // Statystyki postaci
        {
            Player_ShowStats(playerid);
        }

        case 1: // Sklep Premium
        {
            Player_ShowPremiumShop(playerid);
        }

        case 2: // Styl chodzenia
        {
            Player_ShowWalkingStyles(playerid);
        }

        case 3: // Zaloz grupe przestepcza
        {
            if(Player_CanCreateGang(playerid))
            {
                Player_ShowCreateGang(playerid);
            }
            else
            {
                new msg[256];
                new houses = Player_CountHouses(playerid);
                new vehicles = Player_CountVehicles(playerid);
                new housesOk[16], vehiclesOk[16];

                if(houses >= 1) housesOk = "{00FF00}[OK]";
                else housesOk = "{FF0000}[BRAK]";

                if(vehicles >= 1) vehiclesOk = "{00FF00}[OK]";
                else vehiclesOk = "{FF0000}[BRAK]";

                msg[0] = EOS;
                strcat(msg, "{FF0000}Nie spelniasz wymagan!\n\n", sizeof(msg));
                strcat(msg, "{FFFFFF}Wymagania:\n", sizeof(msg));
                format(msg, sizeof(msg), "%s- Domki: %d/1 %s\n", msg, houses, housesOk);
                format(msg, sizeof(msg), "%s- Pojazdy: %d/1 %s\n", msg, vehicles, vehiclesOk);
                strcat(msg, "- Czlonkowie: 3/3 (znajomi)", sizeof(msg));

                ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX,
                    "Wymagania", msg, "OK", "");
            }
        }
    }

    return 1;
}

stock Player_ShowPremiumShop(playerid)
{
    new content[512];
    new vipLevel = pInfo[playerid][player_vip];
    new vipStatus[32];

    if(vipLevel > 0)
    {
        format(vipStatus, sizeof(vipStatus), "{00FF00}VIP Level %d", vipLevel);
    }
    else
    {
        vipStatus = "{FF0000}Brak VIP";
    }

    // Budowanie tekstu etapami
    content[0] = EOS;

    strcat(content, "{F1C40F}=== SKLEP PREMIUM ===\n\n", sizeof(content));

    format(content, sizeof(content), "%s{FFFFFF}Twoj status: %s\n\n", content, vipStatus);

    strcat(content, "{3498DB}Dostepne pakiety:\n", sizeof(content));
    strcat(content, "{FFFFFF}1. VIP Bronze (30 dni) - 10 PLN\n", sizeof(content));
    strcat(content, "   - 3 postacie\n", sizeof(content));
    strcat(content, "   - Dodatkowe style chodzenia\n\n", sizeof(content));

    strcat(content, "{FFFFFF}2. VIP Silver (30 dni) - 20 PLN\n", sizeof(content));
    strcat(content, "   - Wszystko z Bronze\n", sizeof(content));
    strcat(content, "   - Wiecej slotow na pojazdy\n\n", sizeof(content));

    strcat(content, "{FFFFFF}3. VIP Gold (30 dni) - 35 PLN\n", sizeof(content));
    strcat(content, "   - Wszystko z Silver\n", sizeof(content));
    strcat(content, "   - Priorytetowe wsparcie\n\n", sizeof(content));

    strcat(content, "{AAAAAA}Aby zakupic VIP, odwiedz nasz sklep\n", sizeof(content));
    strcat(content, "lub skontaktuj sie z administracja.", sizeof(content));

    ShowPlayerDialog(playerid, DIALOG_PREMIUM_SHOP, DIALOG_STYLE_MSGBOX, "Sklep Premium", content, "Zamknij", "");

    return 1;
}

stock Player_ShowWalkingStyles(playerid)
{
    new content[512];
    new currentStyle = pInfo[playerid][player_walking_anim];

    content = "";
    for(new i = 0; i < sizeof(WalkingStyles); i++)
    {
        if(i == currentStyle)
        {
            format(content, sizeof(content), "%s{00FF00}> %s [AKTYWNY]\n", content, WalkingStyles[i][walk_name]);
        }
        else
        {
            format(content, sizeof(content), "%s{FFFFFF}%s\n", content, WalkingStyles[i][walk_name]);
        }
    }

    ShowPlayerDialog(playerid, DIALOG_WALKING_STYLE, DIALOG_STYLE_LIST, "Styl chodzenia (Alt)", content, "Wybierz", "Anuluj");

    return 1;
}

stock Player_ShowCreateGang(playerid)
{
    new content[256];
    content[0] = EOS;

    strcat(content, "{FFFFFF}Spelniasz wszystkie wymagania!\n\n", sizeof(content));
    strcat(content, "{3498DB}Wymagania:\n", sizeof(content));
    strcat(content, "{00FF00}[OK] {FFFFFF}1 domek\n", sizeof(content));
    strcat(content, "{00FF00}[OK] {FFFFFF}3 czlonkow\n", sizeof(content));
    strcat(content, "{00FF00}[OK] {FFFFFF}1 pojazd\n\n", sizeof(content));
    strcat(content, "{FFFFFF}Podaj nazwe dla swojej grupy:", sizeof(content));

    ShowPlayerDialog(playerid, DIALOG_CREATE_GANG, DIALOG_STYLE_INPUT,
        "Zaloz grupe przestepcza", content, "Dalej", "Anuluj");

    return 1;
}

Dialog_HandlePremiumShop(playerid, response, listitem)
{
    #pragma unused listitem

    if(!response)
    {
        Player_ShowStatsMenu(playerid);
    }

    return 1;
}

Dialog_HandleWalkingStyle(playerid, response, listitem)
{
    if(!response)
    {
        Player_ShowStatsMenu(playerid);
        return 1;
    }

    if(listitem < 0 || listitem >= sizeof(WalkingStyles))
    {
        return 1;
    }

    // Zapisz styl chodzenia
    pInfo[playerid][player_walking_anim] = listitem;
    strcopy(pInfo[playerid][player_walking_lib], WalkingStyles[listitem][walk_lib], 16);
    strcopy(pInfo[playerid][player_walking_name], WalkingStyles[listitem][walk_anim], 24);

    new msg[128];
    format(msg, sizeof(msg), "Ustawiono styl chodzenia: {00FF00}%s", WalkingStyles[listitem][walk_name]);
    SendClientMessage(playerid, COLOR_INFO, msg);
    SendClientMessage(playerid, COLOR_GRAY, "Przytrzymaj klawisz Alt aby chodzic w wybranym stylu.");

    return 1;
}

Dialog_HandleCreateGang(playerid, response, listitem, const inputtext[])
{
    #pragma unused listitem

    if(!response)
    {
        Player_ShowStatsMenu(playerid);
        return 1;
    }

    if(isnull(inputtext) || strlen(inputtext) < 3 || strlen(inputtext) > 32)
    {
        SendClientMessage(playerid, COLOR_ERROR, "Nazwa grupy musi miec od 3 do 32 znakow!");
        Player_ShowCreateGang(playerid);
        return 1;
    }

    // Zapisz nazwe tymczasowo
    new tmpName[64];
    format(tmpName, sizeof(tmpName), "%s", inputtext);
    SetPVarString(playerid, "TempGangName", tmpName);

    new msg[256];
    msg[0] = EOS;

    strcat(msg, "{FFFFFF}Czy na pewno chcesz zalozyc grupe?\n\n", sizeof(msg));
    format(msg, sizeof(msg), "%s{3498DB}Nazwa: {FFFFFF}%s\n\n", msg, inputtext);
    strcat(msg, "{AAAAAA}Zalozenie grupy jest nieodwracalne.\n", sizeof(msg));
    strcat(msg, "Bedziesz liderem tej grupy.", sizeof(msg));

    ShowPlayerDialog(playerid, DIALOG_CREATE_GANG_CONFIRM, DIALOG_STYLE_MSGBOX,
        "Potwierdz utworzenie grupy", msg, "Tak", "Nie");

    return 1;
}

Dialog_HandleCreateGangConfirm(playerid, response)
{
    if(!response)
    {
        DeletePVar(playerid, "TempGangName");
        Player_ShowStatsMenu(playerid);
        return 1;
    }

    new gangName[64];
    GetPVarString(playerid, "TempGangName", gangName, sizeof(gangName));
    DeletePVar(playerid, "TempGangName");

    if(isnull(gangName))
    {
        SendClientMessage(playerid, COLOR_ERROR, "Wystapil blad. Sprobuj ponownie.");
        return 1;
    }

    // Znajdz wolny slot na grupe
    new groupId = Group_FindFreeSlot();
    if(groupId == -1)
    {
        SendClientMessage(playerid, COLOR_ERROR, "Nie mozna utworzyc wiecej grup. Skontaktuj sie z administracja.");
        return 1;
    }

    // Utworz grupe
    gInfo[groupId][group_uid] = groupId;
    strcopy(gInfo[groupId][group_name], gangName, 64);
    gInfo[groupId][group_type] = 4; // Gang
    gInfo[groupId][group_flags] = GROUP_FLAG_GANG;
    gInfo[groupId][group_leader] = pInfo[playerid][player_uid];
    gInfo[groupId][group_bank] = 0;
    gInfo[groupId][group_color] = 0xAA3333FF;
    gInfo[groupId][group_created] = true;

    // Dodaj gracza do grupy jako lidera
    for(new i = 0; i < MAX_PLAYER_GROUPS; i++)
    {
        if(pInfo[playerid][player_group][i] == 0)
        {
            pInfo[playerid][player_group][i] = groupId;
            pInfo[playerid][player_group_perm][i] = 0xFFFFFFFF; // Wszystkie uprawnienia (ranga 9 - lider)
            break;
        }
    }

    // Zapisz grupe
    Group_Save(groupId);

    new msg[128];
    format(msg, sizeof(msg), "Utworzono grupe przestepcza: {F1C40F}%s", gangName);
    SendClientMessage(playerid, COLOR_GREEN, msg);
    SendClientMessage(playerid, COLOR_INFO, "Jestes liderem tej grupy. Uzyj /g aby zarzadzac grupa.");

    return 1;
}

// ===========================================================================
// HANDLERY POTWIERDZENIA
// ===========================================================================

static g_ConfirmCallback[MAX_PLAYERS][64];
static g_ConfirmData[MAX_PLAYERS][256];

// Forward declarations
forward Dialog_ShowPendingConfirm(playerid);

// Pending delayed confirm data (used when we need to delay showing a dialog to avoid click-races)
static g_PendingConfirmCallback[MAX_PLAYERS][64];
static g_PendingConfirmTitle[MAX_PLAYERS][64];
static g_PendingConfirmMessage[MAX_PLAYERS][256];
static g_PendingConfirmData[MAX_PLAYERS][256];

ShowConfirmDialogEx(playerid, const callback[], const title[], const message[], const data[] = "")
{
    // Store callback and data safely per-player
    strcopy(g_ConfirmCallback[playerid], callback, 64);
    strcopy(g_ConfirmData[playerid], data, 256);
    printf("[DBG] ShowConfirmDialogEx: player=%d callback='%s' data='%s'\n", playerid, g_ConfirmCallback[playerid], g_ConfirmData[playerid]);

    ShowConfirmDialog(playerid, DIALOG_CONFIRM, title, message);

    return 1;
}

/**
 * Show confirm dialog after a small delay to avoid click-races between menus
 * Example: when clicking a model preview the click can also act as a dialog confirmation
 */
stock ShowConfirmDialogExDelayed(playerid, const callback[], const title[], const message[], const data[] = "", delay_ms = 250)
{
    if(!IsPlayerConnected(playerid)) return 0;

    // Save pending data
    strcopy(g_PendingConfirmCallback[playerid], callback, sizeof g_PendingConfirmCallback[]);
    strcopy(g_PendingConfirmTitle[playerid], title, sizeof g_PendingConfirmTitle[]);
    strcopy(g_PendingConfirmMessage[playerid], message, sizeof g_PendingConfirmMessage[]);
    strcopy(g_PendingConfirmData[playerid], data, sizeof g_PendingConfirmData[]);

    // Schedule a one-shot timer to show the dialog
    SetTimerEx("Dialog_ShowPendingConfirm", delay_ms, false, "i", playerid);
    printf("[DBG] ShowConfirmDialogExDelayed: player=%d callback='%s' delay=%d\n", playerid, g_PendingConfirmCallback[playerid], delay_ms);
    return 1;
}

public Dialog_ShowPendingConfirm(playerid)
{
    if(!IsPlayerConnected(playerid)) return 0;

    // Double-check we still have pending data
    if(g_PendingConfirmCallback[playerid][0] == EOS)
    {
        return 0;
    }

    ShowConfirmDialogEx(playerid, g_PendingConfirmCallback[playerid], g_PendingConfirmTitle[playerid], g_PendingConfirmMessage[playerid], g_PendingConfirmData[playerid]);

    // Clear pending
    g_PendingConfirmCallback[playerid][0] = EOS;
    g_PendingConfirmTitle[playerid][0] = EOS;
    g_PendingConfirmMessage[playerid][0] = EOS;
    g_PendingConfirmData[playerid][0] = EOS;

    return 1;
}

Dialog_HandleConfirm(playerid, response)
{
    if(!response)
    {
        // Anulowano
        g_ConfirmCallback[playerid][0] = EOS;
        g_ConfirmData[playerid][0] = EOS;
        return 1;
    }

    // Log invocation for diagnosis
    printf("[DBG] Dialog_HandleConfirm invoked: player=%d callback='%s' data='%s' mysql_errno=%d\n", playerid, g_ConfirmCallback[playerid], g_ConfirmData[playerid], mysql_errno(mySQLconnection));

    // Wywołaj callback (jeśli istnieje)
    if(strcmp(g_ConfirmCallback[playerid], "phone_contact_delete") == 0)
    {
        new id;
        if(sscanf(g_ConfirmData[playerid], "%d", id) == 1)
        {
            MySQL_ExecuteFormat("DELETE FROM `phone_contacts` WHERE id = %d", id);
            SendClientMessage(playerid, COLOR_SUCCESS, "Contact deleted.");
            Dialog_ShowPhoneContacts(playerid);
        }
    }
    else if(strcmp(g_ConfirmCallback[playerid], "phone_sms_delete") == 0)
    {
        new id;
        if(sscanf(g_ConfirmData[playerid], "%d", id) == 1)
        {
            if(MySQL_IsConnected())
            {
                MySQL_ExecuteFormat("DELETE FROM `phone_sms` WHERE id = %d", id);
                printf("[DBG] Dialog_HandleConfirm: executed DELETE phone_sms id=%d\n", id);
                SendClientMessage(playerid, COLOR_SUCCESS, "Message deleted.");
                Dialog_ShowPhoneSMS(playerid);
            }
            else
            {
                // Queue delete for later flush
                Phone_QueueDelete(id);
                SendClientMessage(playerid, COLOR_GREY, "No DB connection - message will be deleted when connection is restored.");
            }
        }
        else
        {
            printf("[DBG] Dialog_HandleConfirm: failed to parse id from '%s'\n", g_ConfirmData[playerid]);
        }
    }
    else if(strcmp(g_ConfirmCallback[playerid], "carshop_buy") == 0)
    {
        // Data format: "cat;index"
        new cat, idx;

        // Robust parse to avoid sscanf warnings and failures
        new tmp[64];
        strcopy(tmp, g_ConfirmData[playerid], sizeof(tmp));
        new pos = strfind(tmp, ";", true);
        if(pos == -1)
        {
            SendClientMessage(playerid, COLOR_ERROR, "Bad carshop data.");
            printf("[DBG] Dialog_HandleConfirm: failed to parse carshop data '%s'\n", g_ConfirmData[playerid]);
        }
        else
        {
            tmp[pos] = EOS;
            cat = strval(tmp);
            idx = strval(tmp[pos + 1]);

            if(cat < 0 || cat >= CAR_CAT_MAX)
            {
                SendClientMessage(playerid, COLOR_ERROR, "Invalid car selection.");
            }
            else
            {
                printf("[DBG] Dialog_HandleConfirm: parsed carshop data cat=%d idx=%d\n", cat, idx);
                CarShop_HandleBuy(playerid, cat, idx);
            }
        }
    }

    // clear callback data
    g_ConfirmCallback[playerid][0] = EOS;
    g_ConfirmData[playerid][0] = EOS;

    return 1;
}

// Forward declaration for CarShop handler
forward CarShop_HandleBuy(playerid, cat, idx);
forward CarShop_OnInsert(playerid);

// Pending carshop purchase data (populated while async DB insert completes)
new CarShop_PendingFlag[MAX_PLAYERS];
new CarShop_PendingModel[MAX_PLAYERS];
new CarShop_PendingPrice[MAX_PLAYERS];
new Float:CarShop_PendingFX[MAX_PLAYERS];
new Float:CarShop_PendingFY[MAX_PLAYERS];
new Float:CarShop_PendingFZ[MAX_PLAYERS];
new Float:CarShop_PendingFA[MAX_PLAYERS];


// Handle the actual car purchase flow
public CarShop_HandleBuy(playerid, cat, idx)
{
    if(!IsPlayerConnected(playerid)) return 0;

    new name[64]; new price; new modelid;
    if(!CarShop_GetVehicleInfo(cat, idx, name, sizeof(name), price, modelid))
    {
        SendClientMessage(playerid, COLOR_ERROR, "Invalid car selection.");
        return 1;
    }

    if(price <= 0)
    {
        SendClientMessage(playerid, COLOR_ERROR, "Invalid car price.");
        return 1;
    }

    if(!Player_GiveMoney(playerid, -price))
    {
        SendClientMessage(playerid, COLOR_ERROR, "Operation failed: insufficient funds or invalid amount.");
        return 1;
    }

    new Float:fx, Float:fy, Float:fz, Float:fa;
    if(!CarShop_PickParkingSpot(playerid, fx, fy, fz, fa))
    {
        GetPlayerPos(playerid, fx, fy, fz);
        fa = pInfo[playerid][player_pos_a];
    }

    new ownerUid = pInfo[playerid][player_id];

    // Clear any previous pending purchase for safety
    CarShop_PendingFlag[playerid] = 0;

    new valuesStr[256];
    // Match the schema in database/schema.sql (veh_model, veh_pos_x, veh_pos_y, veh_pos_z, veh_pos_a, veh_color1, veh_color2, veh_owner_type, veh_owner, veh_fuel)
    format(valuesStr, sizeof(valuesStr), "%d, %f, %f, %f, %f, %d, %d, %d, %d, %f",
        modelid, fx, fy, fz, fa, -1, -1, VEHICLE_OWNER_TYPE_PLAYER, ownerUid, 100.0);

    // Debug the formatted values string to ensure all fields are present
    printf("[DBG] CarShop: valuesStr='%s'\n", valuesStr);

    // Build the full query locally and execute with mysql_tquery to avoid MySQL_ExecuteFormat arg-limit issues
    new query[512];
    format(query, sizeof(query), "INSERT INTO `vehicles` (`veh_model`,`veh_pos_x`,`veh_pos_y`,`veh_pos_z`,`veh_pos_a`,`veh_color1`,`veh_color2`,`veh_owner_type`,`veh_owner`,`veh_fuel`) VALUES (%s)", valuesStr);
    // Store pending data and execute query with a callback so we can finalize when DB returns the insert id
    CarShop_PendingFlag[playerid] = 1;
    CarShop_PendingModel[playerid] = modelid;
    CarShop_PendingPrice[playerid] = price;
    CarShop_PendingFX[playerid] = fx;
    CarShop_PendingFY[playerid] = fy;
    CarShop_PendingFZ[playerid] = fz;
    CarShop_PendingFA[playerid] = fa;
    mysql_tquery(mySQLconnection, query, "CarShop_OnInsert", "d", playerid);

    // Notify player that purchase is processing (final message will come when DB insert completes)
    SendClientMessage(playerid, COLOR_YELLOW, "Processing purchase... saving vehicle to database.");
    return 1;
}

public CarShop_OnInsert(playerid)
{
    // This is invoked by the MySQL plugin when the previous INSERT completes
    if(!CarShop_PendingFlag[playerid])
    {
        printf("[DBG] CarShop_OnInsert: no pending purchase for pid=%d\n", playerid);
        return 1;
    }

    new uid = cache_insert_id();
    new modelid = CarShop_PendingModel[playerid];
    new price = CarShop_PendingPrice[playerid];
    new Float:fx = CarShop_PendingFX[playerid];
    new Float:fy = CarShop_PendingFY[playerid];
    new Float:fz = CarShop_PendingFZ[playerid];
    new Float:fa = CarShop_PendingFA[playerid];
    // Name not stored per-request; use model id in messaging if needed
    new name[32]; format(name, sizeof(name), "model %d", modelid);

    // Clear pending flag immediately to avoid reentrancy
    CarShop_PendingFlag[playerid] = 0;

    if(uid <= 0)
    {
        // Refund
        Player_GiveMoney(playerid, price);
        if(IsPlayerConnected(playerid))
        {
            SendClientMessage(playerid, COLOR_ERROR, "Operation failed: could not save vehicle to DB. Funds refunded.");
        }
        printf("[DBG] CarShop_OnInsert: DB insert uid invalid for pid=%d model=%d price=%d mysql_errno=%d\n", playerid, modelid, price, mysql_errno(mySQLconnection));
        return 1;
    }

    // Try spawn
    if(IsPlayerConnected(playerid))
    {
        new vid = Vehicle_Create(modelid, fx, fy, fz, fa, -1, -1, OWNER_PLAYER, pInfo[playerid][player_id]);
        if(vid == INVALID_VEHICLE_ID)
        {
            SendClientMessage(playerid, COLOR_SUCCESS, "Purchased %s for $%d. Vehicle saved in DB (UID: %d) but failed to spawn in-game (no space).", name, price, uid);
            Player_SaveData(playerid, "purchase");
            Dialog_ShowPlayerVehicles(playerid);
            return 1;
        }

        new slot = Vehicle_GetSlot(vid);
        if(slot != -1)
        {
            VehicleDbUid[slot] = uid;
            vInfo[slot][veh_spawn_x] = fx;
            vInfo[slot][veh_spawn_y] = fy;
            vInfo[slot][veh_spawn_z] = fz;
            vInfo[slot][veh_spawn_a] = fa;
        }

        Vehicle_SetSpawnPos(vid, fx, fy, fz, fa);
        Vehicle_SetLocked(vid, true);
        Player_SaveData(playerid, "purchase");
        SendClientMessage(playerid, COLOR_SUCCESS, "Purchased %s for $%d. Vehicle spawned on parking (UID: %d, ID: %d).", name, price, uid, vid);
        Dialog_ShowPlayerVehicles(playerid);
    }
    else
    {
        // Player disconnected between purchase and DB callback; leave DB record only
        printf("[DBG] CarShop_OnInsert: player disconnected after purchase pid=%d uid=%d\n", playerid, uid);
    }

    return 1;
}

// ===========================================================================
// EOF
// ===========================================================================

