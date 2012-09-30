/* File: wizard2.c */

/* Purpose: Wizard commands */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"

/*
 * Teleport directly to a town
 */
void teleport_player_town(int town)
{
        int x = 0, y = 0;

	if (autosave_l)
	{
		is_autosave = TRUE;
		msg_print("Autosaving the game...");
		do_cmd_save_game();
		is_autosave = FALSE;
	}

        /* Change town */
        dun_level = 0;
        p_ptr->town_num = town;

        for(x = 0; x < max_wild_x; x++)
                for(y = 0; y < max_wild_y; y++)
                        if(p_ptr->town_num == wilderness[y][x].town) goto finteletown;
finteletown:
        p_ptr->wilderness_y = y;
        p_ptr->wilderness_x = x;

	p_ptr->inside_arena = 0;
	leaving_quest = p_ptr->inside_quest;
	p_ptr->inside_quest = 0;
	p_ptr->leftbldg = FALSE;

	/* Leaving */
	p_ptr->leaving = TRUE;
}


/*
 * Hack -- Rerate Hitpoints
 */
void do_cmd_rerate(void)
{
	int min_value, max_value, i, percent;

#ifdef TEST
	int fubar, mlk = 0;

	for (fubar = 0; fubar < max_k_idx; fubar++)
	{
                if ((k_info[fubar].tval == TV_POTION)||(k_info[fubar].tval == TV_POTION2))
		{
			k_info[fubar].x_attr = 0xBC;
			mlk++;
		}
	}

	msg_format ("%d changes made.", mlk);

#else /* TEST */

#endif /* TEST */

	min_value = (PY_MAX_LEVEL * 3 * (p_ptr->hitdie - 1)) / 8;
	min_value += PY_MAX_LEVEL;

	max_value = (PY_MAX_LEVEL * 5 * (p_ptr->hitdie - 1)) / 8;
	max_value += PY_MAX_LEVEL;

	player_hp[0] = p_ptr->hitdie;

	/* Rerate */
	while (1)
	{
		/* Collect values */
		for (i = 1; i < PY_MAX_LEVEL; i++)
		{
			player_hp[i] = randint(p_ptr->hitdie);
			player_hp[i] += player_hp[i - 1];
		}

		/* Legal values */
		if ((player_hp[PY_MAX_LEVEL - 1] >= min_value) &&
		    (player_hp[PY_MAX_LEVEL - 1] <= max_value)) break;
	}

	percent = (int)(((long)player_hp[PY_MAX_LEVEL - 1] * 200L) /
	                (p_ptr->hitdie + ((PY_MAX_LEVEL - 1) * p_ptr->hitdie)));

	/* Update and redraw hitpoints */
	p_ptr->update |= (PU_HP);
	p_ptr->redraw |= (PR_HP);

	/* Window stuff */
	p_ptr->window |= (PW_PLAYER);

	/* Handle stuff */
	handle_stuff();

	/* Message */
	msg_format("Current Life Rating is %d/100.", percent);
}


#ifdef ALLOW_WIZARD
  
/*
 * Create the artifact of the specified number -- DAN
 *
 */
static void wiz_create_named_art()
{
	object_type forge;
	object_type *q_ptr;
        int i,a_idx;
        cptr p="Number of the artifact :";
        char out_val[80];
        artifact_type *a_ptr;

        if (!get_string(p, out_val, 4)) return;
        a_idx = atoi(out_val);                                

        a_ptr = &a_info[a_idx];

	/* Get local object */
	q_ptr = &forge;

	/* Wipe the object */
	object_wipe(q_ptr);

	/* Ignore "empty" artifacts */
	if (!a_ptr->name) return;

	/* Acquire the "kind" index */
	i = lookup_kind(a_ptr->tval, a_ptr->sval);

	/* Oops */
	if (!i) return;

	/* Create the artifact */
	object_prep(q_ptr, i);

	/* Save the name */
	q_ptr->name1 = a_idx;

	/* Extract the fields */
	q_ptr->pval = a_ptr->pval;
	q_ptr->ac = a_ptr->ac;
	q_ptr->dd = a_ptr->dd;
	q_ptr->ds = a_ptr->ds;
	q_ptr->to_a = a_ptr->to_a;
	q_ptr->to_h = a_ptr->to_h;
	q_ptr->to_d = a_ptr->to_d;
	q_ptr->weight = a_ptr->weight;

	/* Hack -- acquire "cursed" flag */
	if (a_ptr->flags3 & (TR3_CURSED)) q_ptr->ident |= (IDENT_CURSED);

	random_artifact_resistance(q_ptr);

        a_ptr->cur_num = 1;

	/* Drop the artifact from heaven */
	drop_near(q_ptr, -1, py, px);

	/* All done */
	msg_print("Allocated.");
}


/*
 * Hack -- quick debugging hook
 */
static void do_cmd_wiz_hack_ben(void)
{

#if 0

	/* XXX XXX XXX */

	int y, x;

	/* Hack */
	for (y = 0; y < cur_hgt; y++)
	{
		for (x = 0; x < cur_wid; x++)
		{
			cave_type *c_ptr = &cave[y][x];

			s16b o_idx, next_o_idx = 0;

			/* Scan all objects in the grid */
			for (o_idx = c_ptr->o_idx; o_idx; o_idx = next_o_idx)
			{
				object_type *o_ptr;
				object_kind *k_ptr;

				/* Acquire object */
				o_ptr = &o_list[o_idx];

				/* Acquire next object */
				next_o_idx = o_ptr->next_o_idx;

				/* Acquire kind */
				k_ptr = &k_info[o_ptr->k_idx];

				/* Describe */
				msg_format("Loc %d,%d Object '%s' (%d), Lev %d, N1=%d, N2=%d",
				           x, y, k_name + k_ptr->name, o_ptr->k_idx, k_ptr->level,
				           o_ptr->name1, o_ptr->name2);
				msg_print(NULL);
			}

			/* Monster */
			if (c_ptr->m_idx)
			{
				monster_type *m_ptr = &m_list[c_ptr->m_idx];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];

				msg_format("Loc %d,%d Monster '%s' (%d), Lev %d",
				           x, y, r_name + r_ptr->name, m_ptr->r_idx, r_ptr->level);
				msg_print(NULL);
			}
		}
	}

