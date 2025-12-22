#if defined _phone_system_included
    #endinput
#endif
#define _phone_system_included

#include <open.mp>
#include <a_mysql>

// Single-file Phone System
// - Combines core, sms, calls, contacts and GUI
// - Independent of player nick/inventory; operates on phone numbers
// - Uses DB tables: phone_numbers, phone_owners, phone_devices, phone_sms, phone_contacts, phone_calls

// Configuration
#define PHONE_MIN_NUMBER 1000
#define PHONE_MAX_NUMBER 99999

// Forwards for DB cache helpers used across the project
forward cache_num_rows();

// Forwards for phone public callbacks (declared to reduce compiler warnings)
forward Phone_OnNumbersLoaded();
forward Phone_OnEnsureNumber(number);
forward Phone_OnGetPlayerNumber(playerid, str[]);
forward Phone_OnFindOwnerForNumber(number);
forward Phone_OnStartCallCheck(callerPid, number);
forward Phone_Dial(playerid, target);
forward Phone_Hangup(playerid);
forward Phone_OnPhoneColumnsChecked(playerid);
forward Phone_BackfillPhoneDevices(playerid);
forward Phone_QueueDelete(smsId);
forward Phone_FlushQueuedDeletes();
forward Phone_OnContactDetailLoaded(playerid);
forward Phone_OnLookupContactName(phoneNumber, targetNumber);
forward Phone_SendSMS(senderPid, targetPid, const message[]);
forward Phone_SendSMSToUID(senderPid, targetNumber, const unused[], const message[]);
forward Phone_OnSMSViewLoaded(playerid);
forward Phone_DeleteSMS(playerid, smsId);
forward Phone_OnContactsLoaded(playerid);
forward Phone_OnSMSListLoaded(playerid);
forward Phone_OnCallHistoryLoaded_GUI(playerid);
forward Phone_OnSettingsLoaded(playerid);
forward Phone_RunSelfTests(playerid);
forward Phone_RunSelfTests_FlushDelete();

// -----------------------------------------------------------------------------
// Core: caching, ownership, states, orchestration
// -----------------------------------------------------------------------------
#define PHONE_SMS_LIST_MAX 100
#define PHONE_CONTACTS_MAX 200
#define PHONE_HISTORY_MAX 200

new gPhoneState[100000]; // indexed by phone number -> 0=unknown/missing, 1=on, 2=off, 3=destroyed
new gPhoneOwnerUidByNumber[100000]; // number -> owner_uid (0 = none cached)

// per-player lists used by dialog handlers (populated by callbacks)
new g_PhoneSMSList[MAX_PLAYERS][PHONE_SMS_LIST_MAX];
new g_PhoneSMSCount[MAX_PLAYERS];

new g_PhoneContactsList[MAX_PLAYERS][PHONE_CONTACTS_MAX];
new g_PhoneContactsUID[MAX_PLAYERS][PHONE_CONTACTS_MAX];
new g_PhoneContactsCount[MAX_PLAYERS];

new g_PhoneHistoryList[MAX_PLAYERS][PHONE_HISTORY_MAX];
new g_PhoneHistoryCount[MAX_PLAYERS];

// Feature flags / DB capabilities
new g_PhoneDBHasContactCols = 1; // assume contacts supported by default


forward Phone_Init();
public Phone_Init()
{
    // Load all phone_numbers into cache
    new q[256];
    mysql_format(mySQLconnection, q, sizeof(q), "SELECT phone_number, state FROM phone_numbers");
    mysql_tquery(mySQLconnection, q, "Phone_OnNumbersLoaded");
    return 1;
}

public Phone_OnNumbersLoaded()
{
    new rows = cache_num_rows();

    for(new i = 0; i < rows; i++)
    {
        new numStr[16];
        new stateStr[16];
        cache_get_value(i, "phone_number", numStr, sizeof(numStr));
        cache_get_value(i, "state", stateStr, sizeof(stateStr));
        new num = 0;
        if(sscanf(numStr, "%d", num) != 1) continue;
        // ensure number is within valid configured range
        if(num < PHONE_MIN_NUMBER || num > PHONE_MAX_NUMBER) continue;
        new s = 0;
        if(strcmp(stateStr, "on") == 0) s = 1;
        else if(strcmp(stateStr, "off") == 0) s = 2;
        else if(strcmp(stateStr, "destroyed") == 0) s = 3;
        gPhoneState[num] = s;
    }
    return 1;
}

