new DeletePenalty = 0;

stock LoadGlobalPenalties(playerid)
{
	new rows, fields;
	mysql_query(mySQLconnection, sprintf("SELECT member_block_run, member_block_dmg, member_block_ooc, member_block_veh FROM ipb_members WHERE member_id = %d", pGlobal[playerid][glo_id]));
	cache_get_data(rows, fields);

	pGlobal[playerid][glo_run] = cache_get_row_int(0, 0);
	pGlobal[playerid][glo_dmg] = cache_get_row_int(0, 1);
	pGlobal[playerid][glo_ooc] = cache_get_row_int(0, 2);
	pGlobal[playerid][glo_veh] = cache_get_row_int(0, 3);

	if(pGlobal[playerid][glo_run] != 0 && pGlobal[playerid][glo_run] < gettime())
	{
		mysql_query(mySQLconnection, sprintf("UPDATE ipb_members SET member_block_run = 0 WHERE member_id = %d", pGlobal[playerid][glo_id]));
		SendClientMessage(playerid, COLOR_GOLD, "Czas twojej blokady biegania dobieg� ko�ca.");
		pGlobal[playerid][glo_run] = 0;
	}

	if(pGlobal[playerid][glo_dmg] != 0 && pGlobal[playerid][glo_dmg] < gettime())
	{
		mysql_query(mySQLconnection, sprintf("UPDATE ipb_members SET member_block_dmg = 0 WHERE member_id = %d", pGlobal[playerid][glo_id]));
		SendClientMessage(playerid, COLOR_GOLD, "Czas twojej blokady zadawania obra�e� dobieg� ko�ca.");
		pGlobal[playerid][glo_dmg] = 0;
	}

	if(pGlobal[playerid][glo_ooc] != 0 && pGlobal[playerid][glo_ooc] < gettime())
	{
		mysql_query(mySQLconnection, sprintf("UPDATE ipb_members SET member_block_ooc = 0 WHERE member_id = %d", pGlobal[playerid][glo_id]));
		SendClientMessage(playerid, COLOR_GOLD, "Czas twojej blokady czat�w OOC dobieg� ko�ca.");
		pGlobal[playerid][glo_ooc] = 0;
	}

	if(pGlobal[playerid][glo_veh] != 0 && pGlobal[playerid][glo_veh] < gettime())
	{
		mysql_query(mySQLconnection, sprintf("UPDATE ipb_members SET member_block_veh = 0 WHERE member_id = %d", pGlobal[playerid][glo_id]));
		SendClientMessage(playerid, COLOR_GOLD, "Czas twojej blokady prowadzenia pojazd�w dobieg� ko�ca.");
		pGlobal[playerid][glo_veh] = 0;
	}
	return 1;
}

stock AddPlayerPunishLog(playerid, giveplayer_id, punish_type, reason[], extraid = 0, time = 0)
{
	if(!pInfo[playerid][player_logged]) return 1;
    mysql_escape_string(reason, reason, mySQLconnection, 128);
	if(giveplayer_id == -1)
 	{
 	   	mysql_query(mySQLconnection, sprintf("INSERT INTO `ipb_penalties` (`penalty_user_char_id`, `penalty_user_global_id`, `penalty_admin_global_id`, `penalty_expire`, `penalty_type`, `penalty_dateline`, `penalty_reason`) VALUES ('%d', '%d', '-1', '0', '%d', %d, '%s')", pInfo[playerid][player_id], pGlobal[playerid][glo_id], punish_type, gettime(), reason));
	}
	else
	{
		if(extraid == 0)
		{
 			mysql_query(mySQLconnection, sprintf("INSERT INTO `ipb_penalties` (`penalty_user_char_id`, `penalty_user_global_id`, `penalty_admin_global_id`, `penalty_expire`, `penalty_type`, `penalty_dateline`, `penalty_reason`) VALUES ('%d', '%d', '%d', '%d', '%d', %d, '%s')", pInfo[playerid][player_id], pGlobal[playerid][glo_id], pGlobal[giveplayer_id][glo_id], time, punish_type, gettime(), reason));
 		}
 		else
 		{
 			mysql_query(mySQLconnection, sprintf("INSERT INTO `ipb_penalties` (`penalty_user_char_id`, `penalty_user_global_id`, `penalty_admin_global_id`, `penalty_expire`, `penalty_type`, `penalty_dateline`, `penalty_reason`, `penalty_extraid`) VALUES ('%d', '%d', '%d', '0', '%d', %d, '%s', '%d')", pInfo[playerid][player_id], pGlobal[playerid][glo_id], pGlobal[giveplayer_id][glo_id], punish_type, gettime(), reason, extraid));
 		}
	}
	return 1;
}

