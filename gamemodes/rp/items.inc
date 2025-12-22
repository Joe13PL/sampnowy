stock ListPlayerItems(playerid)
{
	new count, string[1024], header[64];
		
	DynamicGui_Init(playerid);
	
	format(string, sizeof(string), "%sNazwa\tWaga\tUID\n", string);
	format(header, sizeof(header), "Lista dost�pnych przedmiot�w [Ud�wig: %dg]", pInfo[playerid][player_capacity]);

	foreach (new item : PlayerItems[playerid])
	{
		new prefix[16]; 
		if( PlayerItem[playerid][item][player_item_used] ) prefix = "{D6EE76}";

		format(string, sizeof(string), "%s %s%s\t%dg\t%d\n", string, prefix, PlayerItem[playerid][item][player_item_name],PlayerItem[playerid][item][player_item_weight], PlayerItem[playerid][item][player_item_uid]);
		DynamicGui_AddRow(playerid, DG_ITEMS_ITEM_ROW, item);	
		count++;
	}
	
	if( count == 0 ) SendGuiInformation(playerid, ""guiopis"Informacja", "Niestety nie posiadasz �adnych przedmiot�w!\nAby naby� pocz�tkowe przedmioty udaj si� do sklepu 24/7.");
	else ShowPlayerDialog(playerid, DIALOG_PLAYER_ITEMS, DIALOG_STYLE_TABLIST_HEADERS, header, string, "U�yj", "Opcje");
	return 1;
}

stock ListPlayerGroundItems(playerid)
{
    new list_items[512], main_query[2048], query[256],
        ground_item_uid, ground_item_name[40];

    format(main_query, sizeof(main_query), "SELECT `item_uid`, `item_name` FROM `ipb_items` WHERE ");

    DynamicGui_Init(playerid);

    if(!IsPlayerInAnyVehicle(playerid))
    {
        new object_id = INVALID_OBJECT_ID, Float:distance,
            Float:PosX, Float:PosY, Float:PosZ;

        GetPlayerPos(playerid, PosX, PosY, PosZ);
        for (new player_object = 0; player_object <= MAX_VISIBLE_OBJECTS; player_object++)
        {
            if(IsValidPlayerObject(playerid, player_object))
            {
                object_id = Streamer_GetItemStreamerID(playerid, STREAMER_TYPE_OBJECT, player_object);
                Streamer_GetDistanceToItem(PosX, PosY, PosZ, STREAMER_TYPE_OBJECT, object_id, distance);

                if(distance <= 3.0 && Object[object_id][object_uid] == 0)
                {
                    ground_item_uid = (Streamer_GetIntData(STREAMER_TYPE_OBJECT, object_id, E_STREAMER_EXTRA_ID) * -1);
                    format(query, sizeof(query), "item_uid = %d", ground_item_uid);

   					if(strlen(main_query) > 64)
   					{
   				    	if(strlen(main_query) + strlen(query) < sizeof(main_query))
   				    	{
				   			strcat(main_query, " OR ", sizeof(main_query));
						}
						else
						{
					    	strcat(main_query, ";", sizeof(main_query));
						}
					}
			  		strcat(main_query, query, sizeof(main_query));
                }
            }
        }

        if(!ground_item_uid)
        {
        	SendGuiInformation(playerid, "Informacja", "Brak przedmiot�w w pobli�u.");
        	return 1;
        }
        
        new rows, fields;
        
        mysql_query(mySQLconnection, main_query);
       	cache_get_data(rows, fields);
       	
		for(new row = 0; row != rows; row++)
		{
			ground_item_uid = cache_get_row_int(row, 0);
			cache_get_row(row, 1, ground_item_name, mySQLconnection, 40);

			format(list_items, sizeof(list_items), "%s\n%d\t%s", list_items, ground_item_uid, ground_item_name);
			DynamicGui_AddRow(playerid, DG_ITEMS_PICKUP_ROW, ground_item_uid);
		}
		
		if(strlen(list_items) > 0)
		{
			ShowPlayerDialog(playerid, DIALOG_ITEMS_PICKUP, DIALOG_STYLE_LIST, "Przedmioty w pobli�u:", list_items, "Podnie�", "Anuluj");
      	}
       	else
       	{
        	SendGuiInformation(playerid, "Informacja", "Brak przedmiot�w w pobli�u.");
		}
    }
    else
    {
    	new vid = GetPlayerVehicleID(playerid), rows, fields;

    	new gid = pInfo[playerid][player_duty_gid];
    	if(gid != -1)
    	{
    		if(!GroupHasFlag(gid, GROUP_FLAG_TAKE))
    		{
		    	if(!CanPlayerUseVehicle(playerid, vid))
		    	{
		    		SendGuiInformation(playerid, "Informacja", "Nie posiadasz uprawnie� do przeszukiwania tego pojazdu.");
		    		return 1;
		    	}
		    }
	    }
	    else
	    {
	    	if(!CanPlayerUseVehicle(playerid, vid))
	    	{
	    		SendGuiInformation(playerid, "Informacja", "Nie posiadasz uprawnie� do przeszukiwania tego pojazdu.");
	    		return 1;
	    	}
	    }

    	mysql_query(mySQLconnection, sprintf("SELECT item_uid, item_name FROM ipb_items WHERE item_owner = %d AND item_ownertype = %d", Vehicle[vid][vehicle_uid], ITEM_OWNER_TYPE_VEHICLE));
    	cache_get_data(rows, fields);

    	for(new row = 0; row != rows; row++)
		{
		    ground_item_uid = cache_get_row_int(row, 0);
			cache_get_row(row, 1, ground_item_name, mySQLconnection, 40);

			format(list_items, sizeof(list_items), "%s\n%d\t%s", list_items, ground_item_uid, ground_item_name);
			DynamicGui_AddRow(playerid, DG_ITEMS_PICKUP_ROW, ground_item_uid);
		}

		if(strlen(list_items) > 0)
		{
			ShowPlayerDialog(playerid, DIALOG_ITEMS_PICKUP, DIALOG_STYLE_LIST, "Przedmioty w poje�dzie:", list_items, "Podnie�", "Anuluj");
      	}
       	else
       	{
        	SendGuiInformation(playerid, "Informacja", "Brak przedmiot�w w poje�dzie.");
		}
    }
    return 1;
}

stock LoadPlayerItem(playerid, limit[] = "", bool:return_id = false)
{
	new rows, fields;
	mysql_query(mySQLconnection, sprintf("SELECT * FROM `ipb_items` %s", limit));  
	cache_get_data(rows, fields);
	
	new i_id = -1, ownertype;
	
	for(new row = 0; row != rows; row++)
	{
		ownertype = cache_get_row_int(row, 14);

		if(ownertype == ITEM_OWNER_TYPE_PLAYER)
		{
			i_id = Iter_Free(PlayerItems[playerid]);
			Iter_Add(PlayerItems[playerid], i_id);
			
			PlayerItem[playerid][i_id][player_item_uid] = cache_get_row_int(row, 0);
			PlayerItem[playerid][i_id][player_item_type] = cache_get_row_int(row, 4);
			PlayerItem[playerid][i_id][player_item_value1] = cache_get_row_int(row, 2);
			PlayerItem[playerid][i_id][player_item_value2] = cache_get_row_int(row, 3);
			PlayerItem[playerid][i_id][player_item_used] = cache_get_row_int(row, 19);
			
			PlayerItem[playerid][i_id][player_item_model] = cache_get_row_int(row, 18);
			
			PlayerItem[playerid][i_id][player_item_extraid] = cache_get_row_int(row, 23);
			PlayerItem[playerid][i_id][player_item_weight] = cache_get_row_int(row, 13);
			PlayerItem[playerid][i_id][player_item_created] = cache_get_row_int(row, 20);
			PlayerItem[playerid][i_id][player_item_condition] = cache_get_row_float(row, 24);
			
			cache_get_row(row, 1, PlayerItem[playerid][i_id][player_item_name], mySQLconnection, 40);

			SetupPlayerItemWeight(playerid, i_id);
		}
		else
		{
			return 1;
		}
	}
	
	if( return_id ) return i_id;
	return 1;
}

stock LoadItem(limit[] = "", bool:return_id = false, bool:force_date = false)
{
	new rows, fields;
	mysql_query(mySQLconnection, sprintf("SELECT * FROM `ipb_items` %s", limit));  
	cache_get_data(rows, fields);
	
	new i_id = -1, ownertype;
	
	for(new row = 0; row != rows; row++)
	{
		ownertype = cache_get_row_int(row, 14);

		if(ownertype == ITEM_OWNER_TYPE_GROUP)
		{
			i_id = Iter_Free(Items);
			Iter_Add(Items, i_id);
			
			Item[i_id][item_uid] = cache_get_row_int(row, 0);
			Item[i_id][item_type] = cache_get_row_int(row, 4);
			Item[i_id][item_value1] = cache_get_row_int(row, 2);
			Item[i_id][item_value2] = cache_get_row_int(row, 3);
			
			Item[i_id][item_owner_type] = cache_get_row_int(row, 14);
			Item[i_id][item_owner] = cache_get_row_int(row, 15);
			Item[i_id][item_group] = cache_get_row_int(row, 17);
			
			Item[i_id][item_model] = cache_get_row_int(row, 18);

			Item[i_id][item_price] = cache_get_row_int(row, 21);
			Item[i_id][item_count] = cache_get_row_int(row, 22);

			Item[i_id][item_extraid] = cache_get_row_int(row, 23);
			Item[i_id][item_weight] = cache_get_row_int(row, 13);
			
			cache_get_row(row, 1, Item[i_id][item_name], mySQLconnection, 40);
		}

		else if(ownertype == ITEM_OWNER_TYPE_GROUND)
		{
			new Float:ground_x, Float:ground_y, Float:ground_z, Float:ground_rx, Float:ground_ry, Float:ground_rz, ground_world, ground_interior, ground_object, ground_model, ground_uid, ground_date;

			ground_date = cache_get_row_int(row, 20);
			ground_world = cache_get_row_int(row, 11);

			if(ground_world == 0)
			{
				if(ground_date + 48*3600 > gettime())
				{
					ground_uid = cache_get_row_int(row, 0);
					ground_model = cache_get_row_int(row, 18);

					ground_x = cache_get_row_float(row, 5);
					ground_y = cache_get_row_float(row, 6);
					ground_z = cache_get_row_float(row, 7);
					ground_rx = cache_get_row_float(row, 8);
					ground_ry = cache_get_row_float(row, 9);
					ground_rz = cache_get_row_float(row, 10);
					
					ground_interior = cache_get_row_int(row, 12);

					ground_object = CreateDynamicObject(ground_model, ground_x, ground_y, ground_z, ground_rx, ground_ry, ground_rz, ground_world, ground_interior, -1, 100.0);
					Streamer_SetIntData(STREAMER_TYPE_OBJECT, ground_object, E_STREAMER_EXTRA_ID, (ground_uid * -1));
				}
				else
				{
					if(force_date == true)
					{
						ground_uid = cache_get_row_int(row, 0);
						ground_model = cache_get_row_int(row, 18);

						ground_x = cache_get_row_float(row, 5);
						ground_y = cache_get_row_float(row, 6);
						ground_z = cache_get_row_float(row, 7);
						ground_rx = cache_get_row_float(row, 8);
						ground_ry = cache_get_row_float(row, 9);
						ground_rz = cache_get_row_float(row, 10);

						ground_interior = cache_get_row_int(row, 12);

						ground_object = CreateDynamicObject(ground_model, ground_x, ground_y, ground_z, ground_rx, ground_ry, ground_rz, ground_world, ground_interior, -1, 100.0);
						Streamer_SetIntData(STREAMER_TYPE_OBJECT, ground_object, E_STREAMER_EXTRA_ID, (ground_uid * -1));
					}
				}
			}
			else
			{
				ground_uid = cache_get_row_int(row, 0);
				ground_model = cache_get_row_int(row, 18);

				ground_x = cache_get_row_float(row, 5);
				ground_y = cache_get_row_float(row, 6);
				ground_z = cache_get_row_float(row, 7);
				ground_rx = cache_get_row_float(row, 8);
				ground_ry = cache_get_row_float(row, 9);
				ground_rz = cache_get_row_float(row, 10);

				ground_interior = cache_get_row_int(row, 12);

				ground_object = CreateDynamicObject(ground_model, ground_x, ground_y, ground_z, ground_rx, ground_ry, ground_rz, ground_world, ground_interior, -1, 100.0);
				Streamer_SetIntData(STREAMER_TYPE_OBJECT, ground_object, E_STREAMER_EXTRA_ID, (ground_uid * -1));
			}
		}
	}
	
	if( return_id ) return i_id;
	return rows;
}

stock DeleteItem(itemid, bool:from_db = false, playerid = INVALID_PLAYER_ID)
{
	if(playerid != INVALID_PLAYER_ID)
	{
		if( from_db ) mysql_query(mySQLconnection, sprintf("DELETE FROM `ipb_items` WHERE `item_uid` = %d", PlayerItem[playerid][itemid][player_item_uid]));

		Iter_Remove(PlayerItems[playerid], itemid);

		for(new z=0; e_player_items:z != e_player_items; z++)
	    {
			PlayerItem[playerid][itemid][e_player_items:z] = 0;
	    }
	}
	else
	{
		if( from_db ) mysql_query(mySQLconnection, sprintf("DELETE FROM `ipb_items` WHERE `item_uid` = %d", Item[itemid][item_uid]));
		Iter_Remove(Items, itemid);
	
		for(new z=0; e_items:z != e_items; z++)
	    {
			Item[itemid][e_items:z] = 0;
	    }
	}
}