stock Phone_EnsureNumberExists(number)
{
    if(number < PHONE_MIN_NUMBER || number > PHONE_MAX_NUMBER) return 0;
    new q[256];
    mysql_format(mySQLconnection, q, sizeof(q), "INSERT INTO phone_numbers (phone_number) VALUES (%d) ON DUPLICATE KEY UPDATE phone_number = phone_number", number);
    mysql_tquery(mySQLconnection, q, "Phone_OnEnsureNumber", "i", number);
    return 1;
}
public Phone_OnEnsureNumber(number)
{
    gPhoneState[number] = 1; // default to on
    return 1;
}

stock Phone_AssignNumberToOwner(owner_uid, number)
{
    if(number < PHONE_MIN_NUMBER || number > PHONE_MAX_NUMBER) return 0;
    new q[256];
    mysql_format(mySQLconnection, q, sizeof(q), "INSERT INTO phone_owners (owner_uid, phone_number) VALUES (%d, %d) ON DUPLICATE KEY UPDATE phone_number = phone_number", owner_uid, number);
    mysql_tquery(mySQLconnection, q);
    gPhoneOwnerUidByNumber[number] = owner_uid;
    return 1;
}

stock GetOrCreatePlayerPrimaryNumber(playerid, output[], size)
{
    if(!IsValidPlayerId(playerid) || !pInfo[playerid][player_logged]) return 0;
    new owner_uid = pInfo[playerid][player_uid];
    new q[256];
    mysql_format(mySQLconnection, q, sizeof(q), "SELECT phone_number FROM phone_owners WHERE owner_uid = %d LIMIT 1", owner_uid);
    mysql_tquery(mySQLconnection, q, "Phone_OnGetPlayerNumber", "is", playerid, output);
    return 1;
}
public Phone_OnGetPlayerNumber(playerid, str[])
{
    // callback stub - not used synchronously in current GUI
    return 1;
}

stock Phone_FindOnlinePidByNumber(number)
{
    if(number < PHONE_MIN_NUMBER || number > PHONE_MAX_NUMBER) return -1;
    new owner_uid = gPhoneOwnerUidByNumber[number];
    if(owner_uid != 0)
    {
        foreach(new i : Player)
        {
            if(pInfo[i][player_uid] == owner_uid && pInfo[i][player_logged]) return i;
        }
    }
    new q[256];
    mysql_format(mySQLconnection, q, sizeof(q), "SELECT owner_uid FROM phone_owners WHERE phone_number = %d LIMIT 1", number);
    mysql_tquery(mySQLconnection, q, "Phone_OnFindOwnerForNumber", "i", number);
    return -1;
}

public Phone_OnFindOwnerForNumber(number)
{
    new rows = cache_num_rows();
    if(rows == 0) return 1;
    new owner_uid_str[32]; cache_get_value(0, "owner_uid", owner_uid_str, sizeof(owner_uid_str));
    new owner_uid = 0; if(sscanf(owner_uid_str, "%d", owner_uid) != 1) return 1;
    if(number >= PHONE_MIN_NUMBER && number <= PHONE_MAX_NUMBER) gPhoneOwnerUidByNumber[number] = owner_uid;
    return 1;
    return 1;
}