stock KickAc(playerid, giveplayer_id, reason[])
{
	if(playerid == INVALID_PLAYER_ID) return 1;
	if(giveplayer_id == INVALID_PLAYER_ID) return 1;
	new string[200], giver_name[24];
	if(giveplayer_id == -1) strmid(giver_name, "System", 0, 6, 24);
	else                    strmid(giver_name, pInfo[giveplayer_id][player_name], 0, strlen(pInfo[giveplayer_id][player_name]), 24);
	
	EscapePL(reason);
	
	format(string, sizeof(string), "~r~Kick~n~~w~Nadawca: %s~n~~w~Odbiorca: %s~n~~y~Powod: %s", giver_name, pInfo[playerid][player_name], reason);
	
	SendFormattedClientMessage(playerid, COLOR_DARK_RED, "> Otrzymujesz kar� (Kick) od %s. Pow�d %s.", giver_name, reason);

	TextDrawSetString(TextPenalty, string);

	TextDrawShowForAll(TextPenalty);

	Kick(playerid);
	AddPlayerPunishLog(playerid, giveplayer_id, PUNISH_KICK, reason);
	DeletePenalty = 8;
	return 1;
}

stock CarUnspawn(playerid, vehicleid, giveplayer_id, reason[])
{
	if(playerid == INVALID_PLAYER_ID) return 1;
	if(vehicleid == INVALID_VEHICLE_ID) return 1;
	if(giveplayer_id == INVALID_PLAYER_ID) return 1;
	new string[200], giver_name[24];
	if(giveplayer_id == -1) strmid(giver_name, "System", 0, 6, 24);
	else                    strmid(giver_name, pInfo[giveplayer_id][player_name], 0, strlen(pInfo[giveplayer_id][player_name]), 24);
	
	EscapePL(reason);
	
	format(string, sizeof(string), "~r~Vehicle UnSpawn~n~~w~Nadawca: %s~n~~w~Odbiorca: %s~n~~y~Powod: %s", giver_name, pInfo[playerid][player_name], reason);
	
	TextDrawSetString(TextPenalty, string);

	new uid = Vehicle[vehicleid][vehicle_uid];
	ClearAnimations(playerid);
	DeleteVehicle(vehicleid, false);
	LoadVehicle(sprintf("WHERE `vehicle_uid` = %d", uid), true);
	
	AddPlayerPunishLog(playerid, giveplayer_id, PUNISH_WARN, reason);
	DeletePenalty = 8;
	return 1;
}

stock Warn(playerid, giveplayer_id, reason[])
{
	if(playerid == INVALID_PLAYER_ID) return 1;
	if(giveplayer_id == INVALID_PLAYER_ID) return 1;
	new string[200], giver_name[24];
	if(giveplayer_id == -1) strmid(giver_name, "System", 0, 6, 24);
	else                    strmid(giver_name, pInfo[giveplayer_id][player_name], 0, strlen(pInfo[giveplayer_id][player_name]), 24);
	
	EscapePL(reason);
	
	format(string, sizeof(string), "~r~Warn~n~~w~Nadawca: %s~n~~w~Odbiorca: %s~n~~y~Powod: %s", giver_name, pInfo[playerid][player_name], reason);

	SendFormattedClientMessage(playerid, COLOR_DARK_RED, "> Otrzymujesz kar� (Warn) od %s. Pow�d %s.", giver_name, reason);
	
	TextDrawSetString(TextPenalty, string);

	TextDrawShowForAll(TextPenalty);

	AddPlayerPunishLog(playerid, giveplayer_id, PUNISH_WARN, reason);
	DeletePenalty = 8;
	return 1;
}

