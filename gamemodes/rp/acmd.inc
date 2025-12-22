COMMAND:mowner(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT)) return SendGuiInformation(playerid, ""guiopis"Wyst�pi� b��d!", "Nie posiadasz uprawnie� do korzystania z tej komendy.");
    if( !IsValidDynamicObject(pInfo[playerid][player_edited_object]) ) return SendGuiInformation(playerid, "Informacja", "Nie edytujesz �adnego obiektu.");

    new o_id = pInfo[playerid][player_edited_object];
    if(o_id == INVALID_OBJECT_ID) return SendGuiInformation(playerid, "Informacja", "Nie edytujesz �adnego obiektu.");

    new option, opt;

    if( sscanf(params, "dd", option, opt) )
	{
		SendClientMessage(playerid, COLOR_GREY, "Tip: /mowner [typ] [owner]");
		return 1;
	}

    SendGuiInformation(playerid, "Informacja", "Obiekt zosta� przepisany.");

    new uid = Object[o_id][object_uid];

    mysql_query(mySQLconnection, sprintf("UPDATE `ipb_objects` SET object_type = %d, object_owner = %d WHERE object_uid = %d", option, opt, uid));

    CancelEdit(playerid);
    Object[o_id][object_is_edited] = false;
    pInfo[playerid][player_edited_object] = -1;
    TextDrawHideForPlayer(playerid, Dashboard[playerid]);

    RefreshPlayer(playerid);

    DeleteObject(o_id, false);
    LoadObject(sprintf("WHERE `object_uid` = %d", uid));

    return 1;
}

COMMAND:masked(playerid, params[])
{
	if(!HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT)) return SendGuiInformation(playerid, ""guiopis"Wyst�pi� b��d!", ""COLOR_GUI_BLUE"Twoje konto nie posiada uprawnie� do skorzystania z tej komendy!");

	new list_players[256];

	foreach(new i: Player)
   	{
        if(strfind(pInfo[i][player_name], "Nieznajomy", true) >= 0)
		{
			format(list_players, sizeof(list_players), "%s\n%d\t%s (%s)", list_players, i, pInfo[i][player_name], pGlobal[i][glo_name]);
		}
    }
	if(strlen(list_players))
	{
	    ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_LIST, "Wyniki wyszukiwania:", list_players, "Okej", "Zamknij");
	}
	else
	{
	    SendGuiInformation(playerid, "Informacja", "Nie znaleziono �adnego zamaskowanego gracza.");
	}
    return 1;
}

COMMAND:animsprawdz(playerid, params[])
{
	if(!HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT)) return SendGuiInformation(playerid, ""guiopis"Wyst�pi� b��d!", ""COLOR_GUI_BLUE"Twoje konto nie posiada uprawnie� do skorzystania z tej komendy!");
	new lib[32], name[32], Float:delta, bool:loop, bool:lockx, bool:locky, bool:freeze, time;
	if( sscanf(params, "s[32]s[32]fddddd", lib, name, delta, loop, lockx, locky, freeze, time) )
	{
		SendClientMessage(playerid, COLOR_GREY, "Tip: /animsprawdz [lib/nazwa/predkosc/loop/lockx/lockyy/freeze/time] ");
		return 1;
	}
	else
	{
		ApplyAnimation(playerid, lib, name, delta, loop, lockx, locky, freeze, time, 1);
	}
	return 1;
}