#endif

	/* Oops */
	msg_print("Oops.");
	(void) probing();
}



#ifdef MONSTER_HORDES

/* Summon a horde of monsters */
static void do_cmd_summon_horde()
{
	int wy = py, wx = px;
	int attempts = 1000;

	while (--attempts)
	{
		scatter(&wy, &wx, py, px, 3, 0);
		if (cave_naked_bold(wy, wx)) break;
	}

	(void)alloc_horde(wy, wx);
}

#endif /* MONSTER_HORDES */


/*
 * Output a long int in binary format.
 */
static void prt_binary(u32b flags, int row, int col)
{
	int        	i;
	u32b        bitmask;

	/* Scan the flags */
	for (i = bitmask = 1; i <= 32; i++, bitmask *= 2)
	{
		/* Dump set bits */
		if (flags & bitmask)
		{
			Term_putch(col++, row, TERM_BLUE, '*');
		}

		/* Dump unset bits */
		else
		{
			Term_putch(col++, row, TERM_WHITE, '-');
		}
	}
}


/*
 * Hack -- Teleport to the target
 */
static void do_cmd_wiz_bamf(void)
{
	/* Must have a target */
	if (!target_who) return;

	/* Teleport to the target */
	teleport_player_to(target_row, target_col);
}


/*
 * Aux function for "do_cmd_wiz_change()".	-RAK-
 */
static void do_cmd_wiz_change_aux(void)
{
	int i;
	int tmp_int;
	long tmp_long;
	char tmp_val[160];
	char ppp[80];


	/* Query the stats */
	for (i = 0; i < 6; i++)
	{
		/* Prompt */
		sprintf(ppp, "%s (3-118): ", stat_names[i]);

		/* Default */
		sprintf(tmp_val, "%d", p_ptr->stat_max[i]);

		/* Query */
		if (!get_string(ppp, tmp_val, 3)) return;

		/* Extract */
		tmp_int = atoi(tmp_val);

		/* Verify */
		if (tmp_int > 18+100) tmp_int = 18+100;
		else if (tmp_int < 3) tmp_int = 3;

		/* Save it */
		p_ptr->stat_cur[i] = p_ptr->stat_max[i] = tmp_int;
	}


	/* Default */
	sprintf(tmp_val, "%ld", (long)(p_ptr->au));

	/* Query */
	if (!get_string("Gold: ", tmp_val, 9)) return;

	/* Extract */
	tmp_long = atol(tmp_val);

	/* Verify */
	if (tmp_long < 0) tmp_long = 0L;

	/* Save */
	p_ptr->au = tmp_long;


	/* Default */
	sprintf(tmp_val, "%ld", (long)(p_ptr->max_exp));

	/* Query */
	if (!get_string("Experience: ", tmp_val, 9)) return;

	/* Extract */
	tmp_long = atol(tmp_val);

	/* Verify */
	if (tmp_long < 0) tmp_long = 0L;

	/* Save */
	p_ptr->max_exp = tmp_long;
	p_ptr->exp = tmp_long;

	/* Update */
	check_experience();
}


/*
 * Change various "permanent" player variables.
 */
static void do_cmd_wiz_change(void)
{
	/* Interact */
	do_cmd_wiz_change_aux();

	/* Redraw everything */
	do_cmd_redraw();
}


/*
 * Wizard routines for creating objects		-RAK-
 * And for manipulating them!                   -Bernd-
 *
 * This has been rewritten to make the whole procedure
 * of debugging objects much easier and more comfortable.
 *
 * The following functions are meant to play with objects:
 * Create, modify, roll for them (for statistic purposes) and more.
 * The original functions were by RAK.
 * The function to show an item's debug information was written
 * by David Reeve Sward <sward+@CMU.EDU>.
 *                             Bernd (wiebelt@mathematik.hu-berlin.de)
 *
 * Here are the low-level functions
 * - wiz_display_item()
 *     display an item's debug-info
 * - wiz_create_itemtype()
 *     specify tval and sval (type and subtype of object)
 * - wiz_tweak_item()
 *     specify pval, +AC, +tohit, +todam
 *     Note that the wizard can leave this function anytime,
 *     thus accepting the default-values for the remaining values.
 *     pval comes first now, since it is most important.
 * - wiz_reroll_item()
 *     apply some magic to the item or turn it into an artifact.
 * - wiz_roll_item()
 *     Get some statistics about the rarity of an item:
 *     We create a lot of fake items and see if they are of the
 *     same type (tval and sval), then we compare pval and +AC.
 *     If the fake-item is better or equal it is counted.
 *     Note that cursed items that are better or equal (absolute values)
 *     are counted, too.
 *     HINT: This is *very* useful for balancing the game!
 * - wiz_quantity_item()
 *     change the quantity of an item, but be sane about it.
 *
 * And now the high-level functions
 * - do_cmd_wiz_play()
 *     play with an existing object
 * - wiz_create_item()
 *     create a new object
 *
 * Note -- You do not have to specify "pval" and other item-properties
 * directly. Just apply magic until you are satisfied with the item.
 *
 * Note -- For some items (such as wands, staffs, some rings, etc), you
 * must apply magic, or you will get "broken" or "uncharged" objects.
 *
 * Note -- Redefining artifacts via "do_cmd_wiz_play()" may destroy
 * the artifact.  Be careful.
 *
 * Hack -- this function will allow you to create multiple artifacts.
 * This "feature" may induce crashes or other nasty effects.
 */

/*
 * Just display an item's properties (debug-info)
 * Originally by David Reeve Sward <sward+@CMU.EDU>
 * Verbose item flags by -Bernd-
 */
