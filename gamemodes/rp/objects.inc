stock GetClosestObjectType(playerid, object_mod)
{
	new object_id,
    	Float:prevdist = 5.0, OID = INVALID_OBJECT_ID, Float:PosX, Float:PosY, Float:PosZ, Float:dist;
     
	GetPlayerPos(playerid, PosX, PosY, PosZ);
 	for (new player_object = 0; player_object <= MAX_VISIBLE_OBJECTS; player_object++)
 	{
    	if(IsValidPlayerObject(playerid, player_object))
     	{
			object_id = Streamer_GetItemStreamerID(playerid, STREAMER_TYPE_OBJECT, player_object);

        	if(GetObjectModel(object_id) == object_mod)
       		{
    			Streamer_GetDistanceToItem(PosX, PosY, PosZ, STREAMER_TYPE_OBJECT, object_id, dist);
    			if((dist < prevdist))
    			{
        			prevdist = dist;
     				OID = object_id;
    			}
         	}
    	}
	}
	return OID;
}

stock GetClosestCustomModel(playerid, object_mod)
{
	new object_id,
    	Float:prevdist = 50.0, OID = INVALID_OBJECT_ID, Float:PosX, Float:PosY, Float:PosZ, Float:dist;
     
	GetPlayerPos(playerid, PosX, PosY, PosZ);
 	for (new player_object = 0; player_object <= MAX_VISIBLE_OBJECTS; player_object++)
 	{
    	if(IsValidPlayerObject(playerid, player_object))
     	{
			object_id = Streamer_GetItemStreamerID(playerid, STREAMER_TYPE_OBJECT, player_object);

        	if(GetObjectModel(object_id) == object_mod)
       		{
    			Streamer_GetDistanceToItem(PosX, PosY, PosZ, STREAMER_TYPE_OBJECT, object_id, dist);
    			if((dist < prevdist))
    			{
        			prevdist = dist;
     				OID = object_id;
    			}
         	}
    	}
	}
	return OID;
}

stock SelectCustomObject(playerid)
{
	new object_id,
    	Float:prevdist = 30.0, OID = INVALID_OBJECT_ID, Float:PosX, Float:PosY, Float:PosZ, Float:dist;
     
	GetPlayerPos(playerid, PosX, PosY, PosZ);
 	for (new player_object = 0; player_object <= MAX_VISIBLE_OBJECTS; player_object++)
 	{
    	if(IsValidPlayerObject(playerid, player_object))
     	{
			object_id = Streamer_GetItemStreamerID(playerid, STREAMER_TYPE_OBJECT, player_object);

			if(CanPlayerEditObject(playerid, object_id) && Object[object_id][object_is_edited] == false)
			{
				Streamer_GetDistanceToItem(PosX, PosY, PosZ, STREAMER_TYPE_OBJECT, object_id, dist);
				if((dist < prevdist))
				{
	    			prevdist = dist;
	 				OID = object_id;
				}
			}
    	}
	}
	return OID;
}

stock GetClosestGate(playerid)
{
	new object_id,
    	Float:prevdist = 5.0, OID = INVALID_OBJECT_ID, Float:PosX, Float:PosY, Float:PosZ, Float:dist;
     
	GetPlayerPos(playerid, PosX, PosY, PosZ);
 	for (new player_object = 0; player_object <= MAX_VISIBLE_OBJECTS; player_object++)
 	{
    	if(IsValidPlayerObject(playerid, player_object))
     	{
			object_id = Streamer_GetItemStreamerID(playerid, STREAMER_TYPE_OBJECT, player_object);

        	if(Object[object_id][object_gate] == 1)
       		{
    			Streamer_GetDistanceToItem(PosX, PosY, PosZ, STREAMER_TYPE_OBJECT, object_id, dist);
    			if((dist < prevdist))
    			{
        			prevdist = dist;
     				OID = object_id;
    			}
         	}
    	}
	}
	return OID;
}

stock GetClosestBusStop(playerid)
{
	new object_id,
    	Float:prevdist = 200.0, OID = INVALID_OBJECT_ID, Float:PosX, Float:PosY, Float:PosZ, Float:dist;
     
	GetPlayerPos(playerid, PosX, PosY, PosZ);
 	for (new player_object = 0; player_object <= MAX_VISIBLE_OBJECTS; player_object++)
 	{
    	if(IsValidPlayerObject(playerid, player_object))
     	{
			object_id = Streamer_GetItemStreamerID(playerid, STREAMER_TYPE_OBJECT, player_object);

        	if(GetObjectModel(object_id) == OBJECT_BUSSTOP)
       		{
    			Streamer_GetDistanceToItem(PosX, PosY, PosZ, STREAMER_TYPE_OBJECT, object_id, dist);
    			if((dist < prevdist))
    			{
        			prevdist = dist;
     				OID = object_id;
    			}
         	}
    	}
	}
	return OID;
}

