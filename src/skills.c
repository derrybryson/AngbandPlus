/* File: skills.c */

/* Purpose: player skills */

/*
 * Copyright (c) 2001 DarkGod
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"


/*
 * Advance the skill point of the skill specified by i and
 * modify related skills
 */
void increase_skill(int i, s16b *invest)
{
	/* No skill points to be allocated */
	if (!p_ptr->skill_points) return;

	/* The skill cannot be increased */
	if (!s_info[i].mod) return;

	/* The skill is already maxed */
        if (s_info[i].value >= SKILL_MAX) return;

        /* Cannot allocate more than player level + 10 levels */
        if (((s_info[i].value + s_info[i].mod) / SKILL_STEP) >= (p_ptr->lev + 6))
        {
                int hgt, wid;

		Term_get_size(&wid, &hgt);
                msg_box("Cannot raise a skill value above 5 + player level.", (int)(hgt / 2), (int)(wid / 2));
                return;
        }

	/* Spend an unallocated skill point */
	p_ptr->skill_points--;

	/* Increase the skill */
        s_info[i].value += s_info[i].mod;
        invest[i]++;
}


/*
 * Descrease the skill point of the skill specified by i and
 * modify related skills
 */
void decrease_skill(int i, s16b *invest)
{
        /* Cannot decrease more */
        if (!invest[i]) return;

	/* The skill cannot be decreased */
	if (!s_info[i].mod) return;

	/* The skill already has minimal value */
	if (!s_info[i].value) return;

	/* Free a skill point */
	p_ptr->skill_points++;

	/* Decrease the skill */
	s_info[i].value -= s_info[i].mod;
        invest[i]--;
}


/*
 * Given the name of a skill, returns skill index or -1 if no
 * such skill is found
 */
s16b find_skill(cptr name)
{
	u16b i;

	/* Scan skill list */
	for (i = 1; i < max_s_idx; i++)
	{
		/* The name matches */
		if (streq(s_info[i].name + s_name, name)) return (i);
	}

	/* No match found */
	return (-1);
}
s16b find_skill_i(cptr name)
{
	u16b i;

	/* Scan skill list */
	for (i = 1; i < max_s_idx; i++)
	{
		/* The name matches */
		if (0 == stricmp(s_info[i].name + s_name, name)) return (i);
	}

	/* No match found */
	return (-1);
}


/*
 *
 */
s16b get_skill(int skill)
{
	return (s_info[skill].value / SKILL_STEP);
}


/*
 * Return "scale" (a misnomer -- this is max value) * (current skill value)
 * / (max skill value)
 */
s16b get_skill_scale(int skill, u32b scale)
{
#if 0
	/* XXX XXX XXX */
	return (((s_info[skill].value / 10) * (scale * (SKILL_STEP / 10)) /
	         (SKILL_MAX / 10)) /
	        (SKILL_STEP / 10));
#else

	u32b temp;

	/*
	 * SKILL_STEP shouldn't matter here because the second parameter is
	 * relatively small (the largest one being somewhere around 200),
	 * AND because we could have used much simpler 0--50 if the ability
	 * progression were only possible at step boundaries.
	 *
	 * Because I'm not at all certain about my interpretation of the mysterious
	 * formula given above, I verified this works the same by using a tiny
	 * scheme program... -- pelpel
	 */
	temp = scale * s_info[skill].value;

	return (temp / SKILL_MAX);

#endif
}


/*
 *
 */
int get_idx(int i)
{
	int j;

	for (j = 1; j < max_s_idx; j++)
	{
		if (s_info[j].order == i)
			return (j);
	}
	return (0);
}

static bool is_known(int s_idx)
{
        int i;

        if (wizard) return TRUE;
        if (s_info[s_idx].value || s_info[s_idx].mod) return TRUE;

        for (i = 0; i < max_s_idx; i++)
        {
                /* It is our child, if we dont know it we contiue to search, if we know it it is enough*/
                if (s_info[i].father == s_idx)
                {
                        if (is_known(i))
                                return TRUE;
                }
        }

        /* Ok know none */
        return FALSE;
}

/*
 *
 */
