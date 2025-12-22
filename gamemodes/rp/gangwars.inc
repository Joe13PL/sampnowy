stock GangWar_Init(playerid, attacker, defender, warea)
{
	new warid = Iter_Free(GangWars);

	GangWar[warid][war_attacker] = Group[attacker][group_uid];
	GangWar[warid][war_defender] = Group[defender][group_uid];
	GangWar[warid][war_def_gid] = defender;
	GangWar[warid][war_at_gid] = attacker;
	GangWar[warid][war_start] = gettime();
	GangWar[warid][war_end] = gettime()+180; //gettime() + 24 * 3600;
	GangWar[warid][war_area] = Area[warea][area_uid];
	GangWar[warid][war_def_points] = 0;
	GangWar[warid][war_attack_points] = 0;

	Area[warea][area_attacked] = 1;
	
	mysql_query(mySQLconnection, sprintf("INSERT INTO ipb_gangwars (war_attacker, war_defender, war_area, war_start, war_end) VALUES (%d, %d, %d, %d, %d)", GangWar[warid][war_attacker], GangWar[warid][war_defender], GangWar[warid][war_area], GangWar[warid][war_start], GangWar[warid][war_end]));
	Iter_Add(GangWars, warid);

	mysql_query(mySQLconnection, sprintf("UPDATE ipb_areas SET area_attacked = %d WHERE area_uid = %d", Area[warea][area_attacked], Area[warea][area_uid]));

	SendGuiInformation(playerid, "Informacja", sprintf("Zaatakowa³eœ strefê grupy %s.\nOd tego momentu system bêdzie odlicza³ 24h. Aby wygraæ, musisz zdobyæ wiêcej punktów od broni¹cego.", Group[defender][group_name]));

	foreach(new targetid: Player)
	{
		if(GetPlayerGroupSlot(targetid, defender) != -1)
		{
			SendClientMessage(targetid, COLOR_GOLD, sprintf("Strefa twojej grupy (%s) zosta³a zaatakowana! Do broni!", Group[GangWar[warid][war_def_gid]][group_name]));
			GangZoneFlashForPlayer(targetid, Area[warea][area_zone], 0xFF3C3C80);
		}
	}
}

stock LoadGangWars()
{
	Code_ExTimer_Begin(LoadGangWars);
	new rows, fields;
	mysql_query(mySQLconnection, "SELECT * FROM ipb_gangwars");  
	cache_get_data(rows, fields);

	for(new row = 0; row != rows; row++)
	{
		new i = Iter_Free(GangWars);

		GangWar[i][war_attacker] = cache_get_row_int(row, 1);
		GangWar[i][war_defender] = cache_get_row_int(row, 2);

		GangWar[i][war_area] = cache_get_row_int(row, 3);
		
		GangWar[i][war_start] = cache_get_row_int(row, 4);
		GangWar[i][war_end] = cache_get_row_int(row, 5);
		GangWar[i][war_def_points] = cache_get_row_int(row, 6);
		GangWar[i][war_attack_points] = cache_get_row_int(row, 7);
		
		GangWar[i][war_at_gid] = GetGroupByUid(GangWar[i][war_attacker]);
		GangWar[i][war_def_gid] = GetGroupByUid(GangWar[i][war_defender]);

		if(GangWar[i][war_at_gid] == -1 || GangWar[i][war_def_gid] == -1) continue;
		
		Iter_Add(GangWars, i);
	}
	
	printf("Wczytano gangwars [wczytanych: %d] [czas: %d ms]", rows, Code_ExTimer_End(LoadGangWars));
}

stock SetWarScore(playerid, warid, score)
{
	if(GetPlayerGroupSlot(playerid, GangWar[warid][war_def_gid]) != -1) GangWar[warid][war_def_points] += score;
	if(GetPlayerGroupSlot(playerid, GangWar[warid][war_at_gid]) != -1)  GangWar[warid][war_attack_points] += score;
}

stock CheckWarGroups(killer, victim)
{
	foreach(new warid: GangWars)
	{
		if(GetPlayerGroupSlot(killer, GangWar[warid][war_def_gid]) != -1 && GetPlayerGroupSlot(victim, GangWar[warid][war_at_gid]) != -1) return warid;
		if(GetPlayerGroupSlot(killer, GangWar[warid][war_at_gid]) != -1 && GetPlayerGroupSlot(victim, GangWar[warid][war_def_gid]) != -1) return warid;
	}

	return 0;
}

stock IsGroupAtackedByGroup(defender, atacker)
{
	foreach(new warid: GangWars)
	{
		if(GangWar[warid][war_attacker] == Group[atacker][group_uid] && GangWar[warid][war_defender] == Group[defender][group_uid]) return 1;
	}
	
	return 0;
}

stock GroupVsGroupCheck(areaowner, atacker)
{
	foreach(new warid: GangWars)
	{
		if(GangWar[warid][war_attacker] == Group[areaowner][group_uid] && GangWar[warid][war_defender] == Group[atacker][group_uid] ) return 1;
	}
	
	return 0;
}