static void wiz_display_item(object_type *o_ptr)
{
	int i, j = 13;
        u32b f1, f2, f3, f4;
	char buf[256];

	/* Extract the flags */
        object_flags(o_ptr, &f1, &f2, &f3, &f4);

	/* Clear the screen */
	for (i = 1; i <= 23; i++) prt("", i, j - 2);

	/* Describe fully */
	object_desc_store(buf, o_ptr, TRUE, 3);

	prt(buf, 2, j);

	prt(format("kind = %-5d  level = %-4d  tval = %-5d  sval = %-5d",
	           o_ptr->k_idx, k_info[o_ptr->k_idx].level,
	           o_ptr->tval, o_ptr->sval), 4, j);

	prt(format("number = %-3d  wgt = %-6d  ac = %-5d    damage = %dd%d",
	           o_ptr->number, o_ptr->weight,
	           o_ptr->ac, o_ptr->dd, o_ptr->ds), 5, j);

	prt(format("pval = %-5d  toac = %-5d  tohit = %-4d  todam = %-4d",
	           o_ptr->pval, o_ptr->to_a, o_ptr->to_h, o_ptr->to_d), 6, j);

        prt(format("name1 = %-4d  name2 = %-4d  cost = %ld  pval2 = %-5d",
                   o_ptr->name1, o_ptr->name2, (long)object_value(o_ptr), o_ptr->pval2), 7, j);

	prt(format("ident = %04x  timeout = %-d",
	           o_ptr->ident, o_ptr->timeout), 8, j);

	prt("+------------FLAGS1------------+", 10, j);
	prt("AFFECT........SLAY........BRAND.", 11, j);
	prt("              cvae      xsqpaefc", 12, j);
	prt("siwdcc  ssidsahanvudotgddhuoclio", 13, j);
	prt("tnieoh  trnipttmiinmrrnrrraiierl", 14, j);
	prt("rtsxna..lcfgdkcpmldncltggpksdced", 15, j);
	prt_binary(f1, 16, j);

	prt("+------------FLAGS2------------+", 17, j);
	prt("SUST....IMMUN.RESIST............", 18, j);
	prt("        aefcprpsaefcpfldbc sn   ", 19, j);
	prt("siwdcc  cliooeatcliooeialoshtncd", 20, j);
	prt("tnieoh  ierlifraierliatrnnnrhehi", 21, j);
	prt("rtsxna..dcedslatdcedsrekdfddrxss", 22, j);
	prt_binary(f2, 23, j);

	prt("+------------FLAGS3------------+", 10, j+32);
	prt("fe      ehsi  st    iiiiadta  hp", 11, j+32);
	prt("il   n taihnf ee    ggggcregb vr", 12, j+32);
	prt("re  nowysdose eld   nnnntalrl ym", 13, j+32);
	prt("ec  omrcyewta ieirmsrrrriieaeccc", 14, j+32);
	prt("aa  taauktmatlnpgeihaefcvnpvsuuu", 15, j+32);
	prt("uu  egirnyoahivaeggoclioaeoasrrr", 16, j+32);
	prt("rr  litsopdretitsehtierltxrtesss", 17, j+32);
	prt("aa  echewestreshtntsdcedeptedeee", 18, j+32);
	prt_binary(f3, 19, j+32);
}


/*
 * A structure to hold a tval and its description
 */
typedef struct tval_desc
{
	int        tval;
	cptr       desc;
} tval_desc;

/*
 * A list of tvals and their textual names
 */
static tval_desc tvals[] =
{
	{ TV_SWORD,             "Sword"                },
	{ TV_POLEARM,           "Polearm"              },
	{ TV_HAFTED,            "Hafted Weapon"        },
	{ TV_BOW,               "Bow"                  },
        { TV_BOOMERANG,         "Boomerang"            },
	{ TV_ARROW,             "Arrows"               },
	{ TV_BOLT,              "Bolts"                },
	{ TV_SHOT,              "Shots"                },
	{ TV_SHIELD,            "Shield"               },
	{ TV_CROWN,             "Crown"                },
	{ TV_HELM,              "Helm"                 },
	{ TV_GLOVES,            "Gloves"               },
	{ TV_BOOTS,             "Boots"                },
	{ TV_CLOAK,             "Cloak"                },
	{ TV_DRAG_ARMOR,        "Dragon Scale Mail"    },
	{ TV_HARD_ARMOR,        "Hard Armor"           },
	{ TV_SOFT_ARMOR,        "Soft Armor"           },
	{ TV_RING,              "Ring"                 },
	{ TV_AMULET,            "Amulet"               },
	{ TV_LITE,              "Lite"                 },
	{ TV_POTION,            "Potion"               },
        { TV_POTION2,           "Potion2"              },
	{ TV_SCROLL,            "Scroll"               },
	{ TV_WAND,              "Wand"                 },
	{ TV_STAFF,             "Staff"                },
	{ TV_ROD,               "Rod"                  },
        { TV_VALARIN_BOOK,      "Valarin Spellbook"    },
        { TV_MAGERY_BOOK,       "Magery Spellbook"     },
        { TV_SHADOW_BOOK,       "Shadow Spellbook"     },
	{ TV_CHAOS_BOOK,        "Chaos Spellbook"      },
        { TV_NETHER_BOOK,       "Nether Spellbook"     },
        { TV_CRUSADE_BOOK,      "Crusade Spellbook"    },
        { TV_SIGALDRY_BOOK,     "Sigaldry Spellbook",  },
        { TV_SYMBIOTIC_BOOK,    "Symbiotic Spellbook", },
        { TV_TRIBAL_BOOK,       "Tribal Spellbook"     },
        { TV_MUSIC_BOOK,        "Music Book"           },
        { TV_MAGIC_BOOK,        "Book of Spells"       },
        { TV_PRAYER_BOOK,       "Holy Book"            },
        { TV_ILLUSION_BOOK,     "Book of Illusions"    },
        { TV_MIMIC_BOOK,        "Book of Lore"         },
	{ TV_SPIKE,             "Spikes"               },
	{ TV_DIGGING,           "Digger"               },
	{ TV_CHEST,             "Chest"                },
	{ TV_FOOD,              "Food"                 },
	{ TV_FLASK,             "Flask"                },
        { TV_MSTAFF,            "Mage Staff"           },
        { TV_BATERIE,           "Baterie"              },
        { TV_PARCHEMENT,        "Parchement"           },
        { TV_INSTRUMENT,        "Musical Instrument"   },
        { TV_RUNE1,             "Rune 1"               },
        { TV_RUNE2,             "Rune 2"               },
        { TV_JUNK,              "Junk"                 },
	{ 0,                    NULL                   }
};


