stock SetOffer(sellerid, buyerid, type, price, extraid = -1)
{
	if( sellerid == buyerid )
	{
		SendGuiInformation(sellerid, "Informacja", "Nie mo�esz wysy�a� ofert samemu sobie.");
		return 0;
	}

	if( sellerid != INVALID_PLAYER_ID )
	{
		if( pOffer[sellerid][offer_type] > 0 )
		{
			SendGuiInformation(sellerid, "Informacja", "Nie mo�esz sk�ada� kilku ofert na raz, poczekaj do zako�czenia poprzedniej oferty.");
			return 0;
		}
	}
	
	if( pOffer[buyerid][offer_type] > 0 )
	{
		SendGuiInformation(sellerid, "Informacja", "Gracz, do kt�rego chcia�e� wys�a� oferte aktualnie otrzyma� ju� jedn�.");
		return 0;
	}
	
	if( pInfo[buyerid][player_afk] )
	{
		SendGuiInformation(sellerid, "Informacja", "Gracz, do kt�rego chcia�e� wys�a� oferte aktualnie jest AFK.");
		return 0;
	}

	if( GetPlayerSpeed(buyerid) > 0)
	{
		SendGuiInformation(sellerid, "Informacja", "Gracz, do kt�rego chcesz wys�a� oferte musi by� w bezruchu.");
		return 0;
	}
	
	switch( type )
	{
		case OFFER_TYPE_VCARD:
		{
			if(extraid == -1) return 0;
			
			new itemid = GetPlayerUsedItem(buyerid, ITEM_TYPE_PHONE);
			if( GetPlayerUsedItem(buyerid, ITEM_TYPE_PHONE) == -1 )
			{
				SendGuiInformation(sellerid, "Informacja", "Ten gracz ma wy��czony telefon.");
				return 0;
			}
			
			new rows, fields;
			mysql_query(mySQLconnection, sprintf("SELECT contact_uid FROM `ipb_contacts` WHERE `contact_owner` = %d AND `contact_number` = %d AND `contact_deleted` = 0", Item[itemid][item_uid], Item[extraid][item_value1]));
			cache_get_data(rows, fields);
			
			if( rows )
			{
				SendGuiInformation(sellerid, "Informacja", "Ten gracz ma ju� tw�j vCard.");
				return 0;
			}
		}
		
		case OFFER_TYPE_ITEM:
		{
			if(extraid == -1) return 0;

			new itemid = extraid;

			if(pInfo[buyerid][player_hours] < 1 && price > 0)
			{
				SendGuiInformation(sellerid, "Informacja", "Ten gracz nie posiada przegranej godziny na serwerze.\nMo�e akceptowa� tylko darmow� oferte.");
				return 0;
			}

			if(PlayerItem[sellerid][itemid][player_item_used])
			{
				SendGuiInformation(sellerid, "Informacja", "Nie mo�esz zaoferowa� u�ywanego przedmiotu.\nSchowaj go i spr�buj ponownie.");
				return 0;
			}

			if(pInfo[buyerid][player_capacity] < PlayerItem[sellerid][itemid][player_item_weight])
			{
				SendGuiInformation(sellerid, "Informacja", "Przedmiot ten jest zbyt ci�ki dla tego gracza.");
				SendGuiInformation(buyerid, "Informacja", "Przedmiot ten jest dla Ciebie zbyt ci�ki.");
				return 0;
			} 
		}

		case OFFER_TYPE_INTERVIEW:
		{
			if(pInfo[buyerid][player_interview] > 0)
			{
				SendGuiInformation(sellerid, "Informacja", "Ten gracz jest ju� w trakcie wywiadu. Wpisz ponownie /wywiad aby go zako�czy�.");
				return 0;
			}
		}
	}
	
	pOffer[buyerid][offer_type] = type;
	pOffer[buyerid][offer_sellerid] = sellerid;
	pOffer[buyerid][offer_buyerid] = INVALID_PLAYER_ID;
	pOffer[buyerid][offer_price] = price;

	if(extraid != -1)
	{
		pOffer[buyerid][offer_extraid] = extraid;
	}
	
	if( sellerid != INVALID_PLAYER_ID )
	{
		pOffer[sellerid][offer_type] = type;
		pOffer[sellerid][offer_sellerid] = INVALID_PLAYER_ID;
		pOffer[sellerid][offer_buyerid] = buyerid;
		pOffer[sellerid][offer_price] = price;

		if(extraid != -1)
		{
			pOffer[sellerid][offer_extraid] = extraid;
		}
		
		pOffer[sellerid][offer_start] = gettime();
	}
	pOffer[buyerid][offer_start] = gettime();
	
	return 1;
}