void init_table_aux(int table[MAX_SKILLS][2], int *idx, int father, int lev,
	bool full)
{
	int j, i;

	for (j = 1; j < max_s_idx; j++)
	{
		i = get_idx(j);
		if (s_info[i].father != father) continue;
                if (s_info[i].hidden) continue;
                if (!is_known(i)) continue;

		table[*idx][0] = i;
		table[*idx][1] = lev;
		(*idx)++;
		if (s_info[i].dev || full) init_table_aux(table, idx, i, lev + 1, full);
	}
}


void init_table(int table[MAX_SKILLS][2], int *max, bool full)
{
	*max = 0;
	init_table_aux(table, max, -1, 0, full);
}


bool has_child(int sel)
{
	int i;

	for (i = 1; i < max_s_idx; i++)
	{
                if ((s_info[i].father == sel) && (is_known(i)))
			return (TRUE);
	}
	return (FALSE);
}


/*
 * Dump the skill tree
 */
void dump_skills(FILE *fff)
{
	int i, j, max = 0;
	int table[MAX_SKILLS][2];
	char buf[80];

	init_table(table, &max, TRUE);

	Term_clear();

	fprintf(fff, "\nSkills (points left: %d)", p_ptr->skill_points);

	for (j = 0; j < max; j++)
	{
		int z;

		i = table[j][0];

		if ((s_info[i].value == 0) && (i != SKILL_MISC))
		{
			if (s_info[i].mod == 0) continue;
		}

		sprintf(buf, "\n");

		for (z = 0; z < table[j][1]; z++) strcat(buf, "	 ");

		if (!has_child(i))
		{
			strcat(buf, format(" . %s", s_info[i].name + s_name));
		}
		else
		{
			strcat(buf, format(" - %s", s_info[i].name + s_name));
		}

		fprintf(fff, "%-50s%02ld.%03ld [%01d.%03d]",
		        buf, s_info[i].value / SKILL_STEP, s_info[i].value % SKILL_STEP,
		        s_info[i].mod / 1000, s_info[i].mod % 1000);
	}

	fprintf(fff, "\n");
}


/*
 * Draw the skill tree
 */
void print_skills(int table[MAX_SKILLS][2], int max, int sel, int start)
{
	int i, j;
        int wid, hgt;
        cptr keys;

	Term_clear();
	Term_get_size(&wid, &hgt);

        c_prt(TERM_WHITE, "ToME Skills Screen", 0, 28);
        keys = format("#BEnter#W to develop a branch, #Bup#W/#Bdown#W to move, #Bright#W/#Bleft#W to modify, #B?#W for help");
        display_message(0, 1, strlen(keys), TERM_WHITE, keys);
	c_prt((p_ptr->skill_points) ? TERM_L_BLUE : TERM_L_RED,
	      format("Skill points left: %d", p_ptr->skill_points), 2, 0);
	print_desc_aux(s_info[table[sel][0]].desc + s_text, 3, 0);

	for (j = start; j < start + (hgt - 5); j++)
	{
		byte color = TERM_WHITE;
		char deb = ' ', end = ' ';

		if (j >= max) break;

		i = table[j][0];

		if ((s_info[i].value == 0) && (i != SKILL_MISC))
		{
			if (s_info[i].mod == 0) color = TERM_L_DARK;
			else color = TERM_ORANGE;
		}
		else if (s_info[i].value == SKILL_MAX) color = TERM_L_BLUE;
		if (s_info[i].hidden) color = TERM_L_RED;
		if (j == sel)
		{
			color = TERM_L_GREEN;
			deb = '[';
			end = ']';
		}
		if (!has_child(i))
		{
			c_prt(color, format("%c.%c%s", deb, end, s_info[i].name + s_name),
			      j + 5 - start, table[j][1] * 4);
		}
		else if (s_info[i].dev)
		{
			c_prt(color, format("%c-%c%s", deb, end, s_info[i].name + s_name),
			      j + 5 - start, table[j][1] * 4);
		}
		else
		{
			c_prt(color, format("%c+%c%s", deb, end, s_info[i].name + s_name),
			      j + 5 - start, table[j][1] * 4);
		}
		c_prt(color,
		      format("%02ld.%03ld [%01d.%03d]",
			         s_info[i].value / SKILL_STEP, s_info[i].value % SKILL_STEP,
			         s_info[i].mod / 1000, s_info[i].mod % 1000),
			  j + 5 - start, 60);
	}
}

/*
 * Checks various stuff to do when skills change, like new spells, ...
 */