/*
 * Strip an "object name" into a buffer
 */
static void strip_name(char *buf, int k_idx)
{
	char *t;

	object_kind *k_ptr = &k_info[k_idx];

	cptr str = (k_name + k_ptr->name);


	/* Skip past leading characters */
	while ((*str == ' ') || (*str == '&')) str++;

	/* Copy useful chars */
	for (t = buf; *str; str++)
	{
		if (*str != '~') *t++ = *str;
	}

	/* Terminate the new name */
	*t = '\0';
}


/*
 * Hack -- title for each column
 *
 * XXX XXX XXX This will not work with "EBCDIC", I would think.
 */
static char head[4] =
{ 'a', 'A', '0', ':' };


/*
 * Specify tval and sval (type and subtype of object) originally
 * by RAK, heavily modified by -Bernd-
 *
 * This function returns the k_idx of an object type, or zero if failed
 *
 * List up to 50 choices in three columns
 */
static int wiz_create_itemtype(void)
{
	int i, num, max_num;
	int col, row;
	int tval;

	cptr tval_desc;
	char ch;

	int choice[60];

	char buf[160];


	/* Clear screen */
	Term_clear();

	/* Print all tval's and their descriptions */
	for (num = 0; (num < 60) && tvals[num].tval; num++)
	{
		row = 2 + (num % 20);
		col = 30 * (num / 20);
		ch = head[num/20] + (num%20);
		prt(format("[%c] %s", ch, tvals[num].desc), row, col);
	}

	/* Me need to know the maximal possible tval_index */
	max_num = num;

	/* Choose! */
	if (!get_com("Get what type of object? ", &ch)) return (0);

	/* Analyze choice */
	num = -1;
	if ((ch >= head[0]) && (ch < head[0] + 20)) num = ch - head[0];
	if ((ch >= head[1]) && (ch < head[1] + 20)) num = ch - head[1] + 20;
	if ((ch >= head[2]) && (ch < head[2] + 17)) num = ch - head[2] + 40;

	/* Bail out if choice is illegal */
	if ((num < 0) || (num >= max_num)) return (0);

	/* Base object type chosen, fill in tval */
	tval = tvals[num].tval;
	tval_desc = tvals[num].desc;


	/*** And now we go for k_idx ***/

	/* Clear screen */
	Term_clear();

	/* We have to search the whole itemlist. */
	for (num = 0, i = 1; (num < 60) && (i < max_k_idx); i++)
	{
		object_kind *k_ptr = &k_info[i];

		/* Analyze matching items */
		if (k_ptr->tval == tval)
		{
			/* Hack -- Skip instant artifacts */
                        if (k_ptr->flags3 & (TR3_INSTA_ART)) continue;

			/* Prepare it */
			row = 2 + (num % 20);
			col = 30 * (num / 20);
			ch = head[num/20] + (num%20);

			/* Acquire the "name" of object "i" */
			strip_name(buf, i);

			/* Print it */
			prt(format("[%c] %s", ch, buf), row, col);

			/* Remember the object index */
			choice[num++] = i;
		}
	}

	/* Me need to know the maximal possible remembered object_index */
	max_num = num;

	/* Choose! */
	if (!get_com(format("What Kind of %s? ", tval_desc), &ch)) return (0);

	/* Analyze choice */
	num = -1;
	if ((ch >= head[0]) && (ch < head[0] + 20)) num = ch - head[0];
	if ((ch >= head[1]) && (ch < head[1] + 20)) num = ch - head[1] + 20;
	if ((ch >= head[2]) && (ch < head[2] + 17)) num = ch - head[2] + 40;

	/* Bail out if choice is "illegal" */
	if ((num < 0) || (num >= max_num)) return (0);

	/* And return successful */
	return (choice[num]);
}


/*
 * Tweak an item
 */
static void wiz_tweak_item(object_type *o_ptr)
{
	cptr p;
	char tmp_val[80];


#if 0 /* DG -- A Wizard can do whatever he/she wants */
	/* Hack -- leave artifacts alone */
        if (artifact_p(o_ptr) || o_ptr->art_name) return;
#endif

	p = "Enter new 'pval' setting: ";
        sprintf(tmp_val, "%ld", o_ptr->pval);
	if (!get_string(p, tmp_val, 5)) return;
	o_ptr->pval = atoi(tmp_val);
	wiz_display_item(o_ptr);

        p = "Enter new 'pval2' setting: ";
        sprintf(tmp_val, "%d", o_ptr->pval2);
	if (!get_string(p, tmp_val, 5)) return;
        o_ptr->pval2 = atoi(tmp_val);
	wiz_display_item(o_ptr);

        p = "Enter new 'pval3' setting: ";
        sprintf(tmp_val, "%d", o_ptr->pval3);
	if (!get_string(p, tmp_val, 5)) return;
        o_ptr->pval3 = atoi(tmp_val);
	wiz_display_item(o_ptr);

	p = "Enter new 'to_a' setting: ";
	sprintf(tmp_val, "%d", o_ptr->to_a);
	if (!get_string(p, tmp_val, 5)) return;
	o_ptr->to_a = atoi(tmp_val);
	wiz_display_item(o_ptr);

	p = "Enter new 'to_h' setting: ";
	sprintf(tmp_val, "%d", o_ptr->to_h);
	if (!get_string(p, tmp_val, 5)) return;
	o_ptr->to_h = atoi(tmp_val);
	wiz_display_item(o_ptr);

	p = "Enter new 'to_d' setting: ";
	sprintf(tmp_val, "%d", o_ptr->to_d);
	if (!get_string(p, tmp_val, 5)) return;
	o_ptr->to_d = atoi(tmp_val);
	wiz_display_item(o_ptr);

        p = "Enter new 'name2' setting: ";
        sprintf(tmp_val, "%d", o_ptr->name2);
	if (!get_string(p, tmp_val, 5)) return;
        o_ptr->name2 = atoi(tmp_val);
	wiz_display_item(o_ptr);

        p = "Enter new 'sval' setting: ";
        sprintf(tmp_val, "%d", o_ptr->sval);
	if (!get_string(p, tmp_val, 5)) return;
        o_ptr->sval = atoi(tmp_val);
	wiz_display_item(o_ptr);
}