stock GangWarEnd(gangwar_id)
{
	new attacker = GangWar[gangwar_id][war_at_gid];
	new defender = GangWar[gangwar_id][war_def_gid];
	new winner, looser;

	if(GangWar[gangwar_id][war_def_points] < GangWar[gangwar_id][war_attack_points])
	{
		foreach(new playerid: Player)
		{
			if(GetPlayerGroupSlot(playerid, attacker) != -1)
			{
				SendClientMessage(playerid, COLOR_GOLD, sprintf("Wojna pomiêdzy twoj¹ grup¹ %s oraz %s dobieg³a koñca. Uda³o wam siê wygraæ i przej¹æ strefê!", Group[attacker][group_name], Group[defender][group_name]));
			}
			else if(GetPlayerGroupSlot(playerid, defender) != -1)
			{
				SendClientMessage(playerid, COLOR_GOLD, sprintf("Wojna pomiêdzy twoj¹ grup¹ %s oraz %s dobieg³a koñca. Niestety, atakuj¹cy zdoby³ wieksz¹ iloœæ punktów.", Group[defender][group_name], Group[attacker][group_name]));
			}
		}
		winner = attacker;
		looser = defender;
		printf("[GW] Grupa UID %d wygra³a wojne o strefe %d przeciwko grupie %d (score: %d/%d).", GangWar[gangwar_id][war_attacker], GangWar[gangwar_id][war_area], GangWar[gangwar_id][war_defender], GangWar[gangwar_id][war_attack_points], GangWar[gangwar_id][war_def_points]);
	}
	else
	{
		foreach(new playerid: Player)
		{
			if(GetPlayerGroupSlot(playerid, attacker) != -1)
			{
				if(GangWar[gangwar_id][war_def_points] == GangWar[gangwar_id][war_attack_points])
				{
					SendClientMessage(playerid, COLOR_GOLD, sprintf("Wojna pomiêdzy twoj¹ grup¹ %s oraz %s dobieg³a koñca. Niestety, dosz³o do remisu.", Group[attacker][group_name], Group[defender][group_name]));
				}
				else
				{
					SendClientMessage(playerid, COLOR_GOLD, sprintf("Wojna pomiêdzy twoj¹ grup¹ %s oraz %s dobieg³a koñca. Niestety, obroñca zdoby³ wiêksz¹ iloœæ punktów.", Group[attacker][group_name], Group[defender][group_name]));
				}
			}
			else if(GetPlayerGroupSlot(playerid, defender) != -1)
			{
				SendClientMessage(playerid, COLOR_GOLD, sprintf("Wojna pomiêdzy twoj¹ grup¹ %s oraz %s dobieg³a koñca. Uda³o wam siê wygraæ i obroniæ strefê.", Group[defender][group_name], Group[attacker][group_name]));
			}
		}
		winner = defender;
		looser = attacker;
		printf("[GW] Grupa UID %d wygra³a wojne o strefe %d przeciwko grupie %d (score: %d/%d).", GangWar[gangwar_id][war_defender], GangWar[gangwar_id][war_area], GangWar[gangwar_id][war_attacker], GangWar[gangwar_id][war_def_points], GangWar[gangwar_id][war_attack_points]);
	}
	
	new a_id = GetAreaByUid(GangWar[gangwar_id][war_area]);
	if(a_id != -1)
	{
		Area[a_id][area_owner] = Group[winner][group_uid];
		Area[a_id][area_attacked] = 0;
		mysql_query(mySQLconnection, sprintf("UPDATE ipb_areas SET area_owner = %d, area_attacked = %d WHERE area_uid = %d", Area[a_id][area_owner], Area[a_id][area_attacked], Area[a_id][area_uid]));
	}

	AddBonusProduct(winner);
	RemoveBonusProduct(looser);
	
	mysql_query(mySQLconnection, sprintf("DELETE FROM ipb_gangwars WHERE war_attacker = %d AND war_defender = %d AND war_area = %d", GangWar[gangwar_id][war_attacker], GangWar[gangwar_id][war_defender], GangWar[gangwar_id][war_area]));
}

stock GetGroupAttackers(gid, &firstgroup, &secondgroup, &thirdgroup)
{
	new attackers_count;

	foreach(new warid: GangWars)
	{
		if(GangWar[warid][war_defender] == gid)
		{
			attackers_count++;

			if(firstgroup == 0)
			{
				firstgroup = GangWar[warid][war_at_gid];
				continue;
			} 

			else if(secondgroup == 0)
			{
				secondgroup = GangWar[warid][war_at_gid];
				continue;
			}

			else if(thirdgroup == 0)
			{
				thirdgroup = GangWar[warid][war_at_gid];
				continue;
			}
		}
	}

	return attackers_count;
}

stock GetAttacksCount(gid)
{
	new attacks_count;

	foreach(new warid: GangWars)
	{
		if(GangWar[warid][war_attacker] == gid) attacks_count++;
	}

	return attacks_count;
}