stock GetItemObject(playerid, itemuid)
{
	new object_id,
    	Float:prevdist = 5.0, OID = INVALID_OBJECT_ID, Float:PosX, Float:PosY, Float:PosZ, Float:dist;
     
	GetPlayerPos(playerid, PosX, PosY, PosZ);
 	for (new player_object = 0; player_object <= MAX_VISIBLE_OBJECTS; player_object++)
 	{
    	if(IsValidPlayerObject(playerid, player_object))
     	{
			object_id = Streamer_GetItemStreamerID(playerid, STREAMER_TYPE_OBJECT, player_object);

        	if(Streamer_GetIntData(STREAMER_TYPE_OBJECT, object_id, E_STREAMER_EXTRA_ID) == itemuid*-1)
       		{
    			Streamer_GetDistanceToItem(PosX, PosY, PosZ, STREAMER_TYPE_OBJECT, object_id, dist);
    			if((dist < prevdist))
    			{
        			prevdist = dist;
     				OID = object_id;
    			}
         	}
    	}
	}
	return OID;
}

stock GetClosestZeroGate(playerid)
{
	new object_id,
    	Float:prevdist = 15.0, OID = INVALID_OBJECT_ID, Float:PosX, Float:PosY, Float:PosZ, Float:dist;
     
	GetPlayerPos(playerid, PosX, PosY, PosZ);
 	for (new player_object = 0; player_object <= MAX_VISIBLE_OBJECTS; player_object++)
 	{
    	if(IsValidPlayerObject(playerid, player_object))
     	{
			object_id = Streamer_GetItemStreamerID(playerid, STREAMER_TYPE_OBJECT, player_object);

        	if(Object[object_id][object_gate] == 1)
       		{
    			Streamer_GetDistanceToItem(PosX, PosY, PosZ, STREAMER_TYPE_OBJECT, object_id, dist);
    			if((dist < prevdist))
    			{
        			prevdist = dist;
     				OID = object_id;
    			}
         	}
    	}
	}
	return OID;
}

stock LoadObject(limit[] = "", bool:return_id = false)
{
	new rows, fields;
	mysql_query(mySQLconnection, sprintf("SELECT * FROM ipb_objects %s", limit));  
	cache_get_data(rows, fields);

	new oid;
	
	for(new row = 0; row != rows; row++)
	{
		new model = cache_get_row_int(row, 6),
			vw = cache_get_row_int(row, 7),
			owner = cache_get_row_int(row, 3),
			owner_type = cache_get_row_int(row, 2);
			
		if( owner_type == 0 ) continue;
			
		oid = CreateDynamicObject(model, cache_get_row_float(row, 8), cache_get_row_float(row, 9), cache_get_row_float(row, 10), cache_get_row_float(row, 11), cache_get_row_float(row, 12), cache_get_row_float(row, 13), vw, -1, -1, 200.0);
		
		for(new z=0; e_objects:z != e_objects; z++)
		{
			Object[oid][e_objects:z] = 0;
		}
		
		Object[oid][object_uid] = cache_get_row_int(row, 0);
		Object[oid][object_owner_type] = owner_type;
		Object[oid][object_owner] = owner;
		
		switch( Object[oid][object_owner_type] )
		{
			case OBJECT_OWNER_TYPE_AREA:
			{
				new a_id = GetAreaByUid(owner);
				if( a_id == -1 )
				{
					DeleteObject(oid, false);
					continue;
				}
			}
			
			case OBJECT_OWNER_TYPE_DOOR:
			{
				if(cache_get_row_int(row, 1) != OBJECT_TYPE_ROBBERY)
				{
					new d_id = GetDoorByUid(owner);
					if( d_id == -1 )
					{
						DeleteObject(oid, false);
						continue;
					}
				}
			}
		}

		Streamer_SetIntData(STREAMER_TYPE_OBJECT, oid, E_STREAMER_EXTRA_ID, Object[oid][object_uid]);

		Object[oid][object_vw] = vw;
		Object[oid][object_model] = model;
		Object[oid][object_stream_distance] = 300.0;
		Object[oid][object_type] = cache_get_row_int(row, 1);
		Object[oid][object_gate_opened] = false;
		Object[oid][object_gate] = cache_get_row_int(row, 20);

		Object[oid][object_gate_pos][0] = cache_get_row_float(row, 14);
		Object[oid][object_gate_pos][1] = cache_get_row_float(row, 15);
		Object[oid][object_gate_pos][2] = cache_get_row_float(row, 16);
		Object[oid][object_gate_pos][3] = cache_get_row_float(row, 17);
		Object[oid][object_gate_pos][4] = cache_get_row_float(row, 18);
		Object[oid][object_gate_pos][5] = cache_get_row_float(row, 19);

		Object[oid][object_pos][0] = cache_get_row_float(row, 8);
		Object[oid][object_pos][1] = cache_get_row_float(row, 9);
		Object[oid][object_pos][2] = cache_get_row_float(row, 10);

		Object[oid][object_pos][3] = cache_get_row_float(row, 11);
		Object[oid][object_pos][4] = cache_get_row_float(row, 12);
		Object[oid][object_pos][5] = cache_get_row_float(row, 13);

		Object[oid][object_textured] = cache_get_row_int(row, 24);

		Object[oid][object_logs] = 5;

		Iter_Add(Objects, oid);

		if(Object[oid][object_model] == OBJECT_BUSSTOP)
		{
			Object[oid][object_label] = CreateDynamic3DTextLabel("[[ Aby skorzystaæ z autobusu, wpisz /bus ]]", COLOR_WHITE, Object[oid][object_pos][0], Object[oid][object_pos][1], Object[oid][object_pos][2], 10.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 0, 0, 0);
		}
	}
	
	LoadMaterial(sprintf("WHERE `material_owner` = %d", Object[oid][object_uid]));
	
	if( return_id ) return oid;
	return rows;
}