void recalc_skills(bool init)
{
        static int thaum_level = 0;

        if (init)
        {
                thaum_level = get_skill_scale(SKILL_THAUMATURGY, 100);
        }
        else
        {
                int thaum_gain = 0;

                /* Gain thaum spells */
                while (thaum_level < get_skill_scale(SKILL_THAUMATURGY, 100))
                {
                        if (spell_num == MAX_SPELLS) break;
                        generate_spell(thaum_level);
                        thaum_level++;
                        thaum_gain++;
                }
                if (thaum_gain)
                {
                        if (thaum_gain == 1)
                                msg_print("You have gained one new thaumaturgy spell.");
                        else
                                msg_format("You have gained %d new thaumaturgy spells.", thaum_gain);
                }

                process_hooks(HOOK_RECALC_SKILLS, "()");

		/* Update stuffs */
		p_ptr->update |= (PU_BONUS | PU_HP | PU_MANA | PU_SPELLS | PU_POWERS |
	                      PU_SANITY | PU_BODY);

		/* Redraw various info */
		p_ptr->redraw |= (PR_WIPE | PR_BASIC | PR_EXTRA | PR_MAP);
        }
}

/*
 * Recalc the skill value
 */
void recalc_skills_theory(s16b *invest, s32b *base_val, u16b *base_mod, s32b *bonus)
{
        int i, j;

        /* First we assign the normal points */
        for (i = 0; i < max_s_idx; i++)
        {
                /* Calc the base */
                s_info[i].value = base_val[i] + (base_mod[i] * invest[i]) + bonus[i];

                /* It cannot exceed SKILL_MAX */
                if (s_info[i].value > SKILL_MAX) s_info[i].value = SKILL_MAX;

                /* It cannot go below 0 */
                if (s_info[i].value < 0) s_info[i].value = 0;
        }

        /* Then we modify related skills */
        for (i = 0; i < max_s_idx; i++)
        {
                for (j = 1; j < max_s_idx; j++)
                {
                        /* Ignore self */
                        if (j == i) continue;

                        /* Exclusive skills */
                        if ((s_info[i].action[j] == SKILl_EXCLUSIVE) && invest[i])
                        {
                                /* Turn it off */
                                s_info[j].value = 0;
                        }

                        /* Non-exclusive skills */
                        else if (s_info[i].action[j])
                        {
                                /* Increase / decrease with a % */
                                s32b val = s_info[j].value + (invest[i] * s_info[j].mod * s_info[i].action[j] / 100);

                                /* Skill value cannot be negative */
                                if (val < 0) val = 0;

                                /* It cannot exceed SKILL_MAX */
                                if (val > SKILL_MAX) val = SKILL_MAX;

                                /* Save the modified value */
                                s_info[j].value = val;
                        }
                }
        }
}

/*
 * Interreact with skills
 */