stock Block(playerid, giveplayer_id, reason[])
{
	if(playerid == INVALID_PLAYER_ID) return 1;
	if(giveplayer_id == INVALID_PLAYER_ID) return 1;
	new string[220], giver_name[24];
	if(giveplayer_id == -1) strmid(giver_name, "System", 0, 6, 24);
	else                    strmid(giver_name, pInfo[giveplayer_id][player_name], 0, strlen(pInfo[giveplayer_id][player_name]), 24);
	
	EscapePL(reason);
	
	format(string, sizeof(string), "~r~Blokada Postaci~n~~w~Nadawca: %s~n~~w~Odbiorca: %s~n~~y~Powod: %s", giver_name, pInfo[playerid][player_name], reason);
	
	SendFormattedClientMessage(playerid, COLOR_DARK_RED, "> Otrzymujesz kar� (Blokada postaci) od %s. Pow�d %s.", giver_name, reason);

	TextDrawSetString(TextPenalty, string);

	TextDrawShowForAll(TextPenalty);
	
	mysql_query(mySQLconnection, sprintf("UPDATE `ipb_characters` SET `char_active` = 1 WHERE `char_uid` = %d", pInfo[playerid][player_id]));
	DeletePenalty = 8;
	AddPlayerPunishLog(playerid, giveplayer_id, PUNISH_BLOCK, reason);
	Kick(playerid);
	return 1;
}

stock BlockVeh(playerid, giveplayer_id, time, reason[])
{
	if(playerid == INVALID_PLAYER_ID) return 1;
	if(giveplayer_id == INVALID_PLAYER_ID) return 1;
	new string[220], giver_name[24];
	if(giveplayer_id == -1) strmid(giver_name, "System", 0, 6, 24);
	else                    strmid(giver_name, pInfo[giveplayer_id][player_name], 0, strlen(pInfo[giveplayer_id][player_name]), 24);
	
	EscapePL(reason);

	if(time == 0)
	{
		if(pGlobal[playerid][glo_veh])
		{
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_members` SET `member_block_veh` = 0 WHERE `member_id` = %d", pGlobal[playerid][glo_id]));
			SendClientMessage(giveplayer_id, COLOR_GOLD, sprintf("Blokada prowadzenia pojazd�w gracza %s zosta�a pomyœlnie zdjêta.", pInfo[playerid][player_name]));
			SendClientMessage(playerid, COLOR_GOLD, sprintf("Administrator %s zdj�� twoj� blokade prowadzenia pojazd�w.", pInfo[giveplayer_id][player_name]));
			pGlobal[playerid][glo_veh] = 0;
		}
		return 1;
	}
	
	format(string, sizeof(string), "~r~Blokada prowadzenia pojazdow (%d dni)~n~~w~Nadawca: %s~n~~w~Odbiorca: %s~n~~y~Powod: %s", time,giver_name, pInfo[playerid][player_name], reason);

	SendFormattedClientMessage(playerid, COLOR_DARK_RED, "> Otrzymujesz kar� (Blokada pojazd�w) od %s. Pow�d %s.", giver_name, reason);

	TextDrawSetString(TextPenalty, string);

	TextDrawShowForAll(TextPenalty);


	DeletePenalty = 8;
	AddPlayerPunishLog(playerid, giveplayer_id, PUNISH_BLOCK_VEH, reason, 0, gettime() + time*86400);
	mysql_query(mySQLconnection, sprintf("UPDATE `ipb_members` SET `member_block_veh` = %d WHERE `member_id` = %d", gettime()+time*86400, pGlobal[playerid][glo_id]));
	pGlobal[playerid][glo_veh] = gettime() + time*86400;
	return 1;
}

stock BlockRun(playerid, giveplayer_id, time, reason[])
{
	if(playerid == INVALID_PLAYER_ID) return 1;
	if(giveplayer_id == INVALID_PLAYER_ID) return 1;
	new string[220], giver_name[24];
	if(giveplayer_id == -1) strmid(giver_name, "System", 0, 6, 24);
	else                    strmid(giver_name, pInfo[giveplayer_id][player_name], 0, strlen(pInfo[giveplayer_id][player_name]), 24);
	
	EscapePL(reason);

	if(time == 0)
	{
		if(pGlobal[playerid][glo_run])
		{
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_members` SET `member_block_run` = 0 WHERE `member_id` = %d", pGlobal[playerid][glo_id]));
			SendClientMessage(giveplayer_id, COLOR_GOLD, sprintf("Blokada biegania gracza %s zosta�a pomyœlnie zdjêta.", pInfo[playerid][player_name]));
			SendClientMessage(playerid, COLOR_GOLD, sprintf("Administrator %s zdj�� twoj� blokade biegania.", pInfo[giveplayer_id][player_name]));
			pGlobal[playerid][glo_run] = 0;
		}
		return 1;
	}
	
	format(string, sizeof(string), "~r~Blokada biegania (%d dni)~n~~w~Nadawca: %s~n~~w~Odbiorca: %s~n~~y~Powod: %s",time, giver_name, pInfo[playerid][player_name], reason);
	
	SendFormattedClientMessage(playerid, COLOR_DARK_RED, "> Otrzymujesz kar� (Blokada biegania) od %s. Pow�d %s.", giver_name, reason);

	TextDrawSetString(TextPenalty, string);

	TextDrawShowForAll(TextPenalty);

	DeletePenalty = 8;
	AddPlayerPunishLog(playerid, giveplayer_id, PUNISH_BLOCK_RUN, reason, 0, gettime() + time*86400);
	mysql_query(mySQLconnection, sprintf("UPDATE `ipb_members` SET `member_block_run` = %d WHERE `member_id` = %d", gettime()+time*86400, pGlobal[playerid][glo_id]));
	pGlobal[playerid][glo_run] = gettime() + time*86400;
	return 1;
}