// Calls
stock Phone_StartCallByNumber(callerPid, number)
{
    if(number < PHONE_MIN_NUMBER || number > PHONE_MAX_NUMBER)
    {
        SendClientMessage(callerPid, COLOR_ERROR, "Invalid phone number.");
        return 0;
    }

    if(gPhoneState[number] == 0)
    {
        new q[256];
        mysql_format(mySQLconnection, q, sizeof(q), "SELECT state FROM phone_numbers WHERE phone_number = %d LIMIT 1", number);
        mysql_tquery(mySQLconnection, q, "Phone_OnStartCallCheck", "ii", callerPid, number);
        return 1;
    }

    if(gPhoneState[number] != 1)
    {
        SendClientMessage(callerPid, COLOR_ERROR, "Subscriber temporarily unavailable.");
        return 0;
    }

    new calleePid = Phone_FindOnlinePidByNumber(number);
    if(calleePid == -1)
    {
        SendClientMessage(callerPid, COLOR_ERROR, "Subscriber temporarily unavailable.");
        return 0;
    }

    new callerNumBuf[16];
    new callerNum = 0;
    new slot = Item_FindByType(callerPid, ITEM_TYPE_PHONE);
    if(slot >= 0) callerNum = pItem[callerPid][slot][item_value2];
    if(callerNum == 0) { format(callerNumBuf, sizeof(callerNumBuf), "%d", callerPid); } else { format(callerNumBuf, sizeof(callerNumBuf), "%d", callerNum); }

    new q[256];
    format(q, sizeof(q), "Incoming call from %s", callerNumBuf);
    ShowPlayerDialog(calleePid, DIALOG_PHONE_INCOMING, DIALOG_STYLE_MSGBOX, "Incoming call", q, "Answer", "Reject");

    SetPVarInt(calleePid, "Phone_IncomingCallerNumber", callerNum);
    SetPVarInt(calleePid, "Phone_PendingCallerPid", callerPid);

    return 1;
}

public Phone_OnStartCallCheck(callerPid, number)
{
    new rows = cache_num_rows();
    if(rows == 0)
    {
        SendClientMessage(callerPid, COLOR_ERROR, "That phone number does not exist.");
        return 1;
    }

    new stateStr[16]; cache_get_value(0, "state", stateStr, sizeof(stateStr));
    if(strcmp(stateStr, "on") != 0)
    {
        SendClientMessage(callerPid, COLOR_ERROR, "Subscriber temporarily unavailable.");
        return 1;
    }

    new calleePid = Phone_FindOnlinePidByNumber(number);
    if(calleePid == -1)
    {
        SendClientMessage(callerPid, COLOR_ERROR, "Subscriber temporarily unavailable.");
        return 1;
    }

    new callerNumBuf[16]; format(callerNumBuf, sizeof(callerNumBuf), "%d", callerPid);
    new q[256]; format(q, sizeof(q), "Incoming call from %s", callerNumBuf);
    ShowPlayerDialog(calleePid, DIALOG_PHONE_INCOMING, DIALOG_STYLE_MSGBOX, "Incoming call", q, "Answer", "Reject");
    SetPVarInt(calleePid, "Phone_IncomingCallerNumber", callerPid);
    SetPVarInt(calleePid, "Phone_PendingCallerPid", callerPid);
    return 1;
}

public Phone_CreateOrUpdateDeviceForItem(item_id, phone_number, owner_uid, owner_type)
{
    new q[512];
    mysql_format(mySQLconnection, q, sizeof(q), "INSERT INTO phone_devices (phone_number, item_id, owner_uid, owner_type) VALUES (%d, %d, %d, %d) ON DUPLICATE KEY UPDATE item_id = VALUES(item_id), owner_uid = VALUES(owner_uid), owner_type = VALUES(owner_type), updated_at = NOW()", phone_number, item_id, owner_uid, owner_type);
    mysql_tquery(mySQLconnection, q);
    return 1;
}

public Phone_Dial(playerid, target)
{
    if(target >= PHONE_MIN_NUMBER) {
        Phone_StartCallByNumber(playerid, target);
        return 1;
    }

    if(IsValidPlayerId(target) && pInfo[target][player_logged]) {
        new slot = Item_FindByType(target, ITEM_TYPE_PHONE);
        new targetNum = 0;
        if(slot >= 0) targetNum = pItem[target][slot][item_value2];
        if(targetNum == 0) { SendClientMessage(playerid, COLOR_ERROR, "Subscriber temporarily unavailable."); return 1; }
        Phone_StartCallByNumber(playerid, targetNum);
        return 1;
    }

    SendClientMessage(playerid, COLOR_ERROR, "Invalid target for dialing.");
    return 1;
}

