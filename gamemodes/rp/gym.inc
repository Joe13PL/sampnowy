stock UseGymDumb(playerid)
{
	if(pInfo[playerid][player_gym_carnet] <= 0)
	{
		ApplyAnimation(playerid, "freeweights", "gym_free_putdown", 1, false, false, false, true, 0, 1 );
		SetCameraBehindPlayer(playerid);
		
		RemovePlayerAttachedObject(playerid, ATTACH_SLOT_GYM1);
		RemovePlayerAttachedObject(playerid, ATTACH_SLOT_GYM2);

		pInfo[playerid][player_training] = false;

		Object[pInfo[playerid][player_gym_object]][object_can_be_used] = false;

		SendClientMessage(playerid, COLOR_GOLD, "Czas karnetu dobieg� ko�ca.");
		ClearAnimations(playerid);
		FreezePlayer(playerid, 1000);

		pInfo[playerid][player_gym_carnet] = 0;
		mysql_query(mySQLconnection, sprintf("UPDATE ipb_characters SET char_strength = '%f', char_training = %d WHERE char_uid = %d", pInfo[playerid][player_strength], pInfo[playerid][player_gym_carnet], pInfo[playerid][player_id]));
	}

	pInfo[playerid][player_can_train] = 3;

	// Animka
	switch( random( 2 ) )
	{
		case 0: ApplyAnimation(playerid, "freeweights", "gym_free_A", 1, false, false, false, true, 0, 1 );
		case 1: ApplyAnimation(playerid, "freeweights", "gym_free_B", 1, false, false, false, true, 0, 1 );
	}

	defer ResetDumb[2000](playerid, 1);
}

stock LeaveDumb(playerid)
{
	pInfo[playerid][player_can_train] = 3;
	ApplyAnimation(playerid, "freeweights", "gym_free_down", 1, false, false, false, true, 0, 1);

	// Dodajemy wycisk i sile
	pInfo[playerid][player_gym_count]++;

	if(pInfo[playerid][player_supple] > 0)
	{
		pInfo[playerid][player_strength] += 0.01*pInfo[playerid][player_supple];
	}
	else
	{
		pInfo[playerid][player_strength]+=0.01;
	}

	// Textdraw
	new LocalLabel[170];
	format(LocalLabel,sizeof(LocalLabel),"~w~Wciskaj ~y~strzalki ~u~ ~d~ ~w~ aby wyciskac.~n~~n~Wycisniecia: ~y~%d~n~~w~Sila: ~y~%.2f%%~n~~w~Czas karnetu: ~y~%dmin~n~~n~~w~Uzyj ponownie /trenuj by zakonczyc.", pInfo[playerid][player_gym_count], pInfo[playerid][player_strength], pInfo[playerid][player_gym_carnet]);
	TextDrawSetString(Tutorial[playerid], LocalLabel);
	TextDrawShowForPlayer(playerid, Tutorial[playerid]);

	defer ResetDumb[2000](playerid, 2);
}