stock Item_FastUsage(playerid, const sub_cmd[], param[])
{
	new last_param[40], targetid, price;

	foreach(new item: PlayerItems[playerid])
	{
	 	if(strfind(PlayerItem[playerid][item][player_item_name], sub_cmd, true) >= 0)
	 	{
	 		sscanf(param, "s[40]dd", last_param, targetid, price);
			
			if( !strcmp(last_param, "daj") )
			{
				if(!IsPlayerConnected(targetid)) SendGuiInformation(playerid, "Informacja", "Niepoprawne id gracza.");
				break;
			}

			else if( !strcmp(last_param, "odloz") )
			{
				Item_Drop(item, playerid);
				break;
			}

			else if( !strcmp(last_param, "sprzedaj") )
			{
				if(!IsPlayerConnected(targetid)) SendGuiInformation(playerid, "Informacja", "Niepoprawne id gracza.");
				break;
			}	

 			if(PlayerItem[playerid][item][player_item_used])
 			{
 				Item_Use(item, playerid, true);
 				break;
 			}
 			else
 			{
	 		 	if(strlen(last_param) <= 0 && PlayerItem[playerid][item][player_item_type] == ITEM_TYPE_WEAPON)
	 		 	{
	 		 		SendPlayerInformation(playerid, "~w~Probujac wyjac bron za pomoca komendy ~p~/p~w~ podaj dodaktowy parametr. Miejsce z ktorego chcesz wyjac przedmiot.~n~~n~Przyklad: /p glock zza paska.", 8000);
	 		 		return 1;
	 		 	}
	 		 	
	 		 	Item_Use(item, playerid, true, param);
	 		 	break;
	 		 }
	 	}
	}
	return 1;
}