public Phone_Hangup(playerid)
{
    DeletePVar(playerid, "Phone_IncomingCallerNumber");
    DeletePVar(playerid, "Phone_PendingCallerPid");
    SendClientMessage(playerid, COLOR_INFO, "Call ended.");
    return 1;
}

public Phone_OnPhoneColumnsChecked(playerid)
{
    new rows = cache_num_rows();
    if(rows == 0) { SendClientMessage(playerid, COLOR_ERROR, "Phone schema check returned no data."); return 1; }
    new smsCols[8], callCols[8], contactCols[8];
    cache_get_value(0, "sms", smsCols, sizeof(smsCols));
    cache_get_value(0, "calls", callCols, sizeof(callCols));
    cache_get_value(0, "contacts", contactCols, sizeof(contactCols));
    new msg[256]; format(msg, sizeof(msg), "phone_sms has sender_number: %s, phone_calls has caller_number: %s, phone_contacts has contact_number: %s", smsCols, callCols, contactCols);
    SendClientMessage(playerid, COLOR_INFO, msg);
    return 1;
}

// Backfill + queue
public Phone_BackfillPhoneDevices(playerid)
{
    new q[512];
    mysql_format(mySQLconnection, q, sizeof(q), "INSERT INTO phone_devices (phone_number, item_id, owner_uid, owner_type, created_at, updated_at) SELECT item_phone, item_id, item_owner, item_owner_type, NOW(), NOW() FROM items WHERE item_type = %d ON DUPLICATE KEY UPDATE item_id = VALUES(item_id), owner_uid = VALUES(owner_uid), owner_type = VALUES(owner_type), updated_at = VALUES(updated_at)", ITEM_TYPE_PHONE);
    mysql_tquery(mySQLconnection, q);
    SendClientMessage(playerid, COLOR_INFO, "Phone devices backfill triggered.");
    return 1;
}

new g_PhoneQueuedDeletes[256];
new g_PhoneQueuedDeletesCount = 0;

public Phone_QueueDelete(smsId)
{
    if(g_PhoneQueuedDeletesCount < sizeof(g_PhoneQueuedDeletes))
    {
        g_PhoneQueuedDeletes[g_PhoneQueuedDeletesCount++] = smsId;
    }
    SetTimerEx("Phone_FlushQueuedDeletes", 10000, true);
    return 1;
}

public Phone_FlushQueuedDeletes()
{
    if(g_PhoneQueuedDeletesCount == 0) return 1;
    if(!MySQL_IsConnected()) return 1;

    new toProcess = g_PhoneQueuedDeletesCount;
    for(new i = 0; i < toProcess; i++)
    {
        new id = g_PhoneQueuedDeletes[i];
        MySQL_ExecuteFormat("DELETE FROM `phone_sms` WHERE id = %d", id);
    }

    g_PhoneQueuedDeletesCount = 0;
    return 1;
}

// -----------------------------------------------------------------------------
// Self-test helpers (admin)
// -----------------------------------------------------------------------------
public Phone_RunSelfTests(playerid)
{
    // Ensure test numbers exist
    Phone_EnsureNumberExists(9001);
    Phone_EnsureNumberExists(9002);

    // Insert a test SMS and then request listing (callback populates player's SMS list)
    MySQL_ExecuteFormat("INSERT INTO `phone_sms` (`sender_number`,`receiver_number`,`message`) VALUES (%d, %d, '%s')", 9001, 9002, "Automated test message");
    new q[1024];
    mysql_format(mySQLconnection, q, sizeof(q), "SELECT s.id, s.sender_number, s.receiver_number, s.message, s.timestamp, COALESCE(pc_sender.contact_name, s.sender_number) AS sender_display FROM phone_sms s LEFT JOIN phone_contacts pc_sender ON pc_sender.phone_number = %d AND pc_sender.contact_number = s.sender_number WHERE s.sender_number = %d OR s.receiver_number = %d ORDER BY s.timestamp DESC LIMIT 10", 9001, 9001, 9001);
    mysql_tquery(mySQLconnection, q, "Phone_OnSMSListLoaded", "i", playerid);

    // Insert a test call and request history
    MySQL_ExecuteFormat("INSERT INTO `phone_calls` (`caller_number`,`callee_number`,`status`,`start_time`) VALUES (%d, %d, 'missed', NOW())", 9001, 9002);
    new q2[512];
    mysql_format(mySQLconnection, q2, sizeof(q2), "SELECT id, caller_number, callee_number, start_time, end_time, status FROM phone_calls WHERE caller_number = %d OR callee_number = %d ORDER BY start_time DESC LIMIT 10", 9001, 9001);
    mysql_tquery(mySQLconnection, q2, "Phone_OnCallHistoryLoaded_GUI", "i", playerid);

    // Schedule a flush to try deleting the newly inserted SMS (as a test of queueing and flush)
    SetPVarInt(playerid, "Phone_SelfTestPending", 1);
    SetTimerEx("Phone_RunSelfTests_FlushDelete", 1000, false);

    SendClientMessage(playerid, COLOR_SUCCESS, "Phone self-test initiated.");
    return 1;
}