void do_cmd_skill()
{
	int sel = 0, start = 0, max;
	char c;
	int table[MAX_SKILLS][2];
	int i;
	int wid,hgt;
	s16b skill_points_save;
	s32b *skill_values_save;
	u16b *skill_mods_save;
	s16b *skill_rates_save;
	s16b *skill_invest;
	s32b *skill_bonus;

	recalc_skills(TRUE);

	/* Save the screen */
	screen_save();

	/* Allocate arrays to save skill values */
	C_MAKE(skill_values_save, MAX_SKILLS, s32b);
	C_MAKE(skill_mods_save, MAX_SKILLS, u16b);
	C_MAKE(skill_rates_save, MAX_SKILLS, s16b);
	C_MAKE(skill_invest, MAX_SKILLS, s16b);
	C_MAKE(skill_bonus, MAX_SKILLS, s32b);

	/* Save skill points */
	skill_points_save = p_ptr->skill_points;

	/* Save skill values */
	for (i = 0; i < max_s_idx; i++)
	{
		skill_type *s_ptr = &s_info[i];

		skill_values_save[i] = s_ptr->value;
		skill_mods_save[i] = s_ptr->mod;
		skill_rates_save[i] = s_ptr->rate;
		skill_invest[i] = 0;
	}

	/* Clear the screen */
	Term_clear();

	/* Initialise the skill list */
	init_table(table, &max, FALSE);

	while (TRUE)
	{
		Term_get_size(&wid, &hgt);

                /* Display list of skills */
                recalc_skills_theory(skill_invest, skill_values_save, skill_mods_save, skill_bonus);
                print_skills(table, max, sel, start);

		/* Wait for user input */
		c = inkey();

		/* Leave the skill screen */
		if (c == ESCAPE) break;

		/* Expand / collapse list of skills */
		else if (c == '\r')
		{
			if (s_info[table[sel][0]].dev) s_info[table[sel][0]].dev = FALSE;
			else s_info[table[sel][0]].dev = TRUE;
			init_table(table, &max, FALSE);
		}

		/* Next page */
		else if (c == 'n')
		{
			sel += (hgt - 5);
			if (sel >= max) sel = max - 1;
		}

		/* Previous page */
		else if (c == 'p')
		{
			sel -= (hgt - 5);
			if (sel < 0) sel = 0;
		}

		/* Select / increase a skill */
		else
		{
			int dir;

			/* Allow use of numpad / arrow keys / roguelike keys */
			dir = get_keymap_dir(c);

			/* Move cursor down */
			if (dir == 2) sel++;

			/* Move cursor up */
			if (dir == 8) sel--;

			/* Miscellaneous skills cannot be increased/decreased as a group */
			if (table[sel][0] == SKILL_MISC) continue;

			/* Increase the current skill */
			if (dir == 6) increase_skill(table[sel][0], skill_invest);

			/* Decrease the current skill */
			if (dir == 4) decrease_skill(table[sel][0], skill_invest);

			/* XXX XXX XXX Wizard mode commands outside of wizard2.c */

			/* Increase the skill */
			if (wizard && (c == '+')) skill_bonus[table[sel][0]] += SKILL_STEP;

			/* Decrease the skill */
			if (wizard && (c == '-')) skill_bonus[table[sel][0]] -= SKILL_STEP;

                        /* Contextual help */
			if (c == '?') exec_lua(format("ingame_help('select_context', 'skill', '%s')", s_info[table[sel][0]].name + s_name));;

			/* Handle boundaries and scrolling */
			if (sel < 0) sel = max - 1;
			if (sel >= max) sel = 0;
			if (sel < start) start = sel;
			if (sel >= start + (hgt - 5)) start = sel - (hgt - 5) + 1;
		}
	}


	/* Some skill points are spent */
	if (p_ptr->skill_points != skill_points_save)
	{
		/* Flush input as we ask an important and irreversible question */
		flush();

		/* Ask we can commit the change */
		if (msg_box("Save and use these skill values? (y/n)", (int)(hgt / 2), (int)(wid / 2)) != 'y')
		{
			/* User declines -- restore the skill values before exiting */

			/* Restore skill points */
			p_ptr->skill_points = skill_points_save;

			/* Restore skill values */
			for (i = 0; i < max_s_idx; i++)
			{
				skill_type *s_ptr = &s_info[i];

				s_ptr->value = skill_values_save[i];
				s_ptr->mod = skill_mods_save[i];
				s_ptr->rate = skill_rates_save[i];
			}
		}
	}


	/* Free arrays to save skill values */
	C_FREE(skill_values_save, MAX_SKILLS, s32b);
	C_FREE(skill_mods_save, MAX_SKILLS, u16b);
	C_FREE(skill_rates_save, MAX_SKILLS, s16b);
	C_FREE(skill_invest, MAX_SKILLS, s16b);
	C_FREE(skill_bonus, MAX_SKILLS, s32b);

	/* Load the screen */
	screen_load();

        recalc_skills(FALSE);
}



/*
 * List of melee skills
 */
s16b melee_skills[MAX_MELEE] =
{
	SKILL_MASTERY,
	SKILL_HAND,
	SKILL_BEAR,
};
char *melee_names[MAX_MELEE] =
{
	"Weapon combat",
	"Barehanded combat",
	"Bearform combat",
};
static bool melee_bool[MAX_MELEE];
static int melee_num[MAX_MELEE];

s16b get_melee_skill()
{
	int i;

	for (i = 0; i < MAX_MELEE; i++)
	{
		if (p_ptr->melee_style == melee_skills[i])
			return (i);
	}
	return (0);
}

s16b get_melee_skills()
{
	int i, j = 0;

	for (i = 0; i < MAX_MELEE; i++)
	{
		if (s_info[melee_skills[i]].value && (!s_info[melee_skills[i]].hidden))
		{
			melee_bool[i] = TRUE;
			j++;
		}
		else
			melee_bool[i] = FALSE;
	}

	return (j);
}