/*
 * Apply magic to an item or turn it into an artifact. -Bernd-
 */
static void wiz_reroll_item(object_type *o_ptr)
{
	object_type forge;
	object_type *q_ptr;

	char ch;

	bool changed = FALSE;


	/* Hack -- leave artifacts alone */
	if (artifact_p(o_ptr) || o_ptr->art_name) return;


	/* Get local object */
	q_ptr = &forge;

	/* Copy the object */
	object_copy(q_ptr, o_ptr);


	/* Main loop. Ask for magification and artifactification */
	while (TRUE)
	{
		/* Display full item debug information */
		wiz_display_item(q_ptr);

		/* Ask wizard what to do. */
		if (!get_com("[a]ccept, [n]ormal, [g]ood, [e]xcellent? ", &ch))
		{
			changed = FALSE;
			break;
		}

		/* Create/change it! */
		if (ch == 'A' || ch == 'a')
		{
			changed = TRUE;
			break;
		}

		/* Apply normal magic, but first clear object */
		else if (ch == 'n' || ch == 'N')
		{
			object_prep(q_ptr, o_ptr->k_idx);
			apply_magic(q_ptr, dun_level, FALSE, FALSE, FALSE);
		}

		/* Apply good magic, but first clear object */
		else if (ch == 'g' || ch == 'g')
		{
			object_prep(q_ptr, o_ptr->k_idx);
			apply_magic(q_ptr, dun_level, FALSE, TRUE, FALSE);
		}

		/* Apply great magic, but first clear object */
		else if (ch == 'e' || ch == 'e')
		{
			object_prep(q_ptr, o_ptr->k_idx);
			apply_magic(q_ptr, dun_level, FALSE, TRUE, TRUE);
		}
	}


	/* Notice change */
	if (changed)
	{
		/* Apply changes */
		object_copy(o_ptr, q_ptr);

		/* Recalculate bonuses */
		p_ptr->update |= (PU_BONUS);

		/* Combine / Reorder the pack (later) */
		p_ptr->notice |= (PN_COMBINE | PN_REORDER);

		/* Window stuff */
		p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_SPELL | PW_PLAYER);
	}
}



/*
 * Maximum number of rolls
 */
#define TEST_ROLL 100000


/*
 * Try to create an item again. Output some statistics.    -Bernd-
 *
 * The statistics are correct now.  We acquire a clean grid, and then
 * repeatedly place an object in this grid, copying it into an item
 * holder, and then deleting the object.  We fiddle with the artifact
 * counter flags to prevent weirdness.  We use the items to collect
 * statistics on item creation relative to the initial item.
 */
static void wiz_statistics(object_type *o_ptr)
{
	long i, matches, better, worse, other;

	char ch;
	char *quality;

	bool good, great;

	object_type forge;
	object_type	*q_ptr;
	
	cptr q = "Rolls: %ld, Matches: %ld, Better: %ld, Worse: %ld, Other: %ld";


	/* XXX XXX XXX Mega-Hack -- allow multiple artifacts */
        if (artifact_p(o_ptr))
        {
                if (o_ptr->tval == TV_RANDART) {
                        random_artifacts[o_ptr->sval].generated = FALSE;
                } else {
                        a_info[o_ptr->name1].cur_num = 0;
                }
        }


	/* Interact */
	while (TRUE)
	{
		cptr pmt = "Roll for [n]ormal, [g]ood, or [e]xcellent treasure? ";

		/* Display item */
		wiz_display_item(o_ptr);

		/* Get choices */
		if (!get_com(pmt, &ch)) break;

		if (ch == 'n' || ch == 'N')
		{
			good = FALSE;
			great = FALSE;
			quality = "normal";
		}
		else if (ch == 'g' || ch == 'G')
		{
			good = TRUE;
			great = FALSE;
			quality = "good";
		}
		else if (ch == 'e' || ch == 'E')
		{
			good = TRUE;
			great = TRUE;
			quality = "excellent";
		}
		else
		{
			good = FALSE;
			great = FALSE;
			break;
		}

		/* Let us know what we are doing */
		msg_format("Creating a lot of %s items. Base level = %d.",
		           quality, dun_level);
		msg_print(NULL);

		/* Set counters to zero */
		matches = better = worse = other = 0;

		/* Let's rock and roll */
		for (i = 0; i <= TEST_ROLL; i++)
		{
			/* Output every few rolls */
			if ((i < 100) || (i % 100 == 0))
			{
				/* Do not wait */
				inkey_scan = TRUE;

				/* Allow interupt */
				if (inkey())
				{
					/* Flush */
					flush();

					/* Stop rolling */
					break;
				}

				/* Dump the stats */
				prt(format(q, i, matches, better, worse, other), 0, 0);
				Term_fresh();
			}


			/* Get local object */
			q_ptr = &forge;

			/* Wipe the object */
			object_wipe(q_ptr);

			/* Create an object */
			make_object(q_ptr, good, great);


			/* XXX XXX XXX Mega-Hack -- allow multiple artifacts */
                        if (artifact_p(q_ptr))
                        {
                                if (q_ptr->tval == TV_RANDART) {
                                        random_artifacts[q_ptr->sval].generated = FALSE;
                                } else {
                                        a_info[q_ptr->name1].cur_num = 0;
                                }
                        }


			/* Test for the same tval and sval. */
			if ((o_ptr->tval) != (q_ptr->tval)) continue;
			if ((o_ptr->sval) != (q_ptr->sval)) continue;

			/* Check for match */
			if ((q_ptr->pval == o_ptr->pval) &&
			    (q_ptr->to_a == o_ptr->to_a) &&
			    (q_ptr->to_h == o_ptr->to_h) &&
			    (q_ptr->to_d == o_ptr->to_d))
			{
				matches++;
			}

			/* Check for better */
			else if ((q_ptr->pval >= o_ptr->pval) &&
			         (q_ptr->to_a >= o_ptr->to_a) &&
			         (q_ptr->to_h >= o_ptr->to_h) &&
			         (q_ptr->to_d >= o_ptr->to_d))
			{
				better++;
			}

			/* Check for worse */
			else if ((q_ptr->pval <= o_ptr->pval) &&
			         (q_ptr->to_a <= o_ptr->to_a) &&
			         (q_ptr->to_h <= o_ptr->to_h) &&
			         (q_ptr->to_d <= o_ptr->to_d))
			{
				worse++;
			}

			/* Assume different */
			else
			{
				other++;
			}
		}

		/* Final dump */
		msg_format(q, i, matches, better, worse, other);
		msg_print(NULL);
	}


	/* Hack -- Normally only make a single artifact */
	if (artifact_p(o_ptr)) {
                if (o_ptr->tval == TV_RANDART) {
                        random_artifacts[o_ptr->sval].generated = TRUE;
                } else {
                        a_info[o_ptr->name1].cur_num = 1;
                }
        }
}


