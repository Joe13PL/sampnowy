stock LoadProducts()
{
	Code_ExTimer_Begin(LoadProducts);

	new day= GetDayName();
    if(day == 7)
    {
    	printf("[OpenRP] Wykonano cotygodniowy reset limitu zamawiania produktow.");
    	mysql_query(mySQLconnection, "UPDATE ipb_products SET product_limit_used = 0");
    }

	new rows, fields, p_id = 0;

	mysql_query(mySQLconnection, "SELECT * FROM `ipb_products`");
	cache_get_data(rows, fields);

	for(new row = 0; row != rows; row++)
	{
		p_id++;

		Product[p_id][product_id] = cache_get_row_int(row, 0);
		cache_get_row(row, 1, Product[p_id][product_name], mySQLconnection, 40);
		Product[p_id][product_type] = cache_get_row_int(row, 2);
		Product[p_id][product_owner] = cache_get_row_int(row, 3);
		Product[p_id][product_price] = cache_get_row_int(row, 4);
		Product[p_id][product_value1] = cache_get_row_int(row, 5);
		Product[p_id][product_value2] = cache_get_row_int(row, 6);
		Product[p_id][product_model] = cache_get_row_int(row, 7);
		Product[p_id][product_extra] = cache_get_row_int(row, 8);
		Product[p_id][product_group] = cache_get_row_int(row, 9);
		Product[p_id][product_limit] = cache_get_row_int(row, 10);
		Product[p_id][product_limit_used] = cache_get_row_int(row, 11);
		Product[p_id][product_player] = cache_get_row_int(row, 12);

		Iter_Add(Products, p_id);
	}

	printf("Wczytano produkty [wczytanych: %d] [czas %d ms]", p_id, Code_ExTimer_End(LoadProducts));

	return 1;
}