static void choose_melee()
{
	int i, j, z = 0;

	character_icky = TRUE;
	Term_save();
	Term_clear();

	j = get_melee_skills();
	prt("Choose a melee style:", 0, 0);
	for (i = 0; i < MAX_MELEE; i++)
	{
		if (melee_bool[i])
		{
			prt(format("%c) %s", I2A(z), melee_names[i]), z + 1, 0);
			melee_num[z] = i;
			z++;
		}
	}

	while (TRUE)
	{
		char c = inkey();

		if (c == ESCAPE) break;
		if (A2I(c) < 0) continue;
		if (A2I(c) >= j) continue;

		for (i = 0, z = 0; z < A2I(c); i++)
			if (melee_bool[i]) z++;
		p_ptr->melee_style = melee_skills[melee_num[z]];
                for (i = INVEN_WIELD; p_ptr->body_parts[i - INVEN_WIELD] == INVEN_WIELD; i++)
                {
                        if (inventory[i].k_idx)
                                inven_takeoff(i, 255, FALSE);
                }
		energy_use = 100;
		break;
	}

	Term_load();
	character_icky = FALSE;
}

void select_default_melee()
{
	int i;

        get_melee_skills();
        p_ptr->melee_style = SKILL_MASTERY;
	for (i = 0; i < MAX_MELEE; i++)
	{
		if (melee_bool[i])
		{
                        p_ptr->melee_style = melee_skills[i];
                        break;
		}
	}
}

/*
 * Print a batch of skills.
 */
static void print_skill_batch(int *p, int start, int max, bool mode)
{
	char buff[80];
	int i = start, j = 0;

	if (mode) prt(format("         %-31s", "Name"), 1, 20);

	for (i = start; i < (start + 20); i++)
	{
		if (i >= max) break;

                /* Hack -- only able to learn spells when learning is required */
                if ((p[i] == SKILL_LEARN) && (!must_learn_spells()))
                        continue;
                else if (p[i] > 0)
			sprintf(buff, "  %c-%3d) %-30s", I2A(j), p[i] + 1, s_text + s_info[p[i]].action_desc);
		else
			sprintf(buff, "  %c-%3d) %-30s", I2A(j), 1, "Change melee style");

		if (mode) prt(buff, 2 + j, 20);
		j++;
	}
	if (mode) prt("", 2 + j, 20);
	prt(format("Select a skill (a-%c), @ to select by name, +/- to scroll:", I2A(j - 1)), 0, 0);
}

int do_cmd_activate_skill_aux()
{
	char which;
	int max = 0, i, start = 0;
	int ret;
	bool mode = FALSE;
	int *p;

	C_MAKE(p, max_s_idx, int);

	/* Count the max */

	/* More than 1 melee skill ? */
	if (get_melee_skills() > 1)
	{
		p[max++] = 0;
	}

	for (i = 1; i < max_s_idx; i++)
	{
		if (s_info[i].action_mkey && s_info[i].value && ((!s_info[i].hidden) || (i == SKILL_LEARN)))
		{
			int j;
			bool next = FALSE;

			/* Already got it ? */
                        for (j = 0; j < max; j++)
                        {
				if (s_info[i].action_mkey == s_info[p[j]].action_mkey)
				{
					next = TRUE;
					break;
				}
                        }
                        if (next) continue;

                        /* Hack -- only able to learn spells when learning is required */
                        if ((i == SKILL_LEARN) && (!must_learn_spells()))
                                continue;
                        p[max++] = i;
		}
	}

	if (!max)
	{
		msg_print("You dont have any activable skills.");
		return -1;
	}

	character_icky = TRUE;
	Term_save();

	while (1)
	{
		print_skill_batch(p, start, max, mode);
		which = inkey();

		if (which == ESCAPE)
		{
			ret = -1;
			break;
		}
		else if (which == '*' || which == '?' || which == ' ')
		{
			mode = (mode)?FALSE:TRUE;
			Term_load();
			character_icky = FALSE;
		}
		else if (which == '+')
		{
			start += 20;
			if (start >= max) start -= 20;
			Term_load();
			character_icky = FALSE;
		}
		else if (which == '-')
		{
			start -= 20;
			if (start < 0) start += 20;
			Term_load();
			character_icky = FALSE;
		}
		else if (which == '@')
                {
                        char buf[80];

                        strcpy(buf, "Cast a spell");
                        if (!get_string("Skill action? ", buf, 79))
                                return FALSE;

                        /* Find the skill it is related to */
                        for (i = 1; i < max_s_idx; i++)
                        {
                                if (!strcmp(buf, s_info[i].action_desc + s_text) && get_skill(i))
                                        break;
                        }
                        if ((i < max_s_idx))
                        {
                                ret = i;
                                break;
                        }

		}
		else
		{
			which = tolower(which);
			if (start + A2I(which) >= max)
			{
				bell();
				continue;
			}
			if (start + A2I(which) < 0)
			{
				bell();
				continue;
			}

			ret = p[start + A2I(which)];
			break;
		}
	}
	Term_load();
	character_icky = FALSE;

	C_FREE(p, max_s_idx, int);

	return ret;
}