stock BlockDmg(playerid, giveplayer_id, time, reason[])
{
	if(playerid == INVALID_PLAYER_ID) return 1;
	if(giveplayer_id == INVALID_PLAYER_ID) return 1;
	new string[220], giver_name[24];
	if(giveplayer_id == -1) strmid(giver_name, "System", 0, 6, 24);
	else                    strmid(giver_name, pInfo[giveplayer_id][player_name], 0, strlen(pInfo[giveplayer_id][player_name]), 24);
	
	EscapePL(reason);

	if(time == 0)
	{
		if(pGlobal[playerid][glo_dmg])
		{
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_members` SET `member_block_dmg` = 0 WHERE `member_id` = %d", pGlobal[playerid][glo_id]));
			SendClientMessage(giveplayer_id, COLOR_GOLD, sprintf("Blokada zadawania obra�e� %s zosta�a pomyœlnie zdjêta.", pInfo[playerid][player_name]));
			SendClientMessage(playerid, COLOR_GOLD, sprintf("Administrator %s zdj�� twoj� blokade zadawania obra�e�.", pInfo[giveplayer_id][player_name]));
			pGlobal[playerid][glo_dmg] = 0;
		}
		return 1;
	}
	
	format(string, sizeof(string), "~r~Blokada zabierania dmg (%d dni)~n~~w~Nadawca: %s~n~~w~Odbiorca: %s~n~~y~Powod: %s", time,giver_name, pInfo[playerid][player_name], reason);
	
	SendFormattedClientMessage(playerid, COLOR_DARK_RED, "> Otrzymujesz kar� (Blokada obra�e�) od %s. Pow�d %s.", giver_name, reason);

	TextDrawSetString(TextPenalty, string);

	TextDrawShowForAll(TextPenalty);

	DeletePenalty = 8;
	AddPlayerPunishLog(playerid, giveplayer_id, PUNISH_BLOCK_DMG, reason, 0, gettime() + time*86400);
	mysql_query(mySQLconnection, sprintf("UPDATE `ipb_members` SET `member_block_dmg` = %d WHERE `member_id` = %d", gettime()+time*86400, pGlobal[playerid][glo_id]));
	pGlobal[playerid][glo_dmg] = gettime() + time*86400;
	return 1;
}

stock BlockOoc(playerid, giveplayer_id, time, reason[])
{
	if(playerid == INVALID_PLAYER_ID) return 1;
	if(giveplayer_id == INVALID_PLAYER_ID) return 1;
	new string[220], giver_name[24];
	if(giveplayer_id == -1) strmid(giver_name, "System", 0, 6, 24);
	else                    strmid(giver_name, pInfo[giveplayer_id][player_name], 0, strlen(pInfo[giveplayer_id][player_name]), 24);
	
	EscapePL(reason);

	if(time == 0)
	{
		if(pGlobal[playerid][glo_ooc])
		{
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_members` SET `member_block_ooc` = 0 WHERE `member_id` = %d", pGlobal[playerid][glo_id]));
			SendClientMessage(giveplayer_id, COLOR_GOLD, sprintf("Blokada czatu OOC %s zosta�a pomyœlnie zdjêta.", pInfo[playerid][player_name]));
			SendClientMessage(playerid, COLOR_GOLD, sprintf("Administrator %s zdj�� twoj� blokade czatu OOC.", pInfo[giveplayer_id][player_name]));
			pGlobal[playerid][glo_ooc] = 0;
		}
		return 1;
	}
	
	format(string, sizeof(string), "~r~Blokada czatu OOC (%d dni)~n~~w~Nadawca: %s~n~~w~Odbiorca: %s~n~~y~Powod: %s", time,giver_name, pInfo[playerid][player_name], reason);
	
	TextDrawSetString(TextPenalty, string);

	SendFormattedClientMessage(playerid, COLOR_DARK_RED, "> Otrzymujesz kar� (Blokada OOC) od %s. Pow�d %s.", giver_name, reason);

	TextDrawShowForAll(TextPenalty);

	DeletePenalty = 8;
	AddPlayerPunishLog(playerid, giveplayer_id, PUNISH_BLOCK_OOC, reason, 0, gettime() + time*86400);
	mysql_query(mySQLconnection, sprintf("UPDATE `ipb_members` SET `member_block_ooc` = %d WHERE `member_id` = %d", gettime()+time*86400, pGlobal[playerid][glo_id]));
	pGlobal[playerid][glo_ooc] = gettime() + time*86400;
	return 1;
}

