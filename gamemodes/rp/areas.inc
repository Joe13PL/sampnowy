stock LoadArea(limit[] = "", bool:return_id = false)
{
	new rows, fields;
	mysql_query(mySQLconnection, sprintf("SELECT * FROM `ipb_areas` %s", limit));
	cache_get_data(rows, fields);
	
	new area_id;
	for(new row = 0; row != rows; row++)
	{
		area_id = -1;
		new shape = cache_get_row_int(row, 1);
		
		new Float:point1[3], Float:point2[3];
		
		new str[100];
		cache_get_row(row, 8, str);
		sscanf(str, "p<|>a<f>[3]", point1);
		
		new str2[100];
		cache_get_row(row, 9, str2);
		sscanf(str2, "p<|>a<f>[3]", point2);
		
		if( shape == AREA_SHAPE_SQUARE )
		{
			// Tworzymy prostokatna strefe
			new Float:pointes[8];
			pointes[0] = point1[0];
			pointes[1] = point1[1];
			pointes[2] = point1[0];
			pointes[3] = point2[1];
			pointes[4] = point2[0];
			pointes[5] = point2[1];
			pointes[6] = point2[0];
			pointes[7] = point1[1];
			
			new atype = cache_get_row_int(row, 2);

			if(atype == AREA_TYPE_SURFACE)
			{
				area_id = CreateDynamicPolygon(pointes, point1[2], point2[2], 8, cache_get_row_int(row, 6));
			}
			else
			{
				area_id = CreateDynamicPolygon(pointes, -FLOAT_INFINITY, FLOAT_INFINITY, 8, cache_get_row_int(row, 6));
				Area[area_id][area_zone] = GangZoneCreate(Min(point1[0], point2[0]), Min(point1[1], point2[1]), Max(point1[0], point2[0]), Max(point1[1], point2[1]));
			}
		
          	Area[area_id][area_objects_limit] = cache_get_row_int(row, 5);
		}
		else if( shape == AREA_SHAPE_CIRCLE )
		{
			// Tworzymy okragla strefe
			area_id = CreateDynamicSphere(point1[0], point1[1], point1[2], cache_get_row_float(row, 7), cache_get_row_int(row, 6));
		}
		
		Area[area_id][area_uid] = cache_get_row_int(row, 0);
		
		Area[area_id][area_type] = cache_get_row_int(row, 2);
		Area[area_id][area_owner_type] = cache_get_row_int(row, 3);
		Area[area_id][area_owner] = cache_get_row_int(row, 4);
		Area[area_id][area_flags] = cache_get_row_int(row, 10);
		cache_get_row(row, 11, Area[area_id][area_audio], mySQLconnection, 100);
		Area[area_id][area_price] = cache_get_row_int(row, 12);
		Area[area_id][area_meters] = cache_get_row_int(row, 13);
		Area[area_id][area_visible] = cache_get_row_int(row, 14);
		Area[area_id][area_attacked] = cache_get_row_int(row, 15);

		new gid = GetGroupByUid(Area[area_id][area_owner]);

		if(gid != -1)
		{
			if(Area[area_id][area_visible]) Area[area_id][area_color] = GetGroupColor(gid);
		}
		
		Iter_Add(Areas, area_id);
	}
	
	if( return_id ) return area_id;
	return rows;
}

stock DeleteArea(a_id, bool:from_database = true)
{
	if( from_database ) mysql_query(mySQLconnection, sprintf("DELETE FROM `ipb_areas` WHERE `area_uid` = %d", Area[a_id][area_uid]));
	
	if(Area[a_id][area_type] == AREA_TYPE_NORMAL) GangZoneDestroy(Area[a_id][area_zone]);

	DestroyDynamicArea(a_id);
		
	Iter_Remove(Areas, a_id);
	
	for(new z=0; e_areas:z != e_areas; z++)
    {
		Area[a_id][e_areas:z] = 0;
    }
}

stock ShowPlayerZones(playerid)
{
	foreach(new gzone: Areas)
	{
		if(Area[gzone][area_visible])
		{
			GangZoneShowForPlayer(playerid, Area[gzone][area_zone], Area[gzone][area_color]);
		}
	}
}

stock HidePlayerZones(playerid)
{
	foreach(new gzone: Areas)
	{
		if(Area[gzone][area_visible])
		{
			GangZoneHideForPlayer(playerid, Area[gzone][area_zone]);
		}
	}
}