public Phone_RunSelfTests_FlushDelete()
{
    for(new playerid = 0; playerid < MAX_PLAYERS; playerid++)
    {
        if(GetPVarInt(playerid, "Phone_SelfTestPending") == 1)
        {
            DeletePVar(playerid, "Phone_SelfTestPending");
            if(g_PhoneSMSCount[playerid] > 0)
            {
                new id = g_PhoneSMSList[playerid][0];
                Phone_QueueDelete(id);
                Phone_FlushQueuedDeletes();
                SendClientMessage(playerid, COLOR_INFO, "Phone self-test: queued+flushed delete for latest SMS.");
            }
            else
            {
                SendClientMessage(playerid, COLOR_WARNING, "Phone self-test: no messages found to delete.");
            }
        }
    }
    return 1;
}

// Contacts
stock Phone_AddContactForNumber(playerid, targetNumber, const contactName[])
{
    new slot = Item_FindByType(playerid, ITEM_TYPE_PHONE);
    new myNumber = 0;
    if(slot >= 0) myNumber = pItem[playerid][slot][item_value2];

    if(myNumber == 0)
    {
        myNumber = 1000 + playerid;
        Phone_EnsureNumberExists(myNumber);
        Phone_AssignNumberToOwner(pInfo[playerid][player_uid], myNumber);
    }

    new esc[128]; mysql_escape_string(contactName, esc, mySQLconnection, sizeof(esc));
    new q[512];
    mysql_format(mySQLconnection, q, sizeof(q), "INSERT INTO phone_contacts (phone_number, contact_number, contact_name) VALUES (%d, %d, '%s')", myNumber, targetNumber, esc);
    mysql_tquery(mySQLconnection, q);
    SendClientMessage(playerid, COLOR_SUCCESS, "Contact added.");
    return 1;
}

public Phone_OnContactDetailLoaded(playerid)
{
    new rows = cache_num_rows();
    if(rows == 0) { SendClientMessage(playerid, COLOR_ERROR, "Contact not found."); return 1; }

    new idStr[16], contactNum[16], contactName[64];
    cache_get_value(0, "id", idStr, sizeof(idStr));
    cache_get_value(0, "contact_number", contactNum, sizeof(contactNum));
    cache_get_value(0, "contact_name", contactName, sizeof(contactName));

    new msg[128]; format(msg, sizeof(msg), "Contact: %s (%s)", contactName, contactNum);
    ShowPlayerDialog(playerid, DIALOG_PHONE_CONTACT_VIEW, DIALOG_STYLE_MSGBOX, "Contact", msg, "Back", "Close");
    return 1;
}

stock Phone_LookupContactName(phoneNumber, targetNumber, outbuf[], size)
{
    new q[256];
    mysql_format(mySQLconnection, q, sizeof(q), "SELECT contact_name FROM phone_contacts WHERE phone_number = %d AND contact_number = %d LIMIT 1", phoneNumber, targetNumber);
    mysql_tquery(mySQLconnection, q, "Phone_OnLookupContactName", "si", phoneNumber, targetNumber);
    return 1;
}

public Phone_OnLookupContactName(phoneNumber, targetNumber)
{
    return 1;
}