stock Score(playerid, giveplayer_id, score, reason[])
{
	if(playerid == INVALID_PLAYER_ID) return 1;
	if(giveplayer_id == INVALID_PLAYER_ID) return 1;
	new string[220], giver_name[24];
	if(giveplayer_id == -1) strmid(giver_name, "System", 0, 6, 24);
	else                    strmid(giver_name, pInfo[giveplayer_id][player_name], 0, strlen(pInfo[giveplayer_id][player_name]), 24);
	
	EscapePL(reason);

	if(score < 0)
	{
		format(string, sizeof(string), "~r~~h~Game Score (%d)~n~~w~Nadawca: %s~n~~w~Odbiorca: %s~n~~y~Powod: %s", score,giver_name, pInfo[playerid][player_name], reason);
	}
	else
	{
		format(string, sizeof(string), "~b~~h~~h~Game Score (%d)~n~~w~Nadawca: %s~n~~w~Odbiorca: %s~n~~y~Powod: %s", score,giver_name, pInfo[playerid][player_name], reason);
	}
	
	TextDrawSetString(TextPenalty, string);

	TextDrawShowForAll(TextPenalty);

	pGlobal[playerid][glo_score] += score;
	SetPlayerScore(playerid, pGlobal[playerid][glo_score]);

	AddPlayerPunishLog(playerid, giveplayer_id, PUNISH_GAME_SCORE, reason, score);
	
	mysql_query(mySQLconnection, sprintf("UPDATE `ipb_members` SET `member_game_points` = %d WHERE `member_id` = %d", pGlobal[playerid][glo_score], pGlobal[playerid][glo_id]));
	DeletePenalty = 8;
	return 1;
}