/*
 * Change the quantity of a the item
 */
static void wiz_quantity_item(object_type *o_ptr)
{
	int         tmp_int;

	char        tmp_val[100];


	/* Never duplicate artifacts */
	if (artifact_p(o_ptr) || o_ptr->art_name) return;


	/* Default */
	sprintf(tmp_val, "%d", o_ptr->number);

	/* Query */
	if (get_string("Quantity: ", tmp_val, 2))
	{
		/* Extract */
		tmp_int = atoi(tmp_val);

		/* Paranoia */
		if (tmp_int < 1) tmp_int = 1;
		if (tmp_int > 99) tmp_int = 99;

		/* Accept modifications */
		o_ptr->number = tmp_int;
	}
}



/*
 * Play with an item. Options include:
 *   - Output statistics (via wiz_roll_item)
 *   - Reroll item (via wiz_reroll_item)
 *   - Change properties (via wiz_tweak_item)
 *   - Change the number of items (via wiz_quantity_item)
 */
static void do_cmd_wiz_play(void)
{
	int item;

	object_type	forge;
	object_type *q_ptr;

	object_type *o_ptr;

	char ch;

	bool changed;

	cptr q, s;

	/* Get an item */
	q = "Play with which object? ";
	s = "You have nothing to play with.";
	if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR))) return;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}


	/* The item was not changed */
	changed = FALSE;


	/* Icky */
	character_icky = TRUE;

	/* Save the screen */
	Term_save();


	/* Get local object */
	q_ptr = &forge;
	
	/* Copy object */
	object_copy(q_ptr, o_ptr);


	/* The main loop */
	while (TRUE)
	{
		/* Display the item */
		wiz_display_item(q_ptr);

		/* Get choice */
		if (!get_com("[a]ccept [s]tatistics [r]eroll [t]weak [q]uantity? ", &ch))
		{
			changed = FALSE;
			break;
		}

		if (ch == 'A' || ch == 'a')
		{
			changed = TRUE;
			break;
		}

		if (ch == 's' || ch == 'S')
		{
			wiz_statistics(q_ptr);
		}

		if (ch == 'r' || ch == 'r')
		{
			wiz_reroll_item(q_ptr);
		}

		if (ch == 't' || ch == 'T')
		{
			wiz_tweak_item(q_ptr);
		}

		if (ch == 'q' || ch == 'Q')
		{
			wiz_quantity_item(q_ptr);
		}
	}


	/* Restore the screen */
	Term_load();

	/* Not Icky */
	character_icky = FALSE;


	/* Accept change */
	if (changed)
	{
		/* Message */
		msg_print("Changes accepted.");

		/* Change */
		object_copy(o_ptr, q_ptr);

		/* Recalculate bonuses */
		p_ptr->update |= (PU_BONUS);

		/* Combine / Reorder the pack (later) */
		p_ptr->notice |= (PN_COMBINE | PN_REORDER);

		/* Window stuff */
		p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_SPELL | PW_PLAYER);
	}

	/* Ignore change */
	else
	{
		msg_print("Changes ignored.");
	}
}


/*
 * Wizard routine for creating objects		-RAK-
 * Heavily modified to allow magification and artifactification  -Bernd-
 *
 * Note that wizards cannot create objects on top of other objects.
 *
 * Hack -- this routine always makes a "dungeon object", and applies
 * magic to it, and attempts to decline cursed items.
 */
static void wiz_create_item(void)
{
	object_type	forge;
	object_type *q_ptr;

	int k_idx;


	/* Icky */
	character_icky = TRUE;

	/* Save the screen */
	Term_save();

	/* Get object base type */
	k_idx = wiz_create_itemtype();

	/* Restore the screen */
	Term_load();

	/* Not Icky */
	character_icky = FALSE;


	/* Return if failed */
	if (!k_idx) return;

	/* Get local object */
	q_ptr = &forge;

	/* Create the item */
	object_prep(q_ptr, k_idx);

	/* Apply magic (no messages, no artifacts) */
	apply_magic(q_ptr, dun_level, FALSE, FALSE, FALSE);

	/* Drop the object from heaven */
	drop_near(q_ptr, -1, py, px);

	/* All done */
	msg_print("Allocated.");
}

static void wiz_create_item_2(void)
{
	object_type forge;
	object_type *q_ptr;
        int a_idx;
        cptr p="Number of the object :";
        char out_val[80];

        if (!get_string(p, out_val, 4)) return;
        a_idx = atoi(out_val);                                

	/* Return if failed */
        if (!a_idx) return;

	/* Get local object */
	q_ptr = &forge;

	/* Create the item */
        object_prep(q_ptr, a_idx);

	/* Apply magic (no messages, no artifacts) */
	apply_magic(q_ptr, dun_level, FALSE, FALSE, FALSE);

	/* Drop the object from heaven */
	drop_near(q_ptr, -1, py, px);

	/* All done */
	msg_print("Allocated.");
}


/*
 * Cure everything instantly
 */