// SMS
public Phone_SendSMS(senderPid, targetPid, const message[])
{
    if(!IsValidPlayerId(senderPid) || !IsValidPlayerId(targetPid)) return 0;
    new slot = Item_FindByType(senderPid, ITEM_TYPE_PHONE);
    new senderNum = 0;
    if(slot >= 0) senderNum = pItem[senderPid][slot][item_value2];
    if(senderNum == 0) senderNum = 1000 + senderPid;

    new tslot = Item_FindByType(targetPid, ITEM_TYPE_PHONE);
    new receiverNum = 0;
    if(tslot >= 0) receiverNum = pItem[targetPid][tslot][item_value2];
    if(receiverNum == 0) receiverNum = 1000 + targetPid;

    new esc[512]; mysql_escape_string(message, esc, mySQLconnection, sizeof(esc));
    new q[1024];
    mysql_format(mySQLconnection, q, sizeof(q), "INSERT INTO phone_sms (sender_number, receiver_number, message) VALUES (%d, %d, '%s')", senderNum, receiverNum, esc);
    mysql_tquery(mySQLconnection, q);

    if(pInfo[targetPid][player_logged])
    {
        new display[64];
        format(display, sizeof(display), "New SMS from %d", senderNum);
        SendClientMessage(targetPid, COLOR_INFO, display);
    }
    SendClientMessage(senderPid, COLOR_SUCCESS, "SMS sent.");
    return 1;
}

public Phone_SendSMSToUID(senderPid, targetNumber, const unused[], const message[])
{
    if(!IsValidPlayerId(senderPid)) return 0;
    if(targetNumber < PHONE_MIN_NUMBER || targetNumber > PHONE_MAX_NUMBER) { SendClientMessage(senderPid, COLOR_ERROR, "Invalid target number."); return 0; }

    new slot = Item_FindByType(senderPid, ITEM_TYPE_PHONE);
    new senderNum = 0;
    if(slot >= 0) senderNum = pItem[senderPid][slot][item_value2];
    if(senderNum == 0) senderNum = 1000 + senderPid;

    new esc[512]; mysql_escape_string(message, esc, mySQLconnection, sizeof(esc));
    new q[1024];
    mysql_format(mySQLconnection, q, sizeof(q), "INSERT INTO phone_sms (sender_number, receiver_number, message) VALUES (%d, %d, '%s')", senderNum, targetNumber, esc);
    mysql_tquery(mySQLconnection, q);
    SendClientMessage(senderPid, COLOR_SUCCESS, "SMS queued.");
    return 1;
}

// Helper: send SMS by raw phone number (resolves online recipient if available)
stock Phone_SendSMSByNumber(senderPid, targetNumber, const message[])
{
    new pid = Phone_FindOnlinePidByNumber(targetNumber);
    if(pid != -1)
    {
        Phone_SendSMS(senderPid, pid, message);
    }
    else
    {
        Phone_SendSMSToUID(senderPid, targetNumber, "", message);
    }
    return 1;
}

public Phone_OnSMSViewLoaded(playerid)
{
    new rows = cache_num_rows();
    if(rows == 0) { SendClientMessage(playerid, COLOR_ERROR, "No message found."); return 1; }
    new sender[16], receiver[16], body[512], timestamp[32], senderDisplay[64], receiverDisplay[64];
    cache_get_value(0, "sender_number", sender, sizeof(sender));
    cache_get_value(0, "receiver_number", receiver, sizeof(receiver));
    cache_get_value(0, "body", body, sizeof(body));
    cache_get_value(0, "timestamp", timestamp, sizeof(timestamp));
    cache_get_value(0, "sender_display", senderDisplay, sizeof(senderDisplay));
    cache_get_value(0, "receiver_display", receiverDisplay, sizeof(receiverDisplay));

    new fromName[64];
    if(strlen(senderDisplay) > 0) strcopy(fromName, senderDisplay, sizeof(fromName)); else strcopy(fromName, sender, sizeof(fromName));
    new toName[64];
    if(strlen(receiverDisplay) > 0) strcopy(toName, receiverDisplay, sizeof(toName)); else strcopy(toName, receiver, sizeof(toName));

    new dialog[1024];
    format(dialog, sizeof(dialog), "From: %s\nTo: %s\nTime: %s\n\n%s", fromName, toName, timestamp, body);
    ShowPlayerDialog(playerid, DIALOG_PHONE_SMS_VIEW, DIALOG_STYLE_MSGBOX, "SMS", dialog, "Reply", "Close");
    return 1;
}