stock AreaCreator(playerid)
{
	if( pInfo[playerid][player_carea_type] == AREA_SHAPE_SQUARE )
	{
		if( pInfo[playerid][player_carea_point2][0] == 0.0 && pInfo[playerid][player_carea_point2][1] == 0.0 && pInfo[playerid][player_carea_point2][2] == 0.0 ) return ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Informacja", "Musisz wyznaczyæ dwa punkty tworz¹ce przek¹tn¹.", "OK", "");	
	
		new str[400], a_type, a_ownertype, a_owner;

		if(pInfo[playerid][player_surface_edit])
		{
			a_type = AREA_TYPE_SURFACE;
			a_ownertype = AREA_OWNER_TYPE_DOOR;
			a_owner = GetPlayerVirtualWorld(playerid);
		}
		else
		{
			a_type = AREA_TYPE_NORMAL;
			a_ownertype = AREA_OWNER_TYPE_GLOBAL;
			a_owner = 0;
		}

		strcat(str, "INSERT INTO `ipb_areas` (area_uid, area_shape, area_type, area_ownertype, area_owner, area_objects, area_vw, area_size, area_point1, area_point2) ");
		strcat(str, sprintf("VALUES(null, %d, %d, %d, %d, 0, %d, 0.0, '%f|%f|%f', '%f|%f|%f')", AREA_SHAPE_SQUARE, a_type, a_ownertype, a_owner, GetPlayerVirtualWorld(playerid), pInfo[playerid][player_carea_point1][0], pInfo[playerid][player_carea_point1][1], pInfo[playerid][player_carea_point1][2], pInfo[playerid][player_carea_point2][0], pInfo[playerid][player_carea_point2][1], pInfo[playerid][player_carea_point2][2]+1));
		mysql_query(mySQLconnection, str);
		
		new uid = cache_insert_id();
		new a_id = LoadArea(sprintf("WHERE `area_uid` = %d", uid), true);
		
		pInfo[playerid][player_carea_point1][0] = 0.0;
		pInfo[playerid][player_carea_point1][1] = 0.0;
		pInfo[playerid][player_carea_point1][2] = 0.0;
		
		pInfo[playerid][player_carea_point2][0] = 0.0;
		pInfo[playerid][player_carea_point2][1] = 0.0;
		pInfo[playerid][player_carea_point2][2] = 0.0;
		
		if( IsValidDynamic3DTextLabel(pInfo[playerid][player_carea_label][0]) ) DestroyDynamic3DTextLabel(pInfo[playerid][player_carea_label][0]);
		if( IsValidDynamic3DTextLabel(pInfo[playerid][player_carea_label][1]) ) DestroyDynamic3DTextLabel(pInfo[playerid][player_carea_label][1]);
		
		GangZoneDestroy(pInfo[playerid][player_carea_zone]);
		
		pInfo[playerid][player_creating_area] = false;
		SendPlayerInformation(playerid, "", 0);
		
		if(pInfo[playerid][player_surface_edit])
		{
			//Liczymy metra¿
			new Float:apoints[8];
			GetDynamicPolygonPoints(a_id, apoints);

			new Float:A = GetDistanceBetweenPoints2D(apoints[0], apoints[1], apoints[0], apoints[3]);
			new Float:B = GetDistanceBetweenPoints2D(apoints[0], apoints[1], apoints[4], apoints[3]);

			new Float:z1, Float:z2;
			Streamer_GetFloatData(STREAMER_TYPE_AREA, a_id, E_STREAMER_MIN_Z, z1);
			Streamer_GetFloatData(STREAMER_TYPE_AREA, a_id, E_STREAMER_MAX_Z, z2);

			new Float:H = z2-z1;

			new Float:surface_float = (2*A*B) + (2*B*H) + (2*A*H);
			new surface = floatround(surface_float/3);

			new d_id = GetDoorByUid(GetPlayerVirtualWorld(playerid));

			if(H < 0)
			{
				if(IsValidDynamicArea(Door[d_id][door_area])) DeleteArea(a_id, true);
				pInfo[playerid][player_surface_edit] = false;
				TextDrawHideForPlayer(playerid, Tutorial[playerid]);
				SendGuiInformation(playerid, "Informacja", "Metra¿ nie zosta³ poprawnie wyznaczony. Wysokoœæ jest mniejsza od 0.\nW przypadku piêtrowych budynków, wyznaczaj go od do³u do góry.");
				return 1;
			}

			if(surface > Door[d_id][door_meters])
			{
				if(IsValidDynamicArea(Door[d_id][door_area])) DeleteArea(a_id, true);
				pInfo[playerid][player_surface_edit] = false;
				TextDrawHideForPlayer(playerid, Tutorial[playerid]);
				SendGuiInformation(playerid, "Informacja", sprintf("Wyznaczona powierzchnia przekroczy³a dozwolony metra¿: %dm2/%dm2.", surface, Door[d_id][door_meters]));
				return 1;
			}

			if(surface < 0)
			{
				if(IsValidDynamicArea(Door[d_id][door_area])) DeleteArea(a_id, true);
				pInfo[playerid][player_surface_edit] = false;
				TextDrawHideForPlayer(playerid, Tutorial[playerid]);
				SendGuiInformation(playerid, "Informacja", sprintf("Metra¿ nie zosta³ poprawnie wyznaczony (przek¹tna od lewego dolnego rogu, do prawego górnego).\nWynosi on %dm2 (wartoœc niedozwolona).", surface));
				return 1;
			}

			Door[d_id][door_surface] = surface;
			Door[d_id][door_area] = a_id;
			
			//dodac door_area do zapytania, usunac z loadingu drzwi
			ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Informacja", sprintf("Pomyœlnie edytowano wymiary wnêtrza. U¿yty metra¿: %dm2/%dm2.", surface, Door[d_id][door_meters]), "OK", "");
			mysql_query(mySQLconnection, sprintf("UPDATE ipb_doors SET door_surface = %d WHERE door_uid = %d", Door[d_id][door_surface], Door[d_id][door_uid]));
			pInfo[playerid][player_surface_edit] = false;
			TextDrawHideForPlayer(playerid, Tutorial[playerid]);
		}
		else
		{
			ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Informacja", sprintf("Pomyœlnie stworzy³eœ prostok¹tn¹ strefê [UID: %d, ID: %d].", uid, a_id), "OK", "");
		}
	}
	else if( pInfo[playerid][player_carea_type] == AREA_SHAPE_CIRCLE )
	{
		if( pInfo[playerid][player_carea_point1][0] == 0.0 && pInfo[playerid][player_carea_point1][1] == 0.0 && pInfo[playerid][player_carea_point1][2] == 0.0 ) return ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Informacja", "Aby stworzyæ sferyczn¹ strefê musisz wyznaczyæ punkt œrodka sfery.", "OK", "");	
	
		new str[400];
		strcat(str, "INSERT INTO `ipb_areas` (area_uid, area_shape, area_type, area_ownertype, area_owner, area_objects, area_vw, area_size, area_point1, area_point2) ");
		strcat(str, sprintf("VALUES(null, %d, %d, %d, 0, 0, %d, %f, '%f|%f|%f', '')", AREA_SHAPE_CIRCLE, AREA_TYPE_NORMAL, AREA_OWNER_TYPE_GLOBAL, GetPlayerVirtualWorld(playerid), pInfo[playerid][player_carea_size], pInfo[playerid][player_carea_point1][0], pInfo[playerid][player_carea_point1][1], pInfo[playerid][player_carea_point1][2]));
		mysql_query(mySQLconnection, str);
	
		new uid = cache_insert_id();
		new a_id = LoadArea(sprintf("WHERE `area_uid` = %d", uid), true);
		
		pInfo[playerid][player_carea_point1][0] = 0.0;
		pInfo[playerid][player_carea_point1][1] = 0.0;
		pInfo[playerid][player_carea_point1][2] = 0.0;
		
		pInfo[playerid][player_carea_point2][0] = 0.0;
		pInfo[playerid][player_carea_point2][1] = 0.0;
		pInfo[playerid][player_carea_point2][2] = 0.0;
		
		if( IsValidDynamic3DTextLabel(pInfo[playerid][player_carea_label][0]) ) DestroyDynamic3DTextLabel(pInfo[playerid][player_carea_label][0]);
		if( IsValidDynamic3DTextLabel(pInfo[playerid][player_carea_label][1]) ) DestroyDynamic3DTextLabel(pInfo[playerid][player_carea_label][1]);
		
		GangZoneDestroy(pInfo[playerid][player_carea_zone]);
		
		pInfo[playerid][player_creating_area] = false;
		SendPlayerInformation(playerid, "", 0);
		
		ShowPlayerDialog(playerid, DIALOG_INFO, DIALOG_STYLE_MSGBOX, "Informacja", sprintf("Pomyœlnie stworzy³eœ sferyczna strefê [UID: %d, ID: %d].", uid, a_id), "OK", "");
	}
	return 1;
}