COMMAND:duty(playerid, params[])
{
	new slot;
	if( sscanf(params, "d", slot) )
	{
		if( !HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /duty [slot]");
		if( pInfo[playerid][player_admin_duty] )
		{
			strcopy(pInfo[playerid][player_name], pInfo[playerid][player_duty_tmp], MAX_PLAYER_NAME+1);
			pInfo[playerid][player_duty_tmp][0] = EOS;

			UpdatePlayerLabel(playerid);

			pInfo[playerid][player_admin_duty] = false;

			new duty_time = gettime() - pInfo[playerid][player_admin_duty_start] - pInfo[playerid][player_admin_duty_afk_time];

			SendClientMessage(playerid, 0xD7A064FF, sprintf("Zszed�e� ze s�u�by administratora. Czas przebyty na s�u�bie: %d min %d sek.", floatround(duty_time/60, floatround_floor), duty_time%60));
			mysql_query(mySQLconnection, sprintf("INSERT INTO `ipb_game_sessions` (session_uid, session_type, session_extraid, session_owner, session_start, session_end) VALUES (null, %d, %d, %d, %d, %d)", SESSION_TYPE_ADMIN_DUTY, 0, pInfo[playerid][player_id], pInfo[playerid][player_admin_duty_start], (gettime() - pInfo[playerid][player_admin_duty_afk_time])));

			pInfo[playerid][player_admin_duty_afk_time] = 0;
			pInfo[playerid][player_admin_duty_start] = 0;
		}
		else
		{
			if( GetPlayerUsedItem(playerid, ITEM_TYPE_MASKA) > -1 ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Nie mo�esz wej�� na duty z za�o�on� mask�.");
			new name[MAX_PLAYER_NAME+1];
	   		GetPlayerName(playerid, name, sizeof(name));

			strcopy(pInfo[playerid][player_duty_tmp], name, MAX_PLAYER_NAME+1);
			strcopy(pInfo[playerid][player_name], pGlobal[playerid][glo_name], MAX_PLAYER_NAME+1);
			EscapePL(pInfo[playerid][player_name]);

			UpdatePlayerLabel(playerid);

			pInfo[playerid][player_admin_duty] = true;
			pInfo[playerid][player_admin_duty_start] = gettime();

			if( HasCrewFlag(playerid, CREW_FLAG_ITEMS) )
			{
				pInfo[playerid][player_capacity] = 50000;
			}

			SendClientMessage(playerid, 0xD7A064FF, "Wszed�e� na s�u�be administratora. Aby zej��, u�yj ponownie tej komendy.");
		}
	}
	else
	{
		cmd_grupy(playerid, sprintf("%d duty", slot));
	}
	return 1;
}

COMMAND:aprodukt(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_ITEMS) ) return SendGuiInformation(playerid, ""guiopis"Wyst�pi� b��d!", ""COLOR_GUI_BLUE"Twoje konto nie posiada uprawnie� do skorzystania z tej komendy!");
	new sub_cmd[20], rest[64], uid, cmd_value;
	if( sscanf(params, "s[32]S()[80]", sub_cmd, rest) )
	{
		SendClientMessage(playerid, COLOR_GREY, "Tip: /aprodukt [reload/limit/resetlimit/extraid/value1/value2/typ/cena]");
		return 1;
	}
	else
	{
		if( !strcmp(sub_cmd, "load") )
		{
			if(sscanf(rest, "d", uid)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aprodukt reload [UID]");
			new prod = LoadProduct(uid);
			SendClientMessage(playerid, COLOR_LIGHTER_RED, sprintf("Pomy�lnie za�adowano produkt %s (UID %d).", Product[prod][product_name], Product[prod][product_id]));
		}
		else if( !strcmp(sub_cmd, "reload") )
		{
			if(sscanf(rest, "d", uid)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aprodukt reload [UID]");
			new prod = ReloadProduct(uid);
			SendClientMessage(playerid, COLOR_LIGHTER_RED, sprintf("Pomy�lnie prze�adowano produkt %s (UID %d).", Product[prod][product_name], Product[prod][product_id]));
		}
		else if( !strcmp(sub_cmd, "limit") )
		{
			if(sscanf(rest, "dd", uid, cmd_value)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aprodukt limit [uid produktu] [limit]");
			new prodid = GetProductByUid(uid);
			if(prodid == - 1) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aprodukt limit [uid produktu]");
			Product[prodid][product_limit] = cmd_value;
			mysql_query(mySQLconnection, sprintf("UPDATE ipb_products SET product_limit_count = %d WHERE product_uid = %d", cmd_value, Product[prodid][product_id]));
			SendClientMessage(playerid, COLOR_LIGHTER_RED, "Limit zamawiania zosta� edytowany.");
		}
		else if( !strcmp(sub_cmd, "resetlimit") )
		{
			if(sscanf(rest, "d", uid)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aprodukt resetlimit [uid produktu]");
			new prodid = GetProductByUid(uid);
			if(prodid == - 1) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aprodukt resetlimit [uid produktu]");
			Product[prodid][product_limit_used] = 0;
			mysql_query(mySQLconnection, sprintf("UPDATE ipb_products SET product_limit_used = 0 WHERE product_uid = %d", Product[prodid][product_id]));
			SendClientMessage(playerid, COLOR_LIGHTER_RED, "Zu�ycie limitu zosta�o zresetowane.");
		}
		else if( !strcmp(sub_cmd, "typ") )
		{
			if(sscanf(rest, "dd", uid, cmd_value)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aprodukt typ [uid produktu] [typ]");
			new prodid = GetProductByUid(uid);
			if(prodid == - 1) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aprodukt typ [uid produktu]");
			Product[prodid][product_type] = cmd_value;
			mysql_query(mySQLconnection, sprintf("UPDATE ipb_products SET product_type = %d WHERE product_uid = %d", cmd_value, Product[prodid][product_id]));
			SendClientMessage(playerid, COLOR_LIGHTER_RED, "Typ produktu zosta� edytowany.");
		}
		else if( !strcmp(sub_cmd, "value1") )
		{
			if(sscanf(rest, "dd", uid, cmd_value)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aprodukt value1 [uid produktu] [warto��]");
			new prodid = GetProductByUid(uid);
			if(prodid == - 1) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aprodukt value1 [uid produktu]");
			Product[prodid][product_value1] = cmd_value;
			mysql_query(mySQLconnection, sprintf("UPDATE ipb_products SET product_value1 = %d WHERE product_uid = %d", cmd_value, Product[prodid][product_id]));
			SendClientMessage(playerid, COLOR_LIGHTER_RED, "Value1 produktu zosta� edytowany.");
		}
		else if( !strcmp(sub_cmd, "value2") )
		{
			if(sscanf(rest, "dd", uid, cmd_value)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aprodukt value2 [uid produktu] [warto��]");
			new prodid = GetProductByUid(uid);
			if(prodid == - 1) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aprodukt value2 [uid produktu]");
			Product[prodid][product_value2] = cmd_value;
			mysql_query(mySQLconnection, sprintf("UPDATE ipb_products SET product_value2 = %d WHERE product_uid = %d", cmd_value, Product[prodid][product_id]));
			SendClientMessage(playerid, COLOR_LIGHTER_RED, "Value2 produktu zosta� edytowany.");
		}
		else if( !strcmp(sub_cmd, "cena") )
		{
			if(sscanf(rest, "dd", uid, cmd_value)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aprodukt cena [uid produktu] [cena]");
			new prodid = GetProductByUid(uid);
			if(prodid == - 1) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aprodukt cena [uid produktu]");
			Product[prodid][product_price] = cmd_value;
			mysql_query(mySQLconnection, sprintf("UPDATE ipb_products SET product_price = %d WHERE product_uid = %d", cmd_value, Product[prodid][product_id]));
			SendClientMessage(playerid, COLOR_LIGHTER_RED, "Cena produktu zosta�a edytowana.");
		}
		else if( !strcmp(sub_cmd, "nazwa") )
		{
			new pname[80];
			if(sscanf(rest, "ds[80]", uid, pname)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aprodukt nazwa [uid produktu] [nazwa]");
			new prodid = GetProductByUid(uid);
			if(prodid == - 1) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aprodukt nazwa [uid produktu]");
			if(strlen(pname) > 38) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aprodukt nazwa [uid produktu] [nazwa]");
			sscanf(pname, "s[80]", Product[prodid][product_name]);
			mysql_real_escape_string(pname, pname, mySQLconnection, 80);
			mysql_query(mySQLconnection, sprintf("UPDATE ipb_products SET product_name = '%s' WHERE product_uid = %d", pname, Product[prodid][product_id]));
			SendClientMessage(playerid, COLOR_LIGHTER_RED, "Nazwa produktu zosta�a edytowana.");
		}
		else if( !strcmp(sub_cmd, "extraid") )
		{
			if(sscanf(rest, "dd", uid, cmd_value)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aprodukt extraid [uid produktu] [warto��]");
			new prodid = GetProductByUid(uid);
			if(prodid == - 1) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aprodukt extraid [uid produktu]");
			Product[prodid][product_extra] = cmd_value;
			mysql_query(mySQLconnection, sprintf("UPDATE ipb_products SET product_extraid = %d WHERE product_uid = %d", cmd_value, Product[prodid][product_id]));
			SendClientMessage(playerid, COLOR_LIGHTER_RED, "Extraid produktu zosta�o edytowane.");
		}
		else if( !strcmp(sub_cmd, "usun") )
		{
			if(sscanf(rest, "d", uid)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aprodukt usun [uid produktu]");
			RemoveProduct(uid);
			SendClientMessage(playerid, COLOR_LIGHTER_RED, sprintf("Skasowano produkt o UID %d.", uid));
		}

	}
	return 1;
}

COMMAND:setnick(playerid, params[])
{
	if(!HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT)) return SendGuiInformation(playerid, ""guiopis"Wyst�pi� b��d!", ""COLOR_GUI_BLUE"Twoje konto nie posiada uprawnie� do skorzystania z tej komendy!");
	new id, nickname[MAX_PLAYER_NAME+1];
	if( sscanf(params, "ds[25]", id, nickname) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /setnick [id gracza] [nick]");
	if( !IsPlayerConnected(id) ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Gracza o podanym id nie ma na serwerze.");
    if( !pInfo[id][player_logged] ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Gracz o podanym id nie jest zalogowany.");

    strcopy(pInfo[id][player_name], nickname, MAX_PLAYER_NAME+1);

	UpdatePlayerLabel(id);
	return 1;
}

COMMAND:setstatus(playerid, params[])
{
    if(!HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT)) return SendGuiInformation(playerid, ""guiopis"Wyst�pi� b��d!", ""COLOR_GUI_BLUE"Twoje konto nie posiada uprawnie� do skorzystania z tej komendy!");
    new targetid, status[32];
    if( sscanf(params, "us[32]", targetid, status) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /setstatus [id gracza/czesc nicku] [tre�c statusu]");
    if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Gracza o podanym id nie ma na serwerze.");
    if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Gracz o podanym id nie jest zalogowany.");
    if( strlen(status) > 32 ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /setstatus [id gracza/czesc nicku] [tre�c statusu]");

  	if( !strcmp(status, "0", true) )
  	{
  		pInfo[targetid][player_extra_status] = EOS;
  		SendClientMessage(playerid, COLOR_LIGHTER_RED, "Status zosta� usuni�ty.");
  		RemovePlayerStatus(targetid, PLAYER_STATUS_EXTRA);
  		UpdatePlayerLabel(targetid);
  		mysql_query(mySQLconnection, sprintf("UPDATE ipb_characters SET char_extra_status = '' WHERE char_uid = %d", pInfo[targetid][player_id]));
  		return 1;
  	}
    mysql_query(mySQLconnection, sprintf("UPDATE `ipb_characters` SET `char_extra_status` = '%s'  WHERE `char_uid` = %d", status, pInfo[targetid][player_id]));
    sscanf(status, "s[32]", pInfo[targetid][player_extra_status]);

    SendFormattedClientMessage(targetid, COLOR_LIGHTER_RED, "Administrator %s ustawi� Ci specjalny status - %s.", pInfo[playerid][player_name], status);
    if( playerid != targetid ) SendFormattedClientMessage(playerid, COLOR_LIGHTER_RED, "Ustawi�e� graczowi %s (UID: %d, ID: %d) specjalny status - %s.", pInfo[targetid][player_name], pInfo[targetid][player_id], targetid, status);

    AddPlayerStatus(targetid, PLAYER_STATUS_EXTRA);
    UpdatePlayerLabel(targetid);
    return 1;
}

COMMAND:banip(playerid, params[])
{
	if(!HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT)) return SendGuiInformation(playerid, ""guiopis"Wyst�pi� b��d!", ""COLOR_GUI_BLUE"Twoje konto nie posiada uprawnie� do skorzystania z tej komendy!");

	new pid;
	if(sscanf(params, "us[256]", pid)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /banip [id gracza/czesc nicku] [pow�d]");
	Ban(pid);

	SendClientMessage(playerid, COLOR_GOLD, sprintf("Zbanowa�e� IP gracza o nicku: %s [GUID: %d]", pInfo[pid][player_name], pGlobal[pid][glo_id]));
	return 1;
}

COMMAND:jobrespawn(playerid, params[])
{
	if(!HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT)) return SendGuiInformation(playerid, ""guiopis"Wyst�pi� b��d!", ""COLOR_GUI_BLUE"Twoje konto nie posiada uprawnie� do skorzystania z tej komendy!");
	new count;
	foreach(new vid : Vehicles)
	{
		if( Vehicle[vid][vehicle_owner_type] == VEHICLE_OWNER_TYPE_JOB )
		{
			if( Vehicle[vid][vehicle_occupants] > 0 ) continue;

			SetVehiclePos(vid, Vehicle[vid][vehicle_park][0], Vehicle[vid][vehicle_park][1], Vehicle[vid][vehicle_park][2]-0.3);
			SetVehicleZAngle(vid, Vehicle[vid][vehicle_park][3]);
			SetVehicleVirtualWorld(vid, Vehicle[vid][vehicle_park_world]);
			LinkVehicleToInterior(vid, Vehicle[vid][vehicle_park_interior]);
			SetVehicleHealth(vid, 1000);
			Vehicle[vid][vehicle_destroyed] = false;
			Vehicle[vid][vehicle_damage][0] = 0;
			Vehicle[vid][vehicle_damage][1] = 0;
			Vehicle[vid][vehicle_damage][2] = 0;
			Vehicle[vid][vehicle_damage][3] = 0;
			Vehicle[vid][vehicle_fuel_current] = 50;

			UpdateVehicleDamageStatus(vid, Vehicle[vid][vehicle_damage][0], Vehicle[vid][vehicle_damage][1], Vehicle[vid][vehicle_damage][2], Vehicle[vid][vehicle_damage][3]);
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_vehicles` SET `vehicle_health` = 1000 WHERE `vehicle_uid` = %d", Vehicle[vid][vehicle_uid]));

			Vehicle[vid][vehicle_locked] = true;
			count++;
		}
	}

	SendGuiInformation(playerid, ""guiopis"Powiadomienie", sprintf("Zrespawnowano %d pojazd�w z prac dorywczych.", count));
	return 1;
}

COMMAND:agmx(playerid, params[])
{
	if(!HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT)) return 1;

	SendClientMessageToAll(COLOR_GOLD, "OpenRP.pl: restart serwera za 10 sekund.");

	foreach(new playeri: Player)
	{
		SavePlayer(playeri);
	}

	defer RestartServer[10000]();
	return 1;
}

COMMAND:apoint(playerid, params[])
{
	if(!HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT)) return SendGuiInformation(playerid, ""guiopis"Wyst�pi� b��d!", ""COLOR_GUI_BLUE"Twoje konto nie posiada uprawnie� do skorzystania z tej komendy!");
	new seta[256], sets[256],
		Float:pX, Float:pY, Float:pZ;
	if(sscanf(params, "s[80]S()[80]", sets, seta)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /apoint [save, tp]]");
	if(!strcmp(sets, "save", true))
	{
		GetPlayerPos(playerid, pX, pY, pZ);
	    SetPVarFloat(playerid,"al_posx", pX);
	    SetPVarFloat(playerid,"al_posy", pY);
	    SetPVarFloat(playerid,"al_posz", pZ);
	    if(GetPVarInt(playerid, "al_pos") == 1)
		{
			SendClientMessage(playerid, COLOR_GOLD, sprintf("Nowa pozycja zapisana. [%f|%f|%f]", pX, pY, pZ));
		} else {
			SendClientMessage(playerid, COLOR_GOLD, sprintf("Pozycja zapisana. [%f:%f:%f]", pX, pY, pZ));
		}
		SetPVarInt(playerid, "al_pos", 1);
	}
	if(!strcmp(sets, "tp", true))
	{
		if(GetPVarInt(playerid, "al_pos") == 0)
		{
			SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Nie zapisa�e� jeszcze �adnej pozycji.");
		} else {
			if(GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
			{
				SetVehiclePos(GetPlayerVehicleID(playerid), GetPVarFloat(playerid, "al_posx"), GetPVarFloat(playerid, "al_posy"), GetPVarFloat(playerid, "al_posz"));
			} else {
				RP_PLUS_SetPlayerPos(playerid, GetPVarFloat(playerid, "al_posx"), GetPVarFloat(playerid, "al_posy"), GetPVarFloat(playerid, "al_posz"));
			}
			SendClientMessage(playerid, COLOR_GOLD, "Teleportowa�e� si� do ostatnio zapisanej pozycji.");
		}
	}
	return 1;
}

COMMAND:ado(playerid, params[])
{
	if(!HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Wyst�pi� b��d!", ""COLOR_GUI_BLUE"Twoje konto nie posiada uprawnie� do skorzystania z tej komendy!");
	new text[128], str[128];
	if(sscanf(params, "s[128]", text)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /ado [tekst]");
	format(str, sizeof(str), "** %s **", text);
	SendClientMessageToAll(0x9B91ECFF, str);
	return 1;
}

COMMAND:spec(playerid, params[])
{
	return cmd_rc(playerid, params);
}

COMMAND:rc(playerid, params[])
{
    if(!HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT)) return SendGuiInformation(playerid, ""guiopis"Wyst�pi� b��d!", ""COLOR_GUI_BLUE"Twoje konto nie posiada uprawnie� do skorzystania z tej komendy!");
    new id;

    if(!sscanf(params, "d", id))
    {
    	if( !IsPlayerConnected(id) )
    	{
    		SendClientMessage(playerid, COLOR_GREY, "Niepoprawne ID gracza.");
    		return 1;
    	}

    	if( pInfo[id][player_spec] != INVALID_PLAYER_ID )
    	{
    		SendClientMessage(playerid, COLOR_GREY, "Ten gracz ju� kogo� obserwuje, przechodze do kolejnego ID.");
    		return cmd_spec(playerid, sprintf("%d", Iter_Next(Player, id)));
    	}

	    SetPlayerInterior(playerid, GetPlayerInterior(id));
	    SetPlayerVirtualWorld(playerid, GetPlayerVirtualWorld(id));

	    new string[340];

	    format(string, sizeof(string), "~n~~w~Global: %s		Zdrowie: ~p~%0.1f		~w~Portfel: ~g~%d		~w~Godziny: ~y~%d		Armour: ~w~%0.1f~n~~n~~n~~n~~w~Uzywaj klawisza ~r~LALT lub ~r~SPACJA ~w~aby przeskakiwac miedzy graczami.~n~~r~LSHIFT ~w~odswieza specowanie danego gracza.", pGlobal[id][glo_name],pInfo[id][player_health], GetPlayerMoney(id), pInfo[id][player_hours], pInfo[id][player_armour]);

	    TextDrawSetString(DashBoardPane2[playerid], string);
		TextDrawShowForPlayer(playerid, DashBoardPane2[playerid]);

	    if(IsPlayerInAnyVehicle(id))
	    {
            TogglePlayerSpectating(playerid, true);
            PlayerSpectateVehicle(playerid, GetPlayerVehicleID(id));
	    }
	    else
	    {
            TogglePlayerSpectating(playerid, true);
            PlayerSpectatePlayer(playerid, id);
	    }

	    pInfo[playerid][player_spec] = id;
	    pInfo[playerid][player_quit_vw] = GetPlayerVirtualWorld(playerid);
	}
	else
	{
		if(pInfo[playerid][player_spec] != INVALID_PLAYER_ID)
	    {
            TogglePlayerSpectating(playerid, false);

			TextDrawHideForPlayer(playerid, DashBoardPane2[playerid]);

            SetPlayerSkin(playerid, pInfo[playerid][player_skin]);
            SetPlayerVirtualWorld(playerid, pInfo[playerid][player_quit_vw]);
            pInfo[playerid][player_spec] = INVALID_PLAYER_ID;
	    }
	    else
	    {
	    	SendClientMessage(playerid, COLOR_GREY, "Tip: /rc [id gracza]");
	    }
	}
    return 1;
}

COMMAND:apos(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Wyst�pi� b��d!", ""COLOR_GUI_BLUE"Twoje konto nie posiada uprawnie� do skorzystania z tej komendy!");
	new Float:x, Float:y, Float:z;
    if(sscanf(params, "fff", x,y,z))
        return SendClientMessage(playerid, COLOR_GREY, "Tip: /apos [x, y, z]");

    RP_PLUS_SetPlayerPos(playerid, x, y, z);
	return 1;
}

COMMAND:freeze(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Wyst�pi� b��d!", ""COLOR_GUI_BLUE"Twoje konto nie posiada uprawnie� do skorzystania z tej komendy!");
	new id;
    if(sscanf(params, "u", id))
        return SendClientMessage(playerid, COLOR_GREY, "Tip: /freeze [id]");

    if( !IsPlayerConnected(id) ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Gracza o podanym id nie ma na serwerze.");

    if(pInfo[id][player_freezed]==false)
    {
	    TogglePlayerControllable(id, false);
   		SendFormattedClientMessage(playerid, COLOR_GOLD, "Zamrozi�e� gracza %s (ID: %d, UID: %d).", pInfo[id][player_name], id, pInfo[id][player_id]);
   		SendFormattedClientMessage(id, COLOR_GOLD, "Administrator %s zamrozi� Ci�.", pInfo[playerid][player_name]);
   		pInfo[id][player_freezed]=true;
	}
	else
	{
	    TogglePlayerControllable(id, true);
   		SendFormattedClientMessage(playerid, COLOR_GOLD, "Odmrozi�e� gracza %s (ID: %d, UID: %d).", pInfo[id][player_name], id, pInfo[id][player_id]);
   		SendFormattedClientMessage(id, COLOR_GOLD, "Administrator %s odmrozi� Ci�.", pInfo[playerid][player_name]);
   		pInfo[id][player_freezed]=false;
	}
	return 1;
}

COMMAND:sethours(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) && !HasCrewFlag(playerid, CREW_FLAG_GM) ) return SendGuiInformation(playerid, ""guiopis"Wyst�pi� b��d!", ""COLOR_GUI_BLUE"Twoje konto nie posiada uprawnie� do skorzystania z tej komendy!");
	new targetid, hrs;
	if( sscanf(params, "ud", targetid, hrs) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /sethours [id gracza/czesc nicku] [ilo�� godzin]");
	if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Gracza o podanym id nie ma na serwerze.");
	if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Gracz o podanym id nie jest zalogowany.");
	if( hrs < 0 ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /sethours [id gracza/czesc nicku] [ilo�� godzin]");

	mysql_query(mySQLconnection, sprintf("UPDATE `ipb_characters` SET `char_hours` = %d  WHERE `char_uid` = %d", hrs, pInfo[targetid][player_id]));
	pInfo[targetid][player_hours]=hrs;

	SendFormattedClientMessage(targetid, COLOR_LIGHTER_RED, "Administrator %s ustawi� Ci %d godzin.", pInfo[playerid][player_name], hrs);
	if( playerid != targetid ) SendFormattedClientMessage(playerid, COLOR_LIGHTER_RED, "Ustawi�e� graczowi %s (UID: %d, ID: %d) %d godzin.", pInfo[targetid][player_name], pInfo[targetid][player_id], targetid, hrs);

	return 1;
}

COMMAND:slap(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Wyst�pi� b��d!", ""COLOR_GUI_BLUE"Twoje konto nie posiada uprawnie� do skorzystania z tej komendy!");
	new id, Float: PPos[3];
    if(sscanf(params, "u", id))
        return SendClientMessage(playerid, COLOR_GREY, "Tip: /slap [id gracza/czesc nicku]");

    if( !IsPlayerConnected(id) ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Gracza o podanym id nie ma na serwerze.");
    GetPlayerPos(id, PPos[0], PPos[1], PPos[2]);
    RP_PLUS_SetPlayerPos(id, PPos[0], PPos[1], PPos[2]+4);

   	SendFormattedClientMessage(playerid, COLOR_GOLD, "Slapn��e� %s (ID: %d, UID: %d).", pInfo[id][player_name], id, pInfo[id][player_id]);
	return 1;
}

COMMAND:givecash(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT)) return SendGuiInformation(playerid, ""guiopis"Wyst�pi� b��d!", ""COLOR_GUI_BLUE"Twoje konto nie posiada uprawnie� do skorzystania z tej komendy!");

	new targetid, skin;
	if( sscanf(params, "ud", targetid, skin) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /givecash [id gracza/czesc nicku] [ilo��]");
	if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Gracza o podanym id nie ma na serwerze.");
	if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Gracz o podanym id nie jest zalogowany.");

	GivePlayerMoney(targetid, skin);
	SendFormattedClientMessage(playerid, COLOR_GOLD, "Doda�e� graczowi %s (UID: %d, ID: %d) forse: %d.", pInfo[targetid][player_name], pInfo[targetid][player_id], targetid, skin);

	return 1;
}

COMMAND:ban(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Wyst�pi� b��d!", ""COLOR_GUI_BLUE"Twoje konto nie posiada uprawnie� do skorzystania z tej komendy!");
	new playa,text[128];
	if(sscanf(params, "us[128]", playa, text)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /ban [id/czesc nicku] [pow�d]");
	if(!IsPlayerConnected(playa)||playa==playerid) return SendClientMessage(playerid, COLOR_GREY, "Tip: Niepoprawne ID gracza.");
	if(strlen(text)<1 || strlen(text)>47) return SendClientMessage(playerid, COLOR_GREY, "Tip: Zbyt dlugi pow�d kary, max 47 znak�w.");

	EscapePL(text);

	BanAc(playa, playerid, text);

	SendFormattedClientMessage(playa, COLOR_LIGHTER_RED, "Administrator %s zbanowa� Twoje konto z powodem: %s.", gInfo[playerid][global_name], text);
	Kick(playa);
	return 1;
}

COMMAND:block(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Wyst�pi� b��d!", ""COLOR_GUI_BLUE"Twoje konto nie posiada uprawnie� do skorzystania z tej komendy!");
	new sub_cmd[20], rest[64], playa, text[128], time;
	if( sscanf(params, "s[32]S()[80]", sub_cmd, rest) )
	{
		SendClientMessage(playerid, COLOR_GREY, "Tip: /block [char/run/dmg/veh/ooc]");
		return 1;
	}
	else
	{
		if( !strcmp(sub_cmd, "char") )
		{
			if(sscanf(rest, "us[128]", playa, text)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /block char [id gracza/czesc nicku] [pow�d]");
			if(!IsPlayerConnected(playa)||playa==playerid) return SendClientMessage(playerid, COLOR_GREY, "Tip: Niepoprawne ID gracza.");
			if(strlen(text)<1||strlen(text)>47) return SendClientMessage(playerid, COLOR_GREY, "Tip: Niepoprawny pow�d kary - min 1, max 47 znak�w.");

			Block(playa, playerid, text);
		}
		else if( !strcmp(sub_cmd, "ooc") )
		{
			if(sscanf(rest, "uds[128]", playa, time, text)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /block ooc [id gracza/czesc nicku] [ilo�� dni] [pow�d]");
			if(!IsPlayerConnected(playa)||playa==playerid) return SendClientMessage(playerid, COLOR_GREY, "Tip: Niepoprawne ID gracza.");
			if(strlen(text)<1||strlen(text)>47) return SendClientMessage(playerid, COLOR_GREY, "Tip: Niepoprawny pow�d kary - min 1, max 47 znak�w.");

			BlockOoc(playa, playerid, time, text);
		}
		else if( !strcmp(sub_cmd, "run") )
		{
			if(sscanf(rest, "uds[128]", playa, time, text)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /block run [id gracza/czesc nicku] [ilo�� dni] [pow�d]");
			if(!IsPlayerConnected(playa)||playa==playerid) return SendClientMessage(playerid, COLOR_GREY, "Tip: Niepoprawne ID gracza.");
			if(strlen(text)<1||strlen(text)>47) return SendClientMessage(playerid, COLOR_GREY, "Tip: Niepoprawny pow�d kary - min 1, max 47 znak�w.");

			BlockRun(playa, playerid, time, text);
		}
		else if( !strcmp(sub_cmd, "dmg") )
		{
			if(sscanf(rest, "uds[128]", playa, time, text)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /block dmg [id gracza/czesc nicku] [ilo�� dni] [pow�d]");
			if(!IsPlayerConnected(playa)||playa==playerid) return SendClientMessage(playerid, COLOR_GREY, "Tip: Niepoprawne ID gracza.");
			if(strlen(text)<1||strlen(text)>47) return SendClientMessage(playerid, COLOR_GREY, "Tip: Niepoprawny pow�d kary - min 1, max 47 znak�w.");

			BlockDmg(playa, playerid, time, text);
		}
		else if( !strcmp(sub_cmd, "veh") )
		{
			if(sscanf(rest, "uds[128]", playa, time, text)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /block veh [id gracza/czesc nicku] [ilo�� dni] [pow�d]");
			if(!IsPlayerConnected(playa)||playa==playerid) return SendClientMessage(playerid, COLOR_GREY, "Tip: Niepoprawne ID gracza.");
			if(strlen(text)<1||strlen(text)>47) return SendClientMessage(playerid, COLOR_GREY, "Tip: Niepoprawny pow�d kary - min 1, max 47 znak�w.");

			BlockVeh(playa, playerid, time, text);
		}
	}
	return 1;
}

COMMAND:fck(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Wyst�pi� b��d!", ""COLOR_GUI_BLUE"Twoje konto nie posiada uprawnie� do skorzystania z tej komendy!");
	new playa, text[128];

	if(sscanf(params, "us[128]", playa, text)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /fck [id gracza/cz�� nicku] [pow�d]");
	if(!IsPlayerConnected(playa)||playa==playerid) return SendClientMessage(playerid, COLOR_GREY, "Tip: Niepoprawne ID gracza.");
	if(strlen(text)<1||strlen(text)>47) return SendClientMessage(playerid, COLOR_GREY, "Tip: Niepoprawny pow�d kary - min 1, max 47 znak�w.");

	CharacterKill(playa, playerid, text);

	return 1;
}

COMMAND:aj(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Wyst�pi� b��d!", ""COLOR_GUI_BLUE"Twoje konto nie posiada uprawnie� do skorzystania z tej komendy!");
	new playa,text[128],time;
	if(sscanf(params, "uds[128]", playa, time, text)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aj [id gracza/czesc nicku] [czas] [pow�d]");
	if(!IsPlayerConnected(playa)||playa==playerid) return SendClientMessage(playerid, COLOR_GREY, "Tip: Niepoprawne ID gracza.");
	if(strlen(text)<1||strlen(text)>47) return SendClientMessage(playerid, COLOR_GREY, "Tip: Niepoprawny pow�d kary - min 1, max 47 znak�w.");
	if(time>1000) return SendClientMessage(playerid, COLOR_GREY, "Tip: Maksymalny czas AJ to 1000 minut.");

	AdminJail(playa, playerid, text, time);
	return 1;
}

COMMAND:kick(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Wyst�pi� b��d!", ""COLOR_GUI_BLUE"Twoje konto nie posiada uprawnie� do skorzystania z tej komendy!");
	new playa,text[128];
	if(sscanf(params, "us[128]", playa, text)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /kick [id gracza/czesc nicku] [pow�d]");
	if(!IsPlayerConnected(playa)||playa==playerid) return SendClientMessage(playerid, COLOR_GREY, "Tip: Niepoprawne ID gracza.");
	if(strlen(text)<1||strlen(text)>47) return SendClientMessage(playerid, COLOR_GREY, "Tip: Niepoprawny pow�d kary - min 1, max 47 znak�w.");

	KickAc(playa, playerid, text);
	return 1;
}

COMMAND:warn(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Wyst�pi� b��d!", ""COLOR_GUI_BLUE"Twoje konto nie posiada uprawnie� do skorzystania z tej komendy!");
	new playa,text[256];
	if(sscanf(params, "us[128]", playa, text)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /warn [id gracza/czesc nicku] [pow�d]");
	if(strlen(text)<1||strlen(text)>47) return SendClientMessage(playerid, COLOR_GREY, "Tip: Niepoprawny pow�d kary - min 1, max 47 znak�w.");

	Warn(playa, playerid, text);
	return 1;
}

COMMAND:gs(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Wyst�pi� b��d!", ""COLOR_GUI_BLUE"Twoje konto nie posiada uprawnie� do skorzystania z tej komendy!");
	new playa,text[256], score;
	if(sscanf(params, "uds[128]", playa, score, text)) return SendClientMessage(playerid, COLOR_GREY, "Tip: /gs [id gracza/czesc nicku] [ilo��] [pow�d]");
	if(strlen(text)<1||strlen(text)>47) return SendClientMessage(playerid, COLOR_GREY, "Tip: Niepoprawny pow�d kary - min 1, max 47 znak�w.");

	Score(playa, playerid, score, text);
	return 1;
}

COMMAND:pflags(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Wyst�pi� b��d!", ""COLOR_GUI_BLUE"Twoje konto nie posiada uprawnie� do skorzystania z tej komendy!");

	new targetid;
	if( sscanf(params, "u", targetid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /pflags [id gracza/czesc nicku]");

	if( !IsPlayerConnected(targetid) || !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Gracza o podanym id nie ma na serwerze.");

	DynamicGui_Init(playerid);
	DynamicGui_SetDialogValue(playerid, targetid);
	new str[256];

	format(str, sizeof(str), "%s%s01\tBMX\n", str, ((PlayerHasFlag(targetid, PLAYER_FLAG_BMX)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
	DynamicGui_AddRow(playerid, PLAYER_FLAG_BMX);

	format(str, sizeof(str), "%s%s02\tZamawianie\n", str, ((PlayerHasFlag(targetid, PLAYER_FLAG_ORDER)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
	DynamicGui_AddRow(playerid, PLAYER_FLAG_ORDER);

	format(str, sizeof(str), "%s%s03\tUpr burmistrza\n", str, ((PlayerHasFlag(targetid, PLAYER_FLAG_PRESIDENT)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
	DynamicGui_AddRow(playerid, PLAYER_FLAG_PRESIDENT);

	ShowPlayerDialog(playerid, DIALOG_GIVE_FLAG, DIALOG_STYLE_LIST, sprintf("Flagi postaci %s [Global: %d, UID: %d, ID: %d]", pInfo[targetid][player_name], gInfo[targetid][global_id], pInfo[targetid][player_id], targetid), str, "Wybierz", "Zamknij");
	return 1;
}

COMMAND:aflags(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Wyst�pi� b��d!", ""COLOR_GUI_BLUE"Twoje konto nie posiada uprawnie� do skorzystania z tej komendy!");

	new targetid;
	if( sscanf(params, "u", targetid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aflags [id gracza/czesc nicku]");

	if( !IsPlayerConnected(targetid) || !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Gracza o podanym id nie ma na serwerze.");

	DynamicGui_Init(playerid);
	DynamicGui_SetDialogValue(playerid, targetid);
	new str[500];

	format(str, sizeof(str), "%s%s01\tRanga GameMastera\n", str, ((HasCrewFlag(targetid, CREW_FLAG_GM)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
	DynamicGui_AddRow(playerid, CREW_FLAG_GM);

	format(str, sizeof(str), "%s%s02\tRanga Supportera\n", str, ((HasCrewFlag(targetid, CREW_FLAG_ADMIN)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
	DynamicGui_AddRow(playerid, CREW_FLAG_ADMIN);

	format(str, sizeof(str), "%s%s03\tRanga Admina (czerwony)\n", str, ((HasCrewFlag(targetid, CREW_FLAG_ADMIN_ROOT)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
	DynamicGui_AddRow(playerid, CREW_FLAG_ADMIN_ROOT);

	format(str, sizeof(str), "%s%s04\tZarzadzanie drzwiami\n", str, ((HasCrewFlag(targetid, CREW_FLAG_DOORS)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
	DynamicGui_AddRow(playerid, CREW_FLAG_DOORS);

	format(str, sizeof(str), "%s%s05\tZarzadzanie pojazdami\n", str, ((HasCrewFlag(targetid, CREW_FLAG_VEHICLES)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
	DynamicGui_AddRow(playerid, CREW_FLAG_VEHICLES);

	format(str, sizeof(str), "%s%s06\tZarzadzanie grupami\n", str, ((HasCrewFlag(targetid, CREW_FLAG_GROUPS)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
	DynamicGui_AddRow(playerid, CREW_FLAG_GROUPS);

	format(str, sizeof(str), "%s%s07\tZarzadzanie strefami\n", str, ((HasCrewFlag(targetid, CREW_FLAG_AREAS)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
	DynamicGui_AddRow(playerid, CREW_FLAG_AREAS);

	format(str, sizeof(str), "%s%s08\tZarzadzanie obiektami i 3dtextami\n", str, ((HasCrewFlag(targetid, CREW_FLAG_EDITOR)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
	DynamicGui_AddRow(playerid, CREW_FLAG_EDITOR);

	format(str, sizeof(str), "%s%s09\tZarzadzanie przedmiotami\n", str, ((HasCrewFlag(targetid, CREW_FLAG_ITEMS)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
	DynamicGui_AddRow(playerid, CREW_FLAG_ITEMS);

	ShowPlayerDialog(playerid, DIALOG_GIVE_CREW, DIALOG_STYLE_LIST, sprintf("Flagi ekipy postaci %s [Global: %d, UID: %d, ID: %d]", pInfo[targetid][player_name], gInfo[targetid][global_id], pInfo[targetid][player_id], targetid), str, "Wybierz", "Zamknij");
	return 1;
}

COMMAND:atime(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Nie masz uprawnie� do korzystania z tej komendy.");
	new time;
	if( sscanf(params, "d", time) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /atime [godzina]");
	SetWorldTime(time);
	return 1;
}

COMMAND:aweather(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Nie masz uprawnie� do korzystania z tej komendy.");
	new weather;
	if( sscanf(params, "d", weather) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aweather [id pogody]");

	foreach(new pid: Player)
	{
		if(GetPlayerVirtualWorld(playerid) != 0) continue;
		SetPlayerWeather(pid, weather);
	}

	WorldWeather = weather;
	return 1;
}

COMMAND:setskin(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Nie masz uprawnie� do korzystania z tej komendy.");
	new targetid, skin;
	if( sscanf(params, "ud", targetid, skin) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /setskin [id gracza/czesc nicku] [skin]");
	if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Gracza o podanym id nie ma na serwerze.");
	if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Gracz o podanym id nie jest zalogowany.");

	SetPlayerSkin(targetid, skin);
	pInfo[targetid][player_skin]= skin;
	mysql_query(mySQLconnection, sprintf("UPDATE `ipb_characters` SET `char_skin` = %d WHERE `char_uid` = %d", skin, pInfo[targetid][player_id]));
	SendFormattedClientMessage(targetid, COLOR_LIGHTER_RED, "Administrator %s ustawi� Ci skin %d.", pInfo[playerid][player_name], skin);
	if( playerid != targetid ) SendFormattedClientMessage(playerid, COLOR_LIGHTER_RED, "Ustawi�e� graczowi %s (UID: %d, ID: %d) skin %d.", pInfo[targetid][player_name], pInfo[targetid][player_id], targetid, skin);

	return 1;
}

COMMAND:acto(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Wyst�pi� b��d!", ""COLOR_GUI_BLUE"Twoje konto nie posiada uprawnie� do skorzystania z tej komendy!");

	new cmdParam;
	if( sscanf(params, "d", cmdParam) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /acto [id aktora]");
	if( !Iter_Contains(Actors, cmdParam) ) return SendClientMessage(playerid, COLOR_GREY, "Niepoprawne id aktora.");

	new
		Float:x,
		Float:y,
		Float:z;
	GetActorPos(cmdParam, x, y, z);

	RP_PLUS_SetPlayerPos(playerid, x, y, z);
	SetPlayerVirtualWorld(playerid, GetActorVirtualWorld(cmdParam));

	FreezePlayer(playerid, 2000);

	SendFormattedClientMessage(playerid, COLOR_GOLD, "Przeteleportowa�e� sie do aktora o ID: %d.", cmdParam);

	return 1;
}

COMMAND:reset(playerid, params[])
{
	return cmd_res(playerid, params);
}

COMMAND:res(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Wyst�pi� b��d!", ""COLOR_GUI_BLUE"Twoje konto nie posiada uprawnie� do skorzystania z tej komendy!");

	new cmdParam;
	if( sscanf(params, "u", cmdParam) )
	{
		return cmd_res(playerid, sprintf("%d", playerid));
	}

	if( (!IsPlayerConnected(cmdParam) || !pInfo[cmdParam][player_logged]) ) return SendClientMessage(playerid, COLOR_GREY, "Ten gracz nie jest zalogowany.");

	if(pInfo[cmdParam][player_spawn_type] == SPAWN_TYPE_GLOBAL)
	{
		new spawn;
		spawn = Iter_Random(GlobalSpawns);
		SetPlayerPos(cmdParam,  GlobalSpawn[spawn][gspawn_pos][0], GlobalSpawn[spawn][gspawn_pos][1], GlobalSpawn[spawn][gspawn_pos][2]);
		SetPlayerInterior(playerid, 0);
		SetPlayerVirtualWorld(playerid, 0);
	}
	else if(pInfo[cmdParam][player_spawn_type] == SPAWN_TYPE_GLOBAL_SC)
	{
		new spawn;
		spawn = Iter_Random(ScSpawns);
		SetPlayerPos(cmdParam,  GlobalSpawn[spawn][gspawn_pos][0], GlobalSpawn[spawn][gspawn_pos][1], GlobalSpawn[spawn][gspawn_pos][2]);
		SetPlayerInterior(playerid, 0);
		SetPlayerVirtualWorld(playerid, 0);
	}
	else if(pInfo[cmdParam][player_spawn_type] == SPAWN_TYPE_HOUSE)
	{
		new spawn = pInfo[cmdParam][player_spawn];
		new d_id = GetDoorByUid(spawn);
		if(d_id != -1)
		{
			SetPlayerPos(cmdParam, Door[d_id][door_spawn_pos][0], Door[d_id][door_spawn_pos][1], Door[d_id][door_spawn_pos][2]);
			SetPlayerVirtualWorld(cmdParam, Door[d_id][door_spawn_vw]);
			SetPlayerInterior(cmdParam, Door[d_id][door_spawn_int]);
		}
	}
	else if(pInfo[cmdParam][player_spawn_type] == SPAWN_TYPE_HOTEL)
	{
		new spawn = pInfo[cmdParam][player_spawn];
		new d_id = GetDoorByUid(spawn);
		if(d_id != -1)
		{
			SetPlayerPos(cmdParam, Door[d_id][door_spawn_pos][0], Door[d_id][door_spawn_pos][1], Door[d_id][door_spawn_pos][2]);
			SetPlayerVirtualWorld(cmdParam, Door[d_id][door_spawn_vw]);
			SetPlayerInterior(cmdParam, Door[d_id][door_spawn_int]);
		}
	}

	FreezePlayer(cmdParam, 2000);

	if(cmdParam != playerid)
	{
		SendFormattedClientMessage(playerid, COLOR_GOLD, "Zrestowa�e� gracza %s (ID: %d, UID: %d).", pInfo[cmdParam][player_name], cmdParam, pInfo[cmdParam][player_id]);
	}

	return 1;
}

COMMAND:goto(playerid, params[])
{
	return cmd_to(playerid, params);
}

COMMAND:to(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Wyst�pi� b��d!", ""COLOR_GUI_BLUE"Twoje konto nie posiada uprawnie� do skorzystania z tej komendy!");

	new cmdParam;
	if( sscanf(params, "u", cmdParam) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /to [id gracza/cz�� nicku]");
	if( (!IsPlayerConnected(cmdParam) || !pInfo[cmdParam][player_logged]) ) return SendClientMessage(playerid, COLOR_GREY, "Ten gracz nie jest zalogowany.");

	new
		Float:x,
		Float:y,
		Float:z;
	GetPlayerPos(cmdParam, x, y, z);

	RP_PLUS_SetPlayerPos(playerid, x+1, y, z);
	SetPlayerVirtualWorld(playerid, GetPlayerVirtualWorld(cmdParam));
	SetPlayerInterior(playerid, GetPlayerInterior(cmdParam));

	FreezePlayer(playerid, 2000);


	SendFormattedClientMessage(playerid, COLOR_GOLD, "Przeteleportowa�e� si� do gracza %s (ID: %d, UID: %d).", pInfo[cmdParam][player_name], cmdParam, pInfo[cmdParam][player_id]);

	return 1;
}

COMMAND:gethere(playerid, params[])
{
	return cmd_tm(playerid, params);
}

COMMAND:takecash(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Wyst�pi� b��d!", ""COLOR_GUI_BLUE"Twoje konto nie posiada uprawnie� do skorzystania z tej komendy!");

	new cmdParam, cash;
	if( sscanf(params, "ud", cmdParam, cash) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /takecash [id gracza/cz�� nicku] [kwota]0");
	if( (!IsPlayerConnected(cmdParam) || !pInfo[cmdParam][player_logged]) ) return SendClientMessage(playerid, COLOR_GREY, "Ten gracz nie jest zalogowany.");

	GivePlayerMoney(cmdParam, -cash);

	SendFormattedClientMessage(cmdParam, COLOR_GOLD, "Administrator %s zabra� Ci $%d.", pInfo[playerid][player_name], cash);
	SendFormattedClientMessage(playerid, COLOR_GOLD, "Zabra�e� $%d graczowi %s (ID: %d, UID: %d).", cash, pInfo[cmdParam][player_name], cmdParam, pInfo[cmdParam][player_id]);

	return 1;
}

COMMAND:tm(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Wyst�pi� b��d!", ""COLOR_GUI_BLUE"Twoje konto nie posiada uprawnie� do skorzystania z tej komendy!");

	new cmdParam;
	if( sscanf(params, "u", cmdParam) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /tm [id gracza/cz�� nicku]");
	if( (!IsPlayerConnected(cmdParam) || !pInfo[cmdParam][player_logged]) ) return SendClientMessage(playerid, COLOR_GREY, "Ten gracz nie jest zalogowany.");

	new
		Float:x,
		Float:y,
		Float:z;
	GetPlayerPos(playerid, x, y, z);

	RP_PLUS_SetPlayerPos(cmdParam, x, y, z);
	SetPlayerVirtualWorld(cmdParam, GetPlayerVirtualWorld(playerid));
	SetPlayerInterior(cmdParam, GetPlayerInterior(playerid));

	FreezePlayer(cmdParam, 2000);

	SendFormattedClientMessage(cmdParam, COLOR_GOLD, "Administrator %s przeteleportowa� Cie do siebie.", pInfo[playerid][player_name]);
	SendFormattedClientMessage(playerid, COLOR_GOLD, "Przeteleportowa�e� do siebie gracza %s (ID: %d, UID: %d).", pInfo[cmdParam][player_name], cmdParam, pInfo[cmdParam][player_id]);

	return 1;
}

COMMAND:tp(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Wyst�pi� b��d!", ""COLOR_GUI_BLUE"Twoje konto nie posiada uprawnie� do skorzystania z tej komendy!");

	new cmdParam[2];
	if( sscanf(params, "a<i>[2]", cmdParam) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /tp [kogo] [do kogo]");
	if( (!IsPlayerConnected(cmdParam[0]) || !pInfo[cmdParam[0]][player_logged]) || (!IsPlayerConnected(cmdParam[1]) || !pInfo[cmdParam[1]][player_logged]) ) return SendClientMessage(playerid, COLOR_GREY, "Jeden z graczy nie jest zalogowany.");

	new
		Float:x,
		Float:y,
		Float:z;
	GetPlayerPos(cmdParam[1], x, y, z);

	RP_PLUS_SetPlayerPos(cmdParam[0], x, y, z);
	SetPlayerVirtualWorld(cmdParam[0], GetPlayerVirtualWorld(cmdParam[1]));
	SetPlayerInterior(cmdParam[0], GetPlayerInterior(cmdParam[1]));

	FreezePlayer(cmdParam[0], 2500);

	if( cmdParam[0] == playerid )
	{
		SendFormattedClientMessage(cmdParam[0], COLOR_GOLD, "Przeteleportowa�e� si� do gracza %s (ID: %d, UID: %d).", pInfo[cmdParam[1]][player_name], cmdParam[1], pInfo[cmdParam[1]][player_id]);
	}
	else
	{
		SendFormattedClientMessage(cmdParam[0], COLOR_GOLD, "%s przeteleportowa� Cie do gracza %s (ID: %d, UID: %d).", pInfo[playerid][player_name], pInfo[cmdParam[1]][player_name], cmdParam[1], pInfo[cmdParam[1]][player_id]);
	}

	return 1;
}

COMMAND:aspawn(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Wyst�pi� b��d!", ""COLOR_GUI_BLUE"Twoje konto nie posiada uprawnie� do skorzystania z tej komendy!");

	new sub_cmd[40], param[150];
	if( sscanf(params, "s[40] S()[150]", sub_cmd, param) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aspawn [stworz/usun]");

	if( !strcmp(sub_cmd, "stworz", false) )
	{
		new type;
		if( sscanf(param, "d", type) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aspawn stworz [typ]");

		if(type <1 || type > 2) return SendClientMessage(playerid, COLOR_GREY, "Typy spawn�w: 1(Market), 2(Idlewood)");

		new Float:pPos[4];

		GetPlayerPos(playerid, pPos[0], pPos[1], pPos[2]);
		GetPlayerFacingAngle(playerid, pPos[3]);

		mysql_query(mySQLconnection, sprintf("INSERT INTO `ipb_gspawns` (gspawn_uid, gspawn_posx, gspawn_posy, gspawn_posz, gspawn_posa, gspawn_type) VALUES (null, %f, %f, %f, %f, %d)", pPos[0], pPos[1], pPos[2], pPos[3], type));

		new id = cache_insert_id();
		new i;

		if(type == SPAWN_TYPE_GLOBAL)
		{
		 	i = Iter_Free(GlobalSpawns);
		}
		else
		{
			i = Iter_Free(ScSpawns);
		}

		GlobalSpawn[i][gspawn_id] = id;
		GlobalSpawn[i][gspawn_pos][0] = pPos[0];
		GlobalSpawn[i][gspawn_pos][1] = pPos[1];
		GlobalSpawn[i][gspawn_pos][2] = pPos[2];
		GlobalSpawn[i][gspawn_pos][3] = pPos[3];

		if(type == SPAWN_TYPE_GLOBAL)
		{
		 	Iter_Add(GlobalSpawns, i);
		 	SendFormattedClientMessage(playerid, COLOR_GOLD, "Dodano nowy globalny spawn (Market) (ID: %d).", i);
		}
		else
		{
			Iter_Add(ScSpawns, i);
			SendFormattedClientMessage(playerid, COLOR_GOLD, "Dodano nowy globalny spawn (South Central) (ID: %d).", i);
		}
	}
	else if( !strcmp(sub_cmd, "usun") )
	{
		new id;
		if( sscanf(param, "d", id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aspawn usun [id spawnu]");
		if( id < 0 || id >= MAX_GLOBAL_SPAWNS ) return SendClientMessage(playerid, COLOR_GREY, "Niepoprawne id spawnu.");
		if( !Iter_Contains(GlobalSpawns, id) ) return SendClientMessage(playerid, COLOR_GREY, "Niepoprawne id spawnu.");

		mysql_query(mySQLconnection, sprintf("DELETE FROM `ipb_gspawns` WHERE `gspawn_uid` = %d", GlobalSpawn[id][gspawn_id]));

		for(new z=0; e_global_spawns:z != e_global_spawns; z++)
		{
			GlobalSpawn[id][e_global_spawns:z] = 0;
		}

		Iter_Remove(GlobalSpawns, id);

		SendFormattedClientMessage(playerid, COLOR_GOLD, "Usuni�to globalny spawn (ID: %d).", id);
	}
	else return SendClientMessage(playerid, COLOR_GREY, "Tip: /aspawn [stworz/usun]");

	return 1;
}

COMMAND:glob(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) && !HasCrewFlag(playerid, CREW_FLAG_GM) ) return SendGuiInformation(playerid, ""guiopis"Wyst�pi� b��d!", ""COLOR_GUI_BLUE"Twoje konto nie posiada uprawnie� do skorzystania z tej komendy!");

	new text[290];
	if( sscanf(params, "s[290]", text) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /glob [tekst]");

	format(text, sizeof(text), "[[ %s: %s ]]", pInfo[playerid][player_name], BeautifyString(text));
	foreach(new p : Player)
	{
		if( !pInfo[p][player_logged] ) continue;
		SendClientMessage(p, COLOR_WHITE, text);
	}

	return 1;
}

COMMAND:ac(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Nie masz uprawnie� do korzystania z tej komendy.");
	if( isnull(params) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /ac [tekst]");

	new text[200];
	if( HasCrewFlag(playerid, CREW_FLAG_GM) ) format(text, sizeof(text), "(( [AC] %s%s{EFE7AD}: %s ))", HEX_COLOR_CREW_GM, pInfo[playerid][player_name], params);
	if( HasCrewFlag(playerid, CREW_FLAG_ADMIN) ) format(text, sizeof(text), "(( [AC] %s%s{EFE7AD}: %s ))", HEX_COLOR_CREW_ADMIN, pInfo[playerid][player_name], params);
	if( HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) format(text, sizeof(text), "(( [AC] %s%s{EFE7AD}: %s ))", HEX_COLOR_CREW_ADMIN_ROOT, pInfo[playerid][player_name], params);

	foreach(new p : Player)
	{
		if( !pInfo[p][player_logged] ) continue;
		if( !HasCrewFlag(p, CREW_FLAG_GM) && !HasCrewFlag(p, CREW_FLAG_ADMIN) && !HasCrewFlag(p, CREW_FLAG_ADMIN_ROOT) ) continue;

		SendClientMessage(p, 0xEFE7ADFF, text);
	}

	return 1;
}

COMMAND:hp(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Nie masz uprawnie� do korzystania z tej komendy.");
	new targetid, health;
	if( sscanf(params, "ud", targetid, health) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /hp [id gracza/czesc nicku] [hp]");
	if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Gracz o podanym id nie jest pod��czony.");
	if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Gracz o podanym id nie jest zalogowany.");
	if( health < 0 ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /hp [id gracza] [hp]");
	if( health == 0) return cmd_bw(playerid, sprintf("%d 5", targetid));

	SetPlayerHealth(targetid, health);
	SendFormattedClientMessage(targetid, COLOR_LIGHTER_RED, "Administrator %s ustawi� Ci %d hp.", pInfo[playerid][player_name], health);
	if( playerid != targetid ) SendFormattedClientMessage(playerid, COLOR_LIGHTER_RED, "Ustawi�e� graczowi %s (UID: %d, ID: %d) %d zycia.", pInfo[targetid][player_name], pInfo[targetid][player_id], targetid, health);

	return 1;
}

COMMAND:ad(playerid, params[])
{
	return cmd_adrzwi(playerid, params);
}

COMMAND:adrzwi(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_DOORS) ) return SendGuiInformation(playerid, ""guiopis"Wyst�pi� b��d!", ""COLOR_GUI_BLUE"Twoje konto nie posiada uprawnie� do skorzystania z tej komendy!");

	new sub_cmd[40], param[150];
	if( sscanf(params, "s[40] S()[150]", sub_cmd, param) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi [stworz/usun/pickup/czynsz/mieszkanie/mapicon/goto/przypisz/interior/wejscie/exitvw/objects/przeladuj]");

	if( !strcmp(sub_cmd, "stworz", false) )
	{
		new name[50];
		if( sscanf(param, "s[50]", name) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi stworz [nazwa]");

		new Float:pPos[4];

		GetPlayerPos(playerid, pPos[0], pPos[1], pPos[2]);
		GetPlayerFacingAngle(playerid, pPos[3]);
		mysql_real_escape_string(name, name);

		new str[400];
		strcat(str, "INSERT INTO ipb_doors (door_uid, door_owner, door_ownertype, door_name, door_type, door_pickupid, door_enterx, door_entery, door_enterz, door_entera, door_entervw, door_enterint, door_exitx, door_exity, door_exitz, door_exita) ");
		strcat(str, sprintf("VALUES (null, 0, 0, '%s', 1, 1239, %f, %f, %f, %f, %d, %d, %f, %f, %f, %f)", name, pPos[0], pPos[1], pPos[2], pPos[3], GetPlayerVirtualWorld(playerid), GetPlayerInterior(playerid), pPos[0], pPos[1], pPos[2], pPos[3]));

		mysql_query(mySQLconnection, str);
		new uid = cache_insert_id();

		new did = LoadDoor(sprintf("WHERE `door_uid` = %d", uid), true);

		SendGuiInformation(playerid, ""guiopis"Powiadomienie", sprintf("Drzwi zosta�y utworzone. [UID: %d, ID: %d]", uid, did));
	}
	else if( !strcmp(sub_cmd, "id", false) )
	{
		new d_uid;
		if( sscanf(param, "d", d_uid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi id [uid drzwi]");
		new d_id = GetDoorByUid(d_uid);
		if(d_id == -1) return SendGuiInformation(playerid, "Informacja", "Takie drzwi nie istniej�.");
		SendClientMessage(playerid, COLOR_GOLD, sprintf("SAMPid drzwi o UID %d to: %d", d_uid, d_id));
	}
	else if( !strcmp(sub_cmd, "mieszkanie", false) )
	{
		new d_id;
		if( sscanf(param, "d", d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi mieszkanie [id drzwi]");
		if( !Iter_Contains(Doors, d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi mieszkanie [id drzwi]");

		if(Door[d_id][door_rentable] == 0)
		{
			SendGuiInformation(playerid, ""guiopis"Powiadomienie", "W��czy�e� funkcje wynaj�cia tych drzwi.");
			Door[d_id][door_rentable]=1;
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_doors` SET `door_rentable` = 1 WHERE `door_uid` = %d", Door[d_id][door_uid]));
		}
		else
		{
			SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Wy��cz�e� drzwi z funkcji wynaj�cia.");
			Door[d_id][door_rentable]=0;
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_doors` SET `door_rentable` = 0 WHERE `door_uid` = %d", Door[d_id][door_uid]));
		}
	}
	else if( !strcmp(sub_cmd, "czynsz", false) )
	{
		new d_id, czynsz;
		if( sscanf(param, "dd", d_id, czynsz) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi czynsz [id drzwi] [kwota]");
		if( !Iter_Contains(Doors, d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi czynsz [id drzwi] [kwota]");

		if(Door[d_id][door_rentable] == 1)
		{
			Door[d_id][door_rent]=czynsz;
			SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Nowy czynsz zosta� ustalony.");
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_doors` SET `door_rent` = %d WHERE `door_uid` = %d", czynsz, Door[d_id][door_uid]));

		}
		else
		{
			SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Nie mo�esz ustali� czynszu dla mieszkania kt�re nie ma w��cznej funkcji wynaj�cia.");
		}
	}
	else if( !strcmp(sub_cmd, "metry", false) )
	{
		new d_id, czynsz;
		if( sscanf(param, "dd", d_id, czynsz) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi metry [id drzwi] [ilo��]");
		if( !Iter_Contains(Doors, d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi metry [id drzwi] [ilo��]");

		Door[d_id][door_meters]=czynsz;
		SendGuiInformation(playerid, ""guiopis"Powiadomienie", sprintf("Drzwi maj� teraz %d m2.", czynsz));
		mysql_query(mySQLconnection, sprintf("UPDATE `ipb_doors` SET `door_meters` = %d WHERE `door_uid` = %d", czynsz, Door[d_id][door_uid]));
	}
	else if( !strcmp(sub_cmd, "usun", false) )
	{
		new d_id;
		if( sscanf(param, "d", d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi usun [id drzwi]");
		if( !Iter_Contains(Doors, d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi usun [id drzwi]");

		mysql_query(mySQLconnection, sprintf("UPDATE ipb_characters SET char_spawn = 0, char_spawn_type = 1 WHERE char_spawn_type = %d AND char_spawn = %d", pInfo[playerid][player_spawn_type] == SPAWN_TYPE_HOUSE, Door[d_id][door_uid]));

		SendGuiInformation(playerid, ""guiopis"Powiadomienie", sprintf("Usuni�to drzwi o UID %d.", Door[d_id][door_uid]));
		printf("[ADMIN] %s (UID %d) skasowa� drzwi UID %d. (%s)", pInfo[playerid][player_name], pInfo[playerid][player_id], Door[d_id][door_uid], Door[d_id][door_name]);
		DeleteDoor(d_id);
	}
	else if( !strcmp(sub_cmd, "pickup") )
	{
		new d_id;
		if( sscanf(param, "d", d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi pickup [id drzwi]");
		if( !Iter_Contains(Doors, d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi pickup [id drzwi]");

		new str[512];

		DynamicGui_Init(playerid);
		DynamicGui_SetDialogValue(playerid, d_id);

		format(str, sizeof(str), "%s01\tSerce (1240)\n", str);
		DynamicGui_AddRow(playerid, 0, 1240);

		format(str, sizeof(str), "%s02\tKamizelka (1242)\n", str);
		DynamicGui_AddRow(playerid, 0, 1242);

		format(str, sizeof(str), "%s03\tLiterka 'i' (1239)\n", str);
		DynamicGui_AddRow(playerid, 0, 1239);

		format(str, sizeof(str), "%s04\tNiebieski domek (1272)\n", str);
		DynamicGui_AddRow(playerid, 0, 1272);

		format(str, sizeof(str), "%s05\tZielony domek (1273)\n", str);
		DynamicGui_AddRow(playerid, 0, 1273);

		format(str, sizeof(str), "%s06\tPlik got�wki (1212)\n", str);
		DynamicGui_AddRow(playerid, 0, 1212);

		format(str, sizeof(str), "%s07\tPigulka (1241)\n", str);
		DynamicGui_AddRow(playerid, 0, 1241);

		format(str, sizeof(str), "%s08\tOdznaka (1247)\n", str);
		DynamicGui_AddRow(playerid, 0, 1247);

		format(str, sizeof(str), "%s09\tDolar (1274)\n", str);
		DynamicGui_AddRow(playerid, 0, 1274);

		format(str, sizeof(str), "%s10\tNiebieska koszulka (1275)\n", str);
		DynamicGui_AddRow(playerid, 0, 1275);

		format(str, sizeof(str), "%s11\tSztanga (3072)\n", str);
		DynamicGui_AddRow(playerid, 0, 3072);

		format(str, sizeof(str), "%s12\tLudzik (1314)\n", str);
		DynamicGui_AddRow(playerid, 0, 1314);

		format(str, sizeof(str), "%s13\tTabliczka Sell (19470)\n", str);
		DynamicGui_AddRow(playerid, 0, 19470);

		format(str, sizeof(str), "%s14\tM�otek (18635)\n", str);
		DynamicGui_AddRow(playerid, 0, 18635);

		format(str, sizeof(str), "%s15\tP�yta Radia (1961)\n", str);
		DynamicGui_AddRow(playerid, 0, 1961);

		format(str, sizeof(str), "%s16\tCzerwony domek (19522)\n", str);
		DynamicGui_AddRow(playerid, 0, 19522);

		format(str, sizeof(str), "%s17\t��ty domek (19524)\n", str);
		DynamicGui_AddRow(playerid, 0, 19524);

		format(str, sizeof(str), "%s18\tWalizka (19624)\n", str);
		DynamicGui_AddRow(playerid, 0, 19624);

		format(str, sizeof(str), "%s19\tDyskietka (1277)\n", str);
		DynamicGui_AddRow(playerid, 0, 1277);

		ShowPlayerDialog(playerid, DIALOG_ADRZWI_PICKUP, DIALOG_STYLE_LIST, sprintf("Drzwi %d (%s; SampID: %d)", Door[d_id][door_uid], Door[d_id][door_name], d_id), str, "Wybierz", "Zamknij");
	}
	else if( !strcmp(sub_cmd, "mapicon") )
	{
		new d_id, model;
		if( sscanf(param, "dd", d_id, model) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi mapicon [id drzwi] [model map ikony(-1 aby usunac)]");
		if( !Iter_Contains(Doors, d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi pickup [model map ikony(-1 aby usunac)]");

		mysql_query(mySQLconnection, sprintf("UPDATE `ipb_doors` SET `door_mapmodel` = %d WHERE `door_uid` = %d", model, Door[d_id][door_uid]));

		new uid = Door[d_id][door_uid];
		DeleteDoor(d_id, false);

		new did = LoadDoor(sprintf("WHERE `door_uid` = %d", uid), true);
		SendGuiInformation(playerid, ""guiopis"Powiadomienie", sprintf("Ikona mapy drzwi zosta�a pomy�lnie zmieniona! [ICON: %d, UID: %d, ID: %d]", model, uid, did));
	}
	else if( !strcmp(sub_cmd, "goto") )
	{
		new d_id;
		if( sscanf(param, "d", d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi goto [id drzwi]");
		if( !Iter_Contains(Doors, d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi goto [id drzwi]");

		FreezePlayer(playerid, 2500);

		RP_PLUS_SetPlayerPos(playerid, Door[d_id][door_pos][0], Door[d_id][door_pos][1], Door[d_id][door_pos][2]);
		SetPlayerFacingAngle(playerid, Door[d_id][door_pos][3]);

		SetCameraBehindPlayer(playerid);

		SetPlayerVirtualWorld(playerid, Door[d_id][door_vw]);
		SetPlayerInterior(playerid, Door[d_id][door_int]);

		SendGuiInformation(playerid, ""guiopis"Powiadomienie", sprintf("Teleportowa�e� sie do drzwi [UID: %d, ID: %d].", Door[d_id][door_uid], d_id));
	}
	else if( !strcmp(sub_cmd, "przypisz") )
	{
		new d_id, action[32], who;
		if( sscanf(param, "ds[32]d", d_id, action, who) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi przypisz [id drzwi] [grupa/gracz/globalne] [uid grupy/id gracza(nie uid)]");
		if( !Iter_Contains(Doors, d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi przypisz [id drzwi] [grupa/gracz] [uid grupy/id gracza(nie uid)]");

		if( !strcmp(action, "grupa") )
		{
			new gid = GetGroupByUid(who);
			if( gid == -1 ) return SendClientMessage(playerid, COLOR_GREY, "Grupa o podanym uid nie istnieje.");

			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_doors` SET `door_ownertype` = %d, `door_owner` = %d WHERE `door_uid` = %d", DOOR_OWNER_TYPE_GROUP, who, Door[d_id][door_uid]));

			Door[d_id][door_owner_type] = DOOR_OWNER_TYPE_GROUP;
			Door[d_id][door_owner] = who;

			SendGuiInformation(playerid, ""guiopis"Powiadomienie", sprintf("Pomy�lnie zmieni�e� wlasciciela drzwi [UID: %d, ID: %d] na grupe %s [UID: %d, ID: %d].", Door[d_id][door_uid], d_id, Group[gid][group_name], Group[gid][group_uid], gid));
		}
		else if( !strcmp(action, "gracz") )
		{
			if( !IsPlayerConnected(who) ) return SendClientMessage(playerid, COLOR_GREY, "Gracz o podanym id nie jest pod��czony.");

			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_doors` SET `door_ownertype` = %d, `door_owner` = %d WHERE `door_uid` = %d", DOOR_OWNER_TYPE_PLAYER, pInfo[who][player_id], Door[d_id][door_uid]));

			Door[d_id][door_owner_type] = DOOR_OWNER_TYPE_PLAYER;
			Door[d_id][door_owner] = pInfo[who][player_id];

			SendGuiInformation(playerid, ""guiopis"Powiadomienie", sprintf("Pomy�lnie zmieni�e� wlasciciela drzwi [UID: %d, ID: %d] na gracza %s [UID: %d, ID: %d].", Door[d_id][door_uid], d_id, pInfo[who][player_name], pInfo[who][player_id], who));
		}
		else if( !strcmp(action, "globalne") )
		{
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_doors` SET `door_ownertype` = %d, `door_owner` = 0 WHERE `door_uid` = %d", DOOR_OWNER_TYPE_GLOBAL, Door[d_id][door_uid]));

			Door[d_id][door_owner_type] = DOOR_OWNER_TYPE_GLOBAL;
			Door[d_id][door_owner] = 0;

			SendGuiInformation(playerid, ""guiopis"Powiadomienie", sprintf("Pomy�lnie zmieni�e� wlasciciela drzwi [UID: %d, ID: %d] na globalnego.", Door[d_id][door_uid], d_id));
		}
		else return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi przypisz [id drzwi] [grupa/postac] [uid grupy/id gracza(nie uid)]");
	}
	else if( !strcmp(sub_cmd, "interior") )
	{
		new d_id;
		if( sscanf(param, "d", d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi interior [id drzwi]");
		if( !Iter_Contains(Doors, d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi interior [id drzwi]");

		DoorsDefaultInteriorsList(playerid, d_id);
	}
	else if( !strcmp(sub_cmd, "wejscie") )
	{
		new d_id;
		if( sscanf(param, "d", d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi wejscie [id drzwi]");
		if( !Iter_Contains(Doors, d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi wejscie [id drzwi]");

		GetPlayerPos(playerid, Door[d_id][door_pos][0], Door[d_id][door_pos][1], Door[d_id][door_pos][2]);
		GetPlayerFacingAngle(playerid, Door[d_id][door_pos][3]);

		mysql_query(mySQLconnection, sprintf("UPDATE `ipb_doors` SET `door_enterx` = %f, `door_entery` = %f, `door_enterz` = %f, `door_entera` = %f, door_entervw = %d WHERE `door_uid` = %d", Door[d_id][door_pos][0], Door[d_id][door_pos][1], Door[d_id][door_pos][2], Door[d_id][door_pos][3], GetPlayerVirtualWorld(playerid), Door[d_id][door_uid]));

		new uid = Door[d_id][door_uid];
		DeleteDoor(d_id, false);

		new did = LoadDoor(sprintf("WHERE `door_uid` = %d", uid), true);

		SendGuiInformation(playerid, ""guiopis"Powiadomienie", sprintf("Pozycja zewnetrznych drzwi zosta�a pomy�lnie zmieniona [UID: %d, ID: %d].", Door[d_id][door_uid], did));
	}
	else if( !strcmp(sub_cmd, "wyjscie") )
	{
		new d_id;
		if( sscanf(param, "d", d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi wyjscie [id drzwi]");
		if( !Iter_Contains(Doors, d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi wyjscie [id drzwi]");

		GetPlayerPos(playerid, Door[d_id][door_spawn_pos][0], Door[d_id][door_spawn_pos][1], Door[d_id][door_spawn_pos][2]);
		GetPlayerFacingAngle(playerid, Door[d_id][door_spawn_pos][3]);

		mysql_query(mySQLconnection, sprintf("UPDATE `ipb_doors` SET `door_exitx` = %f, `door_exity` = %f, `door_exitz` = %f, `door_exita` = %f WHERE `door_uid` = %d", Door[d_id][door_spawn_pos][0], Door[d_id][door_spawn_pos][1], Door[d_id][door_spawn_pos][2], Door[d_id][door_spawn_pos][3], Door[d_id][door_uid]));

		new uid = Door[d_id][door_uid];
		DeleteDoor(d_id, false);

		new did = LoadDoor(sprintf("WHERE `door_uid` = %d", uid), true);

		SendGuiInformation(playerid, ""guiopis"Powiadomienie", sprintf("Pozycja wewnetrznych drzwi zosta�a pomy�lnie zmieniona [UID: %d, ID: %d].", Door[d_id][door_uid], did));
	}
	else if( !strcmp(sub_cmd, "objects") )
	{
		new d_id, limit;
		if( sscanf(param, "dd", d_id, limit) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi objects [id drzwi] [limit obiekt�w]");
		if( !Iter_Contains(Doors, d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi objects [id drzwi] [limit obiekt�w]");
		if( limit < 0 ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi objects [id drzwi] [limit obiekt�w]");

		mysql_query(mySQLconnection, sprintf("UPDATE `ipb_doors` SET `door_objects` = %d WHERE `door_uid` = %d", limit, Door[d_id][door_uid]));

		Door[d_id][door_objects_limit] = limit;

		SendGuiInformation(playerid, ""guiopis"Powiadomienie", sprintf("Pomy�lnie Zmieni�e� limit obiekt�w drzwi [LIMIT: %d, UID: %d, ID: %d].", limit, Door[d_id][door_uid], d_id));
	}
	else if( !strcmp(sub_cmd, "przeladuj") )
	{
		Code_ExTimer_Begin(DoorsReload);
		for(new d_id;d_id<MAX_DOORS;d_id++)
		{
			if( Iter_Contains(Doors, d_id) ) DeleteDoor(d_id, false);
		}

		new count = LoadDoor();

		SendGuiInformation(playerid, ""guiopis"Powiadomienie", sprintf("Pomy�lnie prze�adowa�e� wszystkie drzwi [ilo��: %d, Czas: %d ms].", count, Code_ExTimer_End(DoorsReload)));
	}
	else if( !strcmp(sub_cmd, "exitvw") )
	{
		new d_id, vw;
		if( sscanf(param, "dd", d_id, vw) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi exitvw [id drzwi] [vw]");
		if( !Iter_Contains(Doors, d_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi exitvw [id drzwi] [vw]");
		if( vw < 0 ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi exitvw [id drzwi] [vw]");

		mysql_query(mySQLconnection, sprintf("UPDATE `ipb_doors` SET `door_exitvw` = %d WHERE `door_uid` = %d", vw, Door[d_id][door_uid]));

		new uid = Door[d_id][door_uid];
		DeleteDoor(d_id, false);

		new did = LoadDoor(sprintf("WHERE `door_uid` = %d", uid), true);

		SendGuiInformation(playerid, ""guiopis"Powiadomienie", sprintf("Pomy�lnie zmieni�e� wewnetrzny virtual world drzwi [EXITVW: %d, UID: %d, ID: %d]", vw, Door[did][door_uid], did));
	}
	else return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ad)rzwi [stworz/usun/pickup/mapicon/goto/przypisz/interior/wejscie/exitvw/objects/przeladuj]");
	return 1;
}

COMMAND:strefa(playerid, params[])
{
	new sub_cmd[40], param[150];
	if( sscanf(params, "s[40] S()[150]", sub_cmd, param) )
	{
		if( !HasCrewFlag(playerid, CREW_FLAG_AREAS) )
		{
			SendClientMessage(playerid, COLOR_GREY, "Tip: /strefa [audio]");
		}
		else
		{
			SendClientMessage(playerid, COLOR_GREY, "Tip: /strefa [stworz/usun/przypisz/podglad/obiekty/flagi/goto/cena/metry]");
		}
		return 1;
	}

	if( !strcmp(sub_cmd, "stworz") )
	{
		if( !HasCrewFlag(playerid, CREW_FLAG_AREAS) ) return SendGuiInformation(playerid, "Informacja", "Brak uprawnie� do u�ycia tej komendy.");
		if( pInfo[playerid][player_lookup_area] ) return SendClientMessage(playerid, COLOR_GREY, "Aktualnie podgl�dasz jak�� stref�, musisz najpierw wyl�czy� te opcje.");
		if( pInfo[playerid][player_creating_area] )
		{
			AreaCreator(playerid);
			return 1;
		}

		new type[32], Float:size;
		if( sscanf(param, "s[32]F(0.0)", type, size) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /strefa stworz [sfera/prostokat]");

		if( !strcmp(type, "sfera") )
		{
			if( size == 0.0 ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /strefa stworz sfera [wielkosc]");

			pInfo[playerid][player_carea_size] = size;
			pInfo[playerid][player_carea_type] = AREA_SHAPE_CIRCLE;
			pInfo[playerid][player_creating_area] = true;
			SendPlayerInformation(playerid, "Aby dodac punkt strefy wcisnij ~r~RMB~w~ lub ~r~LMB ~w~aby usunac ostatnio dodany punkt. ~n~Aby anulowac tworzenie strefy wcisnij ~g~LALT + SPACE~w~.~n~~n~~y~Aby stworzyc stref� uzyj ponownie /strefa stworz.", 30000);
		}
		else if( !strcmp(type, "prostokat") )
		{
			pInfo[playerid][player_carea_type] = AREA_SHAPE_SQUARE;
			pInfo[playerid][player_creating_area] = true;
			SendPlayerInformation(playerid, "Aby dodac punkt strefy wcisnij ~r~RMB~w~ lub ~r~LMB ~w~aby usunac ostatnio dodany punkt. ~n~Aby anulowac tworzenie strefy wcisnij ~g~LALT + SPACE~w~.~n~~n~~y~Aby stworzyc stref� uzyj /strefa stworz.", 30000);
		}
		else return SendClientMessage(playerid, COLOR_GREY, "Tip: /strefa stworz [sfera/prostokat]");
	}
	else if( !strcmp(sub_cmd, "cena") )
	{
		if( !HasCrewFlag(playerid, CREW_FLAG_AREAS) ) return SendGuiInformation(playerid, "Informacja", "Brak uprawnie� do u�ycia tej komendy.");
		new a_id, price;
		if( sscanf(param, "dd", a_id, price) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /strefa cena [id strefy] [cena za metr kwadratowy]");
		if( !Iter_Contains(Areas, a_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /strefa cena [id strefy] [cena za metr kwadratowy]");
		if( Area[a_id][area_type] != AREA_TYPE_NORMAL ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /strefa cena [id strefy] [cena za metr kwadratowy]");

		Area[a_id][area_price] = price;
		mysql_query(mySQLconnection, sprintf("UPDATE ipb_areas SET area_price = %d WHERE area_uid = %d", price, Area[a_id][area_uid]));
		SendGuiInformation(playerid, "Informacja", sprintf("Cena za metr kwadratowy zosta�a ustawiona na $%d.", price));
	}
	else if( !strcmp(sub_cmd, "metry") )
	{
		if( !HasCrewFlag(playerid, CREW_FLAG_AREAS) ) return SendGuiInformation(playerid, "Informacja", "Brak uprawnie� do u�ycia tej komendy.");
		new a_id, price;
		if( sscanf(param, "dd", a_id, price) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /strefa metry [id strefy] [minimalna ilo�� metr�w kwadratowych]");
		if( !Iter_Contains(Areas, a_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /strefa metry [id strefy] [minimalna ilo�� metr�w kwadratowych]");
		if( Area[a_id][area_type] != AREA_TYPE_NORMAL ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /strefa metry [id strefy] [minimalna ilo�� metr�w kwadratowych]");

		Area[a_id][area_meters] = price;
		mysql_query(mySQLconnection, sprintf("UPDATE ipb_areas SET area_meters = %d WHERE area_uid = %d", price, Area[a_id][area_uid]));
		SendGuiInformation(playerid, "Informacja", sprintf("Minimalna ilo�� metr�w kwadratowych zosta�a ustawiona na %d.", price));
	}
	else if( !strcmp(sub_cmd, "audio") )
	{
		new audio_stream[100], a_id;
		if( sscanf(param, "s[100]d", audio_stream, a_id) )
		{
			SendClientMessage(playerid, COLOR_GREY, "Tip: /strefa audio [id strefy] [link]");
			SendClientMessage(playerid, COLOR_GREY, "Tip: Wpisz znak pauzy aby usun�� stream audio ze strefy.");
			return 1;
		}
		if(!CanPlayerEditArea(playerid, a_id)) return SendGuiInformation(playerid, "Informacja", "Brak uprawnie� do zarz�dzania t� stref�.");
		sscanf(audio_stream, "s[100]", Area[a_id][area_audio]);
		SendGuiInformation(playerid, "Informacja", "Strumie� audio zosta� pomy�lnie ustawiony.");
	}
	else if( !strcmp(sub_cmd, "usun") )
	{
		if( !HasCrewFlag(playerid, CREW_FLAG_AREAS) ) return SendGuiInformation(playerid, "Informacja", "Brak uprawnie� do u�ycia tej komendy.");
		new a_id;
		if( sscanf(param, "d", a_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /strefa usun [id strefy]");
		if( !Iter_Contains(Areas, a_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /strefa usun [id strefy]");
		if( Area[a_id][area_type] != AREA_TYPE_NORMAL ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /strefa usun [id strefy]");

		ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Informacja", sprintf("Pomy�lnie usun��e� stref� [UID: %d, ID: %d].", Area[a_id][area_uid], a_id), "OK", "");

		DeleteArea(a_id);
	}
	else if( !strcmp(sub_cmd, "przypisz") )
	{
		if( !HasCrewFlag(playerid, CREW_FLAG_AREAS) ) return SendGuiInformation(playerid, "Informacja", "Brak uprawnie� do u�ycia tej komendy.");
		new a_id, action[32], who;
		if( sscanf(param, "ds[32]d", a_id, action, who) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /strefa przypisz [id strefy] [grupa/globalna] [uid grupy]");
		if( !Iter_Contains(Areas, a_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /strefa przypisz [id strefy] [grupa/globalne] [uid grupy]");
		if( Area[a_id][area_type] != AREA_TYPE_NORMAL ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /strefa przypisz [id strefy] [grupa/globalne] [uid grupy]");

		if( !strcmp(action, "grupa") )
		{
			new gid = GetGroupByUid(who);
			if( gid == -1 ) return SendClientMessage(playerid, COLOR_GREY, "Grupa o podanym uid nie istnieje.");

			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_areas` SET `area_ownertype` = %d, `area_owner` = %d WHERE `area_uid` = %d", AREA_OWNER_TYPE_GROUP, who, Area[a_id][area_uid]));

			Area[a_id][area_owner_type] = AREA_OWNER_TYPE_GROUP;
			Area[a_id][area_owner] = who;

			ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Informacja", sprintf("Pomy�lnie zmieni�e� wlasciciela strefy [UID: %d, ID: %d] na grupe %s [UID: %d, ID: %d].", Area[a_id][area_uid], a_id, Group[gid][group_name], Group[gid][group_uid], gid), "OK", "");
		}
		else if( !strcmp(action, "globalna") )
		{
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_areas` SET `area_ownertype` = %d, `area_owner` = 0 WHERE `area_uid` = %d", AREA_OWNER_TYPE_GLOBAL, Area[a_id][area_uid]));

			Area[a_id][area_owner_type] = AREA_OWNER_TYPE_GLOBAL;
			Area[a_id][area_owner] = 0;

			ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Informacja", sprintf("Pomy�lnie zmieni�e� wlasciciela strefy [UID: %d, ID: %d] na globalnego.", Area[a_id][area_uid], a_id), "OK", "");
		}
		else return SendClientMessage(playerid, COLOR_GREY, "Tip: /strefa przypisz [id drzwi] [grupa] [uid grupy]");
	}
	else if( !strcmp(sub_cmd, "goto") )
	{
		if( !HasCrewFlag(playerid, CREW_FLAG_AREAS) ) return SendGuiInformation(playerid, "Informacja", "Brak uprawnie� do u�ycia tej komendy.");
		new a_id, Float:a_pos[8], Float:z_pos;
		if( sscanf(param, "d", a_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /strefa goto [id strefy]");
		if( !Iter_Contains(Areas, a_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /strefa goto [id strefy]");
		if( Area[a_id][area_type] != AREA_TYPE_NORMAL ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /strefa goto [id strefy]");

		GetDynamicPolygonPoints(a_id, a_pos);

		CA_FindZ_For2DCoord(a_pos[0], a_pos[1], z_pos);

		SetPlayerPos(playerid, a_pos[0], a_pos[1], z_pos);
	}
	else if( !strcmp(sub_cmd, "obiekty") )
	{
		if( !HasCrewFlag(playerid, CREW_FLAG_AREAS) ) return SendGuiInformation(playerid, "Informacja", "Brak uprawnie� do u�ycia tej komendy.");
		new a_id, limit;
		if( sscanf(param, "dd", a_id, limit) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /strefa obiekty [id strefy] [limit obiekt�w]");
		if( !Iter_Contains(Areas, a_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /strefa obiekty [id strefy] [limit obiekt�w]");
		if( limit < 0 ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /strefa obiekty [id strefy] [limit obiekt�w]");
		if( Area[a_id][area_type] != AREA_TYPE_NORMAL ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /strefa obiekty [id strefy] [limit obiekt�w]");

		mysql_query(mySQLconnection, sprintf("UPDATE `ipb_areas` SET `area_objects` = %d WHERE `area_uid` = %d", limit, Area[a_id][area_uid]));

		Area[a_id][area_objects_limit] = limit;

		ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Informacja", sprintf("Pomy�lnie zmieni�e� limit obiekt�w strefy [LIMIT: %d, UID: %d, ID: %d].", limit, Area[a_id][area_uid], a_id), "OK", "");
	}
	else if( !strcmp(sub_cmd, "flagi") )
	{
		if( !HasCrewFlag(playerid, CREW_FLAG_AREAS) ) return SendGuiInformation(playerid, "Informacja", "Brak uprawnie� do u�ycia tej komendy.");
		new a_id;
		if( sscanf(param, "d", a_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /strefa flagi [id strefy]");

		if( !Iter_Contains(Areas, a_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /strefa flagi [id strefy]");

		DynamicGui_Init(playerid);
		DynamicGui_SetDialogValue(playerid, a_id);
		new str[700];

		format(str, sizeof(str), "%s%s01\tZbyt narkotykow\n", str, ((AreaHasFlag(a_id, AREA_FLAG_CORNER)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, AREA_FLAG_CORNER);

		format(str, sizeof(str), "%s%s02\tBMX\n", str, ((AreaHasFlag(a_id, AREA_FLAG_BMX)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, AREA_FLAG_BMX);

		format(str, sizeof(str), "%s%s03\tMonitoring\n", str, ((AreaHasFlag(a_id, AREA_FLAG_MONITORING)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, AREA_FLAG_MONITORING);

		format(str, sizeof(str), "%s%s04\tDrive-thru\n", str, ((AreaHasFlag(a_id, AREA_FLAG_DRIVE)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, AREA_FLAG_DRIVE);

		format(str, sizeof(str), "%s%s05\tSadzenie ro�lin\n", str, ((AreaHasFlag(a_id, AREA_FLAG_PLANT)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, AREA_FLAG_PLANT);

		format(str, sizeof(str), "%s%s06\tOgraniczenie parkowania\n", str, ((AreaHasFlag(a_id, AREA_FLAG_PARKING)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, AREA_FLAG_PARKING);

		format(str, sizeof(str), "%s%s07\tOferowanie produkt�w\n", str, ((AreaHasFlag(a_id, AREA_FLAG_OFFER)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, AREA_FLAG_OFFER);

		format(str, sizeof(str), "%s%s08\tSerwis pojazd�w\n", str, ((AreaHasFlag(a_id, AREA_FLAG_SERWIS)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, AREA_FLAG_SERWIS);

		format(str, sizeof(str), "%s%s09\tPraca drwala\n", str, ((AreaHasFlag(a_id, AREA_FLAG_WORK)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, AREA_FLAG_WORK);

		format(str, sizeof(str), "%s%s10\tPraca rybaka\n", str, ((AreaHasFlag(a_id, AREA_FLAG_WORK_FISH)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, AREA_FLAG_WORK_FISH);

		format(str, sizeof(str), "%s%s11\tAnty Jobcar\n", str, ((AreaHasFlag(a_id, AREA_FLAG_LS)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, AREA_FLAG_LS);

		format(str, sizeof(str), "%s%s12\tGra w golfa\n", str, ((AreaHasFlag(a_id, AREA_FLAG_GOLF)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, AREA_FLAG_GOLF);

		format(str, sizeof(str), "%s%s13\tZ�omowanie pojazdu\n", str, ((AreaHasFlag(a_id, AREA_FLAG_ZLOM)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, AREA_FLAG_ZLOM);

		ShowPlayerDialog(playerid, DIALOG_AREA_FLAGS, DIALOG_STYLE_LIST, sprintf("Flagi strefy [UID: %d, ID: %d]", Area[a_id][area_uid], a_id), str, "Wybierz", "Zamknij");
	}
	else if( !strcmp(sub_cmd, "wyczysc") )
	{
		if( !HasCrewFlag(playerid, CREW_FLAG_AREAS) ) return SendGuiInformation(playerid, "Informacja", "Brak uprawnie� do u�ycia tej komendy.");
		new a_id, object_count;
		if( sscanf(param, "d", a_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /strefa wyczysc [id strefy]");
		if( !Iter_Contains(Areas, a_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /strefa wyczysc [id strefy]");
		if( Area[a_id][area_type] != AREA_TYPE_NORMAL ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /strefa wyczysc [id strefy]");

		foreach(new o_id : Objects)
		{
			if( Object[o_id][object_owner_type] != OBJECT_OWNER_TYPE_AREA ) continue;
			if( Object[o_id][object_owner] == Area[a_id][area_uid] )
			{
				mysql_query(mySQLconnection, sprintf("DELETE FROM `ipb_objects` WHERE `object_uid` = %d", Object[o_id][object_uid]));

				DestroyDynamicObject(o_id);

				for(new z=0; e_objects:z != e_objects; z++)
				{
			  		Object[o_id][e_objects:z] = 0;
			    }
			    object_count++;

			    Iter_Remove(Objects, o_id);
				new next = Iter_Next(Objects, o_id);
			    o_id = next;
			}
		}
		ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Informacja", sprintf("Pomy�lnie wyczy�ci�e� stref� z %d obiekt�w. [UID: %d, ID: %d].", object_count, Area[a_id][area_uid], a_id), "OK", "");
	}
	else if( !strcmp(sub_cmd, "podglad") )
	{
		if( !HasCrewFlag(playerid, CREW_FLAG_AREAS) ) return SendGuiInformation(playerid, "Informacja", "Brak uprawnie� do u�ycia tej komendy.");
		if( pInfo[playerid][player_lookup_area] )
		{
			pInfo[playerid][player_lookup_area] = false;
			GangZoneDestroy(pInfo[playerid][player_carea_zone]);

			if( IsValidDynamic3DTextLabel(pInfo[playerid][player_carea_label][0]) ) DestroyDynamic3DTextLabel(pInfo[playerid][player_carea_label][0]);
			if( IsValidDynamic3DTextLabel(pInfo[playerid][player_carea_label][1]) ) DestroyDynamic3DTextLabel(pInfo[playerid][player_carea_label][1]);

			TextDrawHideForPlayer(playerid, Dashboard[playerid]);
		}
		else
		{
			new a_id;
			if( sscanf(param, "d", a_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /strefa podglad [id strefy]");
			if( !Iter_Contains(Areas, a_id) ) return SendClientMessage(playerid, COLOR_GREY, "Strefa o podanym id nie istnieje.");
			if( pInfo[playerid][player_creating_area] ) return SendClientMessage(playerid, COLOR_GREY, "Nie mo�esz korzysta� z podgl�du strefy w trybie tworzenia strefy.");
			if( Area[a_id][area_type] != AREA_TYPE_NORMAL ) return SendClientMessage(playerid, COLOR_GREY, "Nie mo�esz edytowa� strefy tego typu.");

			pInfo[playerid][player_lookup_area] = true;
			new string[140];

			new owner_str[80];
			switch( Area[a_id][area_owner_type] )
			{
				case AREA_OWNER_TYPE_GLOBAL:
				{
					format(owner_str, sizeof(owner_str), "globalne; brak");
				}

				case AREA_OWNER_TYPE_GROUP:
				{
					new gid = GetGroupByUid(Area[a_id][area_owner]);
					format(owner_str, sizeof(owner_str), "grupowe; %s", Group[gid][group_name]);
				}
			}

			format(string, sizeof(string), "%d. Strefa (SampID: %d)~n~~n~~p~Owner: ~w~%d:%d (%s)    ~w~Obiektow: %d/%d", Area[a_id][area_uid], a_id, Area[a_id][area_owner_type], Area[a_id][area_owner], owner_str, CountAreaObjects(a_id), Area[a_id][area_objects_limit]);

			TextDrawSetString(Dashboard[playerid], string);
			TextDrawShowForPlayer(playerid, Dashboard[playerid]);

			new rows, fields;
			new Float:point1[3], Float:point2[3], shape;
			mysql_query(mySQLconnection, sprintf("SELECT area_shape, area_point1, area_point2 FROM `ipb_areas` WHERE `area_uid` = %d", Area[a_id][area_uid]));
			cache_get_data(rows, fields);

			new str[100];
			cache_get_row(0, 1, str);
			sscanf(str, "p<|>a<f>[3]", point1);

			new str2[100];
			cache_get_row(0, 2, str2);
			sscanf(str2, "p<|>a<f>[3]", point2);

			shape = cache_get_row_int(0, 0);

			if( shape == AREA_SHAPE_SQUARE  )
			{
				pInfo[playerid][player_carea_zone] = GangZoneCreate(Min(point1[0], point2[0]), Min(point1[1], point2[1]), Max(point1[0], point2[0]), Max(point1[1], point2[1]));
				GangZoneShowForPlayer(playerid, pInfo[playerid][player_carea_zone], 0xFF3C3C80);

				pInfo[playerid][player_carea_label][0] = CreateDynamic3DTextLabel(sprintf("Punkt pierwszy\n(%f, %f, %f)", point1[0], point1[1], point1[2]), COLOR_LIGHTER_RED, point1[0], point1[1], point1[2], 50.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 0, -1, -1, playerid);
				pInfo[playerid][player_carea_label][1] = CreateDynamic3DTextLabel(sprintf("Punkt drugi\n(%f, %f, %f)", point2[0], point2[1], point2[2]), COLOR_LIGHTER_RED, point2[0], point2[1], point2[2], 50.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 0, -1, -1, playerid);
			}
			else if( shape == AREA_SHAPE_CIRCLE )
			{
				pInfo[playerid][player_carea_label][0] = CreateDynamic3DTextLabel(sprintf("Punkt pierwszy\n(%f, %f, %f)", point1[0], point1[1], point1[2]), COLOR_LIGHTER_RED, point1[0], point1[1], point1[2], 50.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 0, -1, -1, playerid);
			}
		}
	}
	else return SendClientMessage(playerid, COLOR_GREY, "Tip: /strefa [stworz/usun/przypisz/podglad/obiekty/flagi]");
	return 1;
}

COMMAND:bw(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Nie masz uprawnie� do korzystania z tej komendy.");

	new time, targetid;
	if( sscanf(params, "ud", targetid, time) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /bw [id gracza/czesc nicku] [czas]");
	if( time < 0 ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /bw [id gracza] [czas]");
	if( !IsPlayerConnected(targetid) ) return SendClientMessage(playerid, COLOR_GREY, "Gracz o podanym id nie jest pod��czony.");
	if( !pInfo[targetid][player_logged] ) return SendClientMessage(playerid, COLOR_GREY, "Gracz o podanym id nie jest zalogowany.");

	if( time == 0 && pInfo[targetid][player_bw] > 0 )
	{
		ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Informacja", sprintf("Nada�e� graczowi %s [UID: %d, ID: %d] bw na czas: %d minut.", pInfo[targetid][player_name], pInfo[targetid][player_id], targetid, time), "OK", "");
		// koniec bw
		pInfo[targetid][player_bw] = 0;
		pInfo[targetid][player_bw_end_time] = 0;
		mysql_query(mySQLconnection, sprintf("UPDATE `ipb_characters` SET `char_bw` = 0 WHERE `char_uid` = %d", pInfo[targetid][player_id]));

		RemovePlayerStatus(targetid, PLAYER_STATUS_BW);
		SetPlayerHealth(targetid, 20);
		TogglePlayerControllable(targetid, true);
		SetCameraBehindPlayer(targetid);
		ClearAnimations(targetid);
		new skin = GetPlayerSkin(playerid);
		SetPlayerSkin(playerid, skin);
		SetPlayerSpecialAction(targetid, SPECIAL_ACTION_NONE);
		SetPlayerChatBubble(targetid, "", -1, 7.0, 5);
		return 1;
	}
	else if( time == 0 )
	{
		return SendClientMessage(playerid, COLOR_GREY, "Gracz o podanym id nie ma bw, wi�c nie mo�esz mu go zdj��.");
	}

	ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Informacja", sprintf("Nada�e� graczowi %s [UID: %d, ID: %d] bw na czas: %d minut.", pInfo[targetid][player_name], pInfo[targetid][player_id], targetid, time), "OK", "");

	new
		Float:x,
		Float:y,
		Float:z,
		Float:a;
	GetPlayerPos(targetid, x, y, z);
	GetPlayerFacingAngle(targetid, a);


	pInfo[targetid][player_bw] = 60 * time;
	pInfo[targetid][player_bw_end_time] = pInfo[targetid][player_bw] + gettime();

	RP_PLUS_SetPlayerPos(targetid, x, y, z);
	SetPlayerFacingAngle(targetid, a);

	mysql_query(mySQLconnection, sprintf("UPDATE `ipb_characters` SET `char_bw`=%d, `char_posx`='%f', `char_posy`='%f', `char_posz`='%f', `char_posa`='%f', `char_world`=%d, `char_interior`=%d WHERE `char_uid`=%d", pInfo[targetid][player_bw], x, y, z, a, GetPlayerVirtualWorld(targetid), GetPlayerInterior(targetid), pInfo[targetid][player_id]));

	pInfo[targetid][player_quit_pos][0] = x;
	pInfo[targetid][player_quit_pos][1] = y;
	pInfo[targetid][player_quit_pos][2] = z;
	pInfo[targetid][player_quit_pos][3] = a;
	pInfo[targetid][player_quit_vw] = GetPlayerVirtualWorld(targetid);
	pInfo[targetid][player_quit_int] = GetPlayerInterior(targetid);

	defer ApplyAnim[2000](targetid, ANIM_TYPE_BW);

	scrp_SpawnPlayer(targetid);
	return 1;
}

COMMAND:setvw(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Nie masz uprawnie� do korzystania z tej komendy.");

	new targetid, vw;
	if( sscanf(params, "ud", targetid, vw) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /setvw [id gracza/czesc nicku] [vw]");
	if( vw < 0 ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /setvw [id gracza/czesc nicku] [vw]");
	if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Gracz o podanym id nie jest pod��czony.");
	if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Gracz o podanym id nie jest zalogowany.");

	SetPlayerVirtualWorld(targetid, vw);

	if( targetid != playerid ) SendFormattedClientMessage(targetid, COLOR_LIGHTER_RED, "Administrator %s zmieni� Tw�j vw na %d.", pInfo[playerid][player_name], vw);
	SendGuiInformation(playerid, ""guiopis"Powiadomienie", sprintf("Zmieni�e� vw graczowi %s na %d.", pInfo[targetid][player_name], vw));

	return 1;
}

COMMAND:setint(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_GM) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN) && !HasCrewFlag(playerid, CREW_FLAG_ADMIN_ROOT) ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Nie masz uprawnie� do korzystania z tej komendy.");

	new targetid, vw;
	if( sscanf(params, "dd", targetid, vw) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /setint [id gracza] [interior world]");
	if( vw < 0 ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /setint[id gracza] [vw]");
	if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Gracz o podanym id nie jest podlaczony.");
	if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Gracz o podanym id nie jest zalogowany.");

	SetPlayerInterior(targetid, vw);

	if( targetid != playerid ) SendFormattedClientMessage(targetid, COLOR_LIGHTER_RED, "Administrator %s zmieni� Twoj interior world na %d.", pInfo[playerid][player_name], vw);
	SendGuiInformation(playerid, ""guiopis"Powiadomienie", sprintf("Zmieni�e� interior world graczowi %s na %d.", pInfo[targetid][player_name], vw));

	return 1;
}

COMMAND:actor(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_EDITOR) ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Nie masz uprawnie� do korzystania z tej komendy.");

	new sub_cmd[40], param[150];
	if( sscanf(params, "s[40] S()[150]", sub_cmd, param) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /actor [stworz/usun/anim]");

	if( !strcmp(sub_cmd, "stworz") )
	{
		new skin, type;
		if( sscanf(param, "dd", skin, type) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /actor stworz [skin] [typ]");
		new Float:x, Float:y, Float:z, Float:a;
		GetPlayerPos(playerid, x, y, z);
		GetPlayerFacingAngle(playerid, a);

		mysql_query(mySQLconnection, sprintf("INSERT INTO `ipb_actors` (actor_uid, actor_skin, actor_x, actor_y, actor_z, actor_a, actor_type, actor_vw) VALUES (null, %d, %f, %f, %f, %f, %d, %d)", skin, x, y, z, a, type, GetPlayerVirtualWorld(playerid)));

		if( cache_insert_id() )
		{
			new a_id = LoadActor(sprintf("WHERE `actor_uid` = %d", cache_insert_id()), true);
			SendGuiInformation(playerid, "Informacja", sprintf("Pomy�lnie utworzono aktora o ID %d.", a_id));
		}
	}
	else if( !strcmp(sub_cmd, "usun") )
	{
		new a_id;
		if( sscanf(param, "d", a_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /actor usun [id aktora]");
		if( !Iter_Contains(Actors, a_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /actor usun [id aktora]");

		mysql_query(mySQLconnection, sprintf("DELETE FROM ipb_actors WHERE actor_uid = %d", Actor[a_id][actor_uid]));
		DestroyActor(a_id);
		Iter_Remove(Actors, a_id);
	}
	else if( !strcmp(sub_cmd, "przypisz") )
	{
		new a_id, rid;
		if( sscanf(param, "dd", a_id, rid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /actor przypisz [id aktora] [uid napadu]");
		if( !Iter_Contains(Actors, a_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /actor przypisz [id aktora] [uid napadu]");

		mysql_query(mySQLconnection, sprintf("UPDATE ipb_actors SET actor_owner = %d WHERE actor_uid = %d", rid, Actor[a_id][actor_uid]));
		Actor[a_id][actor_owner] = rid;
		Actor[a_id][actor_type] = ACTOR_TYPE_ROBBERY;
		SendGuiInformation(playerid, "Informacja", "Pomy�lnie przypisano aktora do napadu.");
	}
	else if( !strcmp(sub_cmd, "anim") )
	{
		new a_id, animid;
		if( sscanf(param, "dd", a_id, animid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /actor anim [id aktora] [uid animacji]");
		if( !Iter_Contains(Actors, a_id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /actor anim [id aktora] [uid animacji]");

		mysql_query(mySQLconnection, sprintf("UPDATE ipb_actors SET actor_anim = %d WHERE actor_uid = %d", animid, Actor[a_id][actor_uid]));
		Actor[a_id][actor_animation] = animid;

		foreach(new anim_id: Anims)
		{
			if(AnimInfo[anim_id][aUID] == Actor[a_id][actor_animation])
        	{
        	    ApplyActorAnimation(a_id, AnimInfo[anim_id][aLib], AnimInfo[anim_id][aName], AnimInfo[anim_id][aSpeed], bool:AnimInfo[anim_id][aOpt1], bool:AnimInfo[anim_id][aOpt2], bool:AnimInfo[anim_id][aOpt3], bool:AnimInfo[anim_id][aOpt4], AnimInfo[anim_id][aOpt5]);
        	}
		}
	}
	return 1;

}

COMMAND:ag(playerid, params[])
{
	return cmd_agrupa(playerid, params);
}

COMMAND:agrupa(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_GROUPS) ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Nie masz uprawnie� do korzystania z tej komendy.");

	new sub_cmd[40], param[150];
	if( sscanf(params, "s[40] S()[150]", sub_cmd, param) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ag)rupa [stworz/usun/flagi/nazwa/lider/info/typ/wypros/zapros]");

	if( !strcmp(sub_cmd, "stworz") )
	{
		new name[50];
		if( sscanf(param, "s[50]", name) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ag)rupa stworz [nazwa]");

		mysql_real_escape_string(name, name);
		mysql_query(mySQLconnection, sprintf("INSERT INTO `ipb_game_groups` (group_uid, group_name, group_tag, group_created) VALUES (null, '%s', 'TAG', %d)", name, gettime()));

		if( cache_insert_id() )
		{
			new gid = Iter_Free(Groups);
			Iter_Add(Groups, gid);

			Group[gid][group_uid] = cache_insert_id();
			Group[gid][group_bank_money] = 0;

			strcopy(Group[gid][group_name], name);

			ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Informacja", sprintf("Stworzy�e� grupe %s (UID: %d, ID: %d).", Group[gid][group_name], Group[gid][group_uid], gid), "OK", "");
		}
	}
	else if( !strcmp(sub_cmd, "kolor") )
	{
		new color[16], uid;
		if( sscanf(param, "ds[16]", uid,  color) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /ag kolor [uid grupy] [HEX]");
		if(!IsHex(color))  return SendClientMessage(playerid, COLOR_GREY, "Tip: /lider kolor [HEX]");

		format(color, sizeof(color), "0x%sDD", color);
		mysql_query(mySQLconnection, sprintf("UPDATE `ipb_game_groups` SET `group_color` = '%d' WHERE `group_uid` = %d", hexstring(color), uid));
		SendGuiInformation(playerid, "Informacja", "Kolor grupy zosta� zmieniony.");
	}
	else if( !strcmp(sub_cmd, "usun") )
	{
		new g_uid;
		if( sscanf(param, "d", g_uid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ag)rupa usun [uid grupy]");
		new gid = GetGroupByUid(g_uid);
		if( gid == -1 ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Grupa o podanym uid nie istnieje");

		SendGuiInformation(playerid, ""guiopis"Powiadomienie", sprintf("Usun��e� grupe %s [UID: %d, ID: %d] razem ze wszystkimi jej pojazdami i drzwiami.", Group[gid][group_name], Group[gid][group_uid], gid));
		DeleteGroup(gid);
	}
	else if( !strcmp(sub_cmd, "info") )
	{
		new g_uid;
		if( sscanf(param, "d", g_uid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ag)rupa info [uid grupy]");
		new gid = GetGroupByUid(g_uid);
		if( gid == -1 ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Grupa o podanym uid nie istnieje");

		new str[300];

		format(str, sizeof(str), "%sTyp grupy:\t\t%s\n", str, GroupTypes[Group[gid][group_type]]);
		format(str, sizeof(str), "%sBud�et:\t\t\t$%d\n", str, Group[gid][group_bank_money]);
		format(str, sizeof(str), "%sKapita�:\t\t\t$%d\n", str, Group[gid][group_capital]);
		format(str, sizeof(str), "%sPunkty aktywno�ci:\t%s%d\n", str, ((Group[gid][group_activity_points] >= 1000) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)), Group[gid][group_activity_points]);
		format(str, sizeof(str), "%sDotacja:\t\t$%d\n", str, GetGroupDotation(gid));
		format(str, sizeof(str), "%sTag:\t\t\t%s\n", str, GetGroupTag(gid));

		ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_LIST, sprintf("%s (UID: %d) � Informacje", Group[gid][group_name], Group[gid][group_uid]), str, "OK", "");
	}
	else if( !strcmp(sub_cmd, "flagi") )
	{
		new g_uid;
		if( sscanf(param, "d", g_uid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ag)rupa info [uid grupy]");
		new gid = GetGroupByUid(g_uid);
		if( gid == -1 ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Grupa o podanym uid nie istnieje");

		DynamicGui_Init(playerid);
		DynamicGui_SetDialogValue(playerid, gid);
		new str[700];

		format(str, sizeof(str), "%s%s01\tRadio (czat IC)\n", str, ((GroupHasFlag(gid, GROUP_FLAG_IC)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, GROUP_FLAG_IC);

		format(str, sizeof(str), "%s%s02\tCzat OOC\n", str, ((GroupHasFlag(gid, GROUP_FLAG_OOC)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, GROUP_FLAG_OOC);

		format(str, sizeof(str), "%s%s03\tKolorowe nicki\n", str, ((GroupHasFlag(gid, GROUP_FLAG_COLORED)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, GROUP_FLAG_COLORED);

		format(str, sizeof(str), "%s%s04\tCzat departamentowy\n", str, ((GroupHasFlag(gid, GROUP_FLAG_DEP)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, GROUP_FLAG_DEP);

		format(str, sizeof(str), "%s%s05\tOgraniczona s�u�ba\n", str, ((GroupHasFlag(gid, GROUP_FLAG_DUTY)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, GROUP_FLAG_DUTY);

		format(str, sizeof(str), "%s%s06\tPrzetrzymywanie\n", str, ((GroupHasFlag(gid, GROUP_FLAG_KEEP)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, GROUP_FLAG_KEEP);

		format(str, sizeof(str), "%s%s07\tStawianie blokad\n", str, ((GroupHasFlag(gid, GROUP_FLAG_BLOCKADE)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, GROUP_FLAG_BLOCKADE);

		format(str, sizeof(str), "%s%s08\tU�ywanie oflagowanych broni\n", str, ((GroupHasFlag(gid, GROUP_FLAG_WEAPON_FLAG)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, GROUP_FLAG_WEAPON_FLAG);

		format(str, sizeof(str), "%s%s09\tZg�oszenia 911\n", str, ((GroupHasFlag(gid, GROUP_FLAG_911)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, GROUP_FLAG_911);

		format(str, sizeof(str), "%s%s10\tTworzenie wy�cig�w\n", str, ((GroupHasFlag(gid, GROUP_FLAG_RACES)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, GROUP_FLAG_RACES);

		format(str, sizeof(str), "%s%s11\tU�ywanie masek\n", str, ((GroupHasFlag(gid, GROUP_FLAG_MASK)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, GROUP_FLAG_MASK);

		format(str, sizeof(str), "%s%s12\tPrzeszukiwanie\n", str, ((GroupHasFlag(gid, GROUP_FLAG_SEARCH)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, GROUP_FLAG_SEARCH);

		format(str, sizeof(str), "%s%s13\tSkuwanie graczy\n", str, ((GroupHasFlag(gid, GROUP_FLAG_CUFFS)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, GROUP_FLAG_CUFFS);

		format(str, sizeof(str), "%s%s14\tWywa�anie drzwi\n", str, ((GroupHasFlag(gid, GROUP_FLAG_PRYING)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, GROUP_FLAG_PRYING);

		format(str, sizeof(str), "%s%s15\tDost�p do bota\n", str, ((GroupHasFlag(gid, GROUP_FLAG_BOT)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, GROUP_FLAG_BOT);

		format(str, sizeof(str), "%s%s16\tZabieranie\n", str, ((GroupHasFlag(gid, GROUP_FLAG_TAKE)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, GROUP_FLAG_TAKE);

		format(str, sizeof(str), "%s%s17\tLeczenie\n", str, ((GroupHasFlag(gid, GROUP_FLAG_MEDIC)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, GROUP_FLAG_MEDIC);

		format(str, sizeof(str), "%s%s18\tTworzenie paintjob�w\n", str, ((GroupHasFlag(gid, GROUP_FLAG_PAINTJOB)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, GROUP_FLAG_PAINTJOB);

		format(str, sizeof(str), "%s%s19\tBiznes\n", str, ((GroupHasFlag(gid, GROUP_FLAG_BUSINESS)) ? (HEX_COLOR_LIGHTER_GREEN) : (HEX_COLOR_LIGHTER_RED)));
		DynamicGui_AddRow(playerid, GROUP_FLAG_BUSINESS);

		ShowPlayerDialog(playerid, DIALOG_GROUP_FLAGS, DIALOG_STYLE_LIST, sprintf("Flagi grupy [UID: %d]", g_uid), str, "Wybierz", "Zamknij");
	}
	else if( !strcmp(sub_cmd, "nazwa") )
	{
		new g_uid, name[64];
		if( sscanf(param, "ds[64]", g_uid, name) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ag)rupa nazwa [uid grupy] [nazwa]");
		new gid = GetGroupByUid(g_uid);
		if( gid == -1 ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Grupa o podanym id nie istnieje");
		mysql_real_escape_string(name, name);
		strcopy(Group[gid][group_name], name);

		mysql_query(mySQLconnection, sprintf("UPDATE `ipb_game_groups` SET `group_name` = '%s' WHERE `group_uid` = %d", Group[gid][group_name], Group[gid][group_uid]));

		SendGuiInformation(playerid, ""guiopis"Powiadomienie", sprintf("Pomy�lnie zmieni�e� nazwe grupy [UID: %d, ID: %d] na %s.", Group[gid][group_uid], gid, Group[gid][group_name]));
	}
	else if( !strcmp(sub_cmd, "typ") )
	{
		new g_uid;
		if( sscanf(param, "d", g_uid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ag)rupa typ [uid grupy]");
		new gid = GetGroupByUid(g_uid);
		if( gid == -1 ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Grupa o podanym uid nie istnieje");

		new str[400];
		DynamicGui_Init(playerid);
		DynamicGui_SetDialogValue(playerid, gid);

		for(new i;i<sizeof(GroupTypes);i++)
		{
			format(str, sizeof(str), "%s%02d\t%s\n", str, i+1, GroupTypes[i]);
			DynamicGui_AddRow(playerid, 0, i);
		}

		ShowPlayerDialog(playerid, DIALOG_AGRUPA_TYP, DIALOG_STYLE_LIST, sprintf("%s (UID: %d) � Zmiana typu", Group[gid][group_name], Group[gid][group_uid]), str, "Wybierz", "Zamknij");
	}
	else if( !strcmp(sub_cmd, "lider") )
	{
		new g_uid, targetid;
		if( sscanf(param, "dd", g_uid, targetid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ag)rupa lider [uid grupy] [id gracza]");
		new gid = GetGroupByUid(g_uid);
		if( gid == -1 ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Grupa o podanym id nie istnieje");
		if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Gracz o podanym id nie jest pod��czony.");
		if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Gracz o podanym id nie jest zalogowany.");

		new slot = GetPlayerGroupSlot(targetid, gid);
		if( slot > -1 )
		{
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_char_groups` SET `group_perm` = %d WHERE `char_uid` = %d AND `group_belongs` = %d", 63, pInfo[targetid][player_id], Group[gid][group_uid]));

			pGroup[targetid][slot][pg_rank_perm] = 63;

			SendGuiInformation(playerid, ""guiopis"Powiadomienie", sprintf("Nada�e� graczowi %s (UID: %d, ID: %d) uprawnienia lidera w grupie %s (UID: %d).", pInfo[targetid][player_name], pInfo[targetid][player_id], targetid, Group[gid][group_name], Group[gid][group_uid]));
			if( targetid != playerid ) SendGuiInformation(targetid, "Informacja", sprintf("Administrator %s nada� Ci uprawnienia lidera w grupie %s (UID: %d).", pInfo[playerid][player_name], Group[gid][group_name], Group[gid][group_uid]));
		}
		else
		{
			new free_slot = GetPlayerGroupFreeSlot(targetid);
			if( free_slot == -1 ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Gracz o podanym id nie ma ju� wolnego slotu grupy.");

			mysql_query(mySQLconnection, sprintf("INSERT INTO `ipb_char_groups` (char_uid, group_belongs, group_perm) VALUES (%d, %d, %d)", pInfo[targetid][player_id], Group[gid][group_uid], 63));

			pGroup[targetid][free_slot][pg_id] = gid;
			pGroup[targetid][free_slot][pg_rank_perm] = 63;
			SendGuiInformation(playerid, ""guiopis"Powiadomienie", sprintf("Zaprosi�e� gracza %s (UID: %d, ID: %d) i nada�e� mu uprawnienia lidera w grupie %s (UID: %d).", pInfo[targetid][player_name], pInfo[targetid][player_id], targetid, Group[gid][group_name], Group[gid][group_uid]));
			if( targetid != playerid ) SendGuiInformation(targetid, "Informacja", sprintf("Administrator %s zaprosi� Cie do grupy %s (UID: %d) nadaj�c uprawnienia lidera.", pInfo[playerid][player_name], Group[gid][group_name], Group[gid][group_uid]));
		}
	}
	else if( !strcmp(sub_cmd, "zapros") )
	{
		new g_uid, targetid;
		if( sscanf(param, "dd", g_uid, targetid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ag)rupa zapros [uid grupy] [id gracza]");
		new gid = GetGroupByUid(g_uid);
		if( gid == -1 ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Grupa o podanym id nie istnieje");
		if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Gracz o podanym id nie jest pod��czony.");
		if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Gracz o podanym id nie jest zalogowany.");
		if( GetPlayerGroupSlot(targetid, gid) > -1 ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Gracz o podanym id jest ju� w tej grupie.");

		new free_slot = GetPlayerGroupFreeSlot(targetid);
		if( free_slot == -1 ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Gracz o podanym id nie ma ju� wolnego slotu grupy.");

		mysql_query(mySQLconnection, sprintf("INSERT INTO `ipb_char_groups` (char_uid, group_belongs, group_perm) VALUES (%d, %d, %d)", pInfo[targetid][player_id], Group[gid][group_uid], 31));

		pGroup[targetid][free_slot][pg_id] = gid;
		pGroup[targetid][free_slot][pg_rank_perm] = 31;
		SendGuiInformation(playerid, ""guiopis"Powiadomienie", sprintf("Zaprosiles gracza %s (UID: %d, ID: %d) do grupy %s (UID: %d).", pInfo[targetid][player_name], pInfo[targetid][player_id], targetid, Group[gid][group_name], Group[gid][group_uid]));
		if( targetid != playerid ) SendGuiInformation(targetid, "Informacja", sprintf("Administrator %s zaprosil Cie do grupy %s (UID: %d).", pInfo[playerid][player_name], Group[gid][group_name], Group[gid][group_uid]));
	}
	else if( !strcmp(sub_cmd, "wypros") )
	{
		new g_uid, targetid;
		if( sscanf(param, "dd", g_uid, targetid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ag)rupa wypros [uid grupy] [id gracza]");
		new gid = GetGroupByUid(g_uid);
		if( gid == -1 ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Grupa o podanym id nie istnieje");
		if( !IsPlayerConnected(targetid) ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Gracz o podanym id nie jest pod��czony.");
		if( !pInfo[targetid][player_logged] ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Gracz o podanym id nie jest zalogowany.");

		new slot = GetPlayerGroupSlot(targetid, gid);
		if( slot == -1 ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Gracz o podanym id nie jest w tej grupie.");

		mysql_query(mySQLconnection, sprintf("DELETE FROM `ipb_char_groups` WHERE `char_uid` = %d AND `group_belongs` = %d", pInfo[targetid][player_id], Group[gid][group_uid]));

		if( pGroup[targetid][slot][pg_duty] ) PlayerTextDrawHide(targetid, GroupDutyTag);

		CleanPlayerGroup(targetid, slot);

		SendGuiInformation(playerid, ""guiopis"Powiadomienie", sprintf("Wyprosi�e� gracza %s (UID: %d, ID: %d) z grupy %s (UID: %d).", pInfo[targetid][player_name], pInfo[targetid][player_id], targetid, Group[gid][group_name], Group[gid][group_uid]));
		if( targetid != playerid ) SendGuiInformation(targetid, "Informacja", sprintf("Administrator %s wyprosi� Cie z grupy %s (UID: %d).", pInfo[playerid][player_name], Group[gid][group_name], Group[gid][group_uid]));
	}
	else return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ag)rupa [stworz/usun/nazwa/lider/info/typ/wypros/zapros]");
	return 1;
}

COMMAND:av(playerid, params[])
{
	return cmd_avehicle(playerid, params);
}

COMMAND:avehicle(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_VEHICLES) ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Nie masz uprawnie� do korzystania z tej komendy.");

	new sub_cmd[40], param[150];
	if( sscanf(params, "s[40] S()[150]", sub_cmd, param) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(av)ehicle [stworz/usun/info/model/hp/visual/przypisz/id/zaparkuj/resp/spawn/unspawn/paliwotyp/paliwo/goto/gethere/kolor1/kolor2]");

	if( !strcmp(sub_cmd, "stworz") )
	{
		new carmodel[32], c1, c2;
		if( sscanf(param, "dds[32]", c1, c2, carmodel) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(av)ehicle stworz [kolor1] [kolor2] [nazwa]");

		new model = GetVehicleModelIDFromName(carmodel);

		if( model < 400 || model > 611 ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Poda�e� b��dn� nazwe pojazdu.");

		new Float:v_pos[4];
		GetPointInAngleOfPlayer(playerid, v_pos[0], v_pos[1], v_pos[2], 4.0, 0.0);

		GetPlayerFacingAngle(playerid, v_pos[3]);
		if( v_pos[3] > 90.0 ) v_pos[3] -= 90.0;
		else v_pos[3] += 90.0;

		mysql_query(mySQLconnection, sprintf("INSERT INTO `ipb_vehicles` (vehicle_uid, vehicle_model, vehicle_posx, vehicle_posy, vehicle_posz, vehicle_posa, vehicle_world, vehicle_interior, vehicle_color1, vehicle_color2, vehicle_fuel) VALUES (null, %d, %f, %f, %f, %f, %d, %d, %d, %d, '20.0')", model, v_pos[0], v_pos[1], v_pos[2], v_pos[3], GetPlayerVirtualWorld(playerid), GetPlayerInterior(playerid), c1, c2));
		new uid = cache_insert_id();

		new vid = LoadVehicle(sprintf("WHERE `vehicle_uid` = %d", uid), true);

		SendGuiInformation(playerid, ""guiopis"Powiadomienie", sprintf("Stworzy�e� pojazd %s [UID: %d, ID: %d].", VehicleNames[model-400], uid, vid));
	}
	else if( !strcmp(sub_cmd, "usun") )
	{
		new vid;
		if( sscanf(param, "d", vid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle usun [id pojazdu]");
		if( !Iter_Contains(Vehicles, vid) ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Pojazd o podanym id nie istnieje.");

		SendGuiInformation(playerid, ""guiopis"Powiadomienie", sprintf("Pomy�lnie usun��e� pojazd %s [UID: %d, ID: %d]", VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid], vid));

		DeleteVehicle(vid, true);
	}
	else if( !strcmp(sub_cmd, "spawn") )
	{
		new v_uid;
		if( sscanf(param, "d", v_uid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle spawn [uid pojazdu]");
		if( v_uid < 1 ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle spawn [uid pojazdu]");

		new vid = LoadVehicle(sprintf("WHERE `vehicle_uid` = %d", v_uid), true);

		if( vid == INVALID_VEHICLE_ID ) SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Pojazd o podanym uid nie istnieje lub jest ju� zespawnowany.");
		else SendGuiInformation(playerid, ""guiopis"Powiadomienie", sprintf("Zespawnowales pojazd %s [UID: %d, ID: %d].", VehicleNames[GetVehicleModel(vid)-400], v_uid, vid));
	}
	else if( !strcmp(sub_cmd, "tirestate") )
	{
		new vid, tstate;
		if( sscanf(param, "dd", vid, tstate) ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle tirestate[id pojazdu] [state]");
		if( !Iter_Contains(Vehicles, vid) ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Pojazd o podanym id nie istnieje.");

		SendGuiInformation(playerid, ""guiopis"Powiadomienie", sprintf("Zmieniono stan opon pojazdu %s [UID: %d, ID: %d].", VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid], vid));

		new Panels, Doors, Lights, Tires;
		GetVehicleDamageStatus(vid, Panels, Doors, Lights, Tires);
		UpdateVehicleDamageStatus(vid, Panels, Doors, Lights, tstate);
	}
	else if( !strcmp(sub_cmd, "unspawn") )
	{
		new vid;
		if( sscanf(param, "d", vid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle unspawn [id pojazdu]");
		if( !Iter_Contains(Vehicles, vid) ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Pojazd o podanym id nie istnieje.");

		SendGuiInformation(playerid, ""guiopis"Powiadomienie", sprintf("Odspawnowales pojazd %s [UID: %d, ID: %d].", VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid], vid));

		DeleteVehicle(vid);
	}
	else if( !strcmp(sub_cmd, "reset") )
	{
		new vid;
		if( sscanf(param, "d", vid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle reset [id pojazdu]");
		if( !Iter_Contains(Vehicles, vid) ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Pojazd o podanym id nie istnieje.");

		SendGuiInformation(playerid, ""guiopis"Powiadomienie", sprintf("Zresetowales pojazd %s [UID: %d, ID: %d].", VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid], vid));

		SetVehiclePos(vid, Vehicle[vid][vehicle_park][0],Vehicle[vid][vehicle_park][1],Vehicle[vid][vehicle_park][2]);
		SetVehicleZAngle(vid, Vehicle[vid][vehicle_park][3]);
		SetVehicleVirtualWorld(vid, Vehicle[vid][vehicle_park_world]);
		LinkVehicleToInterior(vid, Vehicle[vid][vehicle_park_interior]);
	}
	else if( !strcmp(sub_cmd, "model") )
	{
		new vid, carmodel[32];
		if( sscanf(param, "ds[32]", vid, carmodel) ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle model [id pojazdu] [nazwa pojazdu]");
		if( !Iter_Contains(Vehicles, vid) ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Pojazd o podanym id nie istnieje.");

		new model = GetVehicleModelIDFromName(carmodel);
		if( model < 400 || model > 611 ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Podales b��dn� nazwe pojazdu.");

		mysql_query(mySQLconnection, sprintf("UPDATE `ipb_vehicles` SET `vehicle_model` = %d WHERE `vehicle_uid` = %d", model, Vehicle[vid][vehicle_uid]));

		new uid = Vehicle[vid][vehicle_uid];
		DeleteVehicle(vid);

		new v_id = LoadVehicle(sprintf("WHERE `vehicle_uid` = %d", uid), true);
		SendGuiInformation(playerid, ""guiopis"Powiadomienie", sprintf("Pomy�lnie zmieni�e� model pojazu na %s [UID: %d, ID: %d]", VehicleNames[GetVehicleModel(v_id)-400], uid, v_id));
	}
	else if( !strcmp(sub_cmd, "id") )
	{
		new v_uid;
		if( sscanf(param, "d", v_uid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle id [uid pojazdu]");
		if( v_uid < 1 ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle id [uid pojazdu]");

		new vid = GetVehicleByUid(v_uid);

		if( vid == INVALID_VEHICLE_ID ) SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Pojazd o podanym uid nie jest zespawnowany.");
		else SendGuiInformation(playerid, ""guiopis"Powiadomienie", sprintf("Pojazd %s posiada ID: %d.", VehicleNames[GetVehicleModel(vid)-400], vid));
	}
	else if( !strcmp(sub_cmd, "info") )
	{
		new vid;
		if( sscanf(param, "d", vid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle info [id pojazdu]");
		if( !Iter_Contains(Vehicles, vid) ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Pojazd o podanym id nie istnieje.");

		new owner_str[80];
		switch( Vehicle[vid][vehicle_owner_type] )
		{
			case VEHICLE_OWNER_TYPE_PLAYER:
			{
				new name[MAX_PLAYER_NAME+1];
				GetPlayerNameByUid(Vehicle[vid][vehicle_owner], name);

				format(owner_str, sizeof(owner_str), "gracz; %s", name);
			}

			case VEHICLE_OWNER_TYPE_GROUP:
			{
				new gid = GetGroupByUid(Vehicle[vid][vehicle_owner]);
				format(owner_str, sizeof(owner_str), "grupa; %s", Group[gid][group_name]);
			}

			case VEHICLE_OWNER_TYPE_JOB:
			{
				format(owner_str, sizeof(owner_str), "praca; %d", Vehicle[vid][vehicle_owner]);
			}
		}

		new string[340];
		format(string, sizeof(string), "Pojazd UID %d - %s~n~~n~~y~Owner: ~w~%d:%d (%s)~n~~y~Kolory:~w~ %d:%d~n~", Vehicle[vid][vehicle_uid], VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_owner_type], Vehicle[vid][vehicle_owner], owner_str, Vehicle[vid][vehicle_color][0], Vehicle[vid][vehicle_color][1]);
		format(string, sizeof(string), "%s~n~~y~Paliwo: ~w~%d/%d~n~~y~Rejestracja: ~w~%s~n~~y~HP: ~w~%.1f", string, floatround(Vehicle[vid][vehicle_fuel_current], floatround_ceil), VehicleFuelMax[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_register], Vehicle[vid][vehicle_health]);
		format(string, sizeof(string), "%s~n~~n~~b~~h~~h~VW:~w~ %d~n~~b~~h~~h~INT: ~w~%d~n~~n~Klawisz ~p~N~w~ zamyka to okno.", string, Vehicle[vid][vehicle_park_world], Vehicle[vid][vehicle_park_interior]);


		TextDrawSetString(Tutorial[playerid], string);
		TextDrawShowForPlayer(playerid, Tutorial[playerid]);
	}
	else if( !strcmp(sub_cmd, "hp") )
	{
		new vid, Float:hp;
		if( sscanf(param, "df", vid, hp) ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle hp [id pojazdu] [hp]");
		if( !Iter_Contains(Vehicles, vid) ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Pojazd o podanym id nie istnieje.");
		if( hp < 0.0 ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle hp [id pojazdu] [hp]");

		SetVehicleHealth(vid, hp);
		Vehicle[vid][vehicle_destroyed] = false;
		mysql_query(mySQLconnection, sprintf("UPDATE `ipb_vehicles` SET `vehicle_health` = %f WHERE `vehicle_uid` = %d", hp, Vehicle[vid][vehicle_uid]));

		SendGuiInformation(playerid, ""guiopis"Powiadomienie", sprintf("Ustawiles %.1f hp pojazdowi %s [UID: %d, ID: %d].", hp, VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid], vid));

		logprintf(LOG_VEHICLE, "[/av hp] [VUID: %d, PUID: %d, PGID: %d, VHP: %f]", Vehicle[vid][vehicle_uid], pInfo[playerid][player_id], gInfo[playerid][global_id], hp);
	}
	else if( !strcmp(sub_cmd, "visual") )
	{
		new vid;
		if( sscanf(param, "d", vid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle visual [id pojazdu]");
		if( !Iter_Contains(Vehicles, vid) ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Pojazd o podanym id nie istnieje.");

		Vehicle[vid][vehicle_damage][0] = 0;
		Vehicle[vid][vehicle_damage][1] = 0;
		Vehicle[vid][vehicle_damage][2] = 0;
		Vehicle[vid][vehicle_damage][3] = 0;

		UpdateVehicleDamageStatus(vid, Vehicle[vid][vehicle_damage][0], Vehicle[vid][vehicle_damage][1], Vehicle[vid][vehicle_damage][2], Vehicle[vid][vehicle_damage][3]);

		SaveVehicle(vid);

		SendGuiInformation(playerid, ""guiopis"Powiadomienie", sprintf("Naprawiles uszkodzenia wizualne pojazdu %s [UID: %d, ID: %d].", VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid], vid));
	}
	else if( !strcmp(sub_cmd, "zaparkuj") )
	{
		new vid;
		if( sscanf(param, "d", vid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle zaparkuj [id pojazdu]");
		if( !Iter_Contains(Vehicles, vid) ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Pojazd o podanym id nie istnieje.");

		GetVehiclePos(vid, Vehicle[vid][vehicle_park][0], Vehicle[vid][vehicle_park][1], Vehicle[vid][vehicle_park][2]);
		GetVehicleZAngle(vid, Vehicle[vid][vehicle_park][3]);
		Vehicle[vid][vehicle_park_world] = GetVehicleVirtualWorld(vid);
		Vehicle[vid][vehicle_park_interior] = Vehicle[vid][vehicle_interior];

		mysql_query(mySQLconnection, sprintf("UPDATE `ipb_vehicles` SET `vehicle_posx` = %f, `vehicle_posy` = %f, `vehicle_posz` = %f, `vehicle_posa` = %f, `vehicle_world` = %d, `vehicle_interior` = %d WHERE `vehicle_uid` = %d", Vehicle[vid][vehicle_park][0], Vehicle[vid][vehicle_park][1], Vehicle[vid][vehicle_park][2], Vehicle[vid][vehicle_park][3], Vehicle[vid][vehicle_park_world], Vehicle[vid][vehicle_park_interior] ,Vehicle[vid][vehicle_uid]));

		SendGuiInformation(playerid, ""guiopis"Powiadomienie", sprintf("Zaparkowano pojazd %s [UID: %d, ID: %d].", VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid], vid));
	}
	else if( !strcmp(sub_cmd, "przypisz") )
	{
		new vid, action[10], id;
		if( sscanf(param, "ds[10]d", vid, action, id) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /av przypisz [id pojazdu] [gracz/grupa/praca] [id postaci/uid grupy/id pracy]");

		if( !strcmp(action, "gracz") )
		{
			if( !IsPlayerConnected(id) ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Gracza o podanym id nie ma na serwerze.");
			if( !pInfo[id][player_logged] ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Gracz o podanym id nie jest zalogowany.");
			if( !Iter_Contains(Vehicles, vid) ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Pojazd o podanym id nie istnieje.");

			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_vehicles` SET `vehicle_ownertype` = %d, `vehicle_owner` = %d WHERE `vehicle_uid` = %d", VEHICLE_OWNER_TYPE_PLAYER, pInfo[id][player_id], Vehicle[vid][vehicle_uid]));
			Vehicle[vid][vehicle_owner_type] = VEHICLE_OWNER_TYPE_PLAYER;
			Vehicle[vid][vehicle_owner] = pInfo[id][player_id];

			SendGuiInformation(playerid, ""guiopis"Powiadomienie", sprintf("Przypisano pojazd %s [UID: %d, ID: %d] graczowi %s [ID: %d].", VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid], vid, pInfo[id][player_name], id));
		}
		else if( !strcmp(action, "praca") )
		{
			if( !Iter_Contains(Vehicles, vid) ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Pojazd o podanym id nie istnieje.");
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_vehicles` SET `vehicle_ownertype` = %d, `vehicle_owner` = %d WHERE `vehicle_uid` = %d", VEHICLE_OWNER_TYPE_JOB, id, Vehicle[vid][vehicle_uid]));
			Vehicle[vid][vehicle_owner_type] = VEHICLE_OWNER_TYPE_JOB;
			Vehicle[vid][vehicle_owner] = id;

			SendGuiInformation(playerid, ""guiopis"Powiadomienie", sprintf("Przypisano pojazd %s [UID: %d, ID: %d] pracy dorywczej [ID: %d].", VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid], vid, id));
		}
		else if( !strcmp(action, "grupa") )
		{
			if( !Iter_Contains(Vehicles, vid) ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Pojazd o podanym id nie istnieje.");
			new gid = GetGroupByUid(id);
			if( gid == -1 ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Grupa o podanym uid nie istnieje.");

			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_vehicles` SET `vehicle_ownertype` = %d, `vehicle_owner` = %d WHERE `vehicle_uid` = %d", VEHICLE_OWNER_TYPE_GROUP, id, Vehicle[vid][vehicle_uid]));
			Vehicle[vid][vehicle_owner_type] = VEHICLE_OWNER_TYPE_GROUP;
			Vehicle[vid][vehicle_owner] = id;

			SendGuiInformation(playerid, ""guiopis"Powiadomienie", sprintf("Przypisano pojazd %s [UID: %d, ID: %d] grupie %s [UID: %d].", VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid], vid, Group[gid][group_name], id));
		}
		else return SendClientMessage(playerid, COLOR_GREY, "Tip: /(av)ehicle przypisz [id pojazdu] [postac/grupa] [id postaci/uid grupy]");
	}
	else if( !strcmp(sub_cmd, "paliwotyp") )
	{
		new vid, type;
		if( sscanf(param, "dd", vid, type) ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle paliwotyp [id pojazdu] [1 - benzyna, 2 - gaz, 3 - diesel, 4 - paliwo lotnicze]");
		if( !Iter_Contains(Vehicles, vid) ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Pojazd o podanym id nie istnieje.");
		if( type < 1 || type > 4 ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Poda�e� b��dny typ paliwa.");

		mysql_query(mySQLconnection, sprintf("UPDATE `ipb_vehicles` SET `vehicle_fueltype` = %d WHERE `vehicle_uid` = %d", type, Vehicle[vid][vehicle_uid]));

		Vehicle[vid][vehicle_fuel_type] = type;

		SendGuiInformation(playerid, ""guiopis"Powiadomienie", sprintf("Zmieniono typ paliwa pojazdu %s [UID: %d, ID: %d] na: %s.", VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid], vid, VehicleFuelTypes[type]));
	}
	else if( !strcmp(sub_cmd, "paliwo") )
	{
		new vid, Float:amount;
		if( sscanf(param, "df", vid, amount) ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle paliwo [id pojazdu] [ilo��]");
		if( !Iter_Contains(Vehicles, vid) ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Pojazd o podanym id nie istnieje.");
		if( amount < 0.0 || amount > 3000.0 ) return  SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Poda�e� b��dn� ilo�� paliwa.");

		mysql_query(mySQLconnection, sprintf("UPDATE `ipb_vehicles` SET `vehicle_fuel` = %f WHERE `vehicle_uid` = %d", amount, Vehicle[vid][vehicle_uid]));
		Vehicle[vid][vehicle_fuel_current] = amount;

		SendGuiInformation(playerid, ""guiopis"Powiadomienie", sprintf("Pomy�lnie ustawi�es ilo�� paliwa w poje��dzie %s [UID: %d, ID: %d] na: %.1fL.", VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid], vid, amount));
	}
	else if( !strcmp(sub_cmd, "goto") )
	{
		new vid;
		if( sscanf(param, "d", vid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle goto [id pojazdu]");
		if( !Iter_Contains(Vehicles, vid) ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Pojazd o podanym id nie istnieje.");

		new Float:v_pos[3];
		GetVehiclePos(vid, v_pos[0], v_pos[1], v_pos[2]);

		RP_PLUS_SetPlayerPos(playerid, v_pos[0], v_pos[1], v_pos[2]);
		SetPlayerVirtualWorld(playerid, GetVehicleVirtualWorld(vid));
		SetPlayerInterior(playerid, Vehicle[vid][vehicle_interior]);

		SendGuiInformation(playerid, ""guiopis"Powiadomienie", sprintf("Teleportowa�e� si� do pojazdu %s [UID: %d, ID: %d, VW: %d, INT: %d]", VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid], vid, GetVehicleVirtualWorld(vid), Vehicle[vid][vehicle_interior]));
	}
	else if( !strcmp(sub_cmd, "gethere") )
	{
		new vid;
		if( sscanf(param, "d", vid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle gethere [id pojazdu]");
		if( !Iter_Contains(Vehicles, vid) ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Pojazd o podanym id nie istnieje.");

		new Float:v_pos[4];
		GetPointInAngleOfPlayer(playerid, v_pos[0], v_pos[1], v_pos[2], 4.0, 0.0);

		GetPlayerFacingAngle(playerid, v_pos[3]);
		if( v_pos[3] > 90.0 ) v_pos[3] -= 90.0;
		else v_pos[3] += 90.0;

		SetVehiclePos(vid, v_pos[0], v_pos[1], v_pos[2]);
		SetVehicleZAngle(vid, v_pos[3]);
		SetVehicleVirtualWorld(vid, GetPlayerVirtualWorld(playerid));
		LinkVehicleToInterior(vid, GetPlayerInterior(playerid));

		SendGuiInformation(playerid, ""guiopis"Powiadomienie", sprintf("Przeteleportowales do siebie pojazd %s [UID: %d, ID: %d]", VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid], vid));
	}
	else if( !strcmp(sub_cmd, "kolor1") )
	{
		new vid, color;
		if( sscanf(param, "dd", vid, color) ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle kolor1 [id pojazdu] [kolor]");
		if( !Iter_Contains(Vehicles, vid) ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Pojazd o podanym id nie istnieje.");
		if( color < 0 || color > 255 ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Podany kolor nie istnieje.");

		Vehicle[vid][vehicle_color][0] = color;
		mysql_query(mySQLconnection, sprintf("UPDATE `ipb_vehicles` SET `vehicle_color1` = %d WHERE `vehicle_uid` = %d", color, Vehicle[vid][vehicle_uid]));

		ChangeVehicleColor(vid, Vehicle[vid][vehicle_color][0], Vehicle[vid][vehicle_color][1]);

		SendGuiInformation(playerid, ""guiopis"Powiadomienie", sprintf("Zmieniono kolor pojazdu %s [UID: %d, ID: %d] na %d:%d.", VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid], vid, Vehicle[vid][vehicle_color][0], Vehicle[vid][vehicle_color][1]));
	}
	else if( !strcmp(sub_cmd, "kolor2") )
	{
		new vid, color;
		if( sscanf(param, "dd", vid, color) ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle kolor1 [id pojazdu] [kolor]");
		if( !Iter_Contains(Vehicles, vid) ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Pojazd o podanym id nie istnieje.");
		if( color < 0 || color > 255 ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Podany kolor nie istnieje.");

		Vehicle[vid][vehicle_color][1] = color;
		mysql_query(mySQLconnection, sprintf("UPDATE `ipb_vehicles` SET `vehicle_color2` = %d WHERE `vehicle_uid` = %d", color, Vehicle[vid][vehicle_uid]));

		ChangeVehicleColor(vid, Vehicle[vid][vehicle_color][0], Vehicle[vid][vehicle_color][1]);

		SendGuiInformation(playerid, ""guiopis"Powiadomienie", sprintf("Zmieni�e� kolor pojazdu %s [UID: %d, ID: %d] na %d:%d.", VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid], vid, Vehicle[vid][vehicle_color][0], Vehicle[vid][vehicle_color][1]));
	}
	else if( !strcmp(sub_cmd, "tablica") )
	{
		new plate[32], vid;
		if( sscanf(param, "ds[32]", vid, plate) ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle tablica [id pojazdu] [nowa rejestracja]");
		if( !Iter_Contains(Vehicles, vid) ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Pojazd o podanym id nie istnieje.");
		sscanf(plate, "s[32]", Vehicle[vid][vehicle_register]);
		mysql_query(mySQLconnection, sprintf("UPDATE `ipb_vehicles` SET `vehicle_register` = '%s' WHERE `vehicle_uid` = %d", plate, Vehicle[vid][vehicle_uid]));
		SendGuiInformation(playerid, ""guiopis"Powiadomienie", sprintf("Zmieniono tablice rejestracyjne pojazdu %s [UID: %d, ID: %d].", VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid], vid));
	}
	else if( !strcmp(sub_cmd, "resp") )
	{
		new vid;
		if( sscanf(param, "d", vid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip /(av)ehicle resp [id pojazdu]");
		if( !Iter_Contains(Vehicles, vid) ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Pojazd o podanym id nie istnieje.");

		SetVehiclePos(vid, Vehicle[vid][vehicle_park][0], Vehicle[vid][vehicle_park][1], Vehicle[vid][vehicle_park][2]);
		SetVehicleZAngle(vid, Vehicle[vid][vehicle_park][3]);
		SetVehicleVirtualWorld(vid, Vehicle[vid][vehicle_park_world]);
		LinkVehicleToInterior(vid, Vehicle[vid][vehicle_park_interior]);
		Vehicle[vid][vehicle_engine] = false;
		Vehicle[vid][vehicle_locked] = true;
		UpdateVehicleVisuals(vid);

		SendGuiInformation(playerid, ""guiopis"Powiadomienie", sprintf("Pojazd %s [UID: %d, ID: %d] powr�ci� na miejsce spawnu.", VehicleNames[GetVehicleModel(vid)-400], Vehicle[vid][vehicle_uid], vid));
	}
	else return SendClientMessage(playerid, COLOR_GREY, "Tip: /(av)ehicle [stworz/usun/model/hp/visual/przypisz/id/zaparkuj/spawn/unspawn/resp/paliwotyp/paliwo/goto/gethere/kolor1/kolor2]");
	return 1;
}

COMMAND:ap(playerid, params[])
{
	return cmd_aprzedmiot(playerid, params);
}

COMMAND:aprzedmiot(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_ITEMS) ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Nie masz uprawnie� do korzystania z tej komendy.");

	new sub_cmd[40], param[150];
	if( sscanf(params, "s[40] S()[150]", sub_cmd, param) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ap)rzedmiot [stworz/usun/value1/value2/typ]");

	if( !strcmp(sub_cmd, "stworz") )
	{
		new type, model, val1, val2, name[40];
		if( sscanf(param, "dddd s[40]", type, model, val1, val2, name) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ap)rzedmiot stworz [typ] [model] [value1] [value2] [nazwa]");

		if(type == ITEM_TYPE_WEAPON)
		{
			if(model == 0)
			{
				model = GetWeaponModel(val1);
			}
		}

		if(model == 0 && type != ITEM_TYPE_WEAPON)
		{
			model = 327;
		}

		Item_Create(ITEM_OWNER_TYPE_PLAYER, playerid, type, model, val1, val2, name);

		SendGuiInformation(playerid, ""guiopis"Powiadomienie", sprintf("Stworzy�e� przedmiot %s [TYP: %d, MODEL: %d, V1: %d, V2: %d].", name, type, model, val1, val2));
	}
	else if( !strcmp(sub_cmd, "usun") )
	{
		new itemuid;
		if(	sscanf(param, "d", itemuid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ap)rzedmiot usun [uid przedmiotu]");

		new itemid = GetItemByUid(itemuid, playerid);
		if( itemid > -1 )
		{
			DeleteItem(itemid, true, playerid);
			SendGuiInformation(playerid, ""guiopis"Powiadomienie", sprintf("Skasowa�e� przedmiot o UID: %d.", itemuid));
		}
		else
		{
			SendGuiInformation(playerid, "Informacja", "Ten przedmiot nie jest w twoim posiadaniu lub nie istnieje.");
		}
	}
	else if( !strcmp(sub_cmd, "value1") )
	{
		new itemuid, value1;
		if(	sscanf(param, "dd", itemuid, value1) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ap)rzedmiot value1 [uid przedmiotu] [value1]");

		new itemid = GetItemByUid(itemuid, playerid);
		if( itemid > -1 )
		{
			PlayerItem[playerid][itemid][player_item_value1] = value1;
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_items` SET `item_value1` = %d WHERE `item_uid` = %d", value1, itemuid));
			SendGuiInformation(playerid, ""guiopis"Powiadomienie", sprintf("Zmieni�e� warto�� 1 przedmiotu o UID: %d na: %d.", itemuid, value1));
		}
		else
		{
			SendGuiInformation(playerid, "Informacja", "Ten przedmiot nie jest w twoim posiadaniu lub nie istnieje.");
		}
	}
	else if( !strcmp(sub_cmd, "value2") )
	{
		new itemuid, value2;
		if(	sscanf(param, "dd", itemuid, value2) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ap)rzedmiot value2 [uid przedmiotu] [value2]");

		new itemid = GetItemByUid(itemuid, playerid);
		if( itemid > -1 )
		{
			PlayerItem[playerid][itemid][player_item_value2] = value2;
			SendGuiInformation(playerid, ""guiopis"Powiadomienie", sprintf("Zmieni�e� warto�� 2 przedmiotu o UID: %d na: %d.", itemuid, value2));
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_items` SET `item_value2` = %d WHERE `item_uid` = %d", value2, itemuid));
		}
		else
		{
			SendGuiInformation(playerid, "Informacja", "Ten przedmiot nie jest w twoim posiadaniu lub nie istnieje.");
		}
	}
	else if( !strcmp(sub_cmd, "waga") )
	{
		new itemuid, value2;
		if(	sscanf(param, "dd", itemuid, value2) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ap)rzedmiot waga [uid przedmiotu] [waga]");

		new itemid = GetItemByUid(itemuid, playerid);
		if( itemid > -1 )
		{
			PlayerItem[playerid][itemid][player_item_weight] = value2;
			SendGuiInformation(playerid, ""guiopis"Powiadomienie", sprintf("Zmieni�e� wage przedmiotu o UID: %d na: %d.", itemuid, value2));
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_items` SET `item_weight` = %d WHERE `item_uid` = %d", value2, itemuid));
		}
		else
		{
			SendGuiInformation(playerid, "Informacja", "Ten przedmiot nie jest w twoim posiadaniu lub nie istnieje.");
		}
	}
	else if( !strcmp(sub_cmd, "extraid") )
	{
		new itemuid, groupuid;
		if(	sscanf(param, "dd", itemuid, groupuid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ap)rzedmiot extraid [uid przedmiotu] [extraid]");

		new itemid = GetItemByUid(itemuid, playerid);
		if( itemid > -1 )
		{
			PlayerItem[playerid][itemid][player_item_extraid] = groupuid;
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_items` SET `item_extraid` = %d WHERE `item_uid` = %d", groupuid, itemuid));
			SendGuiInformation(playerid, ""guiopis"Powiadomienie", sprintf("Nadano extraid %d dla przedmiotu UID: %d.", groupuid, itemuid));
		}
		else
		{
			SendGuiInformation(playerid, "Informacja", "Ten przedmiot nie jest w twoim posiadaniu lub nie istnieje.");
		}
	}
	else if( !strcmp(sub_cmd, "flaga") )
	{
		new itemuid, groupuid;
		if(	sscanf(param, "dd", itemuid, groupuid) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ap)rzedmiot flaga [uid przedmiotu] [uid grupy]");

		new itemid = GetItemByUid(itemuid, playerid);
		if( itemid > -1 )
		{
			PlayerItem[playerid][itemid][player_item_group] = groupuid;
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_items` SET `item_group` = %d WHERE `item_uid` = %d", groupuid, itemuid));
			SendGuiInformation(playerid, ""guiopis"Powiadomienie", sprintf("Oflagowano przedmiot o UID: %d dla grupy %d.", itemuid, groupuid));
		}
		else
		{
			SendGuiInformation(playerid, "Informacja", "Ten przedmiot nie jest w twoim posiadaniu lub nie istnieje.");
		}
	}
	else if( !strcmp(sub_cmd, "typ") )
	{
		new itemuid, typ;
		if(	sscanf(param, "dd", itemuid, typ) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ap)rzedmiot typ [uid przedmiotu] [typ]");

		new itemid = GetItemByUid(itemuid, playerid);
		if( itemid > -1 )
		{
			PlayerItem[playerid][itemid][player_item_type] = typ;
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_items` SET `item_type` = %d WHERE `item_uid` = %d", typ, itemuid));
			SendGuiInformation(playerid, ""guiopis"Powiadomienie", sprintf("Zmieni�e� typ przedmiotu o UID: %d na: %d.", itemuid, typ));
		}
		else
		{
			SendGuiInformation(playerid, "Informacja", "Ten przedmiot nie jest w twoim posiadaniu lub nie istnieje.");
		}
	}
	else if( !strcmp(sub_cmd, "model") )
	{
		new itemuid, model;
		if(	sscanf(param, "dd", itemuid, model) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ap)rzedmiot model[uid przedmiotu] [typ]");

		new itemid = GetItemByUid(itemuid, playerid);
		if( itemid > -1 )
		{
			PlayerItem[playerid][itemid][player_item_model] = model;
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_items` SET `item_model` = %d WHERE `item_uid` = %d", model, itemuid));
			SendGuiInformation(playerid, ""guiopis"Powiadomienie", sprintf("Zmieni�e� model przedmiotu o UID: %d na: %d.", itemuid, model));
		}
		else
		{
			SendGuiInformation(playerid, "Informacja", "Ten przedmiot nie jest w twoim posiadaniu lub nie istnieje.");
		}
	}
	else if( !strcmp(sub_cmd, "nazwa") )
	{
		new itemuid, itname[40];
		if(	sscanf(param, "ds[40]", itemuid, itname) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ap)rzedmiot nazwa [uid przedmiotu] [nowa nazwa]");

		new itemid = GetItemByUid(itemuid, playerid);
		if( itemid > -1 )
		{
			sscanf(itname, "s[40]", PlayerItem[playerid][itemid][player_item_name]);
			mysql_query(mySQLconnection, sprintf("UPDATE `ipb_items` SET `item_name` = '%s' WHERE `item_uid` = %d", itname, itemuid));
			SendGuiInformation(playerid, ""guiopis"Powiadomienie", sprintf("Zmieni�e� nazwe przedmiotu o UID: %d na: %s.", itemuid, itname));
		}
		else
		{
			SendGuiInformation(playerid, "Informacja", "Ten przedmiot nie jest w twoim posiadaniu lub nie istnieje.");
		}
	}
	else return SendClientMessage(playerid, COLOR_GREY, "Tip: /(ap)rzedmiot [stworz/usun/value1/value2/typ/lista]");

	return 1;
}

COMMAND:attach(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_ITEMS) ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Nie masz uprawnie� do korzystania z tej komendy.");

	new sub_cmd[40], param[150];
	if( sscanf(params, "s[40] S()[150]", sub_cmd, param) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /attach [stworz/usun/edytuj/print]");

	if( !strcmp(sub_cmd, "stworz") )
	{
		new model, bone;
		if( sscanf(param, "dd", model, bone) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /attach stworz [model] [bone]");

		new freeid = GetPlayerFreeAttachSlot(playerid);
		if( freeid == -1 ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Nie mo�esz przyczepi� ju� wiecej obiekt�w.");

		SetPlayerAttachedObject(playerid, freeid, model, bone);
		SendFormattedClientMessage(playerid, COLOR_GOLD, "Utworzyles nowy obiekt przyczepialny [INDEX: %d]. Aby zapisa� jego pozycje, zapisz edycje za pomoc� ikonky dyskietki.", freeid);

		ao[playerid][freeid][ao_model] = model;
        ao[playerid][freeid][ao_bone] = bone;
	}
	else if( !strcmp(sub_cmd, "usun") )
	{
		new index;
		if( sscanf(param, "d", index) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /attach usun [index]");
		if( !IsPlayerAttachedObjectSlotUsed(playerid, index) ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "W tym slocie nie masz �adnego obiektu.");

		ao[playerid][index][ao_model] = -1;
        ao[playerid][index][ao_bone] = -1;
		ao[playerid][index][ao_x] = 0.0;
        ao[playerid][index][ao_y] = 0.0;
        ao[playerid][index][ao_z] = 0.0;
        ao[playerid][index][ao_rx] = 0.0;
        ao[playerid][index][ao_ry] = 0.0;
        ao[playerid][index][ao_rz] = 0.0;
        ao[playerid][index][ao_sx] = 0.0;
        ao[playerid][index][ao_sy] = 0.0;
        ao[playerid][index][ao_sz] = 0.0;

		RemovePlayerAttachedObject(playerid, index);
		SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Pomy�lnie usun��e� obiekt przyczepialny");
	}
	else if( !strcmp(sub_cmd, "edytuj") )
	{
		new index;
		if( sscanf(param, "d", index) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /attach edytuj [index]");
		if( !IsPlayerAttachedObjectSlotUsed(playerid, index) ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "W tym slocie nie masz �adnego obiektu.");

		EditAttachedObject(playerid, index);
	}
	else if( !strcmp(sub_cmd, "print") )
	{
		new index;
		if( sscanf(param, "d", index) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /attach print [index]");
		if( !IsPlayerAttachedObjectSlotUsed(playerid, index) ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "W tym slocie nie masz �adnego obiektu.");

		printf("[model %d, bone: %d] %f, %f, %f, %f, %f, %f, %f, %f, %f", ao[playerid][index][ao_model], ao[playerid][index][ao_bone], ao[playerid][index][ao_x], ao[playerid][index][ao_y], ao[playerid][index][ao_z], ao[playerid][index][ao_rx], ao[playerid][index][ao_ry], ao[playerid][index][ao_rz], ao[playerid][index][ao_sx], ao[playerid][index][ao_sy], ao[playerid][index][ao_sz]);
	}
	return 1;
}

COMMAND:aplace(playerid, params[])
{
	if( !HasCrewFlag(playerid, CREW_FLAG_DOORS) ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Nie masz uprawnie� do korzystania z tej komendy.");

	new sub_cmd[40], param[150];
	if( sscanf(params, "s[40] S()[150]", sub_cmd, param) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aplace [stworz/usun]");

	if( !strcmp(sub_cmd, "stworz") )
	{
		new type, Float:sx, Float:sy, Float:sz;
		if( sscanf(param, "d", type) )
		{
			TextDrawSetString(Tutorial[playerid], "~w~Typy ~p~miejsc~w~:~n~~n~1. Bank~n~2. Hotel~n~3. Sklep 24/7~n~4. Victim~n~5. Apteka~n~6. Salon aut~n~7. Ammunation~n~8.Toolshop~n~Klawisz ~g~N~w~ zamyka to okno.");
			TextDrawShowForPlayer(playerid, Tutorial[playerid]);
			SendClientMessage(playerid, COLOR_GREY, "Tip: /aplace stworz [typ]");
			return 1;
		}
		new svw = GetPlayerVirtualWorld(playerid);
		GetPlayerPos(playerid, sx, sy, sz);

		new puid = CreateSpecialPlace(type, Float:sx, Float:sy, Float:sz, svw);
		SendGuiInformation(playerid, ""guiopis"Powiadomienie", sprintf("Miejsce UID: %d, typ: %d zosta�o pomy�lnie utworzone.", puid, type));
	}
	else if( !strcmp(sub_cmd, "usun") )
	{
		new index;
		if( sscanf(param, "d", index) ) return SendClientMessage(playerid, COLOR_GREY, "Tip: /aplace usun [uid]");
		SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Pomy�lnie usun��e� to miejsce.");
		new ib = GetPlaceByUid(index);
		if(ib != -1) Iter_Remove(Places, ib);
		DeleteSpecialPlace(index);
	}
	return 1;
}
