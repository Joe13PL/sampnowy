// Wyciszenie warningów tag mismatch
#pragma warning disable 213

#if defined _groups_included
    #endinput
#endif
#define _groups_included






enum E_GROUP_TYPE
{
    GROUP_TYPE_NONE,
    GROUP_TYPE_POLICE,
    GROUP_TYPE_MEDIC,
    GROUP_TYPE_GOVERNMENT,
    GROUP_TYPE_GANG,
    GROUP_TYPE_MAFIA,
    GROUP_TYPE_NEWS,
    GROUP_TYPE_TRANSPORT,
    GROUP_TYPE_MECHANIC,
    GROUP_TYPE_BUSINESS,
    GROUP_TYPE_OTHER
};


#define GROUP_PERM_NONE             0
#define GROUP_PERM_INVITE           (1 << 0)
#define GROUP_PERM_KICK             (1 << 1)
#define GROUP_PERM_RANK             (1 << 2)
#define GROUP_PERM_BANK             (1 << 3)
#define GROUP_PERM_VEHICLES         (1 << 4)
#define GROUP_PERM_DOORS            (1 << 5)
#define GROUP_PERM_LEADER           (1 << 6)
#define GROUP_PERM_DEPUTY           (1 << 7)
#define GROUP_PERM_DUTY             (1 << 8)












stock Groups_Load()
{
    new query[256];
    mysql_format(mySQLconnection, query, sizeof(query),
           "SELECT * FROM `groups` ORDER BY `group_id` ASC"
    );

    mysql_tquery(mySQLconnection, query, "Groups_OnLoad");
}

forward Groups_OnLoad();
public Groups_OnLoad()
{
    new rows = cache_num_rows();
    new loaded = 0;

    for(new i = 0; i < rows; i++)
    {
        new groupId;
        cache_get_value_int(i, "group_id", groupId);

        if(groupId <= 0 || groupId >= MAX_GROUPS)
        {
            continue;
        }

        gInfo[groupId][group_created] = true;

        cache_get_value(i, "group_name", gInfo[groupId][group_name], 64);
        cache_get_value(i, "group_tag", gInfo[groupId][group_tag], 8);
        cache_get_value_int(i, "group_type", gInfo[groupId][group_type]);
        cache_get_value_int(i, "group_color", gInfo[groupId][group_color]);
        cache_get_value_int(i, "group_bank", gInfo[groupId][group_bank]);
        cache_get_value_int(i, "group_leader", gInfo[groupId][group_leader]);
        cache_get_value_int(i, "group_max_members", gInfo[groupId][group_max_members]);
        cache_get_value_int(i, "group_flags", gInfo[groupId][group_flags]);


        cache_get_value(i, "group_rank0", gInfo[groupId][group_rank0], 32);
        cache_get_value(i, "group_rank1", gInfo[groupId][group_rank1], 32);
        cache_get_value(i, "group_rank2", gInfo[groupId][group_rank2], 32);
        cache_get_value(i, "group_rank3", gInfo[groupId][group_rank3], 32);
        cache_get_value(i, "group_rank4", gInfo[groupId][group_rank4], 32);
        cache_get_value(i, "group_rank5", gInfo[groupId][group_rank5], 32);
        cache_get_value(i, "group_rank6", gInfo[groupId][group_rank6], 32);
        cache_get_value(i, "group_rank7", gInfo[groupId][group_rank7], 32);
        cache_get_value(i, "group_rank8", gInfo[groupId][group_rank8], 32);
        cache_get_value(i, "group_rank9", gInfo[groupId][group_rank9], 32);

        Iter_Add(Groups, groupId);
        loaded++;
    }

    printf("[Groups] Wczytano %d grup", loaded);
    return 1;
}


