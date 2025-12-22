stock CheckTextDrawString(const string[])
{
	for(new znak = 0; znak < strlen(string); znak++)
	{
		if(string[znak] == '~')
		{
			if((string[znak+1] == 'r' || string[znak+1] == 'g' || string[znak+1] == 'b' || string[znak+1] == 'w' || string[znak+1] == 'l' || string[znak+1] == 'p' || string[znak+1] == 'n') && string[znak+2] == '~')
			{
				if(string[znak+2] == '~') znak += 2;
			}
			else return 0;
		}
	}
	return 1;
}

stock DestroyPlayerTextDraws(playerid)
{
	DestroyPlayerProgressBar(playerid, pInfo[playerid][player_health_bar]);
	PlayerTextDrawDestroy(playerid, ZoneName);
	PlayerTextDrawDestroy(playerid, GroupDutyTag);
	PlayerTextDrawDestroy(playerid, WhiteScreen);
	PlayerTextDrawDestroy(playerid, TextDrawSanNews);

	for(new i=0;i<5;i++)
	{
		PlayerTextDrawDestroy(playerid, GroupsListRow[i]);
	}

	for(new i=0;i<5;i++)
	{
		PlayerTextDrawDestroy(playerid, GroupsListStaticButtons[i][0]);
		PlayerTextDrawDestroy(playerid, GroupsListStaticButtons[i][1]);
		PlayerTextDrawDestroy(playerid, GroupsListStaticButtons[i][2]);
		PlayerTextDrawDestroy(playerid, GroupsListStaticButtons[i][3]);
		PlayerTextDrawDestroy(playerid, GroupsListStaticButtons[i][4]);
	}

}

stock CreatePlayerTextdraws(playerid)
{	
	CreateGroupPlayerTextdraws(playerid);

	GroupDutyTag = CreatePlayerTextDraw(playerid, 8.000000, 428.000000, "GOV");
	PlayerTextDrawBackgroundColor(playerid, GroupDutyTag, 255);
	PlayerTextDrawFont(playerid, GroupDutyTag, 1);
	PlayerTextDrawLetterSize(playerid, GroupDutyTag, 0.230000, 0.899999);
	PlayerTextDrawColor(playerid, GroupDutyTag, -1);
	PlayerTextDrawSetOutline(playerid, GroupDutyTag, 1);
	PlayerTextDrawSetProportional(playerid, GroupDutyTag, true);

	ZoneName = CreatePlayerTextDraw(playerid, 500.000000, 98.000000, "_");
	PlayerTextDrawBackgroundColor(playerid, ZoneName, 255);
	PlayerTextDrawFont(playerid, ZoneName, 3);
	PlayerTextDrawLetterSize(playerid, ZoneName, 0.310000, 1.000000);
	PlayerTextDrawColor(playerid, ZoneName, -1);
	PlayerTextDrawSetOutline(playerid, ZoneName, 1);
	PlayerTextDrawSetProportional(playerid, ZoneName, true);

	WhiteScreen = CreatePlayerTextDraw(playerid, -20.000000, 0.000000, "_" ); 
 	PlayerTextDrawUseBox(playerid, WhiteScreen, true );
 	PlayerTextDrawBoxColor(playerid, WhiteScreen, 0xFFFFFFFF );
 	PlayerTextDrawAlignment(playerid, WhiteScreen, 0 );
 	PlayerTextDrawBackgroundColor(playerid, WhiteScreen, 0xFFFFFFFF );
 	PlayerTextDrawFont(playerid, WhiteScreen, 3 );
 	PlayerTextDrawLetterSize(playerid, WhiteScreen, 1.000000, 52.200000 );
 	PlayerTextDrawColor(playerid, WhiteScreen, 0xFFFFFFFF );

    TextDrawSanNews = CreatePlayerTextDraw(playerid, -1.000000,438.000000, "_~w~Uzyj komendy ~y~/radio~w~ aby wybrac radiostacje.");
    PlayerTextDrawUseBox(playerid, TextDrawSanNews,true);
    PlayerTextDrawBoxColor(playerid, TextDrawSanNews,25);
    PlayerTextDrawTextSize(playerid, TextDrawSanNews,640.000000,0.000000);
    PlayerTextDrawAlignment(playerid, TextDrawSanNews,0);
    PlayerTextDrawBackgroundColor(playerid, TextDrawSanNews,0x000000ff);
    PlayerTextDrawFont(playerid, TextDrawSanNews,1);
    PlayerTextDrawLetterSize(playerid, TextDrawSanNews,0.199999,1.000000);
    PlayerTextDrawColor(playerid, TextDrawSanNews,0xffffffff);
    PlayerTextDrawSetOutline(playerid, TextDrawSanNews,1);
    PlayerTextDrawSetProportional(playerid, TextDrawSanNews,true);
    PlayerTextDrawSetShadow(playerid, TextDrawSanNews,1);
}