static void do_cmd_wiz_cure_all(void)
{
        object_type *o_ptr;
        monster_race *r_ptr;

	/* Remove curses */
	(void)remove_all_curse();

	/* Restore stats */
	(void)res_stat(A_STR);
	(void)res_stat(A_INT);
	(void)res_stat(A_WIS);
	(void)res_stat(A_CON);
	(void)res_stat(A_DEX);
	(void)res_stat(A_CHR);

	/* Restore the level */
	(void)restore_level();

	/* Heal the player */
	p_ptr->chp = p_ptr->mhp;
	p_ptr->chp_frac = 0;

        /* Cure insanity of player */
        p_ptr->csane = p_ptr->msane;
        p_ptr->csane_frac = 0;

        /* Heal the player monster */
        /* Get the carried monster */
        o_ptr = &inventory[INVEN_CARRY];
        if(o_ptr->k_idx)
        {
                int max;
                r_ptr = &r_info[o_ptr->pval];
                max = maxroll(r_ptr->hdice, r_ptr->hside);
                o_ptr->pval2 = max;
        }

	/* Restore mana */
	p_ptr->csp = p_ptr->msp;
	p_ptr->csp_frac = 0;

        /* Restore tank */
        p_ptr->ctp = p_ptr->mtp;

	/* Cure stuff */
	(void)set_blind(0);
	(void)set_confused(0);
	(void)set_poisoned(0);
	(void)set_afraid(0);
	(void)set_paralyzed(0);
	(void)set_image(0);
	(void)set_stun(0);
	(void)set_cut(0);
	(void)set_slow(0);
        p_ptr->black_breath = FALSE;

	/* No longer hungry */
	(void)set_food(PY_FOOD_MAX - 1);

	/* Redraw everything */
	do_cmd_redraw();
}


/*
 * Go to any level
 */
static void do_cmd_wiz_jump(void)
{
	/* Ask for level */
	if (command_arg <= 0)
	{
		char	ppp[80];

		char	tmp_val[160];

		/* Prompt */
                msg_format("dungeon_type : %d", dungeon_type);
                sprintf(ppp, "Jump to level (0-%d): ", d_info[dungeon_type].maxdepth);

		/* Default */
		sprintf(tmp_val, "%d", dun_level);

		/* Ask for a level */
		if (!get_string(ppp, tmp_val, 10)) return;

		/* Extract request */
		command_arg = atoi(tmp_val);
	}

	/* Paranoia */
        if (command_arg < 0) command_arg = 0;

	/* Paranoia */
        if (command_arg > d_info[dungeon_type].maxdepth) command_arg = d_info[dungeon_type].maxdepth;

	/* Accept request */
	msg_format("You jump to dungeon level %d.", command_arg);

	if (autosave_l)
	{
		is_autosave = TRUE;
		msg_print("Autosaving the game...");
		do_cmd_save_game();
		is_autosave = FALSE;
	}

	/* Change level */
	dun_level = command_arg;

	p_ptr->inside_arena = 0;
	leaving_quest = p_ptr->inside_quest;

	/* Leaving an 'only once' quest marks it as failed */
	if (leaving_quest &&
		(quest[leaving_quest].flags & QUEST_FLAG_ONCE) &&
		(quest[leaving_quest].status == QUEST_STATUS_TAKEN))
	{
		quest[leaving_quest].status = QUEST_STATUS_FAILED;
	}

	p_ptr->inside_quest = 0;
	p_ptr->leftbldg = FALSE;

	/* Leaving */
	p_ptr->leaving = TRUE;
}


/*
 * Become aware of a lot of objects
 */
static void do_cmd_wiz_learn(void)
{
	int i;

	object_type forge;
	object_type *q_ptr;

	/* Scan every object */
	for (i = 1; i < max_k_idx; i++)
	{
		object_kind *k_ptr = &k_info[i];

		/* Induce awareness */
		if (k_ptr->level <= command_arg)
		{
			/* Get local object */
			q_ptr = &forge;
			
			/* Prepare object */
			object_prep(q_ptr, i);

			/* Awareness */
			object_aware(q_ptr);
		}
	}
}


/*
 * Summon some creatures
 */
static void do_cmd_wiz_summon(int num)
{
	int i;

	for (i = 0; i < num; i++)
	{
        (void)summon_specific(py, px, dun_level, 0);
	}
}


/*
 * Summon a creature of the specified type
 *
 * XXX XXX XXX This function is rather dangerous
 */
static void do_cmd_wiz_named(int r_idx, bool slp)
{
	int i, x, y;

	/* Paranoia */
	/* if (!r_idx) return; */

	/* Prevent illegal monsters */
	if (r_idx >= max_r_idx-1) return;

	/* Try 10 times */
	for (i = 0; i < 10; i++)
	{
		int d = 1;

		/* Pick a location */
		scatter(&y, &x, py, px, d, 0);

		/* Require empty grids */
		if (!cave_empty_bold(y, x)) continue;

		/* Place it (allow groups) */
        if (place_monster_aux(y, x, r_idx, slp, TRUE, FALSE)) break;
	}
}


/*
 * Summon a creature of the specified type
 *
 * XXX XXX XXX This function is rather dangerous
 */
static void do_cmd_wiz_named_friendly(int r_idx, bool slp)
{
	int i, x, y;

	/* Paranoia */
	/* if (!r_idx) return; */

	/* Prevent illegal monsters */
	if (r_idx >= max_r_idx-1) return;

	/* Try 10 times */
	for (i = 0; i < 10; i++)
	{
		int d = 1;

		/* Pick a location */
		scatter(&y, &x, py, px, d, 0);

		/* Require empty grids */
		if (!cave_empty_bold(y, x)) continue;

		/* Place it (allow groups) */
        if (place_monster_aux(y, x, r_idx, slp, TRUE, TRUE)) break;
	}
}



/*
 * Hack -- Delete all nearby monsters
 */
static void do_cmd_wiz_zap(void)
{
	int        i;

	/* Genocide everyone nearby */
	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];

		/* Paranoia -- Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Delete nearby monsters */
		if (m_ptr->cdis <= MAX_SIGHT) delete_monster_idx(i);
	}
}


#ifdef ALLOW_SPOILERS

/*
 * External function
 */