stock Group_Save(groupId)
{
    if(groupId <= 0 || groupId >= MAX_GROUPS || !gInfo[groupId][group_created])
    {
        return 0;
    }

    new query[2048];
    new escapedName[129], escapedTag[17];
    new escapedRank[10][65];

    mysql_escape_string(gInfo[groupId][group_name], escapedName, mySQLconnection, sizeof(escapedName));
    mysql_escape_string(gInfo[groupId][group_tag], escapedTag, mySQLconnection, sizeof(escapedTag));

    mysql_escape_string(gInfo[groupId][group_rank0], escapedRank[0], mySQLconnection, sizeof(escapedRank[]));
    mysql_escape_string(gInfo[groupId][group_rank1], escapedRank[1], mySQLconnection, sizeof(escapedRank[]));
    mysql_escape_string(gInfo[groupId][group_rank2], escapedRank[2], mySQLconnection, sizeof(escapedRank[]));
    mysql_escape_string(gInfo[groupId][group_rank3], escapedRank[3], mySQLconnection, sizeof(escapedRank[]));
    mysql_escape_string(gInfo[groupId][group_rank4], escapedRank[4], mySQLconnection, sizeof(escapedRank[]));
    mysql_escape_string(gInfo[groupId][group_rank5], escapedRank[5], mySQLconnection, sizeof(escapedRank[]));
    mysql_escape_string(gInfo[groupId][group_rank6], escapedRank[6], mySQLconnection, sizeof(escapedRank[]));
    mysql_escape_string(gInfo[groupId][group_rank7], escapedRank[7], mySQLconnection, sizeof(escapedRank[]));
    mysql_escape_string(gInfo[groupId][group_rank8], escapedRank[8], mySQLconnection, sizeof(escapedRank[]));
    mysql_escape_string(gInfo[groupId][group_rank9], escapedRank[9], mySQLconnection, sizeof(escapedRank[]));

    mysql_format(mySQLconnection, query, sizeof(query),
        "UPDATE `groups` SET `group_name` = '%s', `group_tag` = '%s', `group_type` = %d, `group_color` = %d, `group_bank` = %d, `group_leader` = %d, `group_max_members` = %d, `group_flags` = %d, \
         `group_rank0` = '%s', `group_rank1` = '%s', `group_rank2` = '%s', `group_rank3` = '%s', `group_rank4` = '%s', \
         `group_rank5` = '%s', `group_rank6` = '%s', `group_rank7` = '%s', `group_rank8` = '%s', `group_rank9` = '%s' \
         WHERE `group_id` = %d",
        escapedName, escapedTag, gInfo[groupId][group_type],
        gInfo[groupId][group_color], gInfo[groupId][group_bank],
        gInfo[groupId][group_leader], gInfo[groupId][group_max_members],
        gInfo[groupId][group_flags],
        escapedRank[0], escapedRank[1], escapedRank[2], escapedRank[3], escapedRank[4],
        escapedRank[5], escapedRank[6], escapedRank[7], escapedRank[8], escapedRank[9],
        groupId
    );

    mysql_tquery(mySQLconnection, query);
    return 1;
}






stock Player_GetGroupSlot(playerid, groupId)
{
    if(!IsValidPlayerId(playerid) || !pInfo[playerid][player_logged])
    {
        return -1;
    }

    for(new i = 0; i < MAX_PLAYER_GROUPS; i++)
    {
        if(pInfo[playerid][player_group][i] == groupId)
        {
            return i;
        }
    }

    return -1;
}


stock bool:Player_IsInGroup(playerid, groupId)
{
    return Player_GetGroupSlot(playerid, groupId) != -1;
}


stock Player_IsInGroupType(playerid, E_GROUP_TYPE:groupType)
{
    if(!IsValidPlayerId(playerid))
    {
        return 0;
    }

    for(new i = 0; i < MAX_PLAYER_GROUPS; i++)
    {
        new gid = pInfo[playerid][player_group][i];

        if(gid > 0 && gid < MAX_GROUPS && gInfo[gid][group_type] == _:groupType)
        {
            return gid;
        }
    }

    return 0;
}


stock Player_GetGroupPermissions(playerid, groupId)
{
    new slot = Player_GetGroupSlot(playerid, groupId);

    if(slot == -1)
    {
        return GROUP_PERM_NONE;
    }

    return pInfo[playerid][player_group_perm][slot];
}


stock bool:Player_HasGroupPermission(playerid, groupId, permission)
{
    new perms = Player_GetGroupPermissions(playerid, groupId);


    if(perms & GROUP_PERM_LEADER)
    {
        return true;
    }

    return (perms & permission) != 0;
}