stock ShowPlayerOffer(playerid, fromid, const const header[], const name[], price)
{
	new tmp[MAX_PLAYER_NAME+1], capt[100], inf[100];
	if( fromid != INVALID_PLAYER_ID )
	{
		strcopy(tmp, pInfo[fromid][player_name], MAX_PLAYER_NAME+1);
	}
	else strcopy(tmp, "System");

	HidePlayerDialog(playerid);

	format(capt, sizeof(capt), "Oferta od %s.", tmp);
	format(inf, sizeof(inf), ""HEX_COLOR_LIGHTER_YELLOW"%s: %s\n\n"COLOR_GUI_BLUE"Koszt: $%d. Czy akceptujesz t� ofert�?", header, name, price);
	
	ShowPlayerDialog(playerid, DIALOG_OFFER, DIALOG_STYLE_MSGBOX, capt, inf, "Tak", "Nie");
	
	if( fromid != INVALID_PLAYER_ID ) SendClientMessage(fromid, COLOR_LIGHTER_ORANGE, "Oferta zosta�a wys�ana. Odczekaj chwile, by przekona� si�, czy gracz j� zaakceptuje.");
	
	return 1;
}

stock OnPlayerOfferResponse(playerid, response)
{
	if(playerid == INVALID_PLAYER_ID) return 1;
	
	new sellerid = pOffer[playerid][offer_sellerid];
	new type = pOffer[playerid][offer_type];

	if( !response )
	{
		if( sellerid != INVALID_PLAYER_ID ) GameTextForPlayer(sellerid, "~w~oferta ~r~odrzucona", 3000, 3);
		
		GameTextForPlayer(playerid, "~w~oferta ~r~odrzucona", 3000, 3);

		if(type == OFFER_TYPE_TRIBUTE)
		{
			new gid = pInfo[playerid][player_duty_gid];
			new gidd = pInfo[sellerid][player_duty_gid];
			if(gid == -1 || gidd == -1) return 1;
			SendClientMessage(sellerid, COLOR_GOLD, sprintf("Lider grupy %s (UID: %d) - %s (UID: %d) odm�wi� p�acenia haraczu na rzecz grupy %s (UID %d).", pInfo[playerid][player_name], pInfo[playerid][player_id], Group[gid][group_name], Group[gid][group_uid], Group[gidd][group_name], Group[gidd][group_uid]));
		}

		for(new x=0; e_player_offer:x != e_player_offer; x++)
		{
			pOffer[playerid][e_player_offer:x] = 0;
			if( sellerid != INVALID_PLAYER_ID ) pOffer[sellerid][e_player_offer:x] = 0;
		}
	}
	else
	{
		pOffer[playerid][offer_accepted] = true;
		if( pOffer[playerid][offer_price] == 0 ) OnPlayerOfferFinish(playerid, 0, 1);
		else
		{
			ShowPlayerPayment(playerid);
		}
	}
	return 1;
}