/* Ask & execute a skill */
void do_cmd_activate_skill()
{
	int x_idx;
	bool push = TRUE;

	/* Get the skill, if available */
	if (repeat_pull(&x_idx))
	{
		if ((x_idx < 0) || (x_idx >= max_s_idx)) return;
		push = FALSE;
	}
	else if (!command_arg) x_idx = do_cmd_activate_skill_aux();
	else
	{
		x_idx = command_arg - 1;
		if ((x_idx < 0) || (x_idx >= max_s_idx)) return;
                if (((s_info[x_idx].hidden) && (x_idx != SKILL_LEARN)) || (!s_info[x_idx].value) || (!s_info[x_idx].action_mkey))
                {
                        msg_print("You cannot use this skill.");
                        return;
                }
	}

	if (x_idx == -1) return;

	if (push) repeat_push(x_idx);

	if (!x_idx)
	{
		choose_melee();
		return;
	}

	/* Break goi/manashield */
	if (p_ptr->invuln)
	{
		set_invuln(0);
	}
	if (p_ptr->disrupt_shield)
	{
		set_disrupt_shield(0);
	}

	switch (s_info[x_idx].action_mkey)
	{
		case MKEY_ANTIMAGIC:
			do_cmd_unbeliever();
			break;
		case MKEY_MINDCRAFT:
			do_cmd_mindcraft();
			break;
		case MKEY_ALCHEMY:
			do_cmd_alchemist();
			break;
		case MKEY_MIMIC:
			do_cmd_mimic();
			break;
		case MKEY_POWER_MAGE:
			do_cmd_powermage();
			break;
		case MKEY_RUNE:
			do_cmd_runecrafter();
			break;
		case MKEY_FORGING:
			do_cmd_archer();
			break;
		case MKEY_INCARNATION:
			do_cmd_possessor();
			break;
		case MKEY_TELEKINESIS:
			do_cmd_portable_hole();
			break;
		case MKEY_REALM:
			do_cmd_cast();
			break;
		case MKEY_BLADE:
			do_cmd_blade();
			break;
		case MKEY_SUMMON:
			do_cmd_summoner();
			break;
		case MKEY_NECRO:
			do_cmd_necromancer();
			break;
		case MKEY_SYMBIOTIC:
			do_cmd_symbiotic();
			break;
		case MKEY_TRAP:
			do_cmd_set_trap();
			break;
		case MKEY_STEAL:
			do_cmd_steal();
			break;
		case MKEY_DODGE:
			use_ability_blade();
			break;
		case MKEY_SCHOOL:
			cast_school_spell();
			break;
                case MKEY_LEARN:
                        do_cmd_study();
			break;
                case MKEY_COPY:
                        do_cmd_copy_spell();
                        break;
		default:
			process_hooks(HOOK_MKEY, "(d)", s_info[x_idx].action_mkey);
			break;
	}
}


/* Which magic forbids non FA gloves */
bool forbid_gloves()
{
	if (get_skill(SKILL_SORCERY)) return (TRUE);
	if (get_skill(SKILL_MANA)) return (TRUE);
	if (get_skill(SKILL_FIRE)) return (TRUE);
	if (get_skill(SKILL_AIR)) return (TRUE);
	if (get_skill(SKILL_WATER)) return (TRUE);
	if (get_skill(SKILL_EARTH)) return (TRUE);
	if (get_skill(SKILL_THAUMATURGY)) return (TRUE);
	return (FALSE);
}

/* Which gods forbid edged weapons */
bool forbid_non_blessed()
{
	GOD(GOD_ERU) return (TRUE);
	return (FALSE);
}