extern void do_cmd_spoilers(void);

#endif /* ALLOW_SPOILERS */



/*
 * Hack -- declare external function
 */
extern void do_cmd_debug(void);



/*
 * Ask for and parse a "debug command"
 * The "command_arg" may have been set.
 */
void do_cmd_debug(void)
{
	int     x,y;
	char    cmd;


	/* Get a "debug command" */
	get_com("Debug Command: ", &cmd);

	/* Analyze the command */
	switch (cmd)
	{
		/* Nothing */
		case ESCAPE:
		case ' ':
		case '\n':
		case '\r':
		break;


#ifdef ALLOW_SPOILERS

		/* Hack -- Generate Spoilers */
		case '"':
		do_cmd_spoilers();
		break;

#endif /* ALLOW_SPOILERS */


		/* Hack -- Help */
		case '?':
		do_cmd_help();
		break;


		/* Cure all maladies */
		case 'a':
		do_cmd_wiz_cure_all();
		break;

		/* Teleport to target */
		case 'b':
		do_cmd_wiz_bamf();
		break;

		/* Create any object */
                case '-':
                wiz_create_item_2();
		break;

		/* Create any object */
		case 'c':
		wiz_create_item();
		break;

		/* Create a named artifact */
		case 'C':
                wiz_create_named_art();
		break;

		/* Detect everything */
		case 'd':
		detect_all();
		break;

                /* Change of Dungeon type */
                case 'D':
                dungeon_type = command_arg;
                dun_level = d_info[dungeon_type].mindepth;
                msg_format("You go into %s", d_text + d_info[dungeon_type].text);

                /* Leaving */
                p_ptr->leaving = TRUE;
		break;

		/* Edit character */
		case 'e':
		do_cmd_wiz_change();
		break;

                /* Change grid's mana */
                case 'E':
                cave[py][px].mana = command_arg;
		break;

		/* View item info */
		case 'f':
		identify_fully();
		break;

		/* Good Objects */
		case 'g':
		if (command_arg <= 0) command_arg = 1;
		acquirement(py, px, command_arg, FALSE, TRUE);
		break;

		/* Hitpoint rerating */
		case 'h':
		do_cmd_rerate(); break;

#ifdef MONSTER_HORDES
		case 'H':
		do_cmd_summon_horde(); break;
#endif /* MONSTER_HORDES */

		/* Identify */
		case 'i':
		(void)ident_spell();
		break;

		/* Go up or down in the dungeon */
		case 'j':
		do_cmd_wiz_jump();
		break;

		/* Self-Knowledge */
		case 'k':
			self_knowledge();
			break;

		/* Learn about objects */
		case 'l':
			do_cmd_wiz_learn();
			break;

		/* Magic Mapping */
		case 'm':
			map_area();
			break;

		/* Mutation */
		case 'M':
			(void) gain_random_mutation(command_arg);
			break;

		/* Specific reward */
		case 'r':
			(void) gain_level_reward(command_arg);
			break;

		/* Summon _friendly_ named monster */
		case 'N':
			do_cmd_wiz_named_friendly(command_arg, TRUE);
			break;

		/* Summon Named Monster */
		case 'n':
			do_cmd_wiz_named(command_arg, TRUE);
			break;

		/* Object playing routines */
                case 'o':
			do_cmd_wiz_play();
			break;

		/* Phase Door */
		case 'p':
			teleport_player(10);
			break;

		/* Complete a Quest -KMW- */
		case 'q':
		{
                        if (quest[command_arg].status == QUEST_STATUS_TAKEN)
                        {
                                quest[command_arg].status = QUEST_STATUS_COMPLETED;
                                msg_print("Completed Quest");
                                msg_print(NULL);
                                break;
                        }
			break;
		}

		/* Make every dungeon square "known" to test streamers -KMW- */
		case 'u':
		{
			for(y=0;y < cur_hgt;y++)
			{
				for(x=0;x < cur_wid;x++)
				{
					cave[y][x].info |= (CAVE_GLOW | CAVE_MARK);
				}
			}
			wiz_lite();
			break;
		}

		/* Summon Random Monster(s) */
		case 's':
			if (command_arg <= 0) command_arg = 1;
			do_cmd_wiz_summon(command_arg);
			break;

		/* Teleport */
		case 't':
			teleport_player(100);
			break;

                /* Teleport to a town */
                case 'T':
                        teleport_player_town(command_arg);
			break;

		/* Very Good Objects */
		case 'v':
			if (command_arg <= 0) command_arg = 1;
			acquirement(py, px, command_arg, TRUE, TRUE);
			break;

		/* Wizard Light the Level */
		case 'w':
		wiz_lite();
		break;

                /* Make a wish */
                case 'W':
                make_wish();
		break;

		/* Increase Experience */
		case 'x':
		if (command_arg)
		{
			gain_exp(command_arg);
		}
		else
		{
			gain_exp(p_ptr->exp + 1);
		}
		break;

		/* Zap Monsters (Genocide) */
		case 'z':
		do_cmd_wiz_zap();
		break;

		/* Hack -- whatever I desire */
		case '_':
		do_cmd_wiz_hack_ben();
		break;

                /* Mimic shape changing */
                case '*':
                p_ptr->tim_mimic=100;
                p_ptr->mimic_form=command_arg;
                /* Redraw title */
                p_ptr->redraw |= (PR_TITLE);
                /* Recalculate bonuses */
                p_ptr->update |= (PU_BONUS);
                break;

                /* Gain a fate */
                case '+':
                {
                int i;
                gain_fate(command_arg);
                for(i = 0; i < MAX_FATES; i++)
                        fates[i].know = TRUE;
                break;
                }

                /* Change the feature of the map */
                case 'F':
                msg_format("Old feature: %d", cave[py][px].feat);
                cave_set_feat(py,px,command_arg);
                break;

                case '/':
                summon_specific(py,px,p_ptr->max_dlv[dungeon_type],command_arg);
                break;

		/* Not a Wizard Command */
		default:
		msg_print("That is not a valid debug command.");
		break;
	}
}


#else

#ifdef MACINTOSH
static int i = 0;
#endif

#endif