stock Item_Use(itemid, playerid = INVALID_PLAYER_ID, bool:via_cmd = false, addparam[]="")
{	
	switch( PlayerItem[playerid][itemid][player_item_type] )
	{
		case ITEM_TYPE_WEAPON:
		{
			if( PlayerItem[playerid][itemid][player_item_used] )
			{
				new wslot = GetWeaponSlot(PlayerItem[playerid][itemid][player_item_value1]);
				
				if(PlayerItem[playerid][itemid][player_item_value1] == 41)
				{
					new tmp, ammo;
					GetPlayerWeaponData(playerid, 9, tmp, ammo);
					PlayerItem[playerid][itemid][player_item_value2] = ammo;
				}
				else
				{
					PlayerItem[playerid][itemid][player_item_value2] = pWeapon[playerid][wslot][pw_ammo];
				}
				
				PlayerItem[playerid][itemid][player_item_used] = false;
				
				if( pWeapon[playerid][wslot][pw_object_index] > -1 ) RemovePlayerAttachedObject(playerid, pWeapon[playerid][wslot][pw_object_index]);
				
				pWeapon[playerid][wslot][pw_itemid] = -1;
				pWeapon[playerid][wslot][pw_id] = -1;
				pWeapon[playerid][wslot][pw_ammo] = -1;
				pWeapon[playerid][wslot][pw_object_index] = -1;
				
				ResetPlayerWeapons(playerid);
				
				for(new i;i<13;i++)
				{
					if( pWeapon[playerid][i][pw_itemid] > -1 ) GivePlayerWeapon(playerid, pWeapon[playerid][i][pw_id], pWeapon[playerid][i][pw_ammo]);
				}
				
				mysql_query(mySQLconnection, sprintf("UPDATE `ipb_items` SET `item_value2` = %d, `item_condition` = %f WHERE `item_uid` = %d", PlayerItem[playerid][itemid][player_item_value2], PlayerItem[playerid][itemid][player_item_condition], PlayerItem[playerid][itemid][player_item_uid]));

				SetPVarInt(playerid, "taser", 0);

				if(via_cmd == true)
				{
					if(strlen(addparam))
					{
						cmd_ame(playerid, sprintf("chowa %s %s.", PlayerItem[playerid][itemid][player_item_name], addparam));
					}
					else
					{
						cmd_ame(playerid, sprintf("chowa %s.", PlayerItem[playerid][itemid][player_item_name]));
					}
				}
				else
				{
					cmd_ame(playerid, sprintf("chowa %s.", PlayerItem[playerid][itemid][player_item_name]));
				}
			}
			else
			{
				if(IsPlayerInAnyVehicle(playerid)) return SendGuiInformation(playerid, "Informacja", "Akcja niedozwolona w poje�dzie.");

				new wslot = GetWeaponSlot(PlayerItem[playerid][itemid][player_item_value1]);

				// Sprawdzamy czy ma godziny
				if( pInfo[playerid][player_hours] < 1 ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Musisz przegra� conajmniej godzin� na serwerze aby u�y� broni.");
				
				// Sprawdzamy czy jest amunicja
				if( PlayerItem[playerid][itemid][player_item_value2] == 0 ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "W tej broni amunicja zosta�a ca�kowicie zu�yta.");

				if( PlayerItem[playerid][itemid][player_item_condition] <= 0 ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Ta bro� jest ca�kowicie zniszczona.");
				
				// Sprawdzamy czy czasem nie ma juz broni tego samego typu
				new wtype = GetWeaponType(PlayerItem[playerid][itemid][player_item_value1]);
				for(new i;i<13;i++)
				{
					if( pWeapon[playerid][i][pw_itemid] > -1 && GetWeaponType(pWeapon[playerid][i][pw_id]) == wtype ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Aktualnie u�ywasz broni tego typu.\nSchowaj obecn�, a nast�pnie spr�buj ponownie.");
				}
				
				if( GetPlayerUsedItem(playerid, ITEM_TYPE_GLOVES) == -1 )
				{
					mysql_query(mySQLconnection, sprintf("INSERT INTO `ipb_fingerprints` (finger_owner, finger_person) VALUES ('%d', '%d')", PlayerItem[playerid][itemid][player_item_uid], pInfo[playerid][player_id]));
				}
				
				if( PlayerItem[playerid][itemid][player_item_extraid] == 1)
				{
					new gid = pInfo[playerid][player_duty_gid];
					if(gid == -1) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Nie posiadasz uprawnie� do u�ycia tego przedmiotu, lub nie jeste� na s�u�bie grupy z flag� upowa�niaj�c� do tego.");
					if(!(Group[gid][group_flags] & GROUP_FLAG_WEAPON_FLAG)) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Nie posiadasz uprawnie� do u�ycia tego przedmiotu, lub nie jeste� na s�u�bie grupy z flag� upowa�niaj�c� do tego.");
				}

				if( PlayerItem[playerid][itemid][player_item_extraid] == 2)
				{
					new gid = pInfo[playerid][player_duty_gid];
					if(gid == -1) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Nie posiadasz uprawnie� do u�ycia tego przedmiotu, lub nie jeste� na s�u�bie grupy z flag� upowa�niaj�c� do tego.");
					if(!(Group[gid][group_flags] & GROUP_FLAG_911)) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Nie posiadasz uprawnie� do u�ycia tego przedmiotu, lub nie jeste� na s�u�bie grupy z flag� upowa�niaj�c� do tego.");
					SetPVarInt(playerid, "taser", 1);
				}

				if( PlayerItem[playerid][itemid][player_item_extraid] == 3)
				{
					if(!(pInfo[playerid][player_documents] & DOCUMENT_WEAPON)) return SendGuiInformation(playerid, "Informacja", "Nie posiadasz licencji na bro�.");
				}

				//Edycja obiektu doczepialnego broni
				new ow = PlayerItem[playerid][itemid][player_item_value1], rows, fields;

				mysql_query(mySQLconnection, sprintf("SELECT * FROM ipb_attached_objects WHERE attach_owner = %d AND attach_model = %d LIMIT 1", pInfo[playerid][player_id], WeaponVisualModel[ow]));
				cache_get_data(rows, fields);

				if(wslot == -1) return SendGuiInformation(playerid, "Informacja", "Spr�buj ponownie.");

				new index = GetPlayerFreeWeaponAttachSlot(playerid);

				if(index != -1)
				{
					if( rows )
					{
						ao[playerid][index][ao_model] = cache_get_row_int(0, 3);
						ao[playerid][index][ao_x] = cache_get_row_float(0, 5);
				        ao[playerid][index][ao_y] = cache_get_row_float(0, 6);
				        ao[playerid][index][ao_z] = cache_get_row_float(0, 7);
				        ao[playerid][index][ao_rx] = cache_get_row_float(0, 8);
				        ao[playerid][index][ao_ry] = cache_get_row_float(0, 9);
				        ao[playerid][index][ao_rz] = cache_get_row_float(0, 10);

						ao[playerid][index][ao_inserted] = true;
					}
					else
					{
						ao[playerid][index][ao_inserted] = false;
					}
				}

				
				pWeapon[playerid][wslot][pw_itemid] = itemid;
				pWeapon[playerid][wslot][pw_id] = PlayerItem[playerid][itemid][player_item_value1];
				pWeapon[playerid][wslot][pw_ammo] = PlayerItem[playerid][itemid][player_item_value2];
				GivePlayerWeapon(playerid, PlayerItem[playerid][itemid][player_item_value1], PlayerItem[playerid][itemid][player_item_value2]);
				
				PlayerItem[playerid][itemid][player_item_used] = true;

				if(via_cmd == false)
				{
					cmd_ame(playerid, sprintf("wyci�ga %s.", PlayerItem[playerid][itemid][player_item_name]));
				}
				else
				{
					cmd_ame(playerid, sprintf("wyci�ga %s %s.", PlayerItem[playerid][itemid][player_item_name], addparam));
				}
			}
		}

       	case ITEM_TYPE_ACCESS:
        {
        	if(PlayerItem[playerid][itemid][player_item_value1] == VEHICLE_ACCESSORY_AUDIO)
        	{
        		if(!IsPlayerInAnyVehicle(playerid)) return SendGuiInformation(playerid, "Informacja", "Nie znajdujesz si� w �adnym poje�dzie.");
        		new vid = GetPlayerVehicleID(playerid);
        		if(vid == INVALID_VEHICLE_ID) return SendGuiInformation(playerid, "Informacja", "Nie znajdujesz si� w �adnym poje�dzie.");
        		if(!CanPlayerUseVehicle(playerid, vid)) return SendGuiInformation(playerid, "Informacja", "Brak uprawnie� do zarz�dzania tym pojazdem.");
        		if(VehicleHasAccessory(vid, VEHICLE_ACCESSORY_AUDIO)) return SendGuiInformation(playerid, "Informacja", "Ten pojazd posiada ju� radio.");

        		Vehicle[vid][vehicle_accessories] += VEHICLE_ACCESSORY_AUDIO;
                           
                mysql_query(mySQLconnection, sprintf("UPDATE ipb_vehicles SET vehicle_access = '%d' WHERE vehicle_uid = %d", Vehicle[vid][vehicle_accessories], Vehicle[vid][vehicle_uid]));

                SendGuiInformation(playerid,"Informacja", "Radio zosta�o pomy�lnie zamontowane w twoim poje�dzie.");
                cmd_ame(playerid, "montuje radio w samochodzie.");
                DeleteItem(itemid, true, playerid);
        		return 1;
        	}

            if( !PlayerItem[playerid][itemid][player_item_used] )
            {
            	if( GetPlayerUsedItem(playerid, ITEM_TYPE_ACCESS) > -1)
				{
					SendGuiInformation(playerid, "Informacja", "U�ywasz ju� jakiego� przedmiotu komponentu.");
					return 1;
				}

                SendGuiInformation(playerid, ""guiopis"Powiadomienie", "U�y�e� komponentu, mo�esz go teraz zamonotowa� przez /o komponent.");
                PlayerItem[playerid][itemid][player_item_used]=true;
            }
            else
            {
                PlayerItem[playerid][itemid][player_item_used]=false;
            }
        }

        case ITEM_TYPE_DOOR_ACCESS:
        {
            if( !PlayerItem[playerid][itemid][player_item_used] )
            {
            	if( GetPlayerUsedItem(playerid, ITEM_TYPE_DOOR_ACCESS) > -1)
				{
					SendGuiInformation(playerid, "Informacja", "U�ywasz ju� jakiego� przedmiotu o tym samym typie.");
					return 1;
				}

                SendGuiInformation(playerid, ""guiopis"Powiadomienie", "U�y�e� komponentu do drzwi, mo�esz go teraz zamonotowa� przez /o zamontuj.");
                PlayerItem[playerid][itemid][player_item_used]=true;
            }
            else
            {
                PlayerItem[playerid][itemid][player_item_used]=false;
            }
        }

		case ITEM_TYPE_BOMB:
		{
			if( PlayerItem[playerid][itemid][player_item_used] )
			{
				PlayerItem[playerid][itemid][player_item_used]=false;
				DeleteItem(itemid, true, playerid);
			}
			else
			{
				SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Aby u�y� �adunku wybuchowego podejd� do auta w kt�rym chcesz zamontowa� nowy �adunek i u�yj komendy /ladunek.");
				PlayerItem[playerid][itemid][player_item_used]=true;
			}
		}

		case ITEM_TYPE_MOLOTOV:
		{
			if(Fire[fire_is_burning]) return SendGuiInformation(playerid, "Informacja", "W tym momencie pali si� jaki� inny budynek.");
			if(!IsAnyFirefighterOnline()) return SendGuiInformation(playerid, "Informacja", "Aby u�y� tego przedmiotu na s�u�bie musz� by� stra�acy.");

			new gid = pInfo[playerid][player_duty_gid];
			if(gid == -1) return SendGuiInformation(playerid, "Informacja", "Nie znajdujesz si� na s�u�bie grupy dla kt�rej przeznaczony jest ten przedmiot.");
			if(Group[gid][group_uid] != PlayerItem[playerid][itemid][player_item_value2]) return SendGuiInformation(playerid, "Informacja", "Nie znajdujesz si� na s�u�bie grupy dla kt�rej przeznaczony jest ten przedmiot.");

			new d_uid = GetPlayerVirtualWorld(playerid);
			if(d_uid == 0 ) return SendGuiInformation(playerid, "Informacja", "Tego przedmiotu mo�esz u�y� tylko w drzwiach do niego przypisanych.");
			if(d_uid != PlayerItem[playerid][itemid][player_item_value1]) return SendGuiInformation(playerid, "Informacja", "Ten przedmiot nie jest przeznaczony do u�ycia w tych drzwiach.");

			new d_id = GetDoorByUid(d_uid);
			if(d_id == -1) return SendGuiInformation(playerid, "Informacja", "Nie znaleziono drzwi o UID przypisanym do przedmiotu.");

			new source = CreateScriptedSource(d_id, Door[d_id][door_pos][0], Door[d_id][door_pos][1], Door[d_id][door_pos][2]);
			if(source == -1) return printf("[Fire] Niepoprawny source id.");
			SetupScriptedFire(source);

			SendGuiInformation(playerid, "Informacja", "Po�ar zosta� pomy�lnie zainicjowany.");

			if( DoorHasAccess(d_id, DOOR_ACCESS_ALARM) )
			{
				foreach(new p : Player)
				{
					if( IsPlayerInAnyGroup(p) )
					{
						new gidd = pInfo[p][player_duty_gid];
						if(gidd != -1)
						{
							if( Group[gidd][group_type] == GROUP_TYPE_LSPD)
							{
								SendClientMessage(p, COLOR_GOLD, "Pilne wezwanie: podpalono biznes (pozycja pokazana na mapie).");
								Streamer_AppendArrayData(STREAMER_TYPE_MAP_ICON, Fire[fire_map_icon], E_STREAMER_PLAYER_ID, p);
							}
						}
					}
				}
			}

			if( DoorHasAccess(d_id, DOOR_ACCESS_CAMERA) )
			{
				new suspects_list[256];
				foreach(new p: Player)
				{
					if(GetPlayerVirtualWorld(playerid) == Door[d_id][door_uid])
					{
						format(suspects_list, sizeof(suspects_list), "%s %s", suspects_list, pInfo[p][player_name]);
					}
				}

				if(strlen(suspects_list))
				{
					mysql_query(mySQLconnection, sprintf("DELETE FROM ipb_game_cameras WHERE camera_door = %d", Door[d_id][door_uid]));
					mysql_query(mySQLconnection, sprintf("INSERT INTO `ipb_game_cameras` (camera_door, camera_suspects) VALUES ('%d', '%s')", Door[d_id][door_uid], suspects_list));
				}
			}
			
			DeleteItem(itemid, true, playerid);
		}

		case ITEM_TYPE_DEMOLITION:
		{
			if(!SherifsOnline()) return SendGuiInformation(playerid, "Informacja", "Aby u�y� tego przedmiotu na s�u�bie musz� by� cz�onkowie Sheriff's Department.");

			new gid = pInfo[playerid][player_duty_gid];
			if(gid == -1) return SendGuiInformation(playerid, "Informacja", "Nie znajdujesz si� na s�u�bie grupy dla kt�rej przeznaczony jest ten przedmiot.");
			if(Group[gid][group_uid] != PlayerItem[playerid][itemid][player_item_value2]) return SendGuiInformation(playerid, "Informacja", "Nie znajdujesz si� na s�u�bie grupy dla kt�rej przeznaczony jest ten przedmiot.");

			new d_uid = GetPlayerVirtualWorld(playerid);
			if(d_uid == 0 ) return SendGuiInformation(playerid, "Informacja", "Tego przedmiotu mo�esz u�y� tylko w drzwiach do niego przypisanych.");
			if(d_uid != PlayerItem[playerid][itemid][player_item_value1]) return SendGuiInformation(playerid, "Informacja", "Ten przedmiot nie jest przeznaczony do u�ycia w tych drzwiach.");

			new d_id = GetDoorByUid(d_uid);
			if(d_id == -1) return SendGuiInformation(playerid, "Informacja", "Nie znaleziono drzwi o UID przypisanym do przedmiotu.");

			pInfo[playerid][player_demolition] = d_uid;
			TextDrawSetString(Tutorial[playerid], sprintf("Niszczenie ~p~wnetrza~w~ w toku.~n~Aktualny stan zniszczen: ~r~~h~~h~%d%%~n~~w~Kazda minuta to 10 procent zniszczen.", Door[d_id][door_demolition]));
			TextDrawShowForPlayer(playerid, Tutorial[playerid]);

			if( DoorHasAccess(d_id, DOOR_ACCESS_ALARM) )
			{
				foreach(new p : Player)
				{
					if( IsPlayerInAnyGroup(p) )
					{
						new gidd = pInfo[p][player_duty_gid];
						if(gidd != -1)
						{
							if( Group[gidd][group_type] == GROUP_TYPE_LSPD)
							{
								SendClientMessage(p, COLOR_GOLD, "Pilne wezwanie: demolka lokalu (pozycja pokazana na mapie).");
								Streamer_AppendArrayData(STREAMER_TYPE_MAP_ICON, Fire[fire_map_icon], E_STREAMER_PLAYER_ID, p);
							}
						}
					}
				}
			}

			if( DoorHasAccess(d_id, DOOR_ACCESS_CAMERA) )
			{
				new suspects_list[256];
				foreach(new p: Player)
				{
					if(GetPlayerVirtualWorld(playerid) == Door[d_id][door_uid])
					{
						format(suspects_list, sizeof(suspects_list), "%s %s", suspects_list, pInfo[p][player_name]);
					}
				}

				if(strlen(suspects_list))
				{
					mysql_query(mySQLconnection, sprintf("DELETE FROM ipb_game_cameras WHERE camera_door = %d", Door[d_id][door_uid]));
					mysql_query(mySQLconnection, sprintf("INSERT INTO `ipb_game_cameras` (camera_door, camera_suspects) VALUES ('%d', '%s')", Door[d_id][door_uid], suspects_list));
				}
			}

			DeleteItem(itemid, true, playerid);
		}

		case ITEM_TYPE_MASKA:
		{
			if( PlayerItem[playerid][itemid][player_item_used] )
			{
				if(pInfo[playerid][player_admin_duty] == true) return SendGuiInformation(playerid, "Informacja", "Nie mo�esz u�y� maski b�d�c na duty admina.");
				strcopy(pInfo[playerid][player_name], pInfo[playerid][player_duty_tmp], MAX_PLAYER_NAME+1);
				pInfo[playerid][player_duty_tmp][0] = EOS;
				PlayerItem[playerid][itemid][player_item_used]=false;

				UpdatePlayerLabel(playerid);
				
				SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Maska zosta�a zdj�ta.\nTw�j nick wraca do normalnego stanu.");
			}
			else
			{
				if(pInfo[playerid][player_admin_duty] == true) return SendGuiInformation(playerid, "Informacja", "Nie mo�esz u�y� maski b�d�c na duty admina.");
				if(PlayerItem[playerid][itemid][player_item_value1] > 0)
				{
					new gid = pInfo[playerid][player_duty_gid];
					if(gid == -1) return SendGuiInformation(playerid, "Informacja", "Nie znajdujesz si� na s�u�bie gruby z flag� do u�ywania masek.");
					if( !GroupHasFlag(gid, GROUP_FLAG_MASK)) return SendGuiInformation(playerid, "Informacja", "Nie znajdujesz si� na s�u�bie gruby z flag� do u�ywania masek.");

					new name[MAX_PLAYER_NAME+1];
			   		GetPlayerName(playerid, name, sizeof(name));

					strcopy(pInfo[playerid][player_duty_tmp], name, MAX_PLAYER_NAME+1);

					new nazwa[MAX_PLAYER_NAME+1];
					format(nazwa, sizeof(nazwa), "Nieznajomy_%d", pInfo[playerid][player_id]);
					PlayerItem[playerid][itemid][player_item_used]=true;

					strcopy(pInfo[playerid][player_name], nazwa, MAX_PLAYER_NAME+1);

					UpdatePlayerLabel(playerid);
					
					PlayerItem[playerid][itemid][player_item_value1] --;
					mysql_query(mySQLconnection, sprintf("UPDATE `ipb_items` SET `item_value1` = %d WHERE `item_uid` = %d", PlayerItem[playerid][itemid][player_item_value1], PlayerItem[playerid][itemid][player_item_uid]));
					SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Maska zosta�a za�o�ona.");
				}
				else
				{
					SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Przedmiot, kt�ry pr�bujesz aktualnie u�y� nie posiada ju� u�y�.\nZostanie on teraz usuni�ty z Twojego ekwipunku.");
					DeleteItem(itemid, true, playerid);
				}
			}
		}

		case ITEM_TYPE_EMPTY_PLATE:
		{
			if( !PlayerItem[playerid][itemid][player_item_used] )
			{
				ShowPlayerDialog(playerid, DIALOG_CD_LINK, DIALOG_STYLE_INPUT, "Tworzenie p�yty CD", "Podaj link do audio streama, max 128 znak�w.", "Stw�rz", "Anuluj");
				DynamicGui_Init(playerid);
				DynamicGui_SetDialogValue(playerid, itemid);
			}
		}

		case ITEM_TYPE_PLATE:
        {
            if( !PlayerItem[playerid][itemid][player_item_used] )
            {
                if(GetPlayerState(playerid) == PLAYER_STATE_PASSENGER || GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
                {
                    new vehid = GetPlayerVehicleID(playerid), rows, fields;
 					
 					if( !CanPlayerUseVehicle(playerid, vehid) ) return SendGuiInformation(playerid, "Informacja", "Brak uprawnie� do zarz�dzania tym pojazdem.");
 					if( !VehicleHasAccessory(vehid, VEHICLE_ACCESSORY_AUDIO)) return SendGuiInformation(playerid, "Informacja", "Ten pojazd nie posiada radia.\nMo�esz je kupi� w ka�dym 24/7.");

                    mysql_query(mySQLconnection, sprintf("SELECT cd_link FROM `ipb_cds` WHERE `cd_item` = %d", PlayerItem[playerid][itemid][player_item_uid]));
                   	cache_get_data(rows, fields);
 
                    new cdlink[128];
                    cache_get_row(0, 0, cdlink, mySQLconnection, 128);
 
                    Vehicle[vehid][vehicle_streaming] = 1;
                    strmid(Vehicle[vehid][vehicle_stream], cdlink, 0, strlen(cdlink), 128);
 
                    foreach(new p: Player)
                    {
                        if(GetPlayerVehicleID(p) == vehid)
                        {
                            PlayAudioStreamForPlayer(p, cdlink);
                        }
                    }

                    ProxMessage(playerid, "wk�ada p�yte do odtwarzacza w samochodzie.", PROX_ME);
                    PlayerItem[playerid][itemid][player_item_used]=true;
                }
                else
                {
                    if(GetPlayerVirtualWorld(playerid) == 0)
                    {
                    	SendGuiInformation(playerid, "Informacja", "Nie znajdujesz si� w �adnych drzwiach lub poje�dzie.");
                    	return 1;
                    }
                    else
                    {
                    	new did = GetDoorByUid(GetPlayerVirtualWorld(playerid)), rows, fields;
                    	if(did == -1) return SendGuiInformation(playerid, "Informacja", "Nie znajdujesz si� w �adnych drzwiach lub poje�dzie.");
                    	if( !CanPlayerEditDoor(playerid, did) ) return SendGuiInformation(playerid, "Informacja", "Nie posiadasz uprawnie� do edycji tych drzwi.");

                    	mysql_query(mySQLconnection, sprintf("SELECT cd_link FROM `ipb_cds` WHERE `cd_item` = %d", PlayerItem[playerid][itemid][player_item_uid]));
	                   	cache_get_data(rows, fields);
	 
	                    new cdlink[128];
	                    cache_get_row(0, 0, cdlink, mySQLconnection, 128);
	 
	                    strmid(Door[did][door_audio], cdlink, 0, strlen(cdlink), 128);
	 
	                   	foreach(new p : Player)
						{
							if( GetPlayerVirtualWorld(p) == Door[did][door_spawn_vw] )
							{
								if( !isnull(Door[did][door_audio]) ) PlayAudioStreamForPlayer(p, Door[did][door_audio], 0);
								else StopAudioStreamForPlayer(p);
							}
						}

	                    cmd_ame(playerid, "uruchamia muzyk� w budynku.");
	                    PlayerItem[playerid][itemid][player_item_used]=true;
                    }
                }
            }
            else
            {
                if(GetPlayerState(playerid) == PLAYER_STATE_PASSENGER || GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
                {
                    new vehid = GetPlayerVehicleID(playerid);
                    Vehicle[vehid][vehicle_streaming] = 0;
                    StopAudioStreamForPlayer(playerid);
 
                    foreach(new p: Player)
                    {
                        if(GetPlayerVehicleID(p) == vehid)
                        {
                            StopAudioStreamForPlayer(p);
                        }
                    }
 
                    ProxMessage(playerid, "wyjmuje p�yt� z odtwarzacza w samochodzie.", PROX_ME);
                    PlayerItem[playerid][itemid][player_item_used]=false;
                }
                else
                {
                    if(GetPlayerVirtualWorld(playerid) == 0)
                    {
                    	SendGuiInformation(playerid, "Informacja", "Nie znajdujesz si� w �adnych drzwiach lub poje�dzie.");
                    	return 1;
                    }

                    new did = GetDoorByUid(GetPlayerVirtualWorld(playerid));
                	if(did == -1) return SendGuiInformation(playerid, "Informacja", "Nie znajdujesz si� w �adnych drzwiach lub poje�dzie.");
                	if( !CanPlayerEditDoor(playerid, did) ) return SendGuiInformation(playerid, "Informacja", "Nie posiadasz uprawnie� do edycji tych drzwi.");
                	strmid(Door[did][door_audio], "-", 0, 2, 128);
                	cmd_ame(playerid, "wy��czy� muzyk� w budynku.");

                	foreach(new p : Player)
					{
						if( GetPlayerVirtualWorld(p) == Door[did][door_spawn_vw] )
						{
							StopAudioStreamForPlayer(p);
						}
					}

                	PlayerItem[playerid][itemid][player_item_used]=false;
                }
            }
        }

		case ITEM_TYPE_ROPE:
        {
            if( PlayerItem[playerid][itemid][player_item_used] )
            {
                GameTextForPlayer(playerid, "~w~lina ~y~uzyta", 3000, 3);
                PlayerItem[playerid][itemid][player_item_used] = false;
            }
            else
            {
                GameTextForPlayer(playerid, "~w~lina ~y~zdjeta", 3000, 3);
                PlayerItem[playerid][itemid][player_item_used] = true;
            }
        }
		
		case ITEM_TYPE_GLOVES:
		{
			if( PlayerItem[playerid][itemid][player_item_used] )
			{
				ProxMessage(playerid, "zdejmuje r�kawiczki.", PROX_ME);
				RemovePlayerStatus(playerid, PLAYER_STATUS_REKAWICZKI);
				PlayerItem[playerid][itemid][player_item_used]=false;
			}
			else
			{
				ProxMessage(playerid, "zak�ada r�kawiczki.", PROX_ME);
				AddPlayerStatus(playerid, PLAYER_STATUS_REKAWICZKI);
				PlayerItem[playerid][itemid][player_item_used]=true;
			}
		}

		case ITEM_TYPE_TUNING:
		{
			if( !PlayerItem[playerid][itemid][player_item_used] )
			{
				if( GetPlayerUsedItem(playerid, ITEM_TYPE_TUNING) > -1)
				{
					SendGuiInformation(playerid, "Informacja", "U�ywasz ju� jakiego� przedmiotu tuningu.");
					return 1;
				}

				SendGuiInformation(playerid, ""guiopis"Powiadomienie", "U�y�e� komponentu, mo�esz go teraz zamonotowa� przez /o montaz.");
				PlayerItem[playerid][itemid][player_item_used]=true;
			}
			else
			{
				PlayerItem[playerid][itemid][player_item_used]=false;
			}
		}

		case ITEM_TYPE_ATTACH:
		{
			if( PlayerItem[playerid][itemid][player_item_used] )
			{
				PlayerItem[playerid][itemid][player_item_used]=false;
				RemovePlayerAttachedObject(playerid, PlayerItem[playerid][itemid][player_item_value2]);
			}
			else
			{
				new slot = GetPlayerFreeAttachSlot(playerid);
				if(slot == -1) return SendGuiInformation(playerid, "Informacja", "Brak wolnych slot�w dla doczepialnych obiekt�w.");

				new Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz, Float:sx, Float:sy, Float:sz, rows, fields;
				mysql_query(mySQLconnection, sprintf("SELECT * FROM ipb_attached_objects WHERE attach_owner = %d AND attach_model = %d LIMIT 1", pInfo[playerid][player_id], PlayerItem[playerid][itemid][player_item_model]));
				cache_get_data(rows, fields);

				if( rows )
				{
					x  = cache_get_row_float(0, 5);
					y  = cache_get_row_float(0, 6);
					z  = cache_get_row_float(0, 7);
					rx = cache_get_row_float(0, 8);
					ry = cache_get_row_float(0, 9);
					rz = cache_get_row_float(0, 10);
					sx = cache_get_row_float(0, 11);
					sy = cache_get_row_float(0, 12);
					sz = cache_get_row_float(0, 13);

					RemovePlayerAttachedObject(playerid, slot);
					SetPlayerAttachedObject(playerid, slot, PlayerItem[playerid][itemid][player_item_model], PlayerItem[playerid][itemid][player_item_value1], x, y, z, rx, ry, rz, sx, sy, sz);
					SendPlayerInformation(playerid, "~w~Znaleziono twoje ulozenie tego obiektu.~n~Jesli nie chcesz go zmieniac po prostu wcisnij ~y~ESC~w~.", 6000);
					PlayerItem[playerid][itemid][player_item_used]=true;
					PlayerItem[playerid][itemid][player_item_value2]=slot;
					ao[playerid][slot][ao_inserted]=true;
					EditAttachedObject(playerid, slot);
				}
				else
				{
					RemovePlayerAttachedObject(playerid, slot);
					SetPlayerAttachedObject(playerid, slot, PlayerItem[playerid][itemid][player_item_model], PlayerItem[playerid][itemid][player_item_value1]);
					SendPlayerInformation(playerid, "~w~Nie znaleziono w bazie ulozenia tego obiektu. Zrob to teraz i wcisnij ~y~ikone zapisu~w~.", 6000);
					PlayerItem[playerid][itemid][player_item_used]=true;
					PlayerItem[playerid][itemid][player_item_value2]=slot;
					ao[playerid][slot][ao_inserted]=false;
					EditAttachedObject(playerid, slot);
				}
			}
		}

		case ITEM_TYPE_BAG:
		{
	    	mysql_query(mySQLconnection, sprintf("SELECT item_uid, item_name FROM ipb_items WHERE item_owner = %d AND item_ownertype = %d", PlayerItem[playerid][itemid][player_item_uid], ITEM_OWNER_TYPE_ITEM));

	    	new rows, fields, header[32], bag_item_uid, bag_item_name[40], list_items[256];

	    	cache_get_data(rows, fields);

	    	for(new row = 0; row != rows; row++)
			{
			    bag_item_uid = cache_get_row_int(row, 0);
   				cache_get_row(row, 1, bag_item_name, mySQLconnection, 40);

				format(list_items, sizeof(list_items), "%s\n%d\t%s", list_items, bag_item_uid, bag_item_name);
			}

			format(header, sizeof(header), "UID\tNazwa\n");

			if(strlen(list_items) > 0)
			{
				format(list_items, sizeof(list_items), "%s%s", header, list_items);
				ShowPlayerDialog(playerid, DIALOG_TAKE_BAG, DIALOG_STYLE_TABLIST_HEADERS, "Przedmioty w torbie:", list_items, "Wyjmij", "Anuluj");
				DynamicGui_SetDialogValue(playerid, itemid);
	      	}
	       	else
	       	{
	        	SendGuiInformation(playerid, ""guiopis"Informacja", "W tej torbie nie ma �adnego przedmiotu.");
			}
		}
		
		case ITEM_TYPE_AMMO:
		{
			if( !PlayerItem[playerid][itemid][player_item_used] )
			{
				if( PlayerItem[playerid][itemid][player_item_extraid] == 1)
				{
					new gid = pInfo[playerid][player_duty_gid];
					if(gid == -1) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Nie posiadasz uprawnie� do u�ycia tego przedmiotu, lub nie jeste� na s�u�bie grupy z flag� upowa�niaj�c� do tego.");
					if(!(Group[gid][group_flags] & GROUP_FLAG_WEAPON_FLAG)) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Nie posiadasz uprawnie� do u�ycia tego przedmiotu, lub nie jeste� na s�u�bie grupy z flag� upowa�niaj�c� do tego.");
				}

				if(PlayerItem[playerid][itemid][player_item_extraid] == 3)
				{
					if(!(pInfo[playerid][player_documents] & DOCUMENT_WEAPON)) return SendGuiInformation(playerid, "Informacja", "Nie posiadasz licencji na bro�.");
				}
				
				DynamicGui_Init(playerid);
				DynamicGui_SetDialogValue(playerid, itemid);
				new count, string[200];
				foreach(new item : PlayerItems[playerid])
				{
					if( PlayerItem[playerid][item][player_item_type] == ITEM_TYPE_WEAPON && GetWeaponType(PlayerItem[playerid][item][player_item_value1]) == PlayerItem[playerid][itemid][player_item_value1] && !PlayerItem[playerid][item][player_item_used] )
					{
						format(string, sizeof(string), "%s%d\t\t%s\n", string, PlayerItem[playerid][item][player_item_uid], PlayerItem[playerid][item][player_item_name]);
						DynamicGui_AddRow(playerid, item);
						count++;
					}
				}
				if( count == 0 ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Nie posiadasz �adnych nieu�ywanych broni do kt�rych pasuje ta amunicja.");
				else ShowPlayerDialog(playerid, DIALOG_USE_AMMO, DIALOG_STYLE_LIST, "Wybierz bro� do kt�rej chcesz za�adowa� amunicj�:", string, "Wybierz", "Zamknij");
			}
		}
		
		case ITEM_TYPE_PHONE:
		{
			if( PlayerItem[playerid][itemid][player_item_used] )
			{
				cmd_tel(playerid, "");
			}
			else
			{
				if( GetPlayerUsedItem(playerid, ITEM_TYPE_PHONE) > -1 ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Masz ju� w��czony inny telefon.");
			
				if( PlayerItem[playerid][itemid][player_item_value1] == 0 )
				{
					new str[7];
					format(str, sizeof(str), "%04d", pInfo[playerid][player_id]);
					
					new length = 6 - strlen(str);
					
					if( length == 2 ) format(str, sizeof(str), "%02d%s", rand(10, 99), str);
					else if( length == 1 ) format(str, sizeof(str), "%d%s", rand(1, 9), str);
					
					PlayerItem[playerid][itemid][player_item_value1] = strval(str);
					
					mysql_query(mySQLconnection, sprintf("UPDATE `ipb_items` SET `item_value1` = %d WHERE `item_uid` = %d", PlayerItem[playerid][itemid][player_item_value1], PlayerItem[playerid][itemid][player_item_uid]));
				}
				
				PlayerItem[playerid][itemid][player_item_used] = true;
				
				mysql_query(mySQLconnection, sprintf("UPDATE `ipb_items` SET `item_used` = 1 WHERE `item_uid` = %d", PlayerItem[playerid][itemid][player_item_uid]));
				
				GameTextForPlayer(playerid, "~w~Telefon ~g~wlaczony", 3000, 3);

				pInfo[playerid][player_phone_model] = PlayerItem[playerid][itemid][player_item_model];
			}
		}
		case ITEM_TYPE_WATCH:
		{
			new hour, minute, second,string[64];
			gettime(hour, minute, second);

			format(string, sizeof(string), "~w~Godzina:~w~ ~p~%02d:%02d:%02d~p~~w~.", hour, minute, second);
			GameTextForPlayer(playerid, string, 5000, 1);

			ProxMessage(playerid, sprintf("spogl�da na %s.", PlayerItem[playerid][itemid][player_item_name]), PROX_SERWERME);

			ApplyAnimation(playerid,"COP_AMBIENT","Coplook_watch",4.1,false,false,false,false,0,1);
		}

		case ITEM_TYPE_HOWITZER:
		{
			if(GetPlayerWeapon(playerid)==0)
			{
				SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Musisz mie� w r�kach M4 do kt�rego chcesz zaczepi� granatnik.");
			}
			SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Za�adowa�e� do swojego karabinu granatnik.");
			PlayerItem[playerid][itemid][player_item_used] = true;
			pInfo[playerid][player_howitzer]=PlayerItem[playerid][itemid][player_item_value1];
			DeleteItem(itemid, true, playerid);
		}

		case ITEM_TYPE_DRINK:
		{
			//value1 = special_action (20 - piwo, 22 - wino, 23 - sprunk)
			SetPlayerSpecialAction(playerid, PlayerItem[playerid][itemid][player_item_value1]);
			DeleteItem(itemid, true, playerid);
		}

		case ITEM_TYPE_SUPLEMENT:
		{
			pInfo[playerid][player_supple] = PlayerItem[playerid][itemid][player_item_value1];
			SendClientMessage(playerid, COLOR_GOLD, sprintf("U�yto od�ywki, od teraz twoja si�a b�dzie ros�a %d razy szybciej z ka�dym wyci�ni�ciem.", pInfo[playerid][player_supple]));
			DeleteItem(itemid, true, playerid);
		}

		case ITEM_TYPE_CLOTH:
		{
			if(!PlayerItem[playerid][itemid][player_item_used])
			{
				ProxMessage(playerid, "przebiera si�.", PROX_ME);
				SetPlayerSkin(playerid, PlayerItem[playerid][itemid][player_item_value1]);
				pInfo[playerid][player_skin] = PlayerItem[playerid][itemid][player_item_value1];
				mysql_query(mySQLconnection, sprintf("UPDATE ipb_characters SET char_skin = '%d' WHERE char_uid = '%d'", pInfo[playerid][player_skin], pInfo[playerid][player_id]));
				PlayerItem[playerid][itemid][player_item_used] = true;
			}
			else
			{
				PlayerItem[playerid][itemid][player_item_used] = false;
			}
		}

		case ITEM_TYPE_DILDO_CHAINSAW:
		{
			new a_id = pInfo[playerid][player_area];
			if(a_id < 1 || !AreaHasFlag(a_id, AREA_FLAG_WORK)) return SendGuiInformation(playerid, "Informacja", "Nie mo�esz u�y� tutaj tego przedmiotu.");

			if(pInfo[playerid][player_job] == 0) return SendGuiInformation(playerid, "Informacja", "Nie jeste� zatrudniony w �adnej pracy dorywczej.");

			if(pInfo[playerid][player_job] == WORK_TYPE_LUMBERJACK)
			{
				if(!PlayerItem[playerid][itemid][player_item_used])
				{
					PlayerItem[playerid][itemid][player_item_used] = true;
					SetPlayerAttachedObject(playerid, 8, 341, 6,0.068999, 0.068000, 0.119000, -2.799999, -23.600008, -4.200000, 1.000000, 1.000000, 1.000000);
				}
				else
				{
					PlayerItem[playerid][itemid][player_item_used] = false;
					RemovePlayerAttachedObject(playerid, 8);
				}
			}
		}

		case ITEM_TYPE_LINE:
		{
			if(!PlayerItem[playerid][itemid][player_item_used])
			{
				PlayerItem[playerid][itemid][player_item_used] = true;
			}
			else
			{
				PlayerItem[playerid][itemid][player_item_used] = false;
			}
		}

		case ITEM_TYPE_CUBE:
		{
			new los = random(PlayerItem[playerid][itemid][player_item_value1]);

			ProxMessage(playerid, sprintf("wyrzuci� %d oczek na %d.", los+1, PlayerItem[playerid][itemid][player_item_value1]), PROX_SERWERME);
		}

		case ITEM_TYPE_CANISTER:
		{
			if(PlayerItem[playerid][itemid][player_item_value1] == 0) return SendGuiInformation(playerid, "Informacja", "Ten kanister jest pusty.");
			new vid = GetNearestVehicle(playerid);
			if (vid == -1) return SendGuiInformation(playerid, "Informacja", "Znajdujesz si� zbyt daleko od jakiegokolwiek pojazdu.");

			if(Vehicle[vid][vehicle_fuel_type] != PlayerItem[playerid][itemid][player_item_value2])
			{
				SendGuiInformation(playerid, "Informacja", "Typ paliwa w kanistrze r�ni si� od przyjmowanego przez silnik pojazdu.");
				return 1;
			}

			new amount = PlayerItem[playerid][itemid][player_item_value1];
			new actual = floatround(Vehicle[vid][vehicle_fuel_current], floatround_ceil) + amount;
			new maximum = VehicleFuelMax[GetVehicleModel(vid)-400];

			if(actual > maximum)
			{
				SendGuiInformation(playerid, "Informacja", "Bak tego pojazdu nie pomie�ci tyle paliwa.");
				return 1;
			}

			Vehicle[vid][vehicle_fuel_current] += float(amount);
			PlayerItem[playerid][itemid][player_item_value1] = 0;
			mysql_query(mySQLconnection, sprintf("UPDATE ipb_items SET item_value1 = '%d' WHERE item_uid = %d", PlayerItem[playerid][itemid][player_item_value1], PlayerItem[playerid][itemid][player_item_uid]));
			ApplyAnimation(playerid, "INT_HOUSE", "wash_up",4.1, false, false, false, false, 0, 1);
			SendGuiInformation(playerid, "Informacja", "Pojazd zosta� pomy�lnie zatankowany.");
			cmd_ame(playerid, "wlewa zawarto�� kanistra do baku.");
			SaveVehicle(vid);
		}

		case ITEM_TYPE_FOOD:
		{
			if(pInfo[playerid][player_health]<21)
			{
				SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Maj�c poni�ej 21 procent zdrowia musisz uda� sie do apteki lub szpitala w celu regeneracji poziomu HP.");
				return 1;
			}
			if(pInfo[playerid][player_health]>=100)
			{
				SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Masz 100hp, nie mozesz nic wi�cej zje��.");
				return 1;
			}

			new str[32];
			pInfo[playerid][player_health]+=PlayerItem[playerid][itemid][player_item_value1];

			format(str, sizeof(str), "spo�ywa %s", PlayerItem[playerid][itemid][player_item_name]);
			DeleteItem(itemid, true, playerid);
			ProxMessage(playerid, str, PROX_ME);
			if(pInfo[playerid][player_health]>100)
			{
				pInfo[playerid][player_health]=100;
				SetPlayerHealth(playerid, 100);
			}
		}

		case ITEM_TYPE_GRENADE:
		{
			new d_id = GetPlayerDoorEntrance(playerid);
			if(d_id == -1 ) return SendClientMessage(playerid, COLOR_GREY, "Nie stoisz przy �adnych drzwiach.");

			switch(PlayerItem[playerid][itemid][player_item_value1])
			{
				case GRENADE_TYPE_FLASH: pInfo[playerid][player_dialog_tmp1] = GRENADE_TYPE_FLASH;
				case GRENADE_TYPE_SMOKE: pInfo[playerid][player_dialog_tmp1] = GRENADE_TYPE_SMOKE;
				case GRENADE_TYPE_BANG: pInfo[playerid][player_dialog_tmp1] = GRENADE_TYPE_BANG;
			}

			pInfo[playerid][player_dialog_tmp2] = d_id;
			pInfo[playerid][player_dialog_tmp4] = itemid;
			
			ShowPlayerDialog(playerid, DIALOG_GRENADE, DIALOG_STYLE_LIST, "Rzut granatem", "Daleki rzut - efekt otrzymaj� osoby w promieniu 2-15m od drzwi\nKr�tki rzut - efekt otrzymaj� osoby w promieniu 7m od drzwi", "OK", "Anuluj");
		}

		case ITEM_TYPE_CIGGY:
		{
			if(PlayerItem[playerid][itemid][player_item_value1]>0)
			{
				SetPlayerSpecialAction(playerid, SPECIAL_ACTION_SMOKE_CIGGY);
				PlayerItem[playerid][itemid][player_item_value1]--;
			}
			else
			{
				DeleteItem(itemid, true, playerid);
			}
		}

		case ITEM_TYPE_LEK:
		{
			new str[32];
			if(PlayerItem[playerid][itemid][player_item_value1] == 1)
			{
				if(pInfo[playerid][player_health]<=29)
				{
					pInfo[playerid][player_health]+=30;
					SetPlayerHealth(playerid, floatround(pInfo[playerid][player_health]));
					format(str, sizeof(str), "spo�ywa %s", PlayerItem[playerid][itemid][player_item_name]);
					ProxMessage(playerid, str, PROX_ME);
					DeleteItem(itemid, true, playerid);
				}
				else
				{
					SendPlayerInformation(playerid, "~w~Aby uzupelnic reszte punktow ~g~~h~zdrowia~w~ zjedz cos.", 5000);
				}
			}
			else if(PlayerItem[playerid][itemid][player_item_value1] == 2)
			{
				if(pInfo[playerid][player_health]<=29)
				{
					pInfo[playerid][player_health]+=30;
					SetPlayerHealth(playerid, floatround(pInfo[playerid][player_health]));
					format(str, sizeof(str), "spo�ywa %s", PlayerItem[playerid][itemid][player_item_name]);
					ProxMessage(playerid, str, PROX_ME);
					DeleteItem(itemid, true, playerid);
				}
				else
				{
					SendPlayerInformation(playerid, "~w~Aby uzupelnic reszte punktow ~g~~h~zdrowia~w~ zjedz cos.", 5000);
				}
			}
			else if(PlayerItem[playerid][itemid][player_item_value1] == 3)
			{
				SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Na chwile obecn� przedmiot nie ma �adnego zastosowania.");
			}
		}
		
		case ITEM_TYPE_DRUG:
		{
			if(pInfo[playerid][player_dealing]) return SendGuiInformation(playerid, "Informacja", "Poczekaj na zako�czenie handlu.");

			if( GetPlayerUsedItem(playerid, ITEM_TYPE_GLOVES) == -1 )
			{
				mysql_query(mySQLconnection, sprintf("INSERT INTO `ipb_fingerprints` (finger_owner, finger_person) VALUES ('%d', '%d')", PlayerItem[playerid][itemid][player_item_uid], pInfo[playerid][player_id]));
			}

			new header[50];
			format(header, sizeof(header), "%s [%d]", PlayerItem[playerid][itemid][player_item_name], PlayerItem[playerid][itemid][player_item_uid]);

			switch(PlayerItem[playerid][itemid][player_item_value1])
			{
				case DRUG_TYPE_COCAINE:
				{
					ShowPlayerDialog(playerid, DIALOG_USE_DRUG, DIALOG_STYLE_LIST, header, "1. Za�yj\n2. Handluj w strefie\n3. Podziel na porcje\n4. Gotuj na crack", "OK", "Anuluj");
				}
				default:
				{
					ShowPlayerDialog(playerid, DIALOG_USE_DRUG, DIALOG_STYLE_LIST, header, "1. Za�yj\n2. Handluj w strefie\n3. Podziel na porcje", "OK", "Anuluj");
				}
			}

			DynamicGui_Init(playerid);
			DynamicGui_SetDialogValue(playerid, itemid);
		}

		case ITEM_TYPE_CORPSE:
		{
			new death_name[64];
		    if(PlayerItem[playerid][itemid][player_item_value2] == 0)
		    {
		    	format(death_name, sizeof(death_name), "pobicie");
		    }
		    if(PlayerItem[playerid][itemid][player_item_value2] > 0 && PlayerItem[playerid][itemid][player_item_value2] <=3)
		    {
		    	format(death_name, sizeof(death_name), "pobicie przy u�yciu broni bia�ej");
		    }
		 	if(PlayerItem[playerid][itemid][player_item_value2] == 4)
		    {
		    	format(death_name, sizeof(death_name), "zad�gany (kr�tkie ostrze)");
		    }
		    if(PlayerItem[playerid][itemid][player_item_value2] > 4 && PlayerItem[playerid][itemid][player_item_value2] <8 )
		    {
		    	format(death_name, sizeof(death_name), "pobicie z u�yciu broni bia�ej");
		    }
		    if(PlayerItem[playerid][itemid][player_item_value2] == 8)
		    {
		    	format(death_name, sizeof(death_name), "zad�gany (d�ugie ostrze)");
		    }
		    if(PlayerItem[playerid][itemid][player_item_value2] == 9)
		    {
		    	format(death_name, sizeof(death_name), "zmasakrowanie (cia�o w kawa�kach, �lady po ci�ciach)");
		    }
		    if(PlayerItem[playerid][itemid][player_item_value2] > 9 && PlayerItem[playerid][itemid][player_item_value2] < 16)
		    {
		    	format(death_name, sizeof(death_name), "pobicie przy u�yciu broni bia�ej");
		    }
		    if(PlayerItem[playerid][itemid][player_item_value2] == 16)
		    {
		    	format(death_name, sizeof(death_name), "eksplozja");
		    }
		    if(PlayerItem[playerid][itemid][player_item_value2] == 18)
		    {
		    	format(death_name, sizeof(death_name), "sp�oni�cie");
		    }
		    if(PlayerItem[playerid][itemid][player_item_value2] > 21 && PlayerItem[playerid][itemid][player_item_value2] < 24)
		    {
		    	format(death_name, sizeof(death_name), "postrza� (bro� kr�tka)");
		    }
		    if(PlayerItem[playerid][itemid][player_item_value2] == 24)
		    {
		    	format(death_name, sizeof(death_name), "postrza� (bro� kr�tka o du�ym kalibrze)");
		    }
		    if(PlayerItem[playerid][itemid][player_item_value2] > 24 && PlayerItem[playerid][itemid][player_item_value2] < 28)
		    {
		    	format(death_name, sizeof(death_name), "postrza� (strzelba)");
		    }
		    if(PlayerItem[playerid][itemid][player_item_value2] > 27 && PlayerItem[playerid][itemid][player_item_value2] < 30)
		    {
		    	format(death_name, sizeof(death_name), "postrza� (bro� automatyczna)");
		    }
		    if(PlayerItem[playerid][itemid][player_item_value2] > 29 && PlayerItem[playerid][itemid][player_item_value2] < 32)
		    {
		    	format(death_name, sizeof(death_name), "postrza� (karabin)");
		    }
		    if(PlayerItem[playerid][itemid][player_item_value2] == 32)
		    {
		    	format(death_name, sizeof(death_name), "postrza� (bro� automatyczna)");
		    }
			if(PlayerItem[playerid][itemid][player_item_value2] == 33)
		    {
		    	format(death_name, sizeof(death_name), "postrza� (strzelba my�liwska)");
		    }
		    if(PlayerItem[playerid][itemid][player_item_value2] == 34)
		    {
		    	format(death_name, sizeof(death_name), "postrza� (karabin snajperski)");
		    }
		    if(PlayerItem[playerid][itemid][player_item_value2] == 35)
		    {
		    	format(death_name, sizeof(death_name), "eksplozja");
		    }
		    if(PlayerItem[playerid][itemid][player_item_value2] == 36)
		    {
		    	format(death_name, sizeof(death_name), "eksplozja");
		    }
		    if(PlayerItem[playerid][itemid][player_item_value2] == 37)
		    {
		    	format(death_name, sizeof(death_name), "sp�oni�cie");
		    }
		 	if(PlayerItem[playerid][itemid][player_item_value2] == 38)
		    {
		    	format(death_name, sizeof(death_name), "postrza� (minigun)");
		    }
		    if(PlayerItem[playerid][itemid][player_item_value2] == 39)
		    {
		    	format(death_name, sizeof(death_name), "eksplozja");
		    }
		    if(PlayerItem[playerid][itemid][player_item_value2] == 41)
		    {
		    	format(death_name, sizeof(death_name), "zatrucie dr�g oddechowych");
		    }
		    if(PlayerItem[playerid][itemid][player_item_value2] == 42)
		    {
		    	format(death_name, sizeof(death_name), "zatrucie ga�nic�");
		    }
		    if(PlayerItem[playerid][itemid][player_item_value2] == 49)
		    {
		    	format(death_name, sizeof(death_name), "wypadek drogowy");
		    }
		    if(PlayerItem[playerid][itemid][player_item_value2] == 51)
		    {
		    	format(death_name, sizeof(death_name), "eksplozja");
		    }
		    if(PlayerItem[playerid][itemid][player_item_value2] == 53)
		    {
		    	format(death_name, sizeof(death_name), "utopienie si�");
		    }
		    if(PlayerItem[playerid][itemid][player_item_value2] == 54)
		    {
		    	format(death_name, sizeof(death_name), "upadek z du�ej wysoko�ci");
		    }

		    new deathtime = PlayerItem[playerid][itemid][player_item_created];
			new deathHour, deathMinute, deathDay, temp;

			TimestampToDate(deathtime, temp, temp, deathDay, deathHour, deathMinute, temp, 1);

			new corpses[128];
			format(corpses, sizeof(corpses), "Identyfikacja:\t%s\nPrzyczyna zgonu:\t%s\nData zgonu: %02d: %02d:%02d\nDNA sprawcy: %d", PlayerItem[playerid][itemid][player_item_name], death_name, deathDay, deathHour, deathMinute, PlayerItem[playerid][itemid][player_item_value1]);
			ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_TABLIST, "Informacje o zw�okach", corpses, "Zamknij", "");
		}

		case ITEM_TYPE_ARMOUR:
		{
			if(PlayerItem[playerid][itemid][player_item_value1] < 1)
			{
				SendPlayerInformation(playerid, "Kevlar zostal calkowicie ~r~~h~zuzyty~w~.", 8000);
				RemovePlayerStatus(playerid, PLAYER_STATUS_ARMOUR);
				DeleteItem(itemid, true, playerid);
				return 1;
			}

			if(!PlayerItem[playerid][itemid][player_item_used])
			{
				if( PlayerItem[playerid][itemid][player_item_extraid] > 0)
				{
					new gid = pInfo[playerid][player_duty_gid];
					if(gid == -1) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Nie masz uprawnie� do u�ywania kamizelki lub nie jeste� na s�u�bie grupy.");
					if(!(Group[gid][group_flags] & GROUP_FLAG_911)) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Nie masz uprawnie� do u�ywania kamizelki lub nie jeste� na s�u�bie grupy.");
				}

				pInfo[playerid][player_armour] = PlayerItem[playerid][itemid][player_item_value1];

				AddPlayerStatus(playerid, PLAYER_STATUS_ARMOUR);
				SetPlayerArmour(playerid, floatround(PlayerItem[playerid][itemid][player_item_value1]));
				PlayerItem[playerid][itemid][player_item_used] = true;
			}
			else
			{
				SetPlayerArmour(playerid, 0);
				RemovePlayerStatus(playerid, PLAYER_STATUS_ARMOUR);
				PlayerItem[playerid][itemid][player_item_used] = false;
			}
		}

		case ITEM_TYPE_ROB_CASH:
		{
			new gid = pInfo[playerid][player_duty_gid];
    		if(gid != -1)
    		{
    			if(Group[gid][group_type] == GROUP_TYPE_LSPD)
    			{
    				GiveGroupMoney(gid, 500);
    				SendGuiInformation(playerid, "Informacja", "Twoja grupa otrzymuje wynagrodzenie w wysoko�ci $500 za przechwycenie tej cz�ci �upu z napadu.");
    				DeleteItem(itemid, true, playerid);
    				return 1;
    			}
    		}

			if(GetPlayerVirtualWorld(playerid) == 0) return SendGuiInformation(playerid, "Informacja", "Nie znajdujesz si� w �adnym budynku.");
			
			new d_id = GetDoorByUid(GetPlayerVirtualWorld(playerid));
			if(d_id == -1) return SendGuiInformation(playerid, "Informacja", "Nie znajdujesz si� w �adnym budynku.");

			if(!CanPlayerUseDoor(playerid, d_id)) return SendGuiInformation(playerid, "Informacja", "Nie znajdujesz si� w budynku do kt�rego masz uprawnienia.");

			GivePlayerMoney(playerid, PlayerItem[playerid][itemid][player_item_value1]);
			SendGuiInformation(playerid, "Informacja", sprintf("Pieni�dze z �upu w kwocie $%d zosta�y dodane do twojej kieszeni.", PlayerItem[playerid][itemid][player_item_value1]));

			pInfo[playerid][player_robbery] = -1;

			DeleteItem(itemid, true, playerid);
		}

		case ITEM_TYPE_ROB_TOOLS:
		{
			if(pInfo[playerid][player_robbery] == -1) return SendGuiInformation(playerid, "Informacja", "Nie bierzesz udzia�u w �adnym napadzie.");
			if(pInfo[playerid][player_rob_time] > 0) return SendGuiInformation(playerid, "Informacja", "Akcja nieodozwolona. Bierzesz ju� udzia� w kradzie�y.");
			if(Robbery[pInfo[playerid][player_robbery]][robbery_actors] > Robbery[pInfo[playerid][player_robbery]][robbery_aimed_actors]) return SendGuiInformation(playerid, "Informacja", "Najpierw obezw�adnij zak�adnik�w.");
			
			new safe = GetClosestObjectType(playerid, OBJECT_CASH_REG);
			if(safe != INVALID_OBJECT_ID)
			{
				if(Object[safe][object_owner] != Robbery[pInfo[playerid][player_robbery]][robbery_uid]) return SendGuiInformation(playerid, "Informacja", "Nie znajdujesz si� w pobli�u kasy.");
				if(Object[safe][object_robbed]) return SendGuiInformation(playerid, "Informacja", "Ta kasa zosta�a ju� okradziona, jest pusta.");
				DeleteItem(itemid, true, playerid);

			    pInfo[playerid][player_rob_time] = 40;
			    pInfo[playerid][player_rob_stage] = STAGE_CASH_REG;
			    TogglePlayerControllable(playerid, false);
		    	ApplyAnimation(playerid, "INT_SHOP", "shop_cashier", 4.0, true, false, false, false, 0, 1);
		    	TextDrawSetString(DoorInfo[playerid], "Otwieranie ~r~kasy~w~ w toku.~n~Pozostaly czas: ~y~40s");
		    	TextDrawShowForPlayer(playerid, DoorInfo[playerid]);
				return 1;
			}

			SendGuiInformation(playerid, "Informacja", "Nie znajdujesz si� w pobli�u kasy.");
		}

		case ITEM_TYPE_ROB_BOMBEL:
		{
			if(pInfo[playerid][player_robbery] == -1) return SendGuiInformation(playerid, "Informacja", "Nie bierzesz udzia�u w �adnym napadzie.");
			if(pInfo[playerid][player_rob_time] > 0) return SendGuiInformation(playerid, "Informacja", "Akcja nieodozwolona. Bierzesz ju� udzia� w kradzie�y.");
			if(Robbery[pInfo[playerid][player_robbery]][robbery_actors] > Robbery[pInfo[playerid][player_robbery]][robbery_aimed_actors]) return SendGuiInformation(playerid, "Informacja", "Najpierw obezw�adnij zak�adnik�w.");

			new safe = GetClosestObjectType(playerid, OBJECT_ROB_DOORS);
			if(safe != INVALID_OBJECT_ID)
			{
				if(!Object[safe][object_gate_opened])
				{
					if(Object[safe][object_owner] != Robbery[pInfo[playerid][player_robbery]][robbery_uid]) return SendGuiInformation(playerid, "Informacja", "Te drzwi nie s� przypisane do napadu w kt�rym bierzesz udzia�.");
					if(Object[safe][object_robbed]) return SendGuiInformation(playerid, "Informacja", "Ten sejf zosta� ju� okradziony.");

					switch(PlayerItem[playerid][itemid][player_item_value1])
					{
						case 1:
						{
							DeleteItem(itemid, true, playerid);
						    cmd_ame(playerid, "pod��cza elektronike do drzwi.");
						    pInfo[playerid][player_rob_time] = 60;
						    pInfo[playerid][player_rob_stage] = STAGE_DOOR;
						    TogglePlayerControllable(playerid, false);
					    	ApplyAnimation(playerid, "COP_AMBIENT", "COPBROWSE_LOOP", 4.0, true, false, false, false, 1);
					    	TextDrawSetString(DoorInfo[playerid], "Otwieranie ~r~drzwi~w~ w toku.~n~Pozostaly czas: ~y~60s");
					    	TextDrawShowForPlayer(playerid, DoorInfo[playerid]);
					   	}
					   	case 2:
					   	{
					   		DeleteItem(itemid, true, playerid);
						    cmd_ame(playerid, "podk�ada bombe czasow� pod drzwi.");
						   	defer ExplodeObject[10000](safe);
						   	GameTextForPlayer(playerid, "~w~Wybuch za ~r~~h~10 ~w~sekund", 4000, 1);
						   	ApplyAnimation(playerid, "BOMBER", "BOM_Plant", 4.0, false, false, false, false, 1);
					   	}
					}

				    return 1;
				}
			}

			safe = GetClosestObjectType(playerid, OBJECT_OPEN_SAFE);
			if(safe != INVALID_OBJECT_ID)
			{
				if(Object[safe][object_owner] != Robbery[pInfo[playerid][player_robbery]][robbery_uid]) return SendGuiInformation(playerid, "Informacja", "Ten sejf nie jest przypisany do napadu w kt�rym bierzesz udzia�.");
				if(Object[safe][object_robbed]) return SendGuiInformation(playerid, "Informacja", "Ten sejf zosta� ju� okradziony.");

				switch(PlayerItem[playerid][itemid][player_item_value1])
				{
					case 1:
					{
					    new safe_door = GetClosestObjectType(playerid, OBJECT_SAFE_DOOR);
					    if(safe_door != INVALID_OBJECT_ID)
					    {
					    	DeleteItem(itemid, true, playerid);
					    	cmd_ame(playerid, "pod��cza elektronike do sejfu.");
					    	
					    	TextDrawSetString(DoorInfo[playerid], "Wlamywanie do ~y~sejfu~w~ w toku.~n~Pozostaly czas: ~y~60s");
					    	TextDrawShowForPlayer(playerid, DoorInfo[playerid]);
					    	
					    	TogglePlayerControllable(playerid, false);
					    	ApplyAnimation(playerid, "COP_AMBIENT", "COPBROWSE_LOOP", 4.0, true, false, false, false, 0);
					    	pInfo[playerid][player_rob_time] = 60;
					    	pInfo[playerid][player_rob_stage] = STAGE_CRACKING;
					    }
				   	}
				   	case 2:
				   	{
				   		new safe_door = GetClosestObjectType(playerid, OBJECT_SAFE_DOOR);
				   		if(safe_door != INVALID_OBJECT_ID)
				   		{
					   		DeleteItem(itemid, true, playerid);
						    cmd_ame(playerid, "podk�ada bombe czasow� pod drzwi sejfu.");

						    TextDrawSetString(DoorInfo[playerid], "Podkladanie ~r~bomby~w~ w toku.~n~Pozostaly czas: ~y~20s");
					    	TextDrawShowForPlayer(playerid, DoorInfo[playerid]);

						    TogglePlayerControllable(playerid, false);
						    ApplyAnimation(playerid, "COP_AMBIENT", "COPBROWSE_LOOP", 4.0, true, false, false, false, 0);
					    	pInfo[playerid][player_rob_time] = 20;
					    	pInfo[playerid][player_rob_stage] = STAGE_PLANTING;
						}
				   	}
				}
			    return 1;
			}
			
			SendGuiInformation(playerid, "Informacja", "Nie znajdujesz si� w pobli�u sejfu lub drzwi.");
			
		}

		case ITEM_TYPE_TOOLBOX:
		{
			if(GetPlayerState(playerid) != PLAYER_STATE_ONFOOT) return SendGuiInformation(playerid, "Informacja", "Nie znajdujesz si� w pobli�u �adnego pojazdu.");

			new vid = GetNearestVehicle(playerid);
			if(vid == -1) return SendGuiInformation(playerid, "Informacja", "Nie znajdujesz si� w pobli�u �adnego pojazdu.");

			if(Vehicle[vid][vehicle_destroyed] == false) return SendGuiInformation(playerid, "Informacja", "Ten samoch�d nie jest ca�kowicie zniszczony.");
			
			new Float:x, Float:y, Float:z;
			GetVehicleHood(vid, x, y, z);

			if(!IsPlayerInRangeOfPoint(playerid, 3.0, x, y, z)) return SendGuiInformation(playerid, "Informacja", "Nie znajdujesz si� przy masce samochodu.");
			if(Vehicle[vid][vehicle_bonnet] == false) return SendGuiInformation(playerid, "Informacja", "Maska samochodu musi by� otwarta.");

			ApplyAnimation(playerid, "BD_FIRE", "wash_up", 4.1, false, false, false, false, 0, 1);

			Vehicle[vid][vehicle_health] = PlayerItem[playerid][itemid][player_item_value1];
			SetVehicleHealth(vid, PlayerItem[playerid][itemid][player_item_value1]);
		    Vehicle[vid][vehicle_destroyed] = false;

		    mysql_query(mySQLconnection, sprintf("UPDATE ipb_vehicles SET vehicle_health = '%f' WHERE vehicle_uid = %d", Vehicle[vid][vehicle_health], Vehicle[vid][vehicle_uid]));

		    DeleteItem(itemid, true, playerid);
		    cmd_ame(playerid, sprintf("naprawia pojazd %s.", VehicleNames[Vehicle[vid][vehicle_model]-400]));
		}

		case ITEM_TYPE_GYM_KARNET:
		{
			if(!PlayerItem[playerid][itemid][player_item_used])
			{
				if(GetPlayerUsedItem(playerid, ITEM_TYPE_GYM_KARNET) > -1) return SendGuiInformation(playerid, "Informacja", "Posiadasz ju� u�yty karnet.");

				if(PlayerItem[playerid][itemid][player_item_value1] < 1)
				{
					SendGuiInformation(playerid, "Informacja", "Przedmiot nie posiada wi�cej u�y�, zostaje skasowany.");
					DeleteItem(itemid, true, playerid);
					return 1;
				}
				
				if(pInfo[playerid][player_gym_carnet] == 0) return SendGuiInformation(playerid, "Informacja", "Wyczerpa�e� limit trening�w na dzi�.");
				SendClientMessage(playerid, COLOR_GOLD, "Karnet zosta� u�yty, udaj sie do maszyny na kt�rej chcesz �wiczyc i u�yj /trenuj.");

				pInfo[playerid][player_gym] = PlayerItem[playerid][itemid][player_item_value2];
				PlayerItem[playerid][itemid][player_item_used] = true;
			}
			else
			{
				if(pInfo[playerid][player_training] == true) return SendGuiInformation(playerid, "Informacja", "Akcja niedozwolona w czasie treningu.");
				PlayerItem[playerid][itemid][player_item_used] = false;
			}
		}
	}
	
	return 1;
}

stock GetPlayerByPhone(number)
{
	foreach(new playerid: Player)
	{
		foreach(new itemid : PlayerItems[playerid])
		{
			if( PlayerItem[playerid][itemid][player_item_value1] == number && PlayerItem[playerid][itemid][player_item_used]) return playerid;
		}
	}
	return INVALID_PLAYER_ID;
}


stock GetItemByData(i_type, i_value1 = -1, i_value2 = -1, &player, &item)
{
	new bool:item_good = false;

	foreach(new playerid: Player)
	{
		foreach(new itemid : PlayerItems[playerid])
		{
			item_good = true;
			if( PlayerItem[playerid][itemid][player_item_type] == i_type )
			{
				if( i_value1 > -1 )
				{
					if( PlayerItem[playerid][itemid][player_item_value1] != i_value1 ) item_good = false;
				}
				
				if( i_value2 > -1 )
				{
					if( PlayerItem[playerid][itemid][player_item_value2] != i_value2 ) item_good = false;
				}
			}
			else 
			{
				item_good = false;
				item = -1;
				player = INVALID_PLAYER_ID;
			}
			
			if( item_good )
			{
				item = itemid;
				player = playerid;
				break;
			}
		}
	}
	
	return 1;
}

stock GetPlayerUsedItem(playerid, i_type)
{
	if(playerid == INVALID_PLAYER_ID) return -1;
	foreach(new itemid : PlayerItems[playerid])
	{
		if( PlayerItem[playerid][itemid][player_item_type] == i_type && PlayerItem[playerid][itemid][player_item_used] ) return itemid;
	}
	
	return -1;
}

stock Item_Drop(itemid, playerid = INVALID_PLAYER_ID, bool:with_message = true)
{
    if( playerid == INVALID_PLAYER_ID) return 1;
    if( PlayerItem[playerid][itemid][player_item_used] ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Nie mo�esz od�o�y� przedmiotu, kt�ry jest w u�yciu.");
   	
    if( !IsPlayerInAnyVehicle(playerid) )
    {
        if( playerid != INVALID_PLAYER_ID )
        {
        	mysql_query(mySQLconnection, sprintf("UPDATE ipb_items SET item_created = %d WHERE item_uid = %d", gettime(), PlayerItem[playerid][itemid][player_item_uid]));

        	new Float:x, Float:y, Float:z, iuid;
            GetPointInAngleOfPlayer(playerid, x, y, z, 1.0, 0.0);
           
            z = floatadd(z, -0.973339);
          
            RefreshPlayer(playerid);
            ApplyAnimation(playerid, "BOMBER", "BOM_Plant", 4.0, false, false, false, false, 0, 1);
           
      		iuid = PlayerItem[playerid][itemid][player_item_uid];

            new str[400];
            strcat(str, sprintf("UPDATE `ipb_items` SET `item_ownertype` = %d, `item_owner` = 0, `item_posx` = %f, `item_posy` = %f, `item_posz` = %f,", ITEM_OWNER_TYPE_GROUND, x, y, z));
            strcat(str, sprintf(" `item_rotx` = '90.8', `item_roty` = '0', `item_rotz` = '0', `item_world` = %d, `item_interior` = %d  WHERE `item_uid` = %d", GetPlayerVirtualWorld(playerid), GetPlayerInterior(playerid), iuid));
            mysql_query(mySQLconnection, str);

            pInfo[playerid][player_capacity] += PlayerItem[playerid][itemid][player_item_weight];
            printf("[ITEM] %s (UID %d) od�o�y� przedmiot na ziemie - UID %d. (%s)", pInfo[playerid][player_name], pInfo[playerid][player_id], PlayerItem[playerid][itemid][player_item_uid], PlayerItem[playerid][itemid][player_item_name]);

            DeleteItem(itemid, false, playerid);
            LoadItem(sprintf("WHERE item_uid = %d", iuid));
           	
           	if(with_message == true)
           	{
            	cmd_ame(playerid, "odk�ada co� na ziemie.");
            }
        }
    }
    else
    {
        new vid = GetPlayerVehicleID(playerid);
        new iuid = PlayerItem[playerid][itemid][player_item_uid];
        if( !CanPlayerUseVehicle(playerid, vid) ) return SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Nie masz uprawnie� do odk�adania przedmiot�w w tym poje�dzie.");
        new model = GetVehicleModel(vid);
        if(model == 509 || model == 510 || model == 481 || model == 461 || model == 462 || model == 463 || model == 468 || model == 521 || model == 522 || model == 523 || model == 581 || model == 586)
        {
            SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Odk�adanie przedmiot�w w jedno�ladach nie jest mo�liwe.");
            return 1;
        }

        new rows, fields;
        mysql_query(mySQLconnection, sprintf("SELECT item_uid FROM ipb_items WHERE item_ownertype = %d AND item_owner = %d", ITEM_OWNER_TYPE_VEHICLE, Vehicle[vid][vehicle_uid]));
        cache_get_data(rows, fields);

        /*new capacity = VehicleTrunkCapacity[model-400];
       	
       	if(rows >= capacity)
       	{
       		SendGuiInformation(playerid, ""guiopis"Powiadomienie", "Ten pojazd nie zmie�ci wi�cej przedmiot�w.");
       		return 1;
       	}*/

        mysql_query(mySQLconnection, sprintf("UPDATE `ipb_items` SET `item_ownertype` = %d, `item_owner` = %d WHERE `item_uid` = %d", ITEM_OWNER_TYPE_VEHICLE, Vehicle[vid][vehicle_uid], iuid));
        ApplyAnimation(playerid, "BOMBER", "BOM_Plant", 4.0, false, false, false, false, 0, 1);
       	printf("[ITEM] %s (UID %d) od�o�y� przedmiot do pojazdu - UID %d. (%s)", pInfo[playerid][player_name], pInfo[playerid][player_id], PlayerItem[playerid][itemid][player_item_uid], PlayerItem[playerid][itemid][player_item_name]);

       	pInfo[playerid][player_capacity] += PlayerItem[playerid][itemid][player_item_weight];
       	DeleteItem(itemid, false, playerid);

       	if(with_message == true)
       	{
        	cmd_ame(playerid, "odk�ada co� w poje�dzie.");
        }
    }
   
    return 1;
}

stock Item_Pickup(itemuid, playerid = INVALID_PLAYER_ID)
{
	if( playerid == INVALID_PLAYER_ID) return 1;
	new rows, fields;
	mysql_query(mySQLconnection, sprintf("SELECT item_weight, item_ownertype, item_name, item_type FROM ipb_items WHERE item_uid = %d AND item_ownertype != %d", itemuid, ITEM_OWNER_TYPE_PLAYER));
	cache_get_data(rows, fields);

	new weight = cache_get_row_int(0, 0);

	if(pInfo[playerid][player_capacity] < weight) return SendGuiInformation(playerid, "Informacja", "Przedmiot, kt�ry pr�bujesz podnie�� jest zbyt ci�ki.");
	
	new ownertype = cache_get_row_int(0, 1);
	new itype = cache_get_row_int(0, 3);
	new itname[40];

	cache_get_row(0, 2, itname, mySQLconnection, 40);

	if( ownertype == ITEM_OWNER_TYPE_GROUND )
	{
		new object_id = GetItemObject(playerid, itemuid);

		if(object_id != INVALID_OBJECT_ID)
		{
			DestroyDynamicObject(object_id);
		}	
		cmd_ame(playerid, sprintf("podnosi %s z ziemi.", itname));
		printf("[ITEM] %s (UID %d) podni�s� przedmiot z ziemi - UID %d. (%s)", pInfo[playerid][player_name], pInfo[playerid][player_id], itemuid, itname);
	}
	else if( ownertype == ITEM_OWNER_TYPE_VEHICLE )
	{
		cmd_ame(playerid, sprintf("podnosi %s z pojazdu.", itname));
		printf("[ITEM] %s (UID %d) podni�s� przedmiot z pojazdu - UID %d. (%s)", pInfo[playerid][player_name], pInfo[playerid][player_id], itemuid, itname);
	}
	else if( ownertype == ITEM_OWNER_TYPE_DOOR )
	{
		cmd_ame(playerid, sprintf("wyci�ga %s ze schowka.", itname));
		printf("[ITEM] %s (UID %d) wyci�gn�� przedmiot ze schowka - UID %d. (%s)", pInfo[playerid][player_name], pInfo[playerid][player_id], itemuid, itname);
	}
	
	pInfo[playerid][player_capacity] -= weight;
	
	ApplyAnimation(playerid, "BOMBER", "BOM_Plant", 4.0, false, false, false, false, 0, 1);
	
	if(itype == ITEM_TYPE_WEAPON || itype == ITEM_TYPE_DRUG)
	{
		if( GetPlayerUsedItem(playerid, ITEM_TYPE_GLOVES) == -1 )
		{
			mysql_query(mySQLconnection, sprintf("INSERT INTO `ipb_fingerprints` (finger_owner, finger_person) VALUES ('%d', '%d')", itemuid, pInfo[playerid][player_id]));
		}
	}

	mysql_query(mySQLconnection, sprintf("UPDATE `ipb_items` SET `item_ownertype` = %d, `item_owner` = %d, `item_posx` = 0.0, `item_posy` = 0.0, `item_posz` = 0.0, `item_rotx` = 0.0, `item_roty` = 0.0, `item_rotz` = 0.0, `item_world` = 0, `item_interior` = 0  WHERE `item_uid` = %d AND item_ownertype != %d", ITEM_OWNER_TYPE_PLAYER, pInfo[playerid][player_id], itemuid, ITEM_OWNER_TYPE_PLAYER ));
	
	if(cache_affected_rows(mySQLconnection))
	{
		LoadPlayerItem(playerid, sprintf("WHERE item_uid = %d", itemuid));
		return 1;
	}
	return 1;
}

stock Item_Create(owner_type, owner_id, i_type, i_model, i_val1, i_val2, const i_name[40], i_extra=0, i_count=0)
{
	new itemid = -1;
	if( owner_type == ITEM_OWNER_TYPE_PLAYER )
	{
		new playerid = owner_id;
		if( owner_id > -1 )
		{
			mysql_query(mySQLconnection, sprintf("INSERT INTO `ipb_items` (`item_uid`,`item_model`,`item_ownertype`,`item_owner`,`item_type`,`item_value1`,`item_value2`,`item_name`,`item_created`, `item_extraid` ) VALUES (null, %d, %d, %d, %d, %d, %d,'%s', %d, %d)", i_model, owner_type, pInfo[playerid][player_id], i_type, i_val1, i_val2, i_name, gettime(), i_extra));
			new uid = cache_insert_id();
			
			itemid = LoadPlayerItem(playerid, sprintf("WHERE `item_uid` = %d", uid), true);
		}
	}
	else if( owner_type == ITEM_OWNER_TYPE_GROUP )
	{
		mysql_query(mySQLconnection, sprintf("INSERT INTO `ipb_items` (`item_uid`,`item_model`,`item_ownertype`,`item_owner`,`item_type`,`item_value1`,`item_value2`,`item_name`,`item_created`,`item_extraid`, `item_group`, `item_count`) VALUES (null, %d, %d, %d, %d, %d, %d,'%s', %d, %d, %d, %d)", i_model, owner_type, owner_id, i_type, i_val1, i_val2, i_name, gettime(), i_extra, owner_id, i_count));
		new uid = cache_insert_id();
		
		itemid = LoadItem(sprintf("WHERE `item_uid` = %d", uid), true);
		SetupItemWeight(itemid);
	}
	return itemid;
}

stock GetItemByUid(itemuid, playerid)
{
	foreach(new item : PlayerItems[playerid])
	{
		if( PlayerItem[playerid][item][player_item_uid] == itemuid ) return item;
	}
	
	return -1;
}

stock GetItemByGlobalUid(itemuid)
{
	foreach(new playerid: Player)
	{
		foreach(new item : PlayerItems[playerid])
		{
			if( PlayerItem[playerid][item][player_item_uid] == itemuid ) return item;
		}
	}
	
	return -1;
}

stock HasPlayerItem(itemtype, playerid)
{
	foreach(new item : PlayerItems[playerid])
	{
		if( PlayerItem[playerid][item][player_item_type] == itemtype ) return item;
	}
	return -1;
}

stock HasPlayerWeapon(playerid)
{
	foreach(new item : PlayerItems[playerid])
	{
		if( PlayerItem[playerid][item][player_item_type] == ITEM_TYPE_WEAPON && PlayerItem[playerid][item][player_item_value1] > 10 && PlayerItem[playerid][item][player_item_value1] < 38) return item;
	}
	return -1;
}

stock GetItemByTargetUid(itemuid, target)
{
	foreach(new item : PlayerItems[target])
	{
		if( PlayerItem[target][item][player_item_uid] == itemuid ) return item;
	}
	return -1;
}


stock GetGroupItemByUid(itemuid)
{
	foreach(new item : Items)
	{
		if( Item[item][item_uid] == itemuid ) return item;
	}
	
	return -1;
}

stock IsValidDrugType(playerid, itemid)
{
	switch(PlayerItem[playerid][itemid][player_item_value1])
	{
		case DRUG_TYPE_COCAINE: return 1;
		case DRUG_TYPE_HEROINE: return 1;
		case DRUG_TYPE_CRACK: return 1;
		case DRUG_TYPE_WEED: return 1;
		case DRUG_TYPE_AMFA: return 1;
		case DRUG_TYPE_METH: return 1;
		case DRUG_TYPE_EXTASY: return 1;
	}
	return 0;
}

stock SetupItemWeight(itemid)
{
	if(itemid == -1) return 1;
	new type = Item[itemid][item_type];

	switch(type)
	{
		case ITEM_TYPE_AMMO:
		{
			Item[itemid][item_weight] = 250;
		}
		case ITEM_TYPE_MASKA:
		{
			Item[itemid][item_weight] = 500;
		}  
		case ITEM_TYPE_PHONE:
		{
			Item[itemid][item_weight] = 120;
		} 
		case ITEM_TYPE_WATCH:
		{
			Item[itemid][item_weight] = 75;
		}
		case ITEM_TYPE_FOOD:
		{
			Item[itemid][item_weight] = 50;
		}  
		case ITEM_TYPE_CIGGY:
		{
			Item[itemid][item_weight] = 35;
		} 
		case ITEM_TYPE_CUBE:
		{
			Item[itemid][item_weight] = 5;
		} 
		case ITEM_TYPE_CLOTH:
		{
			Item[itemid][item_weight] = 250;
		} 
		case ITEM_TYPE_CANISTER:
		{
			Item[itemid][item_weight] = 900;
		} 
		case ITEM_TYPE_LINE:
		{
			Item[itemid][item_weight] = 350;
		} 
		case ITEM_TYPE_TUNING:
		{
			Item[itemid][item_weight] = 3500;
		} 
		case ITEM_TYPE_BAG:
		{
			Item[itemid][item_weight] = 500;
		} 
		case ITEM_TYPE_DRINK:
		{
			Item[itemid][item_weight] = 50;
		} 
		case ITEM_TYPE_DRUG:
		{
			Item[itemid][item_weight] = Item[itemid][item_value2];
		} 
		case ITEM_TYPE_ATTACH:
		{
			Item[itemid][item_weight] = 225;
		} 
		case ITEM_TYPE_ACCESS:
		{
			Item[itemid][item_weight] = 750;
		} 
		case ITEM_TYPE_PLATE:
		{
			Item[itemid][item_weight] = 35;
		} 
		case ITEM_TYPE_ARMOUR:
		{
			Item[itemid][item_weight] = 1000;
		} 
		case ITEM_TYPE_GLOVES:
		{
			Item[itemid][item_weight] = 250;
		} 
		case ITEM_TYPE_CORPSE:
		{
			Item[itemid][item_weight] = 5000;
		}
		case ITEM_TYPE_HOWITZER:
		{
			Item[itemid][item_weight] = 180;
		}
		case ITEM_TYPE_LEK:
		{
			Item[itemid][item_weight] = 70;
		}
		case ITEM_TYPE_GYM_KARNET:
		{
			Item[itemid][item_weight] = 20;
		}
		case ITEM_TYPE_BOMB:
		{
			Item[itemid][item_weight] = 2800;
		}
		case ITEM_TYPE_WEAPON:
		{
			Item[itemid][item_weight] = 850;
		}
		case ITEM_TYPE_EMPTY_PLATE:	Item[itemid][item_weight] = 35;
		case ITEM_TYPE_GRENADE:	Item[itemid][item_weight] = 700;
		case ITEM_TYPE_ROB_BOMBEL: Item[itemid][item_weight] = 1250;
		case ITEM_TYPE_ROB_TOOLS: Item[itemid][item_weight] = 250;
		case ITEM_TYPE_ROB_CASH: Item[itemid][item_weight] = 550;
	}
	mysql_query(mySQLconnection, sprintf("UPDATE ipb_items SET item_weight = %d WHERE item_uid = %d", Item[itemid][item_weight], Item[itemid][item_uid]));
	return 1;
}

stock SetupPlayerItemWeight(playerid, itemid)
{
	if(itemid == -1) return 1;
	new type = PlayerItem[playerid][itemid][player_item_type];

	if(PlayerItem[playerid][itemid][player_item_weight] == 0)
	{
		switch(type)
		{
			case ITEM_TYPE_AMMO: PlayerItem[playerid][itemid][player_item_weight] = 250;

			case ITEM_TYPE_WEAPON: 
			{
				switch(PlayerItem[playerid][itemid][player_item_value1])
				{
					case 1: PlayerItem[playerid][itemid][player_item_weight] = 150;
					case 2: PlayerItem[playerid][itemid][player_item_weight] = 350;
					case 3: PlayerItem[playerid][itemid][player_item_weight] = 280;
					case 4: PlayerItem[playerid][itemid][player_item_weight] = 90;
					case 5: PlayerItem[playerid][itemid][player_item_weight] = 400;
					case 6: PlayerItem[playerid][itemid][player_item_weight] = 420;
					case 7: PlayerItem[playerid][itemid][player_item_weight] = 180;
					case 8: PlayerItem[playerid][itemid][player_item_weight] = 220;
					case 9: PlayerItem[playerid][itemid][player_item_weight] = 2400;
					case 10: PlayerItem[playerid][itemid][player_item_weight] = 90;
					case 11: PlayerItem[playerid][itemid][player_item_weight] = 90;
					case 12: PlayerItem[playerid][itemid][player_item_weight] = 90;
					case 13: PlayerItem[playerid][itemid][player_item_weight] = 90;
					case 14: PlayerItem[playerid][itemid][player_item_weight] = 110;
					case 15: PlayerItem[playerid][itemid][player_item_weight] = 170;
					case 16: PlayerItem[playerid][itemid][player_item_weight] = 80;
					case 17: PlayerItem[playerid][itemid][player_item_weight] = 75;
					case 18: PlayerItem[playerid][itemid][player_item_weight] = 225;
					case 22: PlayerItem[playerid][itemid][player_item_weight] = 870;
					case 23: PlayerItem[playerid][itemid][player_item_weight] = 970;
					case 24: PlayerItem[playerid][itemid][player_item_weight] = 1140;
					case 25: PlayerItem[playerid][itemid][player_item_weight] = 1800;
					case 26: PlayerItem[playerid][itemid][player_item_weight] = 900;
					case 27: PlayerItem[playerid][itemid][player_item_weight] = 2900;
					case 28: PlayerItem[playerid][itemid][player_item_weight] = 660;
					case 29: PlayerItem[playerid][itemid][player_item_weight] = 1600;
					case 30: PlayerItem[playerid][itemid][player_item_weight] = 3100;
					case 31: PlayerItem[playerid][itemid][player_item_weight] = 2900;
					case 32: PlayerItem[playerid][itemid][player_item_weight] = 700;
					case 33: PlayerItem[playerid][itemid][player_item_weight] = 2200;
					case 34: PlayerItem[playerid][itemid][player_item_weight] = 2500;
					case 35: PlayerItem[playerid][itemid][player_item_weight] = 5500;
					case 36: PlayerItem[playerid][itemid][player_item_weight] = 5500;
					case 37: PlayerItem[playerid][itemid][player_item_weight] = 4900;
					case 38: PlayerItem[playerid][itemid][player_item_weight] = 5200;
					case 41: PlayerItem[playerid][itemid][player_item_weight] = 550;
					case 42: PlayerItem[playerid][itemid][player_item_weight] = 1550;
					case 43: PlayerItem[playerid][itemid][player_item_weight] = 850;
					case 46: PlayerItem[playerid][itemid][player_item_weight] = 650;
				}
			}

			case ITEM_TYPE_MASKA: PlayerItem[playerid][itemid][player_item_weight] = 500;
			case ITEM_TYPE_PHONE: PlayerItem[playerid][itemid][player_item_weight] = 120;
			case ITEM_TYPE_WATCH: PlayerItem[playerid][itemid][player_item_weight] = 75;
			case ITEM_TYPE_FOOD: PlayerItem[playerid][itemid][player_item_weight] = 50;
			case ITEM_TYPE_CIGGY: PlayerItem[playerid][itemid][player_item_weight] = 35;
			case ITEM_TYPE_CUBE: PlayerItem[playerid][itemid][player_item_weight] = 5;
			case ITEM_TYPE_CLOTH: PlayerItem[playerid][itemid][player_item_weight] = 250;
			case ITEM_TYPE_CANISTER: PlayerItem[playerid][itemid][player_item_weight] = 900;
			case ITEM_TYPE_LINE: PlayerItem[playerid][itemid][player_item_weight] = 350;
			case ITEM_TYPE_TUNING: PlayerItem[playerid][itemid][player_item_weight] = 3500;
			case ITEM_TYPE_BAG: PlayerItem[playerid][itemid][player_item_weight] = 500; 
			case ITEM_TYPE_DRINK: PlayerItem[playerid][itemid][player_item_weight] = 50;
			case ITEM_TYPE_DRUG: PlayerItem[playerid][itemid][player_item_weight] = PlayerItem[playerid][itemid][player_item_value2];
			case ITEM_TYPE_ATTACH: PlayerItem[playerid][itemid][player_item_weight] = 225;
			case ITEM_TYPE_ACCESS: PlayerItem[playerid][itemid][player_item_weight] = 750;
			case ITEM_TYPE_PLATE: PlayerItem[playerid][itemid][player_item_weight] = 35;
			case ITEM_TYPE_EMPTY_PLATE: PlayerItem[playerid][itemid][player_item_weight] = 35;
			case ITEM_TYPE_ARMOUR: PlayerItem[playerid][itemid][player_item_weight] = 1000;
			case ITEM_TYPE_GLOVES: PlayerItem[playerid][itemid][player_item_weight] = 250;
			case ITEM_TYPE_CORPSE: PlayerItem[playerid][itemid][player_item_weight] = 5000;
			case ITEM_TYPE_HOWITZER: PlayerItem[playerid][itemid][player_item_weight] = 180;
			case ITEM_TYPE_LEK: PlayerItem[playerid][itemid][player_item_weight] = 70;
			case ITEM_TYPE_GYM_KARNET: PlayerItem[playerid][itemid][player_item_weight] = 20;
			case ITEM_TYPE_BOMB: PlayerItem[playerid][itemid][player_item_weight] = 2800;
			case ITEM_TYPE_DOOR_ACCESS: PlayerItem[playerid][itemid][player_item_weight] = 700;
			case ITEM_TYPE_MOLOTOV: PlayerItem[playerid][itemid][player_item_weight] = 450;
			case ITEM_TYPE_DEMOLITION: PlayerItem[playerid][itemid][player_item_weight] = 380;
			case ITEM_TYPE_GRENADE: PlayerItem[playerid][itemid][player_item_weight] = 700;
			case ITEM_TYPE_ROB_BOMBEL: PlayerItem[playerid][itemid][player_item_weight] = 1250;
			case ITEM_TYPE_ROB_TOOLS: PlayerItem[playerid][itemid][player_item_weight] = 250;
			case ITEM_TYPE_ROB_CASH: PlayerItem[playerid][itemid][player_item_weight] = 550;
		}

		mysql_query(mySQLconnection, sprintf("UPDATE ipb_items SET item_weight = %d WHERE item_uid = %d", PlayerItem[playerid][itemid][player_item_weight], PlayerItem[playerid][itemid][player_item_uid]));
	}
	return 1;
}