stock CharacterKill(playerid, giveplayer_id, reason[])
{
	if(playerid == INVALID_PLAYER_ID) return 1;
	if(giveplayer_id == INVALID_PLAYER_ID) return 1;
	new string[220], giver_name[24];
	if(giveplayer_id == -1) strmid(giver_name, "System", 0, 6, 24);
	else                    strmid(giver_name, pInfo[giveplayer_id][player_name], 0, strlen(pInfo[giveplayer_id][player_name]), 24);
	
	EscapePL(reason);
	
	format(string, sizeof(string), "~r~Blokada Postaci~n~~w~Nadawca: %s~n~~w~Odbiorca: %s~n~~y~Powod: %s", giver_name, pInfo[playerid][player_name], reason);
	
	SendFormattedClientMessage(playerid, COLOR_DARK_RED, "> Otrzymujesz kar� (Character kill) od %s. Pow�d %s.", giver_name, reason);

	TextDrawSetString(TextPenalty, string);

	TextDrawShowForAll(TextPenalty);

	mysql_query(mySQLconnection, sprintf("UPDATE `ipb_characters` SET `char_active` = 1 WHERE `char_uid` = %d", pInfo[playerid][player_id]));
	DeletePenalty = 8;

	for(new item;item<MAX_PLAYER_ITEMS;item++)
	{
		if(PlayerItem[playerid][item][player_item_uid] > 0 && !PlayerItem[playerid][item][player_item_used])
		{
			Item_Drop(item, playerid, false);
		}
	}

	new Float:x, Float:y, Float:z;
	GetPlayerPos(playerid, x, y, z);
	z = floatadd(z, -0.973339);

	AddPlayerPunishLog(playerid, giveplayer_id, PUNISH_BLOCK, reason);

	new i_name[40];
    format(i_name, sizeof(i_name), "%s", pInfo[playerid][player_name]);

	new iid = Item_Create(ITEM_OWNER_TYPE_PLAYER, playerid, ITEM_TYPE_CORPSE, 19944, pInfo[playerid][player_killer], pInfo[playerid][player_death], i_name);
	mysql_query(mySQLconnection, sprintf("UPDATE `ipb_items` SET `item_ownertype` = '1', `item_world` = '%d', `item_posx` = '%f', `item_posy` = '%f', `item_posz` = '%f' WHERE `item_uid` = %d", GetPlayerVirtualWorld(playerid), x, y, z, PlayerItem[playerid][iid][player_item_uid]));
	
	new uid  = PlayerItem[playerid][iid][player_item_uid];
	DeleteItem(iid, false, playerid);

	LoadItem(sprintf("WHERE `item_uid` = %d", uid), false, true);

	Kick(playerid);
	return 1;
}

stock BanAc(playerid, giveplayer_id, reason[])
{
	if(playerid == INVALID_PLAYER_ID) return 1;
	if(giveplayer_id == INVALID_PLAYER_ID) return 1;
	new string[200], giver_name[24];
	if(giveplayer_id == -1) strmid(giver_name, "System", 0, 6, 24);
	else                    strmid(giver_name, pInfo[giveplayer_id][player_name], 0, strlen(pInfo[giveplayer_id][player_name]), 24);
	
	EscapePL(reason);
	
	format(string, sizeof(string), "~r~Ban~n~~w~Nadawca: %s~n~~w~Odbiorca: %s~n~~y~Powod: %s", giver_name, pInfo[playerid][player_name], reason);
	
	SendFormattedClientMessage(playerid, COLOR_DARK_RED, "> Otrzymujesz kar� (Ban) od %s. Pow�d %s.", giver_name, reason);

	TextDrawSetString(TextPenalty, string);

	TextDrawShowForAll(TextPenalty);

	mysql_query(mySQLconnection, sprintf("UPDATE `ipb_members` SET `member_game_ban` = 1 WHERE `member_id` = %d", pGlobal[playerid][glo_id]));
	DeletePenalty = 8;

	AddPlayerPunishLog(playerid, giveplayer_id, PUNISH_BAN, reason);
	Kick(playerid);
	return 1;
}

stock AdminJail(playerid, giveplayer_id, reason[], time)
{
	if(playerid == INVALID_PLAYER_ID) return 1;
	if(giveplayer_id == INVALID_PLAYER_ID) return 1;
	new string[200], giver_name[24];
	if(giveplayer_id == -1) strmid(giver_name, "System", 0, 6, 24);
	else                    strmid(giver_name, pInfo[giveplayer_id][player_name], 0, strlen(pInfo[giveplayer_id][player_name]), 24);
	
	EscapePL(reason);

	if(time==0)
	{
	    SendFormattedClientMessage(playerid, COLOR_LIGHTER_RED, "%s wypu�ci� Ci� z Admin Jaila z powodem: %s.", giver_name, reason);
	    SendFormattedClientMessage(giveplayer_id, COLOR_LIGHTER_RED, "Wypu�ci�e� %s z AJ.", pInfo[playerid][player_name]);
	    SetPlayerPos(playerid, 1721.98, -1719.12, 13.5349);
	    mysql_query(mySQLconnection, sprintf("UPDATE ipb_characters SET char_aj = 0 WHERE char_uid = %d", pInfo[playerid][player_id]));
	    pInfo[playerid][player_aj] = 0;
	    SetPlayerVirtualWorld(playerid, 0);
		return 1;
	}
	
	format(string, sizeof(string), "~r~AdminJail (%d min)~n~~w~Nadawca: %s~n~~w~Odbiorca: %s~n~~y~Powod: %s",time,giver_name, pInfo[playerid][player_name], reason);
	
	TextDrawSetString(TextPenalty, string);

	TextDrawShowForAll(TextPenalty);

	SendFormattedClientMessage(playerid, COLOR_DARK_RED, "> Otrzymujesz kar� (AdminJail) od %s. Pow�d %s.", giver_name, reason);
	
	pInfo[playerid][player_aj]=time;

	mysql_query(mySQLconnection, sprintf("UPDATE ipb_characters SET char_aj = %d WHERE char_uid = %d", pInfo[playerid][player_aj], pInfo[playerid][player_id]));
	SetPlayerPos(playerid, 154.0880,-1951.6383,47.8750);
	SetPlayerVirtualWorld(playerid, pInfo[playerid][player_id]);
	AddPlayerPunishLog(playerid, giveplayer_id, PUNISH_AJ, reason);
	DeletePenalty = 8;
	return 1;
}