stock DeleteObject(o_id, bool:from_db = true)
{
	if( from_db )
	{ 
		mysql_query(mySQLconnection, sprintf("DELETE FROM `ipb_objects` WHERE `object_uid` = %d", Object[o_id][object_uid]));
		mysql_query(mySQLconnection, sprintf("DELETE FROM `ipb_materials` WHERE `material_owner` = %d", Object[o_id][object_uid]));
		
		if(Object[o_id][object_textured] > 0)
		{
			mysql_query(mySQLconnection, sprintf("DELETE FROM ipb_materials WHERE material_owner = %d", Object[o_id][object_uid]));
		}
	}

	if(IsValidDynamic3DTextLabel(Object[o_id][object_label]))
	{
		DestroyDynamic3DTextLabel(Object[o_id][object_label]);
	}

	DestroyDynamicObject(o_id);
	Iter_Remove(Objects, o_id);

	for(new z=0; e_objects:z != e_objects; z++)
	{
  		Object[o_id][e_objects:z] = 0;
    }
 	return 1;
}

stock IsValidBlockadeObject(o_id)
{
	if(o_id == INVALID_OBJECT_ID) return 0;
	if(Object[o_id][object_type] != OBJECT_TYPE_BLOCKADE) return 0;

	switch(Object[o_id][object_model])
	{
		case 19834: return 1;
		case 1949: return 1;
		case 1427: return 1;
		case 1424: return 1;
		case 1423: return 1;
		case 1459: return 1;
		case 1237: return 1;
		case 3091: return 1;
		case 3578: return 1;
		case 1425: return 1;
		case 18728: return 1;
		case 1997: return 1;
	}

	return 0;
}

stock IsValidBlockadeModel(model)
{
	switch(model)
	{
		case 19834: return 1;
		case 1949: return 1;
		case 1427: return 1;
		case 1424: return 1;
		case 1423: return 1;
		case 1459: return 1;
		case 1237: return 1;
		case 3091: return 1;
		case 3578: return 1;
		case 1425: return 1;
		case 18728: return 1;
		case 1997: return 1;
	}
	return 0;
}

stock IsObjectEdited(o_id)
{
	if( Object[o_id][object_is_edited] )
	{
		foreach(new p : Player)
		{
			if( pInfo[p][player_edited_object] == o_id ) return 1;
		}
	}
	
	return 0;
}

stock GetObjectDataForPlayer(playerid, &owner, &owner_type)
{
	if( GetPlayerVirtualWorld(playerid) == 0 )
	{
		new a_id = GetPlayerArea(playerid, AREA_TYPE_NORMAL);
		if( a_id != -1 )
		{
			if( CanPlayerEditArea(playerid, a_id) )
			{
				owner = Area[a_id][area_uid];
				owner_type = OBJECT_OWNER_TYPE_AREA;
			}
		}
		else if( HasCrewFlag(playerid, CREW_FLAG_EDITOR) )
		{
			owner = 0;
			owner_type = OBJECT_OWNER_TYPE_GLOBAL;
			
			return 1;
		}
	}
	else
	{
		new d_id = GetDoorByUid(GetPlayerVirtualWorld(playerid));
		
		if( d_id == -1 ) return 1;
		if( Door[d_id][door_type] != DOOR_TYPE_NORMAL ) return 1;
		
		if( HasCrewFlag(playerid, CREW_FLAG_EDITOR) )
		{
			owner = Door[d_id][door_uid];
			owner_type = OBJECT_OWNER_TYPE_DOOR;
			
			return 1;
		}
		
		if( CanPlayerEditDoor(playerid, d_id) )
		{
			owner = Door[d_id][door_uid];
			owner_type = OBJECT_OWNER_TYPE_DOOR;
			
			return 1;
		}
	}
	return 1;
}

stock CanPlayerEditObject(playerid, o_id)
{
	if( HasCrewFlag(playerid, CREW_FLAG_EDITOR) ) return 1;
	
	switch( Object[o_id][object_owner_type] )
	{	
		case OBJECT_OWNER_TYPE_GLOBAL:
		{
			if(IsValidBlockadeObject(o_id) )
			{
				new gid = pInfo[playerid][player_duty_gid];
				if(gid == -1) return 0;
				if(Group[gid][group_flags] & GROUP_FLAG_BLOCKADE) return 1;
			}
		}

		case OBJECT_OWNER_TYPE_DOOR:
		{
			if( CanPlayerEditDoor(playerid, GetDoorByUid(Object[o_id][object_owner])) ) return 1;
		}
		
		case OBJECT_OWNER_TYPE_AREA:
		{
			if( CanPlayerEditArea(playerid, GetAreaByUid(Object[o_id][object_owner])) ) return 1;
		}
	}
	return 0;
}