public Phone_DeleteSMS(playerid, smsId)
{
    new q[128]; mysql_format(mySQLconnection, q, sizeof(q), "DELETE FROM phone_sms WHERE id = %d", smsId);
    mysql_tquery(mySQLconnection, q);
    SendClientMessage(playerid, COLOR_SUCCESS, "Message deleted.");
    return 1;
}

// GUI helpers
stock Phone_GetPlayerNumberSync(playerid)
{
    if(!IsValidPlayerId(playerid) || !pInfo[playerid][player_logged]) return 0;
    new slot = Item_FindByType(playerid, ITEM_TYPE_PHONE);
    if(slot >= 0) return pItem[playerid][slot][item_value2];
    return 1000 + playerid;
}

stock Dialog_ShowPhoneMenu(playerid)
{
    new options[256] = "Dial\nContacts\nSMS\nCall History\nSettings";
    ShowListDialog(playerid, DIALOG_PHONE_MENU, "Phone", options);
    return 1;
}

stock Dialog_ShowPhoneContacts(playerid)
{
    new mynum = Phone_GetPlayerNumberSync(playerid);
    new q[256]; mysql_format(mySQLconnection, q, sizeof(q), "SELECT id, contact_number, contact_name FROM phone_contacts WHERE phone_number = %d ORDER BY contact_name ASC", mynum);
    mysql_tquery(mySQLconnection, q, "Phone_OnContactsLoaded", "i", playerid);
    return 1;
}

public Phone_OnContactsLoaded(playerid)
{
    new rows = cache_num_rows();
    g_PhoneContactsCount[playerid] = 0;
    if(rows == 0)
    {
        g_PhoneContactsCount[playerid] = 0;
        SendClientMessage(playerid, COLOR_INFO, "No contacts.");
        return 1;
    }

    new list[4096]; list[0] = '\0';
    for(new i = 0; i < rows && i < PHONE_CONTACTS_MAX; i++)
    {
        new contactDbId[16], contactNum[16], contactName[64];
        cache_get_value(i, "id", contactDbId, sizeof(contactDbId));
        cache_get_value(i, "contact_number", contactNum, sizeof(contactNum));
        cache_get_value(i, "contact_name", contactName, sizeof(contactName));

        // store mapping for dialog handlers
        new id = 0; sscanf(contactDbId, "%d", id);
        g_PhoneContactsList[playerid][i] = id;
        new uid = 0; sscanf(contactNum, "%d", uid);
        g_PhoneContactsUID[playerid][i] = uid;
        g_PhoneContactsCount[playerid]++;

        new line[128]; format(line, sizeof(line), "%s - %s\n", contactName, contactNum);
        strcat(list, line);
    }

    ShowListDialog(playerid, DIALOG_PHONE_CONTACTS, "Contacts", list);
    return 1;
}

stock Dialog_ShowPhoneSMS(playerid)
{
    new mynum = Phone_GetPlayerNumberSync(playerid);
    new q[1024];
    mysql_format(mySQLconnection, q, sizeof(q), "SELECT s.id, s.sender_number, s.receiver_number, s.message, s.timestamp, COALESCE(pc_sender.contact_name, s.sender_number) AS sender_display FROM phone_sms s LEFT JOIN phone_contacts pc_sender ON pc_sender.phone_number = %d AND pc_sender.contact_number = s.sender_number WHERE s.sender_number = %d OR s.receiver_number = %d ORDER BY s.timestamp DESC LIMIT 100", mynum, mynum, mynum);
    mysql_tquery(mySQLconnection, q, "Phone_OnSMSListLoaded", "i", playerid);
    return 1;
}