stock OnPlayerOfferFinish(playerid, type, response)
{
	new sellerid = pOffer[playerid][offer_sellerid];
	type = pOffer[playerid][offer_type];
	
	if( !response )
	{
		if( sellerid != INVALID_PLAYER_ID )  GameTextForPlayer(sellerid, "~w~oferta ~r~odrzucona", 3000, 3);
	}
	else
	{
		logprintf(LOG_PLAYER, "[OFFER] %s (UID: %d) zaakceptowal oferte od %s (UID %d). Typ oferty: %d, cena: $%d", pInfo[playerid][player_name], pInfo[playerid][player_id], pInfo[sellerid][player_name], pInfo[sellerid][player_id], type, pOffer[playerid][offer_price]);
		if( sellerid != INVALID_PLAYER_ID && type != OFFER_TYPE_REPAIR && type != OFFER_TYPE_REFUEL) 
		{
			if(type == OFFER_TYPE_PRODUCT || type == OFFER_TYPE_GYM || type == OFFER_TYPE_DOCUMENT || type == OFFER_TYPE_MANDAT || type == OFFER_TYPE_MEDIC || type == OFFER_TYPE_ADVERTISE || type == OFFER_TYPE_RP || type == OFFER_TYPE_DOOR_ACCESS || type == OFFER_TYPE_GYM_FIGHT)
			{
				new gid = pInfo[sellerid][player_duty_gid];
				if(gid == -1) return 1;
				Group[gid][group_activity_points] += pOffer[playerid][offer_price];
				mysql_query(mySQLconnection, sprintf("UPDATE ipb_game_groups SET group_activity = %d WHERE group_uid = %d", Group[gid][group_activity_points], Group[gid][group_uid]));

				if(pOffer[playerid][offer_price] >= 40)
				{
					GiveGroupMoney(gid, pOffer[playerid][offer_price]-10);
					GivePlayerMoney(sellerid, 10);
				}
				else
				{
					GiveGroupMoney(gid, pOffer[playerid][offer_price]);
				}

				if(Group[gid][group_tributers] > 0)
				{
					if(Group[gid][group_tribute] < 3500)
					{
						Group[gid][group_tribute] += 10;
						mysql_query(mySQLconnection, sprintf("UPDATE ipb_game_groups SET group_tribute = %d WHERE group_uid = %d", Group[gid][group_tribute], Group[gid][group_uid]));
					}
				}
			}
			else
			{
				if(type != OFFER_TYPE_TAXI)
				{
					GivePlayerMoney(sellerid, pOffer[playerid][offer_price]);
				}
			}		
			GameTextForPlayer(sellerid, "~w~oferta ~g~zaakceptowana", 3000, 3);
			GameTextForPlayer(playerid, "~w~oferta ~g~zaakceptowana", 3000, 3);
		}
		
		switch( type )
		{
			case OFFER_TYPE_VCARD:
			{
				new itemid = GetPlayerUsedItem(playerid, ITEM_TYPE_PHONE);
				if( itemid > -1 )
				{
					new number[64];
					valstr(number, PlayerItem[sellerid][pOffer[playerid][offer_extraid]][player_item_value1]);
					mysql_query(mySQLconnection, sprintf("INSERT INTO `ipb_contacts` VALUES (null, %d, '%s', %d, 0)", PlayerItem[sellerid][pOffer[playerid][offer_extraid]][player_item_value1], number, PlayerItem[playerid][itemid][player_item_uid]));
					ProxMessage(sellerid, sprintf("wysy�a vCard %s", pInfo[playerid][player_name]), PROX_ME);
				}	
			}

			case OFFER_TYPE_PASS:
			{
				SetPlayerSpecialAction(sellerid, SPECIAL_ACTION_NONE);
				SetPlayerSpecialAction(playerid, pOffer[playerid][offer_extraid]);
			}

			case OFFER_TYPE_TOW:
			{
				new v = GetPlayerVehicleID(playerid);
				new vid = pOffer[playerid][offer_extraid];
				AttachTrailerToVehicle(vid, v);
				SendClientMessage(sellerid, COLOR_GOLD, "Holowanie zako�czy� mo�esz komend� /hak.");
				SetPVarInt(sellerid, "holowanie", 1);
			}

			case OFFER_TYPE_TRIBUTE:
			{
				new gid = pInfo[playerid][player_duty_gid];
				if(gid == -1) return SendGuiInformation(sellerid, "Informacja", "Zszed�e� se s�u�by grupy.");
				Group[gid][group_tributers] = pOffer[playerid][offer_extraid];
				new tgid = pInfo[sellerid][player_duty_gid];
				SendClientMessage(sellerid, COLOR_GOLD, sprintf("Grupa %s (UID: %d) zaakceptowa�a haracz od twojej grupy (%s, UID: %d).", Group[gid][group_name], Group[gid][group_uid], Group[tgid][group_name], Group[tgid][group_uid]));
				mysql_query(mySQLconnection, sprintf("UPDATE ipb_game_groups SET group_tributers = %d WHERE group_uid = %d", pOffer[playerid][offer_extraid], Group[gid][group_uid]));
			}

			case OFFER_TYPE_MEDIC:
			{
				cmd_ame(sellerid, sprintf("podaje lek %s.", pInfo[playerid][player_name]));
				SetPlayerHealth(playerid, 50);
			}

			case OFFER_TYPE_INVITE:
			{
				new gid = pOffer[playerid][offer_extraid];
				if(!Iter_Contains(Groups, gid)) return 1;

				new free_slot = GetPlayerGroupFreeSlot(playerid);
				if( free_slot == -1 ) return SendGuiInformation(playerid, "Informacja", "Nie posiadasz wolnych slot�w.");

				mysql_query(mySQLconnection, sprintf("INSERT INTO `ipb_char_groups` (char_uid, group_belongs, group_perm) VALUES (%d, %d, %d)", pInfo[playerid][player_id], Group[gid][group_uid], 0));
				
				pGroup[playerid][free_slot][pg_id] = gid;
				pGroup[playerid][free_slot][pg_rank_perm] = 0;
				if(!PlayerHasAchievement(playerid, ACHIEV_CAREER)) AddAchievement(playerid, ACHIEV_CAREER, 200);
			}

			case OFFER_TYPE_ADVERTISE:
			{
				SendClientMessage(sellerid, COLOR_GOLD, "Gracz zaakceptowa� oferte, nadaj teraz reklame o wybranej przez niego tre�ci.");
				SendClientMessage(playerid, COLOR_GOLD, "Zaakceptowa�e� oferte reklamy, reporter jest teraz zobowi�zany do jej nadania.");
			}

			case OFFER_TYPE_GYM:
            {
            	new gid = pInfo[sellerid][player_duty_gid];
            	if(gid == -1) return 1;
            	
                Item_Create(ITEM_OWNER_TYPE_PLAYER, playerid, ITEM_TYPE_GYM_KARNET, 2709, 10, Group[gid][group_uid], "Karnet");
                SendGuiInformation(playerid, "Informacja", "Karnet zosta� dodany do ekwipunku.");
            }

            case OFFER_TYPE_GYM_FIGHT:
            {
            	new gid = pInfo[sellerid][player_duty_gid];
            	if(gid == -1) return 1;
            	
                SendGuiInformation(playerid, "Informacja", "Mo�esz rozpocz�� trening na worku.");

                new style = pOffer[playerid][offer_extraid];

                SetPlayerFightingStyle(playerid, style);
                TextDrawSetString(Tutorial[playerid], "Zacznij uderzac worek by rozpoczac ~p~trening~w~.");
                TextDrawShowForPlayer(playerid, Tutorial[playerid]);

                pInfo[playerid][player_fight_training] = true;
            }

            case OFFER_TYPE_INTERVIEW:
            {
            	pInfo[sellerid][player_interview] = pInfo[sellerid][player_duty_gid];
            	pInfo[playerid][player_interview] = pInfo[sellerid][player_duty_gid];
                SendClientMessage(playerid, COLOR_GOLD, "Rozpocz�to wywiad. Wszystko co teraz napiszesz na zwyk�ym czacie zostanie wy�wietlone na pasku Weazel News.");
                SendClientMessage(sellerid, COLOR_GOLD, "Rozpocz�to wywiad. Wszystko co teraz napiszesz na zwyk�ym czacie zostanie wy�wietlone na pasku Weazel News.");
            }

			case OFFER_TYPE_REPAIR:
			{				
				new vehid = pOffer[playerid][offer_extraid];

				pInfo[sellerid][player_repair_car] = pOffer[playerid][offer_extraid];
				pInfo[sellerid][player_repair_time] = 30;
				pInfo[sellerid][player_repairing] = 1;
				Attach3DTextLabelToVehicle(Vehicle[vehid][vehicle_state_label], vehid, 0.0, 1.1, 0.0);
			}

			case OFFER_TYPE_REFUEL:
			{
				new str[64];
				new vehid = GetPlayerVehicleID(playerid);
				new amount = pOffer[playerid][offer_extraid];
				new model = Vehicle[vehid][vehicle_model];
				Vehicle[vehid][vehicle_fuel_current] = Vehicle[vehid][vehicle_fuel_current] + float(amount);

				format(str, sizeof(str), "tankuje pojazd %s.", VehicleNames[model-400]);
				ProxMessage(sellerid, str, PROX_ME);
				ApplyAnimation(sellerid, "INT_HOUSE", "wash_up",4.1, false, false, false, false, 0, 1);

				new price = pInfo[sellerid][player_dialog_tmp1];
				GivePlayerMoney(playerid, price);
			}

			case OFFER_TYPE_MONTAGE:
			{
				new vehid = GetPlayerVehicleID(playerid);

				pInfo[sellerid][player_montage_car] = vehid;
				pInfo[sellerid][player_montage_time] = 120;
				pInfo[sellerid][player_montage_comp] = pOffer[playerid][offer_extraid];
				Attach3DTextLabelToVehicle(Vehicle[vehid][vehicle_state_label], vehid, 0.0, 1.1, 0.0);
				pInfo[sellerid][player_tuning] = 1;
			}

			case OFFER_TYPE_DOOR_ACCESS:
			{
				new itemid = pOffer[playerid][offer_extraid];
				new access = PlayerItem[sellerid][itemid][player_item_value1];

				new d_id = GetDoorByUid(GetPlayerVirtualWorld(playerid));

				if(d_id != -1)
				{
					Door[d_id][door_access] += access;
					mysql_query(mySQLconnection, sprintf("UPDATE ipb_doors SET door_access = %d WHERE door_uid = %d", Door[d_id][door_access], Door[d_id][door_uid]));
					SendClientMessage(sellerid, COLOR_GOLD, "Monta� przebieg� pomy�lnie.");
					SendClientMessage(playerid, COLOR_GOLD, "Monta� przebieg� pomy�lnie.");
					DeleteItem(itemid, true, sellerid);
				}
				else
				{
					printf("Door access error, d_id -1.");
				}
			}

			case OFFER_TYPE_RESPRAY:
			{
				new vehid = GetPlayerVehicleID(playerid);

				pInfo[sellerid][player_spraying_car] = vehid;
				pInfo[sellerid][player_spraying_time] = 0;
				pInfo[sellerid][player_spraying_color2] = pOffer[playerid][offer_extraid];
				Attach3DTextLabelToVehicle(Vehicle[vehid][vehicle_state_label], vehid, 0.0, 1.1, 0.0);
				pInfo[sellerid][player_spraying] = 1;
			}

			case OFFER_TYPE_PAINTJOB:
			{
				new vehid = GetPlayerVehicleID(playerid);

				pInfo[sellerid][player_spraying_car] = vehid;
				pInfo[sellerid][player_spraying_time] = 0;
				pInfo[sellerid][player_spraying_color] = pOffer[playerid][offer_extraid];
				Attach3DTextLabelToVehicle(Vehicle[vehid][vehicle_state_label], vehid, 0.0, 1.1, 0.0);
				pInfo[sellerid][player_spraying] = 2;
			}

			case OFFER_TYPE_ACCESS:
			{
				new vehid = GetPlayerVehicleID(playerid);

				pInfo[sellerid][player_montage_car] = vehid;
				pInfo[sellerid][player_montage_time] = 120;
				pInfo[sellerid][player_montage_comp] = pOffer[playerid][offer_extraid];
				Attach3DTextLabelToVehicle(Vehicle[vehid][vehicle_state_label], vehid, 0.0, 1.1, 0.0);
				pInfo[sellerid][player_tuning] = 2;
			}

			case OFFER_TYPE_VEHICLE:
			{
				new vehid = pOffer[playerid][offer_extraid];
				new vehidd = GetPlayerVehicleID(sellerid);

				if( Vehicle[vehid][vehicle_uid] != Vehicle[vehidd][vehicle_uid])
				{
					SendGuiInformation(sellerid, "Informacja", "Odspawnowa�e� pojazd po z�o�eniu oferty, zg�oszono pr�b� bug abusingu.");
					KickAc(sellerid, -1, "Bug abusing try (o: veh)");
					return 1;
				}
				
				Vehicle[vehid][vehicle_owner] = pInfo[playerid][player_id];

				mysql_query(mySQLconnection, sprintf("UPDATE ipb_vehicles SET vehicle_owner = %d WHERE vehicle_uid = %d", Vehicle[vehid][vehicle_owner], Vehicle[vehid][vehicle_uid]));

				SendGuiInformation(playerid, "Informacja", "Zakupi�e� pojazd zgodnie z ofert�.");	
				SendGuiInformation(sellerid, "Informacja", "Pojazd zosta� sprzedany pomy�lnie.");
				RemovePlayerFromVehicle(sellerid);	
			}

			case OFFER_TYPE_ITEM:
			{
				new itemid = pOffer[playerid][offer_extraid];
				if( PlayerItem[sellerid][itemid][player_item_used] )
				{
					SendGuiInformation(sellerid, "Informacja", "U�y�e� przedmiotu po wys�aniu oferty. Otrzymujesz kicka za pr�b� bug abusingu.");
					KickAc(sellerid, -1, "Bug abusing try (item use)");
					return 1;
				}

				if(!Iter_Contains(PlayerItems[sellerid], itemid))
				{
					KickAc(sellerid, -1, "Bug abusing try (item drop)");
					return 1;
				}

				mysql_query(mySQLconnection, sprintf("UPDATE ipb_items SET item_owner = %d WHERE item_uid = %d", pInfo[playerid][player_id], PlayerItem[sellerid][itemid][player_item_uid]));
				new uid = PlayerItem[sellerid][itemid][player_item_uid];
				DeleteItem(itemid, false, sellerid);
				new item = LoadPlayerItem(playerid, sprintf("WHERE item_uid = %d", uid));

				pInfo[sellerid][player_capacity] += PlayerItem[playerid][item][player_item_weight];
				pInfo[playerid][player_capacity] -= PlayerItem[playerid][item][player_item_weight];

				if(!PlayerHasAchievement(sellerid, ACHIEV_ITEM)) AddAchievement(sellerid, ACHIEV_ITEM, 300);
			}

			case OFFER_TYPE_DOOR:
			{
				new d_id = pOffer[playerid][offer_extraid];

				Door[d_id][door_owner] = pInfo[playerid][player_id];
				mysql_query(mySQLconnection, sprintf("UPDATE ipb_doors SET door_owner = %d WHERE door_uid = %d", Door[d_id][door_owner], Door[d_id][door_uid]));
				SendClientMessage(playerid, COLOR_GOLD, sprintf("Gratulacje, zosta�e� nowym w�a�cicielem drzwi %s.", Door[d_id][door_name]));
				SendClientMessage(sellerid, COLOR_GOLD, sprintf("Drzwi %s zosta�y sprzedane.", Door[d_id][door_name]));
			}

			case OFFER_TYPE_PRODUCT:
			{
				new itemid = pOffer[playerid][offer_extraid];
				new gid = pInfo[sellerid][player_duty_gid];
				new itemname[40];
				format(itemname, sizeof(itemname), Item[itemid][item_name]);

				Item_Create(ITEM_OWNER_TYPE_PLAYER, playerid, Item[itemid][item_type], Item[itemid][item_model], Item[itemid][item_value1], Item[itemid][item_value2], itemname, Item[itemid][item_extraid]);

				printf("[OFFER %s (%d)] %s (UID %d) sprzeda� produkt %s (UID %d) dla gracza %s (UID %d). Cena: $%d.", Group[gid][group_name], Group[gid][group_uid], pInfo[sellerid][player_name], pInfo[sellerid][player_id], Item[itemid][item_name], Item[itemid][item_uid], pInfo[playerid][player_name], pInfo[playerid][player_id], Item[itemid][item_price]);	
				
				pInfo[sellerid][player_capacity] += Item[itemid][item_weight];
				pInfo[playerid][player_capacity] -= Item[itemid][item_weight];

				if(Item[itemid][item_count] <= 1)
				{
					Item[itemid][item_group] = 0;
					Item[itemid][item_count] = 0;
					DeleteItem(itemid, true);
				}
				else
				{
					Item[itemid][item_count]--;
					mysql_query(mySQLconnection, sprintf("UPDATE ipb_items SET item_count = %d WHERE item_uid = %d", Item[itemid][item_count], Item[itemid][item_uid]));
				}

				SendGuiInformation(playerid, "Informacja", "Przedmiot zosta� dodany do ekwipunku.");	
				SendGuiInformation(sellerid, "Informacja", "Produkt zosta� sprzedany.");
			}

			case OFFER_TYPE_TAXI:
			{
				pInfo[sellerid][player_taxi_veh] = GetPlayerVehicleID(sellerid);
				pInfo[playerid][player_taxi_veh] = GetPlayerVehicleID(sellerid);
				pInfo[sellerid][player_taxi_price] = pOffer[playerid][offer_extraid];
				pInfo[sellerid][player_taxi_distance] = 0;
				pInfo[sellerid][player_taxi_passenger] = playerid;
			
				pInfo[sellerid][player_taxi_drive] = true;
			}

			case OFFER_TYPE_YO:
			{
				new welcome_type = pOffer[playerid][offer_extraid],
		    	Float:PosX, Float:PosY, Float:PosZ, Float:PosA;

		    	new offererid = pOffer[playerid][offer_sellerid];

				GetPlayerPos(offererid, PosX, PosY, PosZ);
				
				if(welcome_type <= 6)
				{
					GetXYInFrontOfPlayer(offererid, PosX, PosY, 1.0);
				}
				else
				{
		            GetXYInFrontOfPlayer(offererid, PosX, PosY, 0.5);
				}
				
				GetPlayerFacingAngle(offererid, PosA);

				SetPlayerPos(playerid, PosX, PosY, PosZ);
				SetPlayerFacingAngle(playerid, (floatabs(PosA) - 180.0));
				
				switch(welcome_type)
				{
				    case 1:
				    {
		  				ApplyAnimation(offererid, "GANGS", "hndshkfa", 4.1, false, false, false, false, 0, 1);
						ApplyAnimation(playerid, "GANGS", "hndshkfa", 4.1, false, false, false, false, 0, 1);
				    }
				    case 2:
				    {
		  				ApplyAnimation(offererid, "GANGS", "hndshkba", 4.1, false, false, false, false, 0, 1);
						ApplyAnimation(playerid, "GANGS", "hndshkba", 4.1, false, false, false, false, 0, 1);
				    }
				    case 3:
				    {
				    	ApplyAnimation(offererid, "GANGS", "hndshkaa", 4.1, false, false, false, false, 0, 1);
						ApplyAnimation(playerid, "GANGS", "hndshkaa", 4.1, false, false, false, false, 0, 1);
				    }
				    case 4:
				    {
		  				ApplyAnimation(offererid, "GANGS", "hndshkda", 4.1, false, false, false, false, 0, 1);
						ApplyAnimation(playerid, "GANGS", "hndshkda", 4.1, false, false, false, false, 0, 1);
				    }
				    case 5:
				    {
		  				ApplyAnimation(offererid, "GANGS", "hndshkca", 4.1, false, false, false, false, 0, 1);
						ApplyAnimation(playerid, "GANGS", "hndshkca", 4.1, false, false, false, false, 0, 1);
				    }
				    case 6:
				    {
		  				ApplyAnimation(offererid, "GANGS", "hndshkcb", 4.1, false, false, false, false, 0, 1);
						ApplyAnimation(playerid, "GANGS", "hndshkcb", 4.1, false, false, false, false, 0, 1);
				    }
				    case 7:
				    {
						ApplyAnimation(offererid, "KISSING", "Playa_Kiss_01", 4.1, false, false, false, false, 0, 1);
						ApplyAnimation(playerid, "KISSING", "Playa_Kiss_01", 4.1, false, false, false, false, 0, 1);
					}
					case 8:
					{
						ApplyAnimation(offererid, "KISSING", "Playa_Kiss_02", 4.1, false, false, false, false, 0, 1);
						ApplyAnimation(playerid, "KISSING", "Playa_Kiss_02", 4.1, false, false, false, false, 0, 1);
					}
					case 9:
					{
						ApplyAnimation(offererid, "KISSING", "Playa_Kiss_03", 4.1, false, false, false, false, 0, 1);
						ApplyAnimation(playerid, "KISSING", "Playa_Kiss_03", 4.1, false, false, false, false, 0, 1);
					}
				}
			}
			
			case OFFER_TYPE_DOCUMENT:
			{
				pInfo[playerid][player_documents] += pOffer[playerid][offer_extraid];
				mysql_query(mySQLconnection, sprintf("UPDATE `ipb_characters` SET `char_documents` = %d WHERE `char_uid` = %d", pInfo[playerid][player_documents], pInfo[playerid][player_id]));
			}
		}
	}
	
	for(new x=0; e_player_offer:x != e_player_offer; x++)
	{
		pOffer[playerid][e_player_offer:x] = 0;
		if( sellerid != INVALID_PLAYER_ID ) pOffer[sellerid][e_player_offer:x] = 0;
	}
	return 1;
}