stock CanPlayerUseObject(playerid, o_id)
{
	if( HasCrewFlag(playerid, CREW_FLAG_EDITOR) ) return 1;
	
	switch( Object[o_id][object_owner_type] )
	{	
		case OBJECT_OWNER_TYPE_DOOR:
		{
			if( CanPlayerUseDoor(playerid, GetDoorByUid(Object[o_id][object_owner])) ) return 1;
		}
		
		case OBJECT_OWNER_TYPE_AREA:
		{
			new areaa = GetAreaByUid(Object[o_id][object_owner]);
			if (areaa == -1) return 0;

			if(Area[areaa][area_owner_type] == AREA_OWNER_TYPE_GROUP)
			{
				new gid = GetGroupByUid(Area[areaa][area_owner]);
				if(gid == -1) return 0;

				if( GetPlayerGroupSlot(playerid, gid) != -1 ) return 1;
			}
		}
	}
	return 0;
}

stock Float:GetPlayerDistanceToObject(playerid, object_id)
{
	if( !IsValidDynamicObject(object_id) ) return 0.0;
	
	new Float:p_pos[3], Float:distance;
	GetPlayerPos(playerid, p_pos[0], p_pos[1], p_pos[2]);
	
	Streamer_GetDistanceToItem(p_pos[0], p_pos[1], p_pos[2], STREAMER_TYPE_OBJECT, object_id, distance);
	return distance;
}

stock UpdateObjectInfoTextdraw(playerid, o_id)
{
	new string[200];

	switch( Object[o_id][object_owner_type] )
	{
		case OBJECT_OWNER_TYPE_GLOBAL: format(string, sizeof(string), "%d. Obiekt globalny (ID: %d)", Object[o_id][object_uid], o_id);
		case OBJECT_OWNER_TYPE_DOOR: format(string, sizeof(string), "%d. Obiekt drzwi (ID: %d)", Object[o_id][object_uid], o_id);
		case OBJECT_OWNER_TYPE_AREA: format(string, sizeof(string), "%d. Obiekt strefowy (ID: %d)", Object[o_id][object_uid], o_id);
	}

	format(string, sizeof(string), "%s~n~~n~~p~Owner: ~w~%d:%d    ~b~Typ: ~w~%d    Model: %d", string, Object[o_id][object_owner_type], Object[o_id][object_owner], Object[o_id][object_type], Object[o_id][object_model]);	
	format(string, sizeof(string), "%s~n~~r~Pozycja: ~w~%.2f, %.2f, %.2f~n~~r~Rotacja: ~w~%.2f, %.2f, %.2f", string, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2], Object[o_id][object_pos][3], Object[o_id][object_pos][4], Object[o_id][object_pos][5]);

	TextDrawSetString(Dashboard[playerid], string);
}