/*
 * The autoskiller gets fed with the desired skill values at level 50 and determines
 * where to invest each levels
 */

/* Checks if a given autoskill chart is realisable */
int validate_autoskiller(s32b *ideal)
{
	int i;
	s32b count = 0;

	for (i = 0; i < max_s_idx; i++)
	{
		s32b z, c;

		skill_type *s_ptr = &s_info[i];

		if (!ideal[i]) continue;

		/* How much */
		z = (ideal[i] * SKILL_STEP) - s_ptr->value;

		/* How many points will we need to get there ? */
		if (s_ptr->mod)
			c = z / s_ptr->mod;
		else
			c = 99999;
		count += c;
	}
	return (SKILL_NB_BASE * 49) - count;
}

void autoskiller_level(s32b *ideal)
{
#if 0
	/* DGDGDGDG */
	int chart[196];
	int final[196];
	int ideal[3] = {50, 30, 25};
	int real[3] = {0, 0, 0};
	int mod[3] = {450, 1000, 1100};
	float pl[3], left[3] = {0, 0, 0}, finalization = 1;
	int c[3], z;
	int i, max = 0, ok = 3, a;

	for (i = 0; i < 3; i++)
	{
		c[i] = (ideal[i] * 1000) / mod[i];
		printf("point need c[%d] = %d\n", i, c[i]);
		max += c[i];

		pl[i] = (float)c[i] / 50.0;
		printf("pl[%d] = %f\n", i, pl[i]);
	}
	printf("skill balance %d\n", 196 - max);

	for (i = 0; i < 196; i++)
	{
		chart[i] = -1;
	}

	a = 0;
	while ((a < 196) && (ok))
	{
		int z = 0;

		for (z = 0; z < 3; z++)
		{
			int tmp, ii;

			if (real[z] == c[z])
				continue;
			left[z] += pl[z];
			tmp = left[z];
			left[z] -= tmp;
			printf("skill %02d: left %f tmp %d\n", z, left[z], tmp);

			while (tmp >= 1)
			{
				chart[a++] = z;
				real[z]++;
				tmp--;
				if (real[z] == c[z])
				{
					ok--;
					break;
				}
				if (a == 196)
				{
					ok = 0;
					break;
				}
			}
			if (!ok) break;
		}
	}
	printf("ok %d, a %d interval %f\n", ok, a, (float)a / 196);
	real[0] = real[1] = real[2] = 0;
	for (i = 0; i < 196; i++)
	{
		real[chart[i]]++;
	}
	z = 0;
	i = 0;
	while (z < a)
	{
		if (finalization > 1)
		{
			final[i] = chart[z++];
			finalization--;
		}
		else
			final[i] = -1;
		finalization += ((float)a / 196);
		i++;
	}
	for (z = 0; z < 3; z++)
	{
		printf("skill %d real %d ideal %d\n", z, real[z], c[z]);
	}

	for (i = 2; i <= 50; i++)
	{
		printf("level %02d, skills %02d %02d %02d %02d\n", i, final[i * 4], final[i * 4 + 1], final[i * 4 + 2], final[i * 4 + 3]);
	}
#endif
}

/*
 * Gets the base value of a skill, given a race/class/...
 */
void compute_skills(s32b *v, s32b *m, int i)
{
	s32b value, mod;

	/***** general skills *****/

	/* If the skill correspond to the magic realms lets pump them a bit */
	value = gen_skill_base[i];
	mod = gen_skill_mod[i];

	*v = modify_aux(*v,
	                value, gen_skill_basem[i]);
	*m = modify_aux(*m,
	                mod, gen_skill_modm[i]);

	/***** race skills *****/

	value = rp_ptr->skill_base[i];
	mod = rp_ptr->skill_mod[i];

	*v = modify_aux(*v,
	                value, rp_ptr->skill_basem[i]);
	*m = modify_aux(*m,
	                mod, rp_ptr->skill_modm[i]);

	/***** race mod skills *****/

	value = rmp_ptr->skill_base[i];
	mod = rmp_ptr->skill_mod[i];

	*v = modify_aux(*v,
	                value, rmp_ptr->skill_basem[i]);
	*m = modify_aux(*m,
	                mod, rmp_ptr->skill_modm[i]);

	/***** class skills *****/

	value = cp_ptr->skill_base[i];
	mod = cp_ptr->skill_mod[i];

	*v = modify_aux(*v,
	                value, cp_ptr->skill_basem[i]);
	*m = modify_aux(*m,
	                mod, cp_ptr->skill_modm[i]);

	/***** class spec skills *****/

	value = spp_ptr->skill_base[i];
	mod = spp_ptr->skill_mod[i];

	*v = modify_aux(*v,
	                value, spp_ptr->skill_basem[i]);
	*m = modify_aux(*m,
	                mod, spp_ptr->skill_modm[i]);
}