stock AddBonusProduct(gid)
{
	new count;
	foreach(new p_id: Products)
	{
		if(Product[p_id][product_group] == Group[gid][group_uid]) count++;
	}

	if(count > 0 && count < 6)
	{
		new str[300];
		switch(count)
		{
			case 1:
			{
				switch(Group[gid][group_products])
				{
					case GROUP_PRODUCT_DRUGS:
					{
						strcat(str, "INSERT INTO ipb_products (product_type, product_name, product_price, product_value1, product_value2, product_model, product_group, product_limit_count) ");
						strcat(str, sprintf("VALUES (%d, 'Crack', 55, 2, 1, 1575, %d, 150)", ITEM_TYPE_DRUG, Group[gid][group_uid]));
						mysql_query(mySQLconnection, str);
						LoadProduct(cache_insert_id());
					}
					case GROUP_PRODUCT_GUNS:
					{
						new wrand = random(2);

						switch(wrand)
						{
							case 0:
							{
								strcat(str, "INSERT INTO ipb_products (product_type, product_name, product_price, product_value1, product_value2, product_model, product_group, product_limit_count) ");
								strcat(str, sprintf("VALUES (%d, 'Tec9', 1450, 32, 60, 372, %d, 15)", ITEM_TYPE_WEAPON, Group[gid][group_uid]));
								mysql_query(mySQLconnection, str);
								LoadProduct(cache_insert_id());
							}
							case 1:
							{
								strcat(str, "INSERT INTO ipb_products (product_type, product_name, product_price, product_value1, product_value2, product_model, product_group, product_limit_count) ");
								strcat(str, sprintf("VALUES (%d, 'Micro Uzi', 1550, 28, 60, 352, %d, 15)", ITEM_TYPE_WEAPON, Group[gid][group_uid]));
								mysql_query(mySQLconnection, str);
								LoadProduct(cache_insert_id());
							}
						}
					}
					case GROUP_PRODUCT_AMMO:
					{
						new prod_id;

						foreach(new p_id: Products)
						{
							if(Product[p_id][product_group] == Group[gid][group_uid] && Product[p_id][product_type] == ITEM_TYPE_AMMO && Product[p_id][product_value1] == 2) prod_id = p_id;
						}

						if(prod_id != 0)
						{
							mysql_query(mySQLconnection, sprintf("UPDATE ipb_products SET product_limit_count = 70 WHERE product_uid = %d", Product[prod_id][product_id]));
							ReloadProduct(Product[prod_id][product_id]);
						}
					}
					case GROUP_PRODUCT_SPECIAL:
					{
						//
					}
				}
			}
			case 2:
			{
				switch(Group[gid][group_products])
				{
					case GROUP_PRODUCT_DRUGS:
					{
						strcat(str, "INSERT INTO ipb_products (product_type, product_name, product_price, product_value1, product_value2, product_model, product_group, product_limit_count) ");
						strcat(str, sprintf("VALUES (%d, 'Amfetamina', 70, 4, 1, 1575, %d, 150)", ITEM_TYPE_DRUG, Group[gid][group_uid]));
						mysql_query(mySQLconnection, str);
						LoadProduct(cache_insert_id());
					}
					case GROUP_PRODUCT_GUNS:
					{
						strcat(str, "INSERT INTO ipb_products (product_type, product_name, product_price, product_value1, product_value2, product_model, product_group, product_limit_count) ");
						strcat(str, sprintf("VALUES (%d, 'Shotgun', 1200, 25, 30, 349, %d, 15)", ITEM_TYPE_WEAPON, Group[gid][group_uid]));
						mysql_query(mySQLconnection, str);
						LoadProduct(cache_insert_id());
					}
					case GROUP_PRODUCT_AMMO:
					{
						strcat(str, "INSERT INTO ipb_products (product_type, product_name, product_price, product_value1, product_value2, product_model, product_group, product_limit_count) ");
						strcat(str, sprintf("VALUES (%d, 'Amunicja (bro� d�uga)', 740, 3, 40, 19995, %d, 20)", ITEM_TYPE_AMMO, Group[gid][group_uid]));
						mysql_query(mySQLconnection, str);
						LoadProduct(cache_insert_id());
					}
					case GROUP_PRODUCT_SPECIAL:
					{
						//
					}
				}
			}
			case 3:
			{
				switch(Group[gid][group_products])
				{
					case GROUP_PRODUCT_DRUGS:
					{
						strcat(str, "INSERT INTO ipb_products (product_type, product_name, product_price, product_value1, product_value2, product_model, product_group, product_limit_count) ");
						strcat(str, sprintf("VALUES (%d, 'Metaamfetamina', 90, 5, 1, 1575, %d, 100)", ITEM_TYPE_DRUG, Group[gid][group_uid]));
						mysql_query(mySQLconnection, str);
						LoadProduct(cache_insert_id());
					}
					case GROUP_PRODUCT_GUNS:
					{
						new wrand = random(2);

						switch(wrand)
						{
							case 0:
							{
								strcat(str, "INSERT INTO ipb_products (product_type, product_name, product_price, product_value1, product_value2, product_model, product_group, product_limit_count) ");
								strcat(str, sprintf("VALUES (%d, 'Desert Eagle', 1500, 24, 30, 348, %d, 15)", ITEM_TYPE_WEAPON, Group[gid][group_uid]));
								mysql_query(mySQLconnection, str);
								LoadProduct(cache_insert_id());
							}
							case 1:
							{
								strcat(str, "INSERT INTO ipb_products (product_type, product_name, product_price, product_value1, product_value2, product_model, product_group, product_limit_count) ");
								strcat(str, sprintf("VALUES (%d, 'Silencer', 1300, 23, 30, 347, %d, 15)", ITEM_TYPE_WEAPON, Group[gid][group_uid]));
								mysql_query(mySQLconnection, str);
								LoadProduct(cache_insert_id());
							}
						}
					}
					case GROUP_PRODUCT_AMMO:
					{
						new prod_id;

						foreach(new p_id: Products)
						{
							if(Product[p_id][product_group] == Group[gid][group_uid] && Product[p_id][product_type] == ITEM_TYPE_AMMO && Product[p_id][product_value1] == 2) prod_id = p_id;
						}

						if(prod_id != 0)
						{
							mysql_query(mySQLconnection, sprintf("UPDATE ipb_products SET product_limit_count = 100 WHERE product_uid = %d", Product[prod_id][product_id]));
							ReloadProduct(Product[prod_id][product_id]);
						}
					}
					case GROUP_PRODUCT_SPECIAL:
					{
						//
					}
				}
			}
			case 4:
			{
				switch(Group[gid][group_products])
				{
					case GROUP_PRODUCT_DRUGS:
					{
						strcat(str, "INSERT INTO ipb_products (product_type, product_name, product_price, product_value1, product_value2, product_model, product_group, product_limit_count) ");
						strcat(str, sprintf("VALUES (%d, 'Heroina', 75, 6, 1, 1575, %d, 100)", ITEM_TYPE_DRUG, Group[gid][group_uid]));
						mysql_query(mySQLconnection, str);
						LoadProduct(cache_insert_id());
					}
					case GROUP_PRODUCT_GUNS:
					{
						strcat(str, "INSERT INTO ipb_products (product_type, product_name, product_price, product_value1, product_value2, product_model, product_group, product_limit_count) ");
						strcat(str, sprintf("VALUES (%d, 'MP5', 2200, 29, 70, 353, %d, 15)", ITEM_TYPE_WEAPON, Group[gid][group_uid]));
						mysql_query(mySQLconnection, str);
						LoadProduct(cache_insert_id());
					}
					case GROUP_PRODUCT_AMMO:
					{
						new prod_id;

						foreach(new p_id: Products)
						{
							if(Product[p_id][product_group] == Group[gid][group_uid] && Product[p_id][product_type] == ITEM_TYPE_AMMO && Product[p_id][product_value1] == 3) prod_id = p_id;
						}

						if(prod_id != 0)
						{
							mysql_query(mySQLconnection, sprintf("UPDATE ipb_products SET product_limit_count = 50 WHERE product_uid = %d", Product[prod_id][product_id]));
							ReloadProduct(Product[prod_id][product_id]);
						}
					}
					case GROUP_PRODUCT_SPECIAL:
					{
						//
					}
				}
			}
			case 5:
			{
				switch(Group[gid][group_products])
				{
					case GROUP_PRODUCT_DRUGS:
					{
						strcat(str, "INSERT INTO ipb_products (product_type, product_name, product_price, product_value1, product_value2, product_model, product_group, product_limit_count) ");
						strcat(str, sprintf("VALUES (%d, 'Kokaina', 120, 1, 1, 1575, %d, 100)", ITEM_TYPE_DRUG, Group[gid][group_uid]));
						mysql_query(mySQLconnection, str);
						LoadProduct(cache_insert_id());
					}
					case GROUP_PRODUCT_GUNS:
					{
						new wrand = random(2);

						switch(wrand)
						{
							case 0:
							{
								strcat(str, "INSERT INTO ipb_products (product_type, product_name, product_price, product_value1, product_value2, product_model, product_group, product_limit_count) ");
								strcat(str, sprintf("VALUES (%d, 'AK47', 4450, 30, 60, 355, %d, 15)", ITEM_TYPE_WEAPON, Group[gid][group_uid]));
								mysql_query(mySQLconnection, str);
								LoadProduct(cache_insert_id());
							}
							case 1:
							{
								strcat(str, "INSERT INTO ipb_products (product_type, product_name, product_price, product_value1, product_value2, product_model, product_group, product_limit_count) ");
								strcat(str, sprintf("VALUES (%d, 'M4A1', 5250, 31, 60, 356, %d, 15)", ITEM_TYPE_WEAPON, Group[gid][group_uid]));
								mysql_query(mySQLconnection, str);
								LoadProduct(cache_insert_id());
							}
						}
					}
					case GROUP_PRODUCT_AMMO:
					{
						new prod_id;

						foreach(new p_id: Products)
						{
							if(Product[p_id][product_group] == Group[gid][group_uid] && Product[p_id][product_type] == ITEM_TYPE_AMMO && Product[p_id][product_value1] == 3) prod_id = p_id;
						}

						if(prod_id != 0)
						{
							mysql_query(mySQLconnection, sprintf("UPDATE ipb_products SET product_limit_count = 80 WHERE product_uid = %d", Product[prod_id][product_id]));
							ReloadProduct(Product[prod_id][product_id]);
						}
					}
					case GROUP_PRODUCT_SPECIAL:
					{
						//
					}
				}
			}
		}
	}
}