stock EscapePL(name[])
{
    for(new i = 0; name[i] != 0; i++)
    {
        // Check for Polish characters using their byte values
        // Format: 0xC5 0x9B for 'ś', etc.
        
        // Lowercase Polish characters
        if(name[i] == 0xC5 && name[i+1] == 0x9B) { name[i] = 's'; name[i+1] = ' '; } // ś
        else if(name[i] == 0xC4 && name[i+1] == 0x99) { name[i] = 'e'; name[i+1] = ' '; } // ę
        else if(name[i] == 0xC3 && name[i+1] == 0xB3) { name[i] = 'o'; name[i+1] = ' '; } // ó
        else if(name[i] == 0xC4 && name[i+1] == 0x85) { name[i] = 'a'; name[i+1] = ' '; } // ą
        else if(name[i] == 0xC5 && name[i+1] == 0x82) { name[i] = 'l'; name[i+1] = ' '; } // ł
        else if(name[i] == 0xC5 && name[i+1] == 0xBA) { name[i] = 'z'; name[i+1] = ' '; } // ź
        else if(name[i] == 0xC5 && name[i+1] == 0xBC) { name[i] = 'z'; name[i+1] = ' '; } // ż
        else if(name[i] == 0xC4 && name[i+1] == 0x87) { name[i] = 'c'; name[i+1] = ' '; } // ć
        else if(name[i] == 0xC5 && name[i+1] == 0x84) { name[i] = 'n'; name[i+1] = ' '; } // ń
        
        // Uppercase Polish characters
        else if(name[i] == 0xC5 && name[i+1] == 0x9A) { name[i] = 'S'; name[i+1] = ' '; } // Ś
        else if(name[i] == 0xC4 && name[i+1] == 0x98) { name[i] = 'E'; name[i+1] = ' '; } // Ę
        else if(name[i] == 0xC3 && name[i+1] == 0x93) { name[i] = 'O'; name[i+1] = ' '; } // Ó
        else if(name[i] == 0xC4 && name[i+1] == 0x84) { name[i] = 'A'; name[i+1] = ' '; } // Ą
        else if(name[i] == 0xC5 && name[i+1] == 0x81) { name[i] = 'L'; name[i+1] = ' '; } // Ł
        else if(name[i] == 0xC5 && name[i+1] == 0xB9) { name[i] = 'Z'; name[i+1] = ' '; } // Ź
        else if(name[i] == 0xC5 && name[i+1] == 0xBB) { name[i] = 'Z'; name[i+1] = ' '; } // Ż
        else if(name[i] == 0xC4 && name[i+1] == 0x86) { name[i] = 'C'; name[i+1] = ' '; } // Ć
        else if(name[i] == 0xC5 && name[i+1] == 0x83) { name[i] = 'N'; name[i+1] = ' '; } // Ń
    }
    
    // Remove spaces created by replacement
    new j = 0;
    for(new i = 0; name[i] != 0; i++)
    {
        if(name[i] != ' ' || (i > 0 && name[i-1] != ' '))
        {
            name[j++] = name[i];
        }
    }
    name[j] = 0;
}