public Phone_OnSMSListLoaded(playerid)
{
    new rows = cache_num_rows();
    g_PhoneSMSCount[playerid] = 0;
    if(rows == 0) { g_PhoneSMSCount[playerid] = 0; SendClientMessage(playerid, COLOR_INFO, "No messages."); return 1; }

    new list[8192]; list[0] = '\0';
    for(new i = 0; i < rows && i < PHONE_SMS_LIST_MAX; i++)
    {
        new idStr[16], sender[16], body[256], ts[32], senderDisplay[64];
        cache_get_value(i, "id", idStr, sizeof(idStr));
        cache_get_value(i, "sender_number", sender, sizeof(sender));
        cache_get_value(i, "message", body, sizeof(body));
        cache_get_value(i, "timestamp", ts, sizeof(ts));
        // attempt to use sender display if available (DB query may include joins that provide display name)
        cache_get_value(i, "sender_display", senderDisplay, sizeof(senderDisplay));

        new id = 0; sscanf(idStr, "%d", id);
        g_PhoneSMSList[playerid][i] = id;
        g_PhoneSMSCount[playerid]++;

        new displayName[64];
        if(strlen(senderDisplay) > 0) format(displayName, sizeof(displayName), "%s", senderDisplay);
        else format(displayName, sizeof(displayName), "%s", sender);

        new line[512]; format(line, sizeof(line), "%s: %s\n", displayName, body);
        strcat(list, line);
    }

    ShowListDialog(playerid, DIALOG_PHONE_SMS_LIST, "SMS Inbox", list);
    return 1;
}

stock Dialog_ShowPhoneHistory(playerid)
{
    new mynum = Phone_GetPlayerNumberSync(playerid);
    new q[512]; mysql_format(mySQLconnection, q, sizeof(q), "SELECT id, caller_number, callee_number, start_time, end_time, status FROM phone_calls WHERE caller_number = %d OR callee_number = %d ORDER BY start_time DESC LIMIT 100", mynum, mynum);
    mysql_tquery(mySQLconnection, q, "Phone_OnCallHistoryLoaded_GUI", "i", playerid);
    return 1;
}

public Phone_OnCallHistoryLoaded_GUI(playerid)
{
    new rows = cache_num_rows();
    g_PhoneHistoryCount[playerid] = 0;
    if(rows == 0) { g_PhoneHistoryCount[playerid] = 0; SendClientMessage(playerid, COLOR_INFO, "No call history."); return 1; }

    new list[4096]; list[0] = '\0';
    for(new i = 0; i < rows && i < PHONE_HISTORY_MAX; i++)
    {
        new idStr[16], caller[16], callee[16], st[32];
        cache_get_value(i, "id", idStr, sizeof(idStr));
        cache_get_value(i, "caller_number", caller, sizeof(caller));
        cache_get_value(i, "callee_number", callee, sizeof(callee));
        cache_get_value(i, "start_time", st, sizeof(st));

        new id = 0; sscanf(idStr, "%d", id);
        g_PhoneHistoryList[playerid][i] = id;
        g_PhoneHistoryCount[playerid]++;

        new line[128]; format(line, sizeof(line), "%s -> %s at %s\n", caller, callee, st);
        strcat(list, line);
    }

    ShowListDialog(playerid, DIALOG_PHONE_HISTORY, "Call history", list);
    return 1;
}

stock Dialog_ShowPhoneSettings(playerid)
{
    new mynum = Phone_GetPlayerNumberSync(playerid);
    new q[256]; mysql_format(mySQLconnection, q, sizeof(q), "SELECT state FROM phone_numbers WHERE phone_number = %d LIMIT 1", mynum);
    mysql_tquery(mySQLconnection, q, "Phone_OnSettingsLoaded", "i", playerid);
    return 1;
}

public Phone_OnSettingsLoaded(playerid)
{
    new rows = cache_num_rows();
    new stateStr[16] = "on";
    if(rows > 0) cache_get_value(0, "state", stateStr, sizeof(stateStr));

    new dialog[256]; format(dialog, sizeof(dialog), "Phone number: %d\nState: %s", Phone_GetPlayerNumberSync(playerid), stateStr);
    ShowPlayerDialog(playerid, DIALOG_PHONE_MENU, DIALOG_STYLE_MSGBOX, "Phone settings", dialog, "Toggle On/Off", "Close");
    return 1;
}