stock RemoveBonusProduct(gid)
{
	mysql_query(mySQLconnection, sprintf("DELETE FROM ipb_products WHERE product_group = %d ORDER BY product_uid DESC LIMIT 1", Group[gid][group_uid]));
}

stock LoadProduct(prod_uid)
{
	new p_id = Iter_Free(Products), rows, fields;

	mysql_query(mySQLconnection, sprintf("SELECT * FROM `ipb_products` WHERE product_uid = %d LIMIT 1", prod_uid));
	cache_get_data(rows, fields);

	Product[p_id][product_id] = cache_get_row_int(0, 0);
	cache_get_row(0, 1, Product[p_id][product_name], mySQLconnection, 40);
	Product[p_id][product_type] = cache_get_row_int(0, 2);
	Product[p_id][product_owner] = cache_get_row_int(0, 3);
	Product[p_id][product_price] = cache_get_row_int(0, 4);
	Product[p_id][product_value1] = cache_get_row_int(0, 5);
	Product[p_id][product_value2] = cache_get_row_int(0, 6);
	Product[p_id][product_model] = cache_get_row_int(0, 7);
	Product[p_id][product_extra] = cache_get_row_int(0, 8);
	Product[p_id][product_group] = cache_get_row_int(0, 9);
	Product[p_id][product_limit] = cache_get_row_int(0, 10);
	Product[p_id][product_limit_used] = cache_get_row_int(0, 11);
	Product[p_id][product_player] = cache_get_row_int(0, 12);

	Iter_Add(Products, p_id);

	return p_id;
}