stock GetPlayerArea(playerid, type, owner_type = -1)
{
	foreach(new a_id : Areas)
	{
		if( Area[a_id][area_type] == type && IsPlayerInDynamicArea(playerid, a_id) )
		{
			if( owner_type == -1 ) return a_id;
			else if( Area[a_id][area_owner_type] == owner_type ) return a_id;
		}
	}
	return -1;
}

stock GetAreaByUid(a_uid)
{
	foreach(new a_id : Areas)
	{
		if( Area[a_id][area_uid] == a_uid ) return a_id;
	}
	
	return -1;
}

stock CountAreaObjects(a_id)
{
	new count;
	foreach(new o_id : Objects)
	{
		if( Object[o_id][object_owner_type] == OBJECT_OWNER_TYPE_AREA && Object[o_id][object_owner] == Area[a_id][area_uid] ) count++;
	}
	
	return count;
}

stock CountAreaLabels(a_id)
{
	new count;
	foreach(new lid : Labels)
	{
		if( Label[Text3D:lid][label_owner_type] == LABEL_OWNER_TYPE_AREA && Label[Text3D:lid][label_owner] == Area[a_id][area_uid] ) count++;
	}
	
	return count;
}

stock CanPlayerEditArea(playerid, a_id)
{
	if( HasCrewFlag(playerid, CREW_FLAG_AREAS) ) return 1;
	
	if( Area[a_id][area_owner_type] == AREA_OWNER_TYPE_GROUP)
	{
		new gid = pInfo[playerid][player_duty_gid];
		if(gid == -1) return 0;

		new slot = GetPlayerDutySlot(playerid);
		if(slot == -1) return 0;
		if( Area[a_id][area_owner] == Group[gid][group_uid] && WorkerHasFlag(playerid, slot, WORKER_FLAG_LEADER) ) return 1;
	}
	return 0;
}