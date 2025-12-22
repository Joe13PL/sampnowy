stock GetRobberyLevel(rid)
{
	new tmp_name[40];

	switch(Robbery[rid][robbery_level])
	{
		case ROBBERY_LEVEL_EASY: format(tmp_name, sizeof(tmp_name), "{D6EE76}£atwy");
		case ROBBERY_LEVEL_MEDIUM: format(tmp_name, sizeof(tmp_name), "{F2E200}Œredni");
		case ROBBERY_LEVEL_HARD: format(tmp_name, sizeof(tmp_name), "{FF3C3C}Trudny");
		case ROBBERY_LEVEL_HARDCORE: format(tmp_name, sizeof(tmp_name), "{AA3333}Hardcore");
	}

	return tmp_name;
}

stock GetRobberyByUid(uid)
{
	foreach(new r_id : Robberies)
	{
		if( Robbery[r_id][robbery_uid] == uid ) return r_id;
	}
	return -1;
}

stock LoadRobberies()
{
	Code_ExTimer_Begin(LoadRobberies);
	new rows, fields;
	mysql_query(mySQLconnection, "SELECT * FROM ipb_robberies ORDER BY robbery_level DESC");  
	cache_get_data(rows, fields);

	for(new row = 0; row != rows; row++)
	{
		new i = Iter_Free(Robberies);

		Robbery[i][robbery_uid] = cache_get_row_int(row, 0);
		Robbery[i][robbery_place] = cache_get_row_int(row, 1);
		cache_get_row(row, 2, Robbery[i][robbery_name], mySQLconnection, 60);
		Robbery[i][robbery_requirements] = cache_get_row_int(row, 3);
		Robbery[i][robbery_level] = cache_get_row_int(row, 4);
		Robbery[i][robbery_timestamp] = cache_get_row_int(row, 5);
		Robbery[i][robbery_actors] = CountRobberyActors(i);
		
		Iter_Add(Robberies, i);
	}
	
	printf("Wczytano napady [wczytanych: %d] [czas: %d ms]", rows, Code_ExTimer_End(LoadRobberies));
}

stock CountRobberyActors(rid)
{
	new count;
	foreach(new a_id: Actors)
	{
		if(Actor[a_id][actor_type] == ACTOR_TYPE_ROBBERY && Actor[a_id][actor_owner] == Robbery[rid][robbery_uid]) count++;
	}

	return count;
}