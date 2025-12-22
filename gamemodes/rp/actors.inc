stock LoadActors()
{
	Code_ExTimer_Begin(LoadActor);
	
	new loaded = LoadActor();
	
	ArmDealer = CreateActor(6, 0, 0, 0, 353.0);
	Iter_Add(Actors, ArmDealer);
	Actor[ArmDealer][actor_type] = ACTOR_TYPE_ARMDEALER;	

	printf("Wczytano aktorow [wczytanych: %d] [czas: %d ms]", loaded, Code_ExTimer_End(LoadActor));
}

stock GetClosestActorType(playerid, a_type)
{
	new Float:x, Float:y, Float:z;

	foreach(new a_id: Actors)
	{
		if(Actor[a_id][actor_type] == a_type )
		{
			GetActorPos(a_id, x, y, z);
			if(IsPlayerInRangeOfPoint(playerid, 3.0, x, y, z)) return a_id;
		}
	}

	return INVALID_ACTOR_ID;
}

stock LoadActor(limit[] = "", bool:return_id = false)
{
	new rows, fields;
	mysql_query(mySQLconnection, sprintf("SELECT * FROM ipb_actors %s", limit));  
	cache_get_data(rows, fields);
	
	new a_id;
	
	for(new row = 0; row != rows; row++)
	{
		a_id = CreateActor(cache_get_row_int(row, 1), cache_get_row_float(row, 2), cache_get_row_float(row, 3), cache_get_row_float(row, 4), cache_get_row_float(row, 5));
		
		Iter_Add(Actors, a_id);	
		
		Actor[a_id][actor_uid] = cache_get_row_int(row, 0);
		Actor[a_id][actor_skin] = cache_get_row_int(row, 1);

		Actor[a_id][actor_pos][0] = cache_get_row_float(row, 2);
		Actor[a_id][actor_pos][1] = cache_get_row_float(row, 3);
		Actor[a_id][actor_pos][2] = cache_get_row_float(row, 4);
		Actor[a_id][actor_pos][3] = cache_get_row_float(row, 5);

		Actor[a_id][actor_type] = cache_get_row_int(row, 6);
		Actor[a_id][actor_animation] = cache_get_row_int(row, 7);
		Actor[a_id][actor_action] = cache_get_row_int(row, 8);
		Actor[a_id][actor_vw] = cache_get_row_int(row, 9);
		Actor[a_id][actor_owner] = cache_get_row_int(row, 10);

		SetActorVirtualWorld(a_id, Actor[a_id][actor_vw]);

		if(Actor[a_id][actor_type] == ACTOR_TYPE_ROBBERY)
		{
			SetActorInvulnerable(a_id, false);
		}

		foreach(new anim_id: Anims)
		{
			if(AnimInfo[anim_id][aUID] == Actor[a_id][actor_animation])
        	{
        	    ApplyActorAnimation(a_id, AnimInfo[anim_id][aLib], AnimInfo[anim_id][aName], AnimInfo[anim_id][aSpeed], bool:AnimInfo[anim_id][aOpt1], bool:AnimInfo[anim_id][aOpt2], bool:AnimInfo[anim_id][aOpt3], bool:AnimInfo[anim_id][aOpt4], AnimInfo[anim_id][aOpt5]);
        	}
		}
	}
	
	if( return_id ) return a_id;
	return rows;
}