stock Player_GetGroupRank(playerid, groupId)
{
    new slot = Player_GetGroupSlot(playerid, groupId);

    if(slot == -1)
    {
        return -1;
    }


    new perms = pInfo[playerid][player_group_perm][slot];

    if(perms & GROUP_PERM_LEADER) return 9;
    if(perms & GROUP_PERM_DEPUTY) return 8;


    new rank = 0;
    for(new i = 0; i < 8; i++)
    {
        if(perms & (1 << i)) rank++;
    }

    return clamp(rank, 0, 7);
}


stock Player_GetGroupRankName(playerid, groupId, output[], size = 32)
{
    new rank = Player_GetGroupRank(playerid, groupId);

    if(rank == -1)
    {
        strcopy(output, "Brak", size);
        return 0;
    }

    switch(rank)
    {
        case 0: strcopy(output, gInfo[groupId][group_rank0], size);
        case 1: strcopy(output, gInfo[groupId][group_rank1], size);
        case 2: strcopy(output, gInfo[groupId][group_rank2], size);
        case 3: strcopy(output, gInfo[groupId][group_rank3], size);
        case 4: strcopy(output, gInfo[groupId][group_rank4], size);
        case 5: strcopy(output, gInfo[groupId][group_rank5], size);
        case 6: strcopy(output, gInfo[groupId][group_rank6], size);
        case 7: strcopy(output, gInfo[groupId][group_rank7], size);
        case 8: strcopy(output, gInfo[groupId][group_rank8], size);
        case 9: strcopy(output, gInfo[groupId][group_rank9], size);
        default: strcopy(output, "Nieznana", size);
    }
    return 1;
}






stock Group_AddMember(playerid, groupId, permissions = GROUP_PERM_NONE, salary = 0)
{
    if(!IsValidPlayerId(playerid) || !pInfo[playerid][player_logged])
    {
        return 0;
    }

    if(groupId <= 0 || groupId >= MAX_GROUPS || !gInfo[groupId][group_created])
    {
        return 0;
    }


    new freeSlot = -1;
    for(new i = 0; i < MAX_PLAYER_GROUPS; i++)
    {
        if(pInfo[playerid][player_group][i] == 0)
        {
            freeSlot = i;
            break;
        }
    }

    if(freeSlot == -1)
    {
        return 0;
    }


    if(Player_IsInGroup(playerid, groupId))
    {
        return 0;
    }


    pInfo[playerid][player_group][freeSlot] = groupId;
    pInfo[playerid][player_group_perm][freeSlot] = permissions;
    pInfo[playerid][player_group_salary][freeSlot] = salary;


    Player_MarkDataModified(playerid);


    new msg[128];
    format(msg, sizeof(msg), "You are join group {%06x}%s{FFFFFF}.",
        gInfo[groupId][group_color] >>> 8, gInfo[groupId][group_name]);
    SendClientMessage(playerid, COLOR_GREEN, msg);

    return 1;
}


stock Group_RemoveMember(playerid, groupId)
{
    new slot = Player_GetGroupSlot(playerid, groupId);

    if(slot == -1)
    {
        return 0;
    }


    pInfo[playerid][player_group][slot] = 0;
    pInfo[playerid][player_group_perm][slot] = 0;
    pInfo[playerid][player_group_salary][slot] = 0;


    Player_MarkDataModified(playerid);


    new msg[128];
    format(msg, sizeof(msg), "You are leaving group {%06x}%s{FFFFFF}.",
        gInfo[groupId][group_color] >>> 8, gInfo[groupId][group_name]);
    SendClientMessage(playerid, COLOR_YELLOW, msg);

    return 1;
}


stock Group_SetMemberPermissions(playerid, groupId, permissions)
{
    new slot = Player_GetGroupSlot(playerid, groupId);

    if(slot == -1)
    {
        return 0;
    }

    pInfo[playerid][player_group_perm][slot] = permissions;
    Player_MarkDataModified(playerid);

    return 1;
}


stock Group_SetMemberSalary(playerid, groupId, salary)
{
    new slot = Player_GetGroupSlot(playerid, groupId);

    if(slot == -1)
    {
        return 0;
    }

    pInfo[playerid][player_group_salary][slot] = salary;
    Player_MarkDataModified(playerid);

    return 1;
}