stock CreateTextdraws()
{	
	CreateGroupStaticTextdraws();

	Textdraw2 = TextDrawCreate(578.000000, 5.000000, "~y~san~w~-andreas.~y~pl~n~~w~20.20.2017");
    TextDrawAlignment(Textdraw2, 2);
    TextDrawBackgroundColor(Textdraw2, 100);
    TextDrawFont(Textdraw2, 3);
    TextDrawLetterSize(Textdraw2, 0.330000, 1.100000);
    TextDrawColor(Textdraw2, -1);
    TextDrawSetOutline(Textdraw2, 1);
    TextDrawSetProportional(Textdraw2, true);

	vehicleInfo = TextDrawCreate(438.000000, 366.000000, "Aby uruchomic silnik, wcisnij ~y~LCTRL~w~.~n~Klawisz ~y~LMB ~w~odpala swiatla.");
	TextDrawBackgroundColor(vehicleInfo, 255);
	TextDrawFont(vehicleInfo, 1);
	TextDrawLetterSize(vehicleInfo, 0.220000, 0.899999);
	TextDrawColor(vehicleInfo, -1);
	TextDrawSetOutline(vehicleInfo, 0);
	TextDrawSetProportional(vehicleInfo, true);
	TextDrawSetShadow(vehicleInfo, 1);
	TextDrawUseBox(vehicleInfo, true);
	TextDrawBoxColor(vehicleInfo, 128);
	TextDrawTextSize(vehicleInfo, 611.000000, 0.000000);

	TextPenalty = TextDrawCreate(12.000000, 287.000000, " ");
	TextDrawBackgroundColor(TextPenalty, 255);
	TextDrawFont(TextPenalty, 1);
	TextDrawLetterSize(TextPenalty,0.280000, 1.200000);
	TextDrawColor(TextPenalty, -1);
	TextDrawSetOutline(TextPenalty, 1);
	TextDrawSetProportional(TextPenalty, true);

	for(new pp;pp<MAX_PLAYERS;pp++)
	{

		DoorInfo[pp] = TextDrawCreate(510.000000, 325.000000, "~n~");
        TextDrawAlignment(DoorInfo[pp], 2);
        TextDrawBackgroundColor(DoorInfo[pp], 2083701910);
        TextDrawFont(DoorInfo[pp], 1);
        TextDrawLetterSize(DoorInfo[pp], 0.219996, 1.199977);
        TextDrawColor(DoorInfo[pp], -1);
        TextDrawSetOutline(DoorInfo[pp], 0);
        TextDrawSetProportional(DoorInfo[pp], true);
        TextDrawSetShadow(DoorInfo[pp], 0);
        TextDrawUseBox(DoorInfo[pp], true);
        TextDrawBoxColor(DoorInfo[pp], 100);
        TextDrawTextSize(DoorInfo[pp], 0.000000, 171.000000);

		Tutorial[pp] = TextDrawCreate(16.000000, 125.000000, "~n~");
		TextDrawAlignment(Tutorial[pp], 1);
		TextDrawBackgroundColor(Tutorial[pp], 150);
		TextDrawFont(Tutorial[pp], 1);
		TextDrawLetterSize(Tutorial[pp], 0.210000, 1.000000);
		TextDrawColor(Tutorial[pp], -1);
		TextDrawSetOutline(Tutorial[pp], 1);
		TextDrawSetProportional(Tutorial[pp], true);
		TextDrawSetShadow(Tutorial[pp], 1);
		TextDrawUseBox(Tutorial[pp], true);
		TextDrawBoxColor(Tutorial[pp], 100);
		TextDrawTextSize(Tutorial[pp], 171.000000, 0);

		DashBoardPane[pp] = TextDrawCreate(202.000000, 340.000000, "~n~");
		TextDrawAlignment(DashBoardPane[pp], 2);
		TextDrawBackgroundColor(DashBoardPane[pp], 255);
		TextDrawFont(DashBoardPane[pp], 1);
		TextDrawLetterSize(DashBoardPane[pp], 0.500000, 14.000000);
		TextDrawColor(DashBoardPane[pp], -1);
		TextDrawSetOutline(DashBoardPane[pp], 0);
		TextDrawSetProportional(DashBoardPane[pp], true);
		TextDrawSetShadow(DashBoardPane[pp], 1);
		TextDrawUseBox(DashBoardPane[pp], true);
		TextDrawBoxColor(DashBoardPane[pp], 0x9E9EAD20);
		TextDrawTextSize(DashBoardPane[pp], 730.000000, -950.000000);

		DashBoardPane2[pp] = TextDrawCreate(152.000000, 349.000000, "Sluzba: ~b~grupa                    ~y~Ganton Gym (UID: 249)~n~~n~~w~Rozpoczeto: ~g~14:24               ~w~Tytul: ~r~Lider");
		TextDrawBackgroundColor(DashBoardPane2[pp], 255);
		TextDrawFont(DashBoardPane2[pp], 1);
		TextDrawLetterSize(DashBoardPane2[pp], 0.290000, 1.000000);
		TextDrawColor(DashBoardPane2[pp], -1);
		TextDrawSetOutline(DashBoardPane2[pp], 1);
		TextDrawSetProportional(DashBoardPane2[pp], true);

		Dashboard[pp] = TextDrawCreate(320.000000, 300.000000, "");
		TextDrawAlignment(Dashboard[pp], 2);
		TextDrawBackgroundColor(Dashboard[pp], 255);
		TextDrawFont(Dashboard[pp], 1);
		TextDrawLetterSize(Dashboard[pp], 0.290000, 0.899999);
		TextDrawColor(Dashboard[pp], -1);
		TextDrawSetOutline(Dashboard[pp], 1);
		TextDrawSetProportional(Dashboard[pp], true);
	}
}