stock RotateCustomObject(playerid)
{
	new keysa, uda, lra;
	GetPlayerKeys(playerid, keysa, uda, lra);

	new o_id = pInfo[playerid][player_edited_object];
	new rot = GetPlayerEditorAngle(playerid);

	switch(rot)
	{
		case 270:
		{
			if(lra > 0)
			{
				if(keysa == 1056)
				{
					Object[o_id][object_pos][5] += 1.0;

					if(Object[o_id][object_pos][5] > 360 && Object[o_id][object_pos][5] <= -360)
					{
						Object[o_id][object_pos][5] = 0.0;
					}
					SetDynamicObjectRot(o_id, Object[o_id][object_pos][3], Object[o_id][object_pos][4], Object[o_id][object_pos][5]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

				if(keysa == 40)
				{
					Object[o_id][object_pos][5] += 10.0;

					if(Object[o_id][object_pos][5] > 360 && Object[o_id][object_pos][5] <= -360)
					{
						Object[o_id][object_pos][5] = 0.0;
					}
					SetDynamicObjectRot(o_id, Object[o_id][object_pos][3], Object[o_id][object_pos][4], Object[o_id][object_pos][5]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}	

			    if(keysa == KEY_SPRINT)
			    {
		   			Object[o_id][object_pos][1] -= 1.0;
					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

		 		if(keysa == 1024)
			    {
		   			Object[o_id][object_pos][1] -= 0.01;
					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

				if(keysa == KEY_JUMP)
				{
					Object[o_id][object_pos][5] += 5.0;

					if(Object[o_id][object_pos][5] > 360 && Object[o_id][object_pos][5] <= -360)
					{
						Object[o_id][object_pos][5] = 0.0;
					}
					SetDynamicObjectRot(o_id, Object[o_id][object_pos][3], Object[o_id][object_pos][4], Object[o_id][object_pos][5]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

				Object[o_id][object_pos][1] -= 0.1;
				SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
				UpdateObjectInfoTextdraw(playerid, o_id);
				TextDrawShowForPlayer(playerid, Dashboard[playerid]);
			}

			if(lra < 0)
			{
				if(keysa == 1056)
				{
					Object[o_id][object_pos][5] -= 1.0;

					if(Object[o_id][object_pos][5] > 360 && Object[o_id][object_pos][5] <= -360)
					{
						Object[o_id][object_pos][5] = 0.0;
					}
					SetDynamicObjectRot(o_id, Object[o_id][object_pos][3], Object[o_id][object_pos][4], Object[o_id][object_pos][5]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

				if(keysa == 40)
				{
					Object[o_id][object_pos][5] -= 10.0;

					if(Object[o_id][object_pos][5] > 360 && Object[o_id][object_pos][5] <= -360)
					{
						Object[o_id][object_pos][5] = 0.0;
					}
					SetDynamicObjectRot(o_id, Object[o_id][object_pos][3], Object[o_id][object_pos][4], Object[o_id][object_pos][5]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

			    if(keysa == KEY_SPRINT)
			    {
		   			Object[o_id][object_pos][1] += 1.0;
					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

		 		if(keysa == 1024)
			    {
		   			Object[o_id][object_pos][1] += 0.01;
					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

				if(keysa == KEY_JUMP)
				{
					Object[o_id][object_pos][5] -= 5.0;
					if(Object[o_id][object_pos][5] > 360 && Object[o_id][object_pos][5] <= -360)
					{
						Object[o_id][object_pos][5] = 0.0;
					}
					SetDynamicObjectRot(o_id, Object[o_id][object_pos][3], Object[o_id][object_pos][4], Object[o_id][object_pos][5]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

				Object[o_id][object_pos][1] += 0.1;
				SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1],Object[o_id][object_pos][2]);
				UpdateObjectInfoTextdraw(playerid, o_id);
				TextDrawShowForPlayer(playerid, Dashboard[playerid]);
			}

			if(uda > 0)
			{
				if(keysa == 1056)
				{
					Object[o_id][object_pos][2] -= 0.01;
					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

				if(keysa == 40)
				{
					new Float:down, Float:temp;
					GetDynamicObjectPos(o_id, temp, temp, down);

					Object[o_id][object_pos][2] = down-1;

					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}
				
			    if(keysa == KEY_SPRINT)
			    {
		   			Object[o_id][object_pos][0] -= 1.0;
					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

		 		if(keysa == KEY_JUMP)
			    {
		   			Object[o_id][object_pos][2] -= 0.1;
					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

		 		if(keysa == 1024)
			    {
		   			Object[o_id][object_pos][0] -= 0.01;
					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

				Object[o_id][object_pos][0] -= 0.1;
				SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
				UpdateObjectInfoTextdraw(playerid, o_id);
				TextDrawShowForPlayer(playerid, Dashboard[playerid]);
			}

			if(uda < 0)
			{
				if(keysa == 1056)
				{
					Object[o_id][object_pos][2] += 0.01;
					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

				if(keysa == 40)
				{
					Object[o_id][object_pos][2] += 1.0;
					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

			    if(keysa == KEY_SPRINT)
			    {
		   			Object[o_id][object_pos][0] += 1.0;
					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

			    if(keysa == KEY_JUMP)
			    {
		   			Object[o_id][object_pos][2] += 0.1;
					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

		 		if(keysa == 1024)
			    {
		   			Object[o_id][object_pos][0] += 0.01;
					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

				Object[o_id][object_pos][0] += 0.1;
				SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
				UpdateObjectInfoTextdraw(playerid, o_id);
				TextDrawShowForPlayer(playerid, Dashboard[playerid]);
			}
		}
		case 90:
		{
			if(lra > 0)
			{
				if(keysa == 1056)
				{
					Object[o_id][object_pos][5] += 1.0;

					if(Object[o_id][object_pos][5] > 360 && Object[o_id][object_pos][5] <= -360)
					{
						Object[o_id][object_pos][5] = 0.0;
					}
					SetDynamicObjectRot(o_id, Object[o_id][object_pos][3], Object[o_id][object_pos][4], Object[o_id][object_pos][5]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

				if(keysa == 40)
				{
					Object[o_id][object_pos][5] += 10.0;

					if(Object[o_id][object_pos][5] > 360 && Object[o_id][object_pos][5] <= -360)
					{
						Object[o_id][object_pos][5] = 0.0;
					}
					SetDynamicObjectRot(o_id, Object[o_id][object_pos][3], Object[o_id][object_pos][4], Object[o_id][object_pos][5]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

			    if(keysa == KEY_SPRINT)
			    {
		   			Object[o_id][object_pos][1] += 1.0;
					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

		 		if(keysa == 1024)
			    {
		   			Object[o_id][object_pos][1] += 0.01;
					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

				if(keysa == KEY_JUMP)
				{
					Object[o_id][object_pos][5] += 5.0;

					if(Object[o_id][object_pos][5]> 360 && Object[o_id][object_pos][5] <= -360)
					{
						Object[o_id][object_pos][5] = 0.0;
					}
					SetDynamicObjectRot(o_id, Object[o_id][object_pos][3], Object[o_id][object_pos][4], Object[o_id][object_pos][5]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

				Object[o_id][object_pos][1] += 0.1;
				SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
				UpdateObjectInfoTextdraw(playerid, o_id);
				TextDrawShowForPlayer(playerid, Dashboard[playerid]);
			}

			if(lra < 0)
			{
				if(keysa == 1056)
				{
					Object[o_id][object_pos][5] -= 1.0;

					if(Object[o_id][object_pos][5] > 360 && Object[o_id][object_pos][5] <= -360)
					{
						Object[o_id][object_pos][5] = 0.0;
					}
					SetDynamicObjectRot(o_id, Object[o_id][object_pos][3], Object[o_id][object_pos][4], Object[o_id][object_pos][5]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

				if(keysa == 40)
				{
					Object[o_id][object_pos][5] -= 10.0;

					if(Object[o_id][object_pos][5] > 360 && Object[o_id][object_pos][5] <= -360)
					{
						Object[o_id][object_pos][5] = 0.0;
					}
					SetDynamicObjectRot(o_id, Object[o_id][object_pos][3], Object[o_id][object_pos][4], Object[o_id][object_pos][5]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

			    if(keysa == KEY_SPRINT)
			    {
		   			Object[o_id][object_pos][1] -= 1.0;
					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

		 		if(keysa == 1024)
			    {
		   			Object[o_id][object_pos][1] -= 0.01;
					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

				if(keysa == KEY_JUMP)
				{
					Object[o_id][object_pos][5] -= 5.0;
					if(Object[o_id][object_pos][5] > 360 && Object[o_id][object_pos][5] <= -360)
					{
						Object[o_id][object_pos][5] = 0.0;
					}
					SetDynamicObjectRot(o_id, Object[o_id][object_pos][3], Object[o_id][object_pos][4], Object[o_id][object_pos][5]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

				Object[o_id][object_pos][1] -= 0.1;
				SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1],Object[o_id][object_pos][2]);
				UpdateObjectInfoTextdraw(playerid, o_id);
				TextDrawShowForPlayer(playerid, Dashboard[playerid]);
			}

			if(uda > 0)
			{
				if(keysa == 1056)
				{
					Object[o_id][object_pos][2] -= 0.01;
					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

				if(keysa == 40)
				{
					new Float:down, Float:temp;
					GetDynamicObjectPos(o_id, temp, temp, down);

					Object[o_id][object_pos][2] = down-1;

					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

			    if(keysa == KEY_SPRINT)
			    {
		   			Object[o_id][object_pos][0] += 1.0;
					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

		 		if(keysa == KEY_JUMP)
			    {
		   			Object[o_id][object_pos][2] -= 0.1;
					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

		 		if(keysa == 1024)
			    {
		   			Object[o_id][object_pos][0] += 0.01;
					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

				Object[o_id][object_pos][0] += 0.1;
				SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
				UpdateObjectInfoTextdraw(playerid, o_id);
				TextDrawShowForPlayer(playerid, Dashboard[playerid]);
			}

			if(uda < 0)
			{
				if(keysa == 1056)
				{
					Object[o_id][object_pos][2] += 0.01;
					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

				if(keysa == 40)
				{
					Object[o_id][object_pos][2] += 1.0;
					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

			    if(keysa == KEY_SPRINT)
			    {
		   			Object[o_id][object_pos][0] -= 1.0;
					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

			    if(keysa == KEY_JUMP)
			    {
		   			Object[o_id][object_pos][2] += 0.1;
					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

		 		if(keysa == 1024)
			    {
		   			Object[o_id][object_pos][0] -= 0.01;
					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

				Object[o_id][object_pos][0] -= 0.1;
				SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
				UpdateObjectInfoTextdraw(playerid, o_id);
				TextDrawShowForPlayer(playerid, Dashboard[playerid]);
			}
		}

		case 180:
		{
			if(lra > 0)
			{
				if(keysa == 1056)
				{
					Object[o_id][object_pos][5] += 1.0;

					if(Object[o_id][object_pos][5] > 360 && Object[o_id][object_pos][5] <= -360)
					{
						Object[o_id][object_pos][5] = 0.0;
					}
					SetDynamicObjectRot(o_id, Object[o_id][object_pos][3], Object[o_id][object_pos][4], Object[o_id][object_pos][5]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

				if(keysa == 40)
				{
					Object[o_id][object_pos][5] += 10.0;

					if(Object[o_id][object_pos][5] > 360 && Object[o_id][object_pos][5] <= -360)
					{
						Object[o_id][object_pos][5] = 0.0;
					}
					SetDynamicObjectRot(o_id, Object[o_id][object_pos][3], Object[o_id][object_pos][4], Object[o_id][object_pos][5]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

			    if(keysa == KEY_SPRINT)
			    {
		   			Object[o_id][object_pos][0] -= 1.0;
					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

		 		if(keysa == 1024)
			    {
		   			Object[o_id][object_pos][0] -= 0.01;
					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

				if(keysa == KEY_JUMP)
				{
					Object[o_id][object_pos][5] += 5.0;

					if(Object[o_id][object_pos][5]> 360 && Object[o_id][object_pos][5] <= -360)
					{
						Object[o_id][object_pos][5] = 0.0;
					}
					SetDynamicObjectRot(o_id, Object[o_id][object_pos][3], Object[o_id][object_pos][4], Object[o_id][object_pos][5]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

				Object[o_id][object_pos][0] -= 0.1;
				SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
				UpdateObjectInfoTextdraw(playerid, o_id);
				TextDrawShowForPlayer(playerid, Dashboard[playerid]);
			}

			if(lra < 0)
			{
				if(keysa == 1056)
				{
					Object[o_id][object_pos][5] -= 1.0;

					if(Object[o_id][object_pos][5] > 360 && Object[o_id][object_pos][5] <= -360)
					{
						Object[o_id][object_pos][5] = 0.0;
					}
					SetDynamicObjectRot(o_id, Object[o_id][object_pos][3], Object[o_id][object_pos][4], Object[o_id][object_pos][5]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

				if(keysa == 40)
				{
					Object[o_id][object_pos][5] -= 10.0;

					if(Object[o_id][object_pos][5] > 360 && Object[o_id][object_pos][5] <= -360)
					{
						Object[o_id][object_pos][5] = 0.0;
					}
					SetDynamicObjectRot(o_id, Object[o_id][object_pos][3], Object[o_id][object_pos][4], Object[o_id][object_pos][5]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

			    if(keysa == KEY_SPRINT)
			    {
		   			Object[o_id][object_pos][0] += 1.0;
					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

		 		if(keysa == 1024)
			    {
		   			Object[o_id][object_pos][0] += 0.01;
					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

				if(keysa == KEY_JUMP)
				{
					Object[o_id][object_pos][5] -= 5.0;
					if(Object[o_id][object_pos][5] > 360 && Object[o_id][object_pos][5] <= -360)
					{
						Object[o_id][object_pos][5] = 0.0;
					}
					SetDynamicObjectRot(o_id, Object[o_id][object_pos][3], Object[o_id][object_pos][4], Object[o_id][object_pos][5]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

				Object[o_id][object_pos][0] += 0.1;
				SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1],Object[o_id][object_pos][2]);
				UpdateObjectInfoTextdraw(playerid, o_id);
				TextDrawShowForPlayer(playerid, Dashboard[playerid]);
			}

			if(uda > 0)
			{
				if(keysa == 1056)
				{
					Object[o_id][object_pos][2] -= 0.01;
					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

				if(keysa == 40)
				{
					new Float:down, Float:temp;
					GetDynamicObjectPos(o_id, temp, temp, down);

					Object[o_id][object_pos][2] = down-1;

					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

			    if(keysa == KEY_SPRINT)
			    {
		   			Object[o_id][object_pos][1] += 1.0;
					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

		 		if(keysa == KEY_JUMP)
			    {
		   			Object[o_id][object_pos][2] -= 0.1;
					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

		 		if(keysa == 1024)
			    {
		   			Object[o_id][object_pos][1] += 0.01;
					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

				Object[o_id][object_pos][1] += 0.1;
				SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
				UpdateObjectInfoTextdraw(playerid, o_id);
				TextDrawShowForPlayer(playerid, Dashboard[playerid]);
			}

			if(uda < 0)
			{
				if(keysa == 1056)
				{
					Object[o_id][object_pos][2] += 0.01;
					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

				if(keysa == 40)
				{
					Object[o_id][object_pos][2] += 1.0;
					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

			    if(keysa == KEY_SPRINT)
			    {
		   			Object[o_id][object_pos][1] -= 1.0;
					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

			    if(keysa == KEY_JUMP)
			    {
		   			Object[o_id][object_pos][2] += 0.1;
					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

		 		if(keysa == 1024)
			    {
		   			Object[o_id][object_pos][1] -= 0.01;
					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

				Object[o_id][object_pos][1] -= 0.1;
				SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
				UpdateObjectInfoTextdraw(playerid, o_id);
				TextDrawShowForPlayer(playerid, Dashboard[playerid]);
			}
		}

		case 0:
		{
			if(lra > 0)
			{
				if(keysa == 1056)
				{
					Object[o_id][object_pos][5] += 1.0;

					if(Object[o_id][object_pos][5] > 360 && Object[o_id][object_pos][5] <= -360)
					{
						Object[o_id][object_pos][5] = 0.0;
					}
					SetDynamicObjectRot(o_id, Object[o_id][object_pos][3], Object[o_id][object_pos][4], Object[o_id][object_pos][5]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

				if(keysa == 40)
				{
					Object[o_id][object_pos][5] += 10.0;

					if(Object[o_id][object_pos][5] > 360 && Object[o_id][object_pos][5] <= -360)
					{
						Object[o_id][object_pos][5] = 0.0;
					}
					SetDynamicObjectRot(o_id, Object[o_id][object_pos][3], Object[o_id][object_pos][4], Object[o_id][object_pos][5]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

			    if(keysa == KEY_SPRINT)
			    {
		   			Object[o_id][object_pos][0] += 1.0;
					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

		 		if(keysa == 1024)
			    {
		   			Object[o_id][object_pos][0] += 0.01;
					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

				if(keysa == KEY_JUMP)
				{
					Object[o_id][object_pos][5] += 5.0;

					if(Object[o_id][object_pos][5]> 360 && Object[o_id][object_pos][5] <= -360)
					{
						Object[o_id][object_pos][5] = 0.0;
					}
					SetDynamicObjectRot(o_id, Object[o_id][object_pos][3], Object[o_id][object_pos][4], Object[o_id][object_pos][5]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

				Object[o_id][object_pos][0] += 0.1;
				SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
				UpdateObjectInfoTextdraw(playerid, o_id);
				TextDrawShowForPlayer(playerid, Dashboard[playerid]);
			}

			if(lra < 0)
			{
				if(keysa == 1056)
				{
					Object[o_id][object_pos][5] -= 1.0;

					if(Object[o_id][object_pos][5] > 360 && Object[o_id][object_pos][5] <= -360)
					{
						Object[o_id][object_pos][5] = 0.0;
					}
					SetDynamicObjectRot(o_id, Object[o_id][object_pos][3], Object[o_id][object_pos][4], Object[o_id][object_pos][5]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

				if(keysa == 40)
				{
					Object[o_id][object_pos][5] -= 10.0;

					if(Object[o_id][object_pos][5] > 360 && Object[o_id][object_pos][5] <= -360)
					{
						Object[o_id][object_pos][5] = 0.0;
					}
					SetDynamicObjectRot(o_id, Object[o_id][object_pos][3], Object[o_id][object_pos][4], Object[o_id][object_pos][5]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

			    if(keysa == KEY_SPRINT)
			    {
		   			Object[o_id][object_pos][0] -= 1.0;
					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

		 		if(keysa == 1024)
			    {
		   			Object[o_id][object_pos][0] -= 0.01;
					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

				if(keysa == KEY_JUMP)
				{
					Object[o_id][object_pos][5] -= 5.0;
					if(Object[o_id][object_pos][5] > 360 && Object[o_id][object_pos][5] <= -360)
					{
						Object[o_id][object_pos][5] = 0.0;
					}
					SetDynamicObjectRot(o_id, Object[o_id][object_pos][3], Object[o_id][object_pos][4], Object[o_id][object_pos][5]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

				Object[o_id][object_pos][0] -= 0.1;
				SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1],Object[o_id][object_pos][2]);
				UpdateObjectInfoTextdraw(playerid, o_id);
				TextDrawShowForPlayer(playerid, Dashboard[playerid]);
			}

			if(uda > 0)
			{
				if(keysa == 1056)
				{
					Object[o_id][object_pos][2] -= 0.01;
					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

				if(keysa == 40)
				{
					new Float:down, Float:temp;
					GetDynamicObjectPos(o_id, temp, temp, down);

					Object[o_id][object_pos][2] = down-1;

					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

			    if(keysa == KEY_SPRINT)
			    {
		   			Object[o_id][object_pos][1] -= 1.0;
					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

		 		if(keysa == KEY_JUMP)
			    {
		   			Object[o_id][object_pos][2] -= 0.1;
					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

		 		if(keysa == 1024)
			    {
		   			Object[o_id][object_pos][1] -= 0.01;
					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

				Object[o_id][object_pos][1] -= 0.1;
				SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
				UpdateObjectInfoTextdraw(playerid, o_id);
				TextDrawShowForPlayer(playerid, Dashboard[playerid]);
			}

			if(uda < 0)
			{
				if(keysa == 1056)
				{
					Object[o_id][object_pos][2] += 0.01;
					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

				if(keysa == 40)
				{
					Object[o_id][object_pos][2] += 1.0;
					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

			    if(keysa == KEY_SPRINT)
			    {
		   			Object[o_id][object_pos][1] += 1.0;
					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

			    if(keysa == KEY_JUMP)
			    {
		   			Object[o_id][object_pos][2] += 0.1;
					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

		 		if(keysa == 1024)
			    {
		   			Object[o_id][object_pos][1] += 0.01;
					SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
					UpdateObjectInfoTextdraw(playerid, o_id);
					TextDrawShowForPlayer(playerid, Dashboard[playerid]);
					return 1;
				}

				Object[o_id][object_pos][1] += 0.1;
				SetDynamicObjectPos(o_id, Object[o_id][object_pos][0], Object[o_id][object_pos][1], Object[o_id][object_pos][2]);
				UpdateObjectInfoTextdraw(playerid, o_id);
				TextDrawShowForPlayer(playerid, Dashboard[playerid]);
			}
		}
	}
	return 1;
}

stock GetObjectByUid(uid)
{
	foreach(new o_id : Objects)
	{
		if( Object[o_id][object_uid] == uid ) return o_id;
	}
	return -1;
}

stock RemoveBuildingsForPlayer(playerid)
{
	//Dystrybutory na stacji idlewood
	RemoveBuildingForPlayer(playerid, 1676, 1941.6563, -1778.4531, 14.1406, 0.25);
	RemoveBuildingForPlayer(playerid, 1676, 1941.6563, -1774.3125, 14.1406, 0.25);
	RemoveBuildingForPlayer(playerid, 1676, 1941.6563, -1771.3438, 14.1406, 0.25);
	RemoveBuildingForPlayer(playerid, 1676, 1941.6563, -1767.2891, 14.1406, 0.25);
}