stock ReloadProduct(prod_uid)
{
	new prod_id = GetProductByUid(prod_uid);
	if(prod_id == -1) return 1;

	Product[prod_id][product_id] = 0;
	Product[prod_id][product_type] = 0;
	Product[prod_id][product_owner] = 0;
	Product[prod_id][product_price] = 0;
	Product[prod_id][product_value1] = 0;
	Product[prod_id][product_value2] = 0;
	Product[prod_id][product_model] = 0;
	Product[prod_id][product_extra] = 0;
	Product[prod_id][product_group] = 0;
	Product[prod_id][product_player] = 0;

	Iter_Remove(Products, prod_id);

	new p_id = Iter_Free(Products);
	new rows, fields;

	mysql_query(mySQLconnection, sprintf("SELECT * FROM `ipb_products` WHERE product_uid = %d LIMIT 1", prod_uid));
	cache_get_data(rows, fields);

	Product[p_id][product_id] = cache_get_row_int(0, 0);
	cache_get_row(0, 1, Product[p_id][product_name], mySQLconnection, 40);
	Product[p_id][product_type] = cache_get_row_int(0, 2);
	Product[p_id][product_owner] = cache_get_row_int(0, 3);
	Product[p_id][product_price] = cache_get_row_int(0, 4);
	Product[p_id][product_value1] = cache_get_row_int(0, 5);
	Product[p_id][product_value2] = cache_get_row_int(0, 6);
	Product[p_id][product_model] = cache_get_row_int(0, 7);
	Product[p_id][product_extra] = cache_get_row_int(0, 8);
	Product[p_id][product_group] = cache_get_row_int(0, 9);
	Product[p_id][product_limit] = cache_get_row_int(0, 10);
	Product[p_id][product_limit_used] = cache_get_row_int(0, 11);
	Product[p_id][product_player] = cache_get_row_int(0, 12);

	Iter_Add(Products, p_id);

	return p_id;
}

stock GetProductByUid(prodUid)
{
	foreach (new p : Products)
	{
		if( Product[p][product_id] == prodUid ) return p;
	}

	return -1;
}

stock RemoveProduct(prod_uid)
{
	new prod_id = GetProductByUid(prod_uid);
	if(prod_id == -1) return 1;

	Product[prod_id][product_id] = 0;
	Product[prod_id][product_type] = 0;
	Product[prod_id][product_owner] = 0;
	Product[prod_id][product_price] = 0;
	Product[prod_id][product_value1] = 0;
	Product[prod_id][product_value2] = 0;
	Product[prod_id][product_model] = 0;
	Product[prod_id][product_extra] = 0;
	Product[prod_id][product_group] = 0;
	Product[prod_id][product_player] = 0;

	Iter_Remove(Products, prod_id);

	return 1;
}