stock CreateGroupPlayerTextdraws(playerid)
{
	new Float:baseY = 183.0;
	for(new i=0;i<5;i++)
	{
		GroupsListRow[i] = CreatePlayerTextDraw(playerid, 104.000000, baseY+(i*14), "1       Pig Pen (669)");
		PlayerTextDrawBackgroundColor(playerid, GroupsListRow[i], 255);
		PlayerTextDrawFont(playerid, GroupsListRow[i], 1);
		PlayerTextDrawLetterSize(playerid, GroupsListRow[i], 0.290000, 1.000000);
		PlayerTextDrawColor(playerid, GroupsListRow[i], -1);
		PlayerTextDrawSetOutline(playerid, GroupsListRow[i], 1);
		PlayerTextDrawSetProportional(playerid, GroupsListRow[i], true);
		PlayerTextDrawUseBox(playerid, GroupsListRow[i], true);
		PlayerTextDrawBoxColor(playerid, GroupsListRow[i], 112);
		PlayerTextDrawTextSize(playerid, GroupsListRow[i], 537.000000, 0.000000);
	}
	
	baseY = 184.0;
	for(new i=0;i<5;i++)
	{
		GroupsListStaticButtons[i][0] = CreatePlayerTextDraw(playerid, 344.000000, baseY+(playerid, i*14), "Info");
		PlayerTextDrawAlignment(playerid, GroupsListStaticButtons[i][0], 2);
		PlayerTextDrawBackgroundColor(playerid, GroupsListStaticButtons[i][0], 255);
		PlayerTextDrawFont(playerid, GroupsListStaticButtons[i][0], 1);
		PlayerTextDrawLetterSize(playerid, GroupsListStaticButtons[i][0], 0.190000, 0.799999);
		PlayerTextDrawColor(playerid, GroupsListStaticButtons[i][0], -1);
		PlayerTextDrawSetOutline(playerid, GroupsListStaticButtons[i][0], 1);
		PlayerTextDrawSetProportional(playerid, GroupsListStaticButtons[i][0], true);
		PlayerTextDrawUseBox(playerid, GroupsListStaticButtons[i][0], true);
		PlayerTextDrawBoxColor(playerid, GroupsListStaticButtons[i][0], 112);
		PlayerTextDrawTextSize(playerid, GroupsListStaticButtons[i][0], 12.0, 36.0);
		PlayerTextDrawSetSelectable(playerid, GroupsListStaticButtons[i][0], true);

		GroupsListStaticButtons[i][1] = CreatePlayerTextDraw(playerid, 387.000000, baseY+(playerid, i*14), "Pojazdy");
		PlayerTextDrawAlignment(playerid, GroupsListStaticButtons[i][1], 2);
		PlayerTextDrawBackgroundColor(playerid, GroupsListStaticButtons[i][1], 255);
		PlayerTextDrawFont(playerid, GroupsListStaticButtons[i][1], 1);
		PlayerTextDrawLetterSize(playerid, GroupsListStaticButtons[i][1], 0.190000, 0.799999);
		PlayerTextDrawColor(playerid, GroupsListStaticButtons[i][1], -1);
		PlayerTextDrawSetOutline(playerid, GroupsListStaticButtons[i][1], 1);
		PlayerTextDrawSetProportional(playerid, GroupsListStaticButtons[i][1], true);
		PlayerTextDrawUseBox(playerid, GroupsListStaticButtons[i][1], true);
		PlayerTextDrawBoxColor(playerid, GroupsListStaticButtons[i][1], 112);
		PlayerTextDrawTextSize(playerid, GroupsListStaticButtons[i][1], 10.0, 36.0);
		PlayerTextDrawSetSelectable(playerid, GroupsListStaticButtons[i][1], true);

		GroupsListStaticButtons[i][2] = CreatePlayerTextDraw(playerid, 430.000000, baseY+(playerid, i*14), "Duty");
		PlayerTextDrawAlignment(playerid, GroupsListStaticButtons[i][2], 2);
		PlayerTextDrawBackgroundColor(playerid, GroupsListStaticButtons[i][2], 255);
		PlayerTextDrawFont(playerid, GroupsListStaticButtons[i][2], 1);
		PlayerTextDrawLetterSize(playerid, GroupsListStaticButtons[i][2], 0.190000, 0.799999);
		PlayerTextDrawColor(playerid, GroupsListStaticButtons[i][2], -1);
		PlayerTextDrawSetOutline(playerid, GroupsListStaticButtons[i][2], 1);
		PlayerTextDrawSetProportional(playerid, GroupsListStaticButtons[i][2], true);
		PlayerTextDrawUseBox(playerid, GroupsListStaticButtons[i][2], true);
		PlayerTextDrawBoxColor(playerid, GroupsListStaticButtons[i][2], 112);
		PlayerTextDrawTextSize(playerid, GroupsListStaticButtons[i][2], 10.0, 36.0);
		PlayerTextDrawSetSelectable(playerid, GroupsListStaticButtons[i][2], true);

		GroupsListStaticButtons[i][3] = CreatePlayerTextDraw(playerid, 473.000000, baseY+(playerid, i*14), "Magazyn");
		PlayerTextDrawAlignment(playerid, GroupsListStaticButtons[i][3], 2);
		PlayerTextDrawBackgroundColor(playerid, GroupsListStaticButtons[i][3], 255);
		PlayerTextDrawFont(playerid, GroupsListStaticButtons[i][3], 1);
		PlayerTextDrawLetterSize(playerid, GroupsListStaticButtons[i][3], 0.190000, 0.799999);
		PlayerTextDrawColor(playerid, GroupsListStaticButtons[i][3], -1);
		PlayerTextDrawSetOutline(playerid, GroupsListStaticButtons[i][3], 1);
		PlayerTextDrawSetProportional(playerid, GroupsListStaticButtons[i][3], true);
		PlayerTextDrawUseBox(playerid, GroupsListStaticButtons[i][3], true);
		PlayerTextDrawBoxColor(playerid, GroupsListStaticButtons[i][3], 112);
		PlayerTextDrawTextSize(playerid, GroupsListStaticButtons[i][3], 10.0, 36.0);
		PlayerTextDrawSetSelectable(playerid, GroupsListStaticButtons[i][3], true);

		GroupsListStaticButtons[i][4] = CreatePlayerTextDraw(playerid, 516.000000, baseY+(playerid, i*14), "Online");
		PlayerTextDrawAlignment(playerid, GroupsListStaticButtons[i][4], 2);
		PlayerTextDrawBackgroundColor(playerid, GroupsListStaticButtons[i][4], 255);
		PlayerTextDrawFont(playerid, GroupsListStaticButtons[i][4], 1);
		PlayerTextDrawLetterSize(playerid, GroupsListStaticButtons[i][4], 0.190000, 0.799999);
		PlayerTextDrawColor(playerid, GroupsListStaticButtons[i][4], -1);
		PlayerTextDrawSetOutline(playerid, GroupsListStaticButtons[i][4], 1);
		PlayerTextDrawSetProportional(playerid, GroupsListStaticButtons[i][4], true);
		PlayerTextDrawUseBox(playerid, GroupsListStaticButtons[i][4], true);
		PlayerTextDrawBoxColor(playerid, GroupsListStaticButtons[i][4], 112);
		PlayerTextDrawTextSize(playerid, GroupsListStaticButtons[i][4], 10.0, 36.0);
		PlayerTextDrawSetSelectable(playerid, GroupsListStaticButtons[i][4], true);
	}
}

stock CreateGroupStaticTextdraws()
{
	GroupsListStaticHeader = TextDrawCreate(104.000000, 172.000000, "~y~SLOT           NAZWA GRUPY                                          OPCJE");
	TextDrawBackgroundColor(GroupsListStaticHeader, 255);
	TextDrawFont(GroupsListStaticHeader, 3);
	TextDrawLetterSize(GroupsListStaticHeader, 0.280000, 0.799999);
	TextDrawColor(GroupsListStaticHeader, -1);
	TextDrawSetOutline(GroupsListStaticHeader, 1);
	TextDrawSetProportional(GroupsListStaticHeader, true);
}