/*
 * Initialize a skill with given values
 */
void init_skill(u32b value, s16b mod, int i)
{
        s_info[i].value = value;
        s_info[i].mod = mod;

        if (s_info[i].flags1 & SKF1_HIDDEN)
                s_info[i].hidden = TRUE;
        else
                s_info[i].hidden = FALSE;
}

void do_get_new_skill()
{
        char *items[4];
        int skl[4];
        u32b val[4], mod[4];
        bool used[MAX_SKILLS];
        int available_skills[MAX_SKILLS];
        int max = 0, max_a = 0, res, i;

        /* Check if some skills didnt influence other stuff */
        recalc_skills(TRUE);

        /* Grab the ones we can gain */
        max = 0;
        for (i = 0; i < max_s_idx; i++)
        {
                if (s_info[i].flags1 & SKF1_RANDOM_GAIN)
                        available_skills[max++] = i;
        }
        available_skills[max++] = -1;

        /* Init */
        for (max = 0; max < MAX_SKILLS; max++)
        {
                used[max] = FALSE;
        }

        /* Count the number of available skills */
        while (available_skills[max_a] != -1) max_a++;

        /* Get 4 skills */
        for (max = 0; max < 4; max++)
        {
                int i;
                skill_type *s_ptr;

                /* Get an non used skill */
                do
                {
                        i = rand_int(max_a);
                } while (used[available_skills[i]]);

                s_ptr = &s_info[available_skills[i]];
                used[available_skills[i]] = TRUE;

                if (s_ptr->mod)
                {
                        val[max] = s_ptr->mod * 3;
                        mod[max] = 0;
                }
                else
                {
                        mod[max] = 500;
                        val[max] = 1000;
                }
                if (s_ptr->value + val[max] > SKILL_MAX) val[max] = SKILL_MAX - s_ptr->value;
                skl[max] = available_skills[i];
                items[max] = (char *)string_make(format("%-40s: +%02ld.%03ld value, +%01d.%03d modifier", s_ptr->name + s_name, val[max] / SKILL_STEP, val[max] % SKILL_STEP, mod[max] / SKILL_STEP, mod[max] % SKILL_STEP));
        }

        while (TRUE)
        {
                bool oppose = FALSE;
                int oppose_skill = -1;

                res = ask_menu("Choose a skill to learn(a-d to choose, ESC to cancel)?", (char **)items, 4);

                /* Check we dont oppose an existing skill */
                for (i = 0; i < max_s_idx; i++)
                {
                        if ((s_info[i].action[skl[res]] == SKILl_EXCLUSIVE) && (s_info[i].value))
                        {
                                oppose = TRUE;
                                oppose_skill = i;
                                break;
                        }
                }

                /* Ok we oppose, so be sure */
                if (oppose)
				{
					cptr msg;

					/*
					 * Because this is SO critical a question, we must flush
					 * input to prevent killing character off -- pelpel
					 */
					flush();

					/* Prepare prompt */
					msg = format("This skill is mutualy exclusive with "
								 "at least %s, continue?",
								 s_info[oppose_skill].name + s_name);

					/* The player rejected the choice */
					if (!get_check(msg)) continue;
				}

                /* Ok ? lets learn ! */
                if (res > -1)
                {
                        skill_type *s_ptr = &s_info[skl[res]];
                        s_ptr->value += val[res];
                        s_ptr->mod += mod[res];
                        if (mod[res])
                                msg_format("You can now learn the %s skill.", s_ptr->name + s_name);
                        else
                                msg_format("Your knowledge of the %s skill increase.", s_ptr->name + s_name);
                }
                break;
        }

        /* Free them ! */
	for (max = 0; max < 4; max++)
                string_free(items[max]);

        /* Check if some skills didnt influence other stuff */
        recalc_skills(FALSE);
}