stock Group_GetBank(groupId)
{
    if(groupId <= 0 || groupId >= MAX_GROUPS || !gInfo[groupId][group_created])
    {
        return 0;
    }

    return gInfo[groupId][group_bank];
}


stock bool:Group_GiveMoney(groupId, amount)
{
    if(groupId <= 0 || groupId >= MAX_GROUPS || !gInfo[groupId][group_created])
    {
        return false;
    }

    new newBalance = gInfo[groupId][group_bank] + amount;

    if(newBalance < 0)
    {
        return false;
    }

    gInfo[groupId][group_bank] = newBalance;
    Group_Save(groupId);

    return true;
}


stock Group_PaySalaries(groupId)
{
    if(groupId <= 0 || groupId >= MAX_GROUPS || !gInfo[groupId][group_created])
    {
        return 0;
    }

    new paid = 0;
    new totalCost = 0;


    foreach(new playerid : Player)
    {
        new slot = Player_GetGroupSlot(playerid, groupId);

        if(slot != -1)
        {
            totalCost += pInfo[playerid][player_group_salary][slot];
        }
    }


    if(totalCost > gInfo[groupId][group_bank])
    {

        foreach(new playerid : Player)
        {
            if(Player_HasGroupPermission(playerid, groupId, GROUP_PERM_LEADER))
            {
                SendClientMessage(playerid, COLOR_RED,
                    "[GROUP] Not enough funds in the group bank to pay salaries!");
            }
        }
        return 0;
    }


    foreach(new playerid : Player)
    {
        new slot = Player_GetGroupSlot(playerid, groupId);

        if(slot != -1)
        {
            new salary = pInfo[playerid][player_group_salary][slot];

            if(salary > 0)
            {
                Player_GiveMoney(playerid, salary);
                gInfo[groupId][group_bank] -= salary;
                paid++;

                new msg[128];
                format(msg, sizeof(msg),
                    "You have received a salary of %s$ from group %s.",
                    FormatMoney(salary), gInfo[groupId][group_name]);
                SendClientMessage(playerid, COLOR_GREEN, msg);
            }
        }
    }

    if(paid > 0)
    {
        Group_Save(groupId);
    }

    return paid;
}






stock Group_SendMessage(groupId, const message[], color = COLOR_WHITE)
{
    if(groupId <= 0 || groupId >= MAX_GROUPS || !gInfo[groupId][group_created])
    {
        return 0;
    }

    new sent = 0;

    foreach(new playerid : Player)
    {
        if(Player_IsInGroup(playerid, groupId))
        {
            SendClientMessage(playerid, color, message);
            sent++;
        }
    }

    return sent;
}


stock Group_Chat(playerid, groupId, const message[])
{
    if(!Player_IsInGroup(playerid, groupId))
    {
        return 0;
    }

    new rankName[32];
    Player_GetGroupRankName(playerid, groupId, rankName);

    new formatted[256];
    format(formatted, sizeof(formatted),
        "(( [%s] %s %s: %s ))",
        gInfo[groupId][group_tag], rankName,
        pInfo[playerid][player_name], message);

    Group_SendMessage(groupId, formatted, gInfo[groupId][group_color]);

    return 1;
}






stock Group_GetName(groupId, output[], size = 64)
{
    if(groupId <= 0 || groupId >= MAX_GROUPS || !gInfo[groupId][group_created])
    {
        strcopy(output, "None", size);
        return 0;
    }

    strcopy(output, gInfo[groupId][group_name], size);
    return 1;
}


stock Group_GetOnlineMembers(groupId)
{
    if(groupId <= 0 || groupId >= MAX_GROUPS || !gInfo[groupId][group_created])
    {
        return 0;
    }

    new count = 0;

    foreach(new playerid : Player)
    {
        if(Player_IsInGroup(playerid, groupId))
        {
            count++;
        }
    }

    return count;
}


stock bool:Group_Exists(groupId)
{
    return (groupId > 0 && groupId < MAX_GROUPS && gInfo[groupId][group_created]);
}

/**
 * Znajduje wolny slot na grupe
 */
stock Group_FindFreeSlot()
{
    for(new i = 1; i < MAX_GROUPS; i++)
    {
        if(!gInfo[i][group_created])
        {
            return i;
        }
    }
    return -1;
}






