/* File: bldg.c */

/*
 * Purpose: Building commands
 * Created by Ken Wigle for Kangband - a variant of Angband 2.8.3
 * -KMW-
 *
 * Rewritten for Kangband 2.8.3i using Kamband's version of
 * bldg.c as written by Ivan Tkatchev
 *
 * Changed for ZAngband by Robert Ruehlmann
 */

#include "angband.h"

/* hack as in leave_store in store.c */
static bool leave_bldg = FALSE;

static bool can_eat_food(void)
{
	if (rp_ptr->r_flags & PRF_NO_DIGEST)
	{
		return FALSE;
	}

	if (cp_ptr->c_flags & PCF_NO_DIGEST)
	{
		return FALSE;
	}

	if (p_ptr->pclass == CLASS_VAMPIRE) return FALSE;

	return TRUE;
}


/*
 * Clear the building information
 */
static void clear_bldg(int min_row, int max_row)
{
	int   i;

	for (i = min_row; i <= max_row; i++)
		prt("", i, 0);
}

static void building_prt_gold(void)
{
	char tmp_str[80];

#ifdef JP
	prt("手持ちのお金: ", 23,53);
#else
	prt("Gold Remaining: ", 23, 53);
#endif


	sprintf(tmp_str, "%9ld", (long)p_ptr->au_sum);
	prt(tmp_str, 23, 68);
}


/*
 * Display a building.
 */
static void show_building(building_type* bldg)
{
	char buff[20];
	int i;
	byte action_color = TERM_DARK;
	char tmp_str[80];

	Term_clear();
	sprintf(tmp_str, "%s (%s) %35s", bldg->owner_name, bldg->owner_race, bldg->name);
	prt(tmp_str, 2, 1);

	for (i = 0; i < 8; i++)
	{
		if (bldg->letters[i])
		{
			if (bldg->action_restr[i] == 0)
			{
				if (bldg->costs[i] == 0)
				{
					action_color = TERM_WHITE;
					buff[0] = '\0';
				}
				else
				{
					action_color = TERM_YELLOW;
#ifdef JP
					sprintf(buff, "($%ld)", bldg->costs[i]);
#else
					sprintf(buff, "(%ldgp)", bldg->costs[i]);
#endif

				}
			}
			else if (bldg->action_restr[i] == 1)
			{
				if (!can_eat_food())
				{
					action_color = TERM_L_DARK;
#ifdef JP
					strcpy(buff, "(閉店)");
#else
					strcpy(buff, "(closed)");
#endif

				}
				else if (can_eat_food() && (bldg->costs[i] == 0))
				{
					action_color = TERM_WHITE;
					buff[0] = '\0';
				}
				else
				{
					action_color = TERM_YELLOW;
#ifdef JP
					sprintf(buff, "($%ld)", bldg->costs[i]);
#else
					sprintf(buff, "(%ldgp)", bldg->costs[i]);
#endif

				}
			}

			sprintf(tmp_str," %c) %s %s", bldg->letters[i], bldg->act_names[i], buff);
			c_put_str(action_color, tmp_str, 19+(i/2), 35*(i%2));
		}
	}

#ifdef JP
	prt(" ESC) 建物を出る", 23, 0);
#else
	prt(" ESC) Exit building", 23, 0);
#endif

}


/*
 * arena commands
 */
static void arena_comm(int cmd)
{
	monster_race *r_ptr;
	cptr         name;


	switch (cmd)
	{
		case BACT_ARENA:
			if (p_ptr->arena_number == MAX_ARENA_MONS)
			{
				clear_bldg(5, 19);
#ifdef JP
				prt("アリーナの優勝者！", 5, 0);
				prt("おめでと〜！新しいチャンピオンに敬礼！", 7, 0); 
				prt("優勝賞金として$1,600,000あげる。大事に使ってね。", 8, 0);
#else
				prt("               Arena Victor!", 5, 0);
				prt("Congratulations!  You have defeated all before you.", 7, 0);
				prt("For that, receive the prize: 1,600,000 adamantite pieces", 8, 0);
#endif

				prt("", 10, 0);
				prt("", 11, 0);
				p_ptr->au[SV_GOLD_ADAMANTITE] += 1600000L;
				p_ptr->update |= (PU_GOLD);
#ifdef JP
				msg_print("スペースキーで続行");
#else
				msg_print("Press the space bar to continue");
#endif

				msg_print(NULL);
				p_ptr->arena_number++;
			}
			else if (p_ptr->arena_number > MAX_ARENA_MONS)
			{
				if (p_ptr->arena_number == (MAX_ARENA_MONS + 1))
				{
#ifdef JP
					msg_print("かのぷ〜があなたと戦いたいんだって！");
#else
					msg_format("Canopus wants to fight you!");
#endif

					msg_print(NULL);
#ifdef JP
					if (get_check("どうする？やってみる？"))
#else
					if (get_check("Do you fight? "))
#endif
					{
						p_ptr->exit_bldg = FALSE;
						reset_tim_flags();

						/* Save the surface floor as saved floor */
						prepare_change_floor_mode(CFM_SAVE_FLOORS);

						p_ptr->inside_arena = TRUE;
						p_ptr->leaving = TRUE;
						leave_bldg = TRUE;
					}
					else
					{
#ifdef JP
						msg_print("残念ね。");
#else
						msg_print("We are disappointed.");
#endif
					}
				}
				else if (p_ptr->arena_number == (MAX_ARENA_MONS + 3))
				{
#ifdef JP
					msg_print("すっご〜い！かのぷ〜に勝っちゃうなんて！");
#else
					msg_print("Excellent! You win Canopus!");
#endif

					msg_print(NULL);
#ifdef JP
					if (get_check("じゃあ私とやってみよっか？"))
#else
					if (get_check("Do you fight me? "))
#endif
					{
						p_ptr->exit_bldg = FALSE;
						reset_tim_flags();

						/* Save the surface floor as saved floor */
						prepare_change_floor_mode(CFM_SAVE_FLOORS);

						p_ptr->inside_arena = TRUE;
						p_ptr->leaving = TRUE;
						leave_bldg = TRUE;
					}
					else
					{
#ifdef JP
						msg_print("あなたもサイモンと大差ないのね。");
#else
						msg_print("You are weak same as Simon.");
#endif
					}
				}
				else
				{
#ifdef JP
					msg_print("あなたはアリーナに入り、しばらくの間栄光にひたった。");
#else
					msg_print("You enter the arena briefly and bask in your glory.");
#endif

					msg_print(NULL);
				}
			}
			else if (p_ptr->riding && (p_ptr->pclass != CLASS_BEASTTAMER) && (p_ptr->pclass != CLASS_DRAGONTAMER) && (p_ptr->pclass != CLASS_GENERAL) && (p_ptr->pclass != CLASS_FREYA))
			{
#ifdef JP
				msg_print("ペットに乗ったままではアリーナへ入れさせてもらえなかった。");
#else
				msg_print("You don't have permission to enter with pet.");
#endif

				msg_print(NULL);
			}
			else
			{
				p_ptr->exit_bldg = FALSE;
				reset_tim_flags();

				/* Save the surface floor as saved floor */
				prepare_change_floor_mode(CFM_SAVE_FLOORS);

				p_ptr->inside_arena = TRUE;
				p_ptr->leaving = TRUE;
				leave_bldg = TRUE;
			}
			break;
		case BACT_POSTER:
			if (p_ptr->arena_number == MAX_ARENA_MONS)
#ifdef JP
				msg_print("優勝おめでとう！アリーナでセレモニーをやるから来てね。");
#else
				msg_print("You are victorious. Enter the arena for the ceremony.");
#endif

			else if (p_ptr->arena_number > MAX_ARENA_MONS)
			{
#ifdef JP
				msg_print("あなたはすべての敵に勝利しました。");
#else
				msg_print("You have won against all foes.");
#endif
			}
			else
			{
				r_ptr = &r_info[arena_info[p_ptr->arena_number].r_idx];
				name = (r_name + r_ptr->name);
#ifdef JP
				msg_format("%s に挑戦する勇者はいませんかぁ〜？", name);
#else
				msg_format("Do I hear any challenges against: %s", name);
#endif
			}
			break;
		case BACT_ARENA_RULES:

			/* Save screen */
			screen_save();

			/* Peruse the arena help file */
#ifdef JP
			(void)show_file(TRUE, "arena_j.txt", NULL, 0, 0);
#else
			(void)show_file(TRUE, "arena.txt", NULL, 0, 0);
#endif


			/* Load screen */
			screen_load();

			break;
	}
}


/*
 * display fruit for dice slots
 */
static void display_fruit(int row, int col, int fruit)
{
	switch (fruit)
	{
		case 0: /* lemon */
#ifdef JP
			c_put_str(TERM_YELLOW, "   ####.", row, col);
			c_put_str(TERM_YELLOW, "  #    #", row + 1, col);
			c_put_str(TERM_YELLOW, " #     #", row + 2, col);
			c_put_str(TERM_YELLOW, "#      #", row + 3, col);
			c_put_str(TERM_YELLOW, "#      #", row + 4, col);
			c_put_str(TERM_YELLOW, "#     # ", row + 5, col);
			c_put_str(TERM_YELLOW, "#    #  ", row + 6, col);
			c_put_str(TERM_YELLOW, ".####   ", row + 7, col);
			prt(                   " レモン ", row + 8, col);
#else
			c_put_str(TERM_YELLOW, "   ####.", row, col);
			c_put_str(TERM_YELLOW, "  #    #", row + 1, col);
			c_put_str(TERM_YELLOW, " #     #", row + 2, col);
			c_put_str(TERM_YELLOW, "#      #", row + 3, col);
			c_put_str(TERM_YELLOW, "#      #", row + 4, col);
			c_put_str(TERM_YELLOW, "#     # ", row + 5, col);
			c_put_str(TERM_YELLOW, "#    #  ", row + 6, col);
			c_put_str(TERM_YELLOW, ".####   ", row + 7, col);
			prt(                   " Lemon  ", row + 8, col);
#endif

			break;
		case 1: /* orange */
#ifdef JP
			c_put_str(TERM_ORANGE, "   ##   ", row, col);
			c_put_str(TERM_ORANGE, "  #..#  ", row + 1, col);
			c_put_str(TERM_ORANGE, " #....# ", row + 2, col);
			c_put_str(TERM_ORANGE, "#......#", row + 3, col);
			c_put_str(TERM_ORANGE, "#......#", row + 4, col);
			c_put_str(TERM_ORANGE, " #....# ", row + 5, col);
			c_put_str(TERM_ORANGE, "  #..#  ", row + 6, col);
			c_put_str(TERM_ORANGE, "   ##   ", row + 7, col);
			prt(                   "オレンジ", row + 8, col);
#else
			c_put_str(TERM_ORANGE, "   ##   ", row, col);
			c_put_str(TERM_ORANGE, "  #..#  ", row + 1, col);
			c_put_str(TERM_ORANGE, " #....# ", row + 2, col);
			c_put_str(TERM_ORANGE, "#......#", row + 3, col);
			c_put_str(TERM_ORANGE, "#......#", row + 4, col);
			c_put_str(TERM_ORANGE, " #....# ", row + 5, col);
			c_put_str(TERM_ORANGE, "  #..#  ", row + 6, col);
			c_put_str(TERM_ORANGE, "   ##   ", row + 7, col);
			prt(                   " Orange ", row + 8, col);
#endif

			break;
		case 2: /* sword */
#ifdef JP
			c_put_str(TERM_SLATE, "   Λ   " , row, col);
			c_put_str(TERM_SLATE, "   ||   " , row + 1, col);
			c_put_str(TERM_SLATE, "   ||   " , row + 2, col);
			c_put_str(TERM_SLATE, "   ||   " , row + 3, col);
			c_put_str(TERM_SLATE, "   ||   " , row + 4, col);
			c_put_str(TERM_SLATE, "   ||   " , row + 5, col);
			c_put_str(TERM_UMBER, " |=亜=| " , row + 6, col);
			c_put_str(TERM_UMBER, "   目   " , row + 7, col);
			prt(                  "   剣   " , row + 8, col);
#else
			c_put_str(TERM_SLATE, "   /\\   " , row, col);
			c_put_str(TERM_SLATE, "   ##   " , row + 1, col);
			c_put_str(TERM_SLATE, "   ##   " , row + 2, col);
			c_put_str(TERM_SLATE, "   ##   " , row + 3, col);
			c_put_str(TERM_SLATE, "   ##   " , row + 4, col);
			c_put_str(TERM_SLATE, "   ##   " , row + 5, col);
			c_put_str(TERM_UMBER, " ###### " , row + 6, col);
			c_put_str(TERM_UMBER, "   ##   " , row + 7, col);
			prt(                  " Sword  " , row + 8, col);
#endif

			break;
		case 3: /* shield */
#ifdef JP
			c_put_str(TERM_SLATE, " ###### ", row, col);
			c_put_str(TERM_SLATE, "#      #", row + 1, col);
			c_put_str(TERM_SLATE, "# ++++ #", row + 2, col);
			c_put_str(TERM_SLATE, "# +==+ #", row + 3, col);
			c_put_str(TERM_SLATE, "#  ++  #", row + 4, col);
			c_put_str(TERM_SLATE, " #    # ", row + 5, col);
			c_put_str(TERM_SLATE, "  #  #  ", row + 6, col);
			c_put_str(TERM_SLATE, "   ##   ", row + 7, col);
			prt(                  "   盾   ", row + 8, col);
#else
			c_put_str(TERM_SLATE, " ###### ", row, col);
			c_put_str(TERM_SLATE, "#      #", row + 1, col);
			c_put_str(TERM_SLATE, "# ++++ #", row + 2, col);
			c_put_str(TERM_SLATE, "# +==+ #", row + 3, col);
			c_put_str(TERM_SLATE, "#  ++  #", row + 4, col);
			c_put_str(TERM_SLATE, " #    # ", row + 5, col);
			c_put_str(TERM_SLATE, "  #  #  ", row + 6, col);
			c_put_str(TERM_SLATE, "   ##   ", row + 7, col);
			prt(                  " Shield ", row + 8, col);
#endif

			break;
		case 4: /* plum */
#ifdef JP
			c_put_str(TERM_VIOLET, "   ##   ", row, col);
			c_put_str(TERM_VIOLET, " ###### ", row + 1, col);
			c_put_str(TERM_VIOLET, "########", row + 2, col);
			c_put_str(TERM_VIOLET, "########", row + 3, col);
			c_put_str(TERM_VIOLET, "########", row + 4, col);
			c_put_str(TERM_VIOLET, " ###### ", row + 5, col);
			c_put_str(TERM_VIOLET, "  ####  ", row + 6, col);
			c_put_str(TERM_VIOLET, "   ##   ", row + 7, col);
			prt(                   " プラム ", row + 8, col);
#else
			c_put_str(TERM_VIOLET, "   ##   ", row, col);
			c_put_str(TERM_VIOLET, " ###### ", row + 1, col);
			c_put_str(TERM_VIOLET, "########", row + 2, col);
			c_put_str(TERM_VIOLET, "########", row + 3, col);
			c_put_str(TERM_VIOLET, "########", row + 4, col);
			c_put_str(TERM_VIOLET, " ###### ", row + 5, col);
			c_put_str(TERM_VIOLET, "  ####  ", row + 6, col);
			c_put_str(TERM_VIOLET, "   ##   ", row + 7, col);
			prt(                   "  Plum  ", row + 8, col);
#endif

			break;
		case 5: /* cherry */
#ifdef JP
			c_put_str(TERM_RED, "      ##", row, col);
			c_put_str(TERM_RED, "   ###  ", row + 1, col);
			c_put_str(TERM_RED, "  #..#  ", row + 2, col);
			c_put_str(TERM_RED, "  #..#  ", row + 3, col);
			c_put_str(TERM_RED, " ###### ", row + 4, col);
			c_put_str(TERM_RED, "#..##..#", row + 5, col);
			c_put_str(TERM_RED, "#..##..#", row + 6, col);
			c_put_str(TERM_RED, " ##  ## ", row + 7, col);
			prt(                "チェリー", row + 8, col);
#else
			c_put_str(TERM_RED, "      ##", row, col);
			c_put_str(TERM_RED, "   ###  ", row + 1, col);
			c_put_str(TERM_RED, "  #..#  ", row + 2, col);
			c_put_str(TERM_RED, "  #..#  ", row + 3, col);
			c_put_str(TERM_RED, " ###### ", row + 4, col);
			c_put_str(TERM_RED, "#..##..#", row + 5, col);
			c_put_str(TERM_RED, "#..##..#", row + 6, col);
			c_put_str(TERM_RED, " ##  ## ", row + 7, col);
			prt(                " Cherry ", row + 8, col);
#endif

			break;
	}
}

/*
 * kpoker no (tyuto-hannpa na)pakuri desu...
 * joker ha shineru node haitte masen.
 *
 * TODO: donataka! tsukutte!
 *  - agatta yaku no kiroku (like DQ).
 *  - kakkoii card no e.
 *  - sousa-sei no koujyo.
 *  - code wo wakariyasuku.
 *  - double up.
 *  - Joker... -- done.
 *
 * 9/13/2000 --Koka
 * 9/15/2000 joker wo jissou. soreto, code wo sukosi kakikae. --Habu
 */
#define SUIT_OF(card)  ((card) / 13)
#define NUM_OF(card)   ((card) % 13)
#define IS_JOKER(card) ((card) == 52)

static int cards[5]; /* tefuda no card */

static void reset_deck(int deck[])
{
	int i;
	for (i = 0; i < 53; i++) deck[i] = i;

	/* shuffle cards */
	for (i = 0; i < 53; i++)
	{
		int tmp1 = randint0(53 - i) + i;
		int tmp2 = deck[i];
		deck[i] = deck[tmp1];
		deck[tmp1] = tmp2;
	}
}

static bool have_joker(void)
{
	int i;

	for (i = 0; i < 5; i++)
	{
		if (IS_JOKER(cards[i])) return TRUE;
	}
	return FALSE;
}

static bool find_card_num(int num)
{
	int i;
	for (i = 0; i < 5; i++)
		if (NUM_OF(cards[i]) == num && !IS_JOKER(cards[i])) return TRUE;
	return FALSE;
}

static bool yaku_check_flush(void)
{
	int i, suit;
	bool joker_is_used = FALSE;

	suit = IS_JOKER(cards[0]) ? SUIT_OF(cards[1]) : SUIT_OF(cards[0]);
	for (i = 0; i < 5; i++)
	{
		if (SUIT_OF(cards[i]) != suit)
		{
			if (have_joker() && !joker_is_used)
				joker_is_used = TRUE;
			else
				return FALSE;
		}
	}

	return TRUE;
}

static int yaku_check_straight(void)
{
	int i, lowest = 99;
	bool joker_is_used = FALSE;

	/* get lowest */
	for (i = 0; i < 5; i++)
	{
		if (NUM_OF(cards[i]) < lowest && !IS_JOKER(cards[i]))
			lowest = NUM_OF(cards[i]);
	}

	if (yaku_check_flush())
	{
		if (lowest == 0)
		{
			for (i = 0; i < 4; i++)
			{
				if (!find_card_num(9 + i))
				{
					if (have_joker() && !joker_is_used)
						joker_is_used = TRUE;
					else
						break;
				}
			}
			if (i == 4) return 3; /* Wow! Royal Flush!!! */
		}
		if (lowest == 9)
		{
			for (i = 0; i < 3; i++)
			{
				if (!find_card_num(10 + i))
					break;
			}
			if (i == 3 && have_joker()) return 3; /* Wow! Royal Flush!!! */
		}
	}

	joker_is_used = FALSE;
	for (i = 0; i < 5; i++)
	{
		if (!find_card_num(lowest + i))
		{
			if (have_joker() && !joker_is_used)
				joker_is_used = TRUE;
			else
				return 0;
		}
	}

	if (yaku_check_flush())
		return 2; /* Straight Flush */

	return 1;
}

/*
 * 0:nopair 1:1 pair 2:2 pair 3:3 cards 4:full house 6:4cards
 */
static int yaku_check_pair(void)
{
	int i, i2, matching = 0;

	for (i = 0; i < 5; i++)
	{
		for (i2 = i+1; i2 < 5; i2++)
		{
			if (IS_JOKER(cards[i]) || IS_JOKER(cards[i2])) continue;
			if (NUM_OF(cards[i]) == NUM_OF(cards[i2]))
				matching++;
		}
	}

	if (have_joker())
	{
		switch (matching)
		{
		case 0:
			matching = 1;
			break;
		case 1:
			matching = 3;
			break;
		case 2:
			matching = 4;
			break;
		case 3:
			matching = 6;
			break;
		case 6:
			matching = 7;
			break;
		default:
			/* don't reach */
			break;
		}
	}

	return matching;
}

#define ODDS_5A 3000
#define ODDS_5C 400
#define ODDS_RF 200
#define ODDS_SF 80
#define ODDS_4C 16
#define ODDS_FH 12
#define ODDS_FL 8
#define ODDS_ST 4
#define ODDS_3C 1
#define ODDS_2P 1

static int yaku_check(void)
{
	prt("                            ", 4, 3);

	switch (yaku_check_straight())
	{
	case 3: /* RF! */
#ifdef JP
		c_put_str(TERM_YELLOW, "ロイヤルストレートフラッシュ",  4,  3);
#else
		c_put_str(TERM_YELLOW, "Royal Flush",  4,  3);
#endif
		return ODDS_RF;
	case 2: /* SF! */
#ifdef JP
		c_put_str(TERM_YELLOW, "ストレートフラッシュ",  4,  3);
#else
		c_put_str(TERM_YELLOW, "Straight Flush",  4,  3);
#endif
		return ODDS_SF;
	case 1:
#ifdef JP
		c_put_str(TERM_YELLOW, "ストレート",  4,  3);
#else
		c_put_str(TERM_YELLOW, "Straight",  4,  3);
#endif
		return ODDS_ST;
	default:
		/* Not straight -- fall through */
		break;
	}

	if (yaku_check_flush())
	{

#ifdef JP
		c_put_str(TERM_YELLOW, "フラッシュ",  4, 3);
#else
		c_put_str(TERM_YELLOW, "Flush",  4, 3);
#endif
		return ODDS_FL;
	}

	switch (yaku_check_pair())
	{
	case 1:
#ifdef JP
		c_put_str(TERM_YELLOW, "ワンペア",  4,  3);
#else
		c_put_str(TERM_YELLOW, "One pair",  4,  3);
#endif
		return 0;
	case 2:
#ifdef JP
		c_put_str(TERM_YELLOW, "ツーペア",  4,  3);
#else
		c_put_str(TERM_YELLOW, "Two pair",  4,  3);
#endif
		return ODDS_2P;
	case 3:
#ifdef JP
		c_put_str(TERM_YELLOW, "スリーカード",  4,  3);
#else
		c_put_str(TERM_YELLOW, "Three of a kind",  4,  3);
#endif
		return ODDS_3C;
	case 4:
#ifdef JP
		c_put_str(TERM_YELLOW, "フルハウス",  4,  3);
#else
		c_put_str(TERM_YELLOW, "Full house",  4,  3);
#endif
		return ODDS_FH;
	case 6:
#ifdef JP
		c_put_str(TERM_YELLOW, "フォーカード",  4,  3);
#else
		c_put_str(TERM_YELLOW, "Four of a kind",  4,  3);
#endif
		return ODDS_4C;
	case 7:
		if (!NUM_OF(cards[0]) || !NUM_OF(cards[1]))
		{
#ifdef JP
			c_put_str(TERM_YELLOW, "ファイブエース",  4,  3);
#else
			c_put_str(TERM_YELLOW, "Five ace",  4,  3);
#endif
			return ODDS_5A;
		}
		else
		{
#ifdef JP
			c_put_str(TERM_YELLOW, "ファイブカード",  4,  3);
#else
			c_put_str(TERM_YELLOW, "Five of a kind",  4,  3);
#endif
			return ODDS_5C;
		}
	default:
		break;
	}
	return 0;
}

static void display_kaeruka(int hoge, int kaeruka[])
{
	int i;
	char col = TERM_WHITE;
	for (i = 0; i < 5; i++)
	{
		if (i == hoge) col = TERM_YELLOW;
		else if(kaeruka[i]) col = TERM_WHITE;
		else col = TERM_L_BLUE;
#ifdef JP
		if(kaeruka[i])
			c_put_str(col, "かえる", 14,  5+i*16);
		else
			c_put_str(col, "のこす", 14,  5+i*16);
#else
		if(kaeruka[i])
			c_put_str(col, "Change", 14,  5+i*16);
		else
			c_put_str(col, " Stay ", 14,  5+i*16);
#endif
	}
	if (hoge > 4) col = TERM_YELLOW;
	else col = TERM_WHITE;
#ifdef JP
	c_put_str(col, "決定", 16,  38);
#else
	c_put_str(col, "Sure", 16,  38);
#endif

	/* Hilite current option */
	if (hoge < 5) move_cursor(14, 5+hoge*16);
	else move_cursor(16, 38);
}


static void display_cards(void)
{
	int i, j;
	char suitcolor[4] = {TERM_YELLOW, TERM_L_RED, TERM_L_BLUE, TERM_L_GREEN};
#ifdef JP
	cptr suit[4] = {"★", "●", "¶", "†"};
	cptr card_grph[13][7] = {{"Ａ   %s     ",
				  "            ",
				  "   TOband   ",
				  "            ",
				  "            ",
				  "     %s     ",
				  "          Ａ"},
				 {"２          ",
				  "     %s     ",
				  "            ",
				  "            ",
				  "            ",
				  "     %s     ",
				  "          ２"},
				 {"３          ",
				  "     %s     ",
				  "            ",
				  "     %s     ",
				  "            ",
				  "     %s     ",
				  "          ３"},
				 {"４          ",
				  "   %s  %s   ",
				  "            ",
				  "            ",
				  "            ",
				  "   %s  %s   ",
				  "          ４"},
				 {"５          ",
				  "   %s  %s   ",
				  "            ",
				  "     %s     ",
				  "            ",
				  "   %s  %s   ",
				  "          ５"},
				 {"６          ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "          ６"},
				 {"７          ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "          ７"},
				 {"８          ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "          ８"},
				 {"９ %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s ９"},
				 {"10 %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s 10"},
				 {"Ｊ   Λ     ",
				  "%s   ||     ",
				  "     ||     ",
				  "     ||     ",
				  "     ||     ",
				  "   |=亜=| %s",
				  "     目   Ｊ"},
				 {"Ｑ ######   ",
				  "%s#      #  ",
				  "  # ++++ #  ",
				  "  # +==+ #  ",
				  "   # ++ #   ",
				  "    #  #  %s",
				  "     ##   Ｑ"},
				 {"Ｋ          ",
				  "%s ｀⌒´   ",
				  "  γγγλ  ",
				  "  ο ο ι  ",
				  "   υ    ∂ ",
				  "    σ ノ %s",
				  "          Ｋ"}};
	cptr joker_grph[7] = {    "            ",
				  "     Ｊ     ",
				  "     Ｏ     ",
				  "     Ｋ     ",
				  "     Ｅ     ",
				  "     Ｒ     ",
				  "            "};

#else

	cptr suit[4] = {"[]", "qp", "<>", "db"};
	cptr card_grph[13][7] = {{"A    %s     ",
				  "            ",
				  "   TOband   ",
				  "            ",
				  "            ",
				  "     %s     ",
				  "           A"},
				 {"2           ",
				  "     %s     ",
				  "            ",
				  "            ",
				  "            ",
				  "     %s     ",
				  "           2"},
				 {"3           ",
				  "     %s     ",
				  "            ",
				  "     %s     ",
				  "            ",
				  "     %s     ",
				  "           3"},
				 {"4           ",
				  "   %s  %s   ",
				  "            ",
				  "            ",
				  "            ",
				  "   %s  %s   ",
				  "           4"},
				 {"5           ",
				  "   %s  %s   ",
				  "            ",
				  "     %s     ",
				  "            ",
				  "   %s  %s   ",
				  "           5"},
				 {"6           ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "           6"},
				 {"7           ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "           7"},
				 {"8           ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "           8"},
				 {"9  %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s  9"},
				 {"10 %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s 10"},
				 {"J    /\\     ",
				  "%s   ||     ",
				  "     ||     ",
				  "     ||     ",
				  "     ||     ",
				  "   |=HH=| %s",
				  "     ][    J"},
				 {"Q  ######   ",
				  "%s#      #  ",
				  "  # ++++ #  ",
				  "  # +==+ #  ",
				  "   # ++ #   ",
				  "    #  #  %s",
				  "     ##    Q"},
				 {"K           ",
				  "%s _'~~`_   ",
				  "   jjjjj$&  ",
				  "   q q uu   ",
				  "   c    &   ",
				  "    v__/  %s",
				  "           K"}};
	cptr joker_grph[7] = {    "            ",
				  "     J      ",
				  "     O      ",
				  "     K      ",
				  "     E      ",
				  "     R      ",
				  "            "};
#endif

	for (i = 0; i < 5; i++)
	{
#ifdef JP
		prt("┏━━━━━━┓",  5,  i*16);
#else
		prt(" +------------+ ",  5,  i*16);
#endif
	}

	for (i = 0; i < 5; i++)
	{
		for (j = 0; j < 7; j++)
		{
#ifdef JP
			prt("┃",  j+6,  i*16);
#else
			prt(" |",  j+6,  i*16);
#endif
			if(IS_JOKER(cards[i]))
				c_put_str(TERM_VIOLET, joker_grph[j],  j+6,  2+i*16);
			else
				c_put_str(suitcolor[SUIT_OF(cards[i])], format(card_grph[NUM_OF(cards[i])][j], suit[SUIT_OF(cards[i])], suit[SUIT_OF(cards[i])]),  j+6,  2+i*16);
#ifdef JP
			prt("┃",  j+6,  i*16+14);
#else
			prt("| ",  j+6,  i*16+14);
#endif
		}
	}
	for (i = 0; i < 5; i++)
	{
#ifdef JP
		prt("┗━━━━━━┛", 13,  i*16);
#else
		prt(" +------------+ ", 13,  i*16);
#endif
	}
}

static int do_poker(void)
{
	int i, k = 2;
	char cmd;
	int deck[53]; /* yamafuda : 0...52 */
	int deck_ptr = 0;
	int kaeruka[5]; /* 0:kaenai 1:kaeru */

	bool done = FALSE;
	bool kettei = TRUE;
	bool kakikae = TRUE;

	reset_deck(deck);

	for (i = 0; i < 5; i++)
	{
		cards[i] = deck[deck_ptr++];
		kaeruka[i] = 0; /* default:nokosu */
	}
	
#if 0
	/* debug:RF */
	cards[0] = 12;
	cards[1] = 0;
	cards[2] = 9;
	cards[3] = 11;
	cards[4] = 10;
#endif
#if 0
	/* debug:SF */
	cards[0] = 3;
	cards[1] = 2;
	cards[2] = 4;
	cards[3] = 6;
	cards[4] = 5;
#endif
#if 0
	/* debug:Four Cards */
	cards[0] = 0;
	cards[1] = 0 + 13 * 1;
	cards[2] = 0 + 13 * 2;
	cards[3] = 0 + 13 * 3;
	cards[4] = 51;
#endif
#if 0
	/* debug:Straight */
	cards[0] = 1;
	cards[1] = 0 + 13;
	cards[2] = 3;
	cards[3] = 2 + 26;
	cards[4] = 4;
#endif
#if 0
	/* debug */
	cards[0] = 52;
	cards[1] = 0;
	cards[2] = 1;
	cards[3] = 2;
	cards[4] = 3;
#endif

	/* suteruno wo kimeru */
#ifdef JP
	prt("残すカードを決めて下さい(方向で移動, スペースで選択)。", 0, 0);
#else
	prt("Stay witch? ", 0, 0);
#endif

	display_cards();
	yaku_check();

	while (!done)
	{
		if (kakikae) display_kaeruka(k+kettei*5, kaeruka);
		kakikae = FALSE;
		cmd = inkey();
		switch (cmd)
		{
		case '6': case 'l': case 'L': case KTRL('F'):
			if (!kettei) k = (k+1)%5;
			else {k = 0;kettei = FALSE;}
			kakikae = TRUE;
			break;
		case '4': case 'h': case 'H': case KTRL('B'):
			if (!kettei) k = (k+4)%5;
			else {k = 4;kettei = FALSE;}
			kakikae = TRUE;
			break;
		case '2': case 'j': case 'J': case KTRL('N'):
			if (!kettei) {kettei = TRUE;kakikae = TRUE;}
			break;
		case '8': case 'k': case 'K': case KTRL('P'):
			if (kettei) {kettei = FALSE;kakikae = TRUE;}
			break;
		case ' ': case '\r':
			if (kettei) done = TRUE;
			else {kaeruka[k] = !kaeruka[k];kakikae = TRUE;}
			break;
		default:
			break;
		}
	}
	
	prt("",0,0);

	for (i = 0; i < 5; i++)
		if (kaeruka[i] == 1) cards[i] = deck[deck_ptr++]; /* soshite toru */

	display_cards();
	
	return yaku_check();
}
#undef SUIT_OF
#undef NUM_OF
#undef IS_JOKER
/* end of poker codes --Koka */

static int gamble_play_times;

/*
 * gamble_comm
 */
static bool gamble_comm(int cmd)
{
	int i;
	int roll1, roll2, roll3, choice, odds, win;
	s32b wager;
	s32b maxbet;
	s32b oldgold;

	char out_val[160], tmp_str[80], again;
	cptr p;

	screen_save();

	if (cmd == BACT_GAMBLE_RULES)
	{
		/* Peruse the gambling help file */
#ifdef JP
		(void)show_file(TRUE, "jgambling.txt", NULL, 0, 0);
#else
		(void)show_file(TRUE, "gambling.txt", NULL, 0, 0);
#endif

	}
	else
	{
		/* No money */
		if (p_ptr->au_sum < 1)
		{
#ifdef JP
			msg_print("おい！おまえ一文なしじゃないか！こっから出ていけ！");
#else
			msg_print("Hey! You don't have gold - get out of here!");
#endif

			msg_print(NULL);
			screen_load();
			return FALSE;
		}

		clear_bldg(5, 23);

		maxbet = p_ptr->lev * 200;

		/* We can't bet more than we have */
		maxbet = MIN(maxbet, p_ptr->au_sum);

		/* Get the wager */
		strcpy(out_val, "");
#ifdef JP
		sprintf(tmp_str,"賭け金 (1-%ld)？", maxbet);
#else
		sprintf(tmp_str,"Your wager (1-%ld) ? ", maxbet);
#endif


		/*
		 * Use get_string() because we may need more than
		 * the s16b value returned by get_quantity().
		 */
		if (get_string(tmp_str, out_val, 32))
		{
			/* Strip spaces */
			for (p = out_val; *p == ' '; p++);

			/* Get the wager */
			wager = atol(p);

			if (wager > p_ptr->au_sum)
			{
#ifdef JP
				msg_print("おい！金が足りないじゃないか！出ていけ！");
#else
				msg_print("Hey! You don't have the gold - get out of here!");
#endif

				msg_print(NULL);
				screen_load();
				return (FALSE);
			}
			else if (wager > maxbet)
			{
#ifdef JP
				msg_format("$%ldだけ受けよう。残りは取っときな。", maxbet);
#else
				msg_format("I'll take %ld gold of that. Keep the rest.", maxbet);
#endif

				wager = maxbet;
			}
			else if (wager < 1)
			{
#ifdef JP
				msg_print("ＯＫ、$1からはじめよう。");
#else
				msg_print("Ok, we'll start with 1 gold.");
#endif


				wager = 1;
			}
			msg_print(NULL);
			win = FALSE;
			odds = 0;
			oldgold = p_ptr->au_sum;

#ifdef JP
			sprintf(tmp_str, "ゲーム前の所持金: %9ld", oldgold);
#else
			sprintf(tmp_str, "Gold before game: %9ld", oldgold);
#endif

			prt(tmp_str, 20, 2);

#ifdef JP
			sprintf(tmp_str, "現在の掛け金:     %9ld", wager);
#else
			sprintf(tmp_str, "Current Wager:    %9ld", wager);
#endif

			prt(tmp_str, 21, 2);

			/* Prevent savefile-scumming of the casino */
/*			Rand_quick = TRUE; */
			Rand_value = time(NULL);

			do
			{
#ifdef JP /* Prevent random seed cracking of the casino */
				clock_t clk;
				clk = clock();
				Rand_value *= clk;
#endif
				p_ptr->au_sum -= wager;
				switch (cmd)
				{
				 case BACT_IN_BETWEEN: /* Game of In-Between */
#ifdef JP
					c_put_str(TERM_GREEN, "イン・ビトイーン",5,2);
#else
					c_put_str(TERM_GREEN, "In Between", 5, 2);
#endif

					odds = 4;
					win = FALSE;
					roll1 = randint1(10);
					roll2 = randint1(10);
					choice = randint1(10);
#ifdef JP
					sprintf(tmp_str, "黒ダイス: %d        黒ダイス: %d", roll1, roll2);
#else
					sprintf(tmp_str, "Black die: %d       Black Die: %d", roll1, roll2);
#endif

					prt(tmp_str, 8, 3);
#ifdef JP
					sprintf(tmp_str, "赤ダイス: %d", choice);
#else
					sprintf(tmp_str, "Red die: %d", choice);
#endif

					prt(tmp_str, 11, 14);
					if (((choice > roll1) && (choice < roll2)) ||
						((choice < roll1) && (choice > roll2)))
						win = TRUE;
					break;
				case BACT_CRAPS:  /* Game of Craps */
#ifdef JP
					c_put_str(TERM_GREEN, "クラップス", 5, 2);
#else
					c_put_str(TERM_GREEN, "Craps", 5, 2);
#endif

					win = 3;
					odds = 2;
					roll1 = randint1(6);
					roll2 = randint1(6);
					roll3 = roll1 +  roll2;
					choice = roll3;
#ifdef JP
					sprintf(tmp_str, "１振りめ: %d %d      Total: %d", roll1, 
#else
					sprintf(tmp_str, "First roll: %d %d    Total: %d", roll1,
#endif

						 roll2, roll3);
					prt(tmp_str, 7, 5);
					if ((roll3 == 7) || (roll3 == 11))
						win = TRUE;
					else if ((roll3 == 2) || (roll3 == 3) || (roll3 == 12))
						win = FALSE;
					else
						do
						{
#ifdef JP
							msg_print("なにかキーを押すともう一回振ります。");
#else
							msg_print("Hit any key to roll again");
#endif

							msg_print(NULL);
							roll1 = randint1(6);
							roll2 = randint1(6);
							roll3 = roll1 +  roll2;

#ifdef JP
							sprintf(tmp_str, "出目: %d %d          合計:      %d",
#else
							sprintf(tmp_str, "Roll result: %d %d   Total:     %d",
#endif

								 roll1, roll2, roll3);
							prt(tmp_str, 8, 5);
							if (roll3 == choice)
								win = TRUE;
							else if (roll3 == 7)
								win = FALSE;
						} while ((win != TRUE) && (win != FALSE));
					break;

				case BACT_SPIN_WHEEL:  /* Spin the Wheel Game */
					win = FALSE;
					odds = 9;
#ifdef JP
					c_put_str(TERM_GREEN, "ルーレット", 5, 2);
#else
					c_put_str(TERM_GREEN, "Wheel", 5, 2);
#endif

					prt("0  1  2  3  4  5  6  7  8  9", 7, 5);
					prt("--------------------------------", 8, 3);
					strcpy(out_val, "");
#ifdef JP
					get_string("何番？ (0-9): ", out_val, 32);
#else
					get_string("Pick a number (0-9): ", out_val, 32);
#endif

					for (p = out_val; isspace(*p); p++);
					choice = atol(p);
					if (choice < 0)
					{
#ifdef JP
						msg_print("0番にしとくぜ。");
#else
						msg_print("I'll put you down for 0.");
#endif

						choice = 0;
					}
					else if (choice > 9)
					{
#ifdef JP
						msg_print("ＯＫ、9番にしとくぜ。");
#else
						msg_print("Ok, I'll put you down for 9.");
#endif

						choice = 9;
					}
					msg_print(NULL);
					roll1 = randint0(10);
#ifdef JP
					sprintf(tmp_str, "ルーレットは回り、止まった。勝者は %d番だ。",
#else
					sprintf(tmp_str, "The wheel spins to a stop and the winner is %d",
#endif

						roll1);
					prt(tmp_str, 13, 3);
					prt("", 9, 0);
					prt("*", 9, (3 * roll1 + 5));
					if (roll1 == choice)
						win = TRUE;
					break;

				case BACT_DICE_SLOTS: /* The Dice Slots */
#ifdef JP
					c_put_str(TERM_GREEN, "ダイス・スロット", 5, 2);
					c_put_str(TERM_YELLOW, "レモン   レモン            2", 6, 37);
					c_put_str(TERM_YELLOW, "レモン   レモン   レモン   5", 7, 37);
					c_put_str(TERM_ORANGE, "オレンジ オレンジ オレンジ 10", 8, 37);
					c_put_str(TERM_UMBER, "剣       剣       剣       20", 9, 37);
					c_put_str(TERM_SLATE, "盾       盾       盾       50", 10, 37);
					c_put_str(TERM_VIOLET, "プラム   プラム   プラム   200", 11, 37);
					c_put_str(TERM_RED, "チェリー チェリー チェリー 1000", 12, 37);
#else
					c_put_str(TERM_GREEN, "Dice Slots", 5, 2);
#endif

					win = FALSE;
					roll1 = randint1(21);
					for (i=6;i>0;i--)
					{
						if ((roll1-i) < 1)
						{
							roll1 = 7-i;
							break;
						}
						roll1 -= i;
					}
					roll2 = randint1(21);
					for (i=6;i>0;i--)
					{
						if ((roll2-i) < 1)
						{
							roll2 = 7-i;
							break;
						}
						roll2 -= i;
					}
					choice = randint1(21);
					for (i=6;i>0;i--)
					{
						if ((choice-i) < 1)
						{
							choice = 7-i;
							break;
						}
						choice -= i;
					}
					put_str("/--------------------------\\", 7, 2);
					prt("\\--------------------------/", 17, 2);
					display_fruit(8,  3, roll1 - 1);
					display_fruit(8, 12, roll2 - 1);
					display_fruit(8, 21, choice - 1);
					if ((roll1 == roll2) && (roll2 == choice))
					{
						win = TRUE;
						switch(roll1)
						{
						case 1:
							odds = 5;break;
						case 2:
							odds = 10;break;
						case 3:
							odds = 20;break;
						case 4:
							odds = 50;break;
						case 5:
							odds = 200;break;
						case 6:
							odds = 1000;break;
						}
					}
					else if ((roll1 == 1) && (roll2 == 1))
					{
						win = TRUE;
						odds = 2;
					}
					break;
				case BACT_POKER:
					win = FALSE;
					odds = do_poker();
					if (odds) win = TRUE;
					break;
				}

				if (win)
				{
#ifdef JP
					prt("あなたの勝ち", 16, 37);
#else
					prt("YOU WON", 16, 37);
#endif

					p_ptr->au[SV_GOLD_NOTE] += odds * wager;
#ifdef JP
					sprintf(tmp_str, "倍率: %d", odds);
#else
					sprintf(tmp_str, "Payoff: %d", odds);
#endif

					prt(tmp_str, 17, 37);
				}
				else
				{
#ifdef JP
					prt("あなたの負け", 16, 37);
#else
					prt("You Lost", 16, 37);
#endif

					prt("", 17, 37);
				}
				p_ptr->update |= (PU_GOLD);
				update_stuff();
				gamble_play_times++;
#ifdef JP
				sprintf(tmp_str, "現在の所持金:     %9ld", p_ptr->au_sum);
#else
				sprintf(tmp_str, "Current Gold:     %9ld", p_ptr->au_sum);
#endif

				prt(tmp_str, 22, 2);
#ifdef JP
				prt("もう一度(Y/N)？", 18, 37);
#else
				prt("Again(Y/N)?", 18, 37);
#endif

				move_cursor(18, 52);
				again = inkey();
				prt("", 16, 37);
				prt("", 17, 37);
				prt("", 18, 37);
				if (wager > p_ptr->au_sum)
				{
#ifdef JP
					msg_print("おい！金が足りないじゃないか！ここから出て行け！");
#else
					msg_print("Hey! You don't have the gold - get out of here!");
#endif

					msg_print(NULL);

					/* Get out here */
					break;
				}
			} while ((again == 'y') || (again == 'Y'));

			/* Switch back to complex RNG */
			Rand_quick = FALSE;

			prt("", 18, 37);
			if (p_ptr->au_sum >= oldgold)
			{
#ifdef JP
				msg_print("「今回は儲けたな！でも次はこっちが勝ってやるからな、絶対に！」");
#else
				msg_print("You came out a winner! We'll win next time, I'm sure.");
#endif
			}
			else
			{
#ifdef JP
				msg_print("「金をスッてしまったな、わはは！うちに帰った方がいいぜ。」");
#else
				msg_print("You lost gold! Haha, better head home.");
#endif
			}
		}
		msg_print(NULL);
	}
	screen_load();
	return (TRUE);
}


static void today_target(void)
{
	char buf[160];
	monster_race *r_ptr = &r_info[today_mon];

	clear_bldg(4,18);
#ifdef JP
	c_put_str(TERM_YELLOW, "本日の賞金首", 5, 10);
#else
	prt("Wanted monster that changes from day to day", 5, 10);
#endif
#ifdef JP
	sprintf(buf,"ターゲット： %s",r_name + r_ptr->name);
#else
	sprintf(buf,"target: %s",r_name + r_ptr->name);
#endif
	c_put_str(TERM_YELLOW, buf, 6, 10);
#ifdef JP
	sprintf(buf,"死体 ---- $%d",r_ptr->level * 50 + 100);
#else
	sprintf(buf,"corpse   ---- $%d",r_ptr->level * 50 + 100);
#endif
	prt(buf, 8, 10);
#ifdef JP
	sprintf(buf,"骨   ---- $%d",r_ptr->level * 30 + 60);
#else
	sprintf(buf,"skeleton ---- $%d",r_ptr->level * 30 + 60);
#endif
	prt(buf, 9, 10);
	p_ptr->today_mon = today_mon;
}

static void tsuchinoko(void)
{
	clear_bldg(4,18);
#ifdef JP
	c_put_str(TERM_YELLOW, "一獲千金の大チャンス！！！", 5, 10);
	c_put_str(TERM_YELLOW, "ターゲット：幻の珍獣「ゼブラ」", 6, 10);
	c_put_str(TERM_WHITE, "死体     ---- $1,000,000", 9, 10);
	c_put_str(TERM_WHITE, "骨       ----   $100,000", 10, 10);
#else
	c_put_str(TERM_YELLOW, "Big chance to quick money!!!", 5, 10);
	c_put_str(TERM_YELLOW, "target: the rarest animal 'Zebra'", 6, 10);
	c_put_str(TERM_WHITE, "corpse      ---- $1,000,000", 9, 10);
	c_put_str(TERM_WHITE, "bones       ----   $100,000", 10, 10);
#endif
}

static void shoukinkubi(void)
{
	int i;
	int y = 0;

	clear_bldg(4,18);

#ifdef JP
	prt("死体か骨を持ち帰ると賞金を差し上げます。",4 ,10);
	c_put_str(TERM_YELLOW, "現在の賞金首", 6, 10);
#else
	prt("Offer a prize when you bring a wanted monster's corpse or skeleton",4 ,10);
	c_put_str(TERM_YELLOW, "Wanted monsters", 6, 10);
#endif

	for (i = 0; i < MAX_KUBI; i++)
	{
		char buf[160];
		monster_race *r_ptr = &r_info[(kubi_r_idx[i] > 10000 ? kubi_r_idx[i] - 10000 : kubi_r_idx[i])];

		sprintf(buf,"%-40s ---- ",r_name + r_ptr->name);
		prt(buf, y+7, 10);
		if (kubi_r_idx[i] > 10000)
#ifdef JP
			c_put_str(TERM_RED,"済", y+7, 56);
#else
			c_put_str(TERM_RED,"done", y+7, 56);
#endif
		else
		{
			sprintf(buf,"$%d", 300 * (r_ptr->level + 1));
			prt(buf, y+7, 56);
		}
		y = (y+1) % 10;
		if (!y && (i < MAX_KUBI -1))
		{
#ifdef JP
			prt("何かキーを押してください", 0, 0);
#else
			prt("Hit any key.", 0, 0);
#endif
			(void)inkey();
			prt("", 0, 0);
			clear_bldg(7,18);
		}
	}
}

static bool kankin(void)
{
	int i, j;
	bool change = FALSE;
	char o_name[MAX_NLEN];
	object_type *o_ptr;

	for (i = 0;i <= INVEN_LARM; i++)
	{
		o_ptr = &inventory[i];
		if ((o_ptr->tval == TV_CORPSE) && (o_ptr->sval == SV_CORPSE) && (o_ptr->pval == MON_ZEBRA))
		{
			char buf[MAX_NLEN+20];
			object_desc(o_name, o_ptr, 0);
#ifdef JP
			sprintf(buf, "%s を換金しますか？",o_name);
#else
			sprintf(buf, "Convert %s into money? ",o_name);
#endif
			if (get_check(buf))
			{
#ifdef JP
				msg_format("賞金 %ld$を手に入れた。", 1000000L * o_ptr->number);
#else
				msg_format("You get %ldgp.", 1000000L * o_ptr->number);
#endif
				p_ptr->au[SV_GOLD_MITHRIL] += 1000000L * o_ptr->number;
				p_ptr->update |= (PU_GOLD);
				update_stuff();
				p_ptr->redraw |= (PR_GOLD);
				inven_item_increase(i, -o_ptr->number);
				inven_item_describe(i);
				inven_item_optimize(i);
				change_your_alignment(ALI_GNE, -10);
			}
			change = TRUE;
		}
	}
	for (i = 0;i <= INVEN_LARM; i++)
	{
		o_ptr = &inventory[i];
		if ((o_ptr->tval == TV_CORPSE) && (o_ptr->sval == SV_SKELETON) && (o_ptr->pval == MON_ZEBRA))
		{
			char buf[MAX_NLEN+20];
			object_desc(o_name, o_ptr, 0);
#ifdef JP
			sprintf(buf, "%s を換金しますか？",o_name);
#else
			sprintf(buf, "Convert %s into money? ",o_name);
#endif
			if (get_check(buf))
			{
#ifdef JP
				msg_format("賞金 %ld$を手に入れた。", 100000L * o_ptr->number);
#else
				msg_format("You get %ldgp.", 100000L * o_ptr->number);
#endif
				p_ptr->au[SV_GOLD_MITHRIL] += 100000L * o_ptr->number;
				p_ptr->update |= (PU_GOLD);
				update_stuff();
				p_ptr->redraw |= (PR_GOLD);
				inven_item_increase(i, -o_ptr->number);
				inven_item_describe(i);
				inven_item_optimize(i);
				change_your_alignment(ALI_GNE, -10);
			}
			change = TRUE;
		}
	}

	for (i = 0;i <= INVEN_LARM; i++)
	{
		o_ptr = &inventory[i];
		if ((o_ptr->tval == TV_CORPSE) && (o_ptr->sval == SV_CORPSE) && (streq(r_name + r_info[o_ptr->pval].name, r_name + r_info[today_mon].name)))
		{
			char buf[MAX_NLEN+20];
			object_desc(o_name, o_ptr, 0);
#ifdef JP
			sprintf(buf, "%s を換金しますか？",o_name);
#else
			sprintf(buf, "Convert %s into money? ",o_name);
#endif
			if (get_check(buf))
			{
#ifdef JP
				msg_format("賞金 %ld$を手に入れた。", (r_info[today_mon].level * 50 + 100) * o_ptr->number);
#else
				msg_format("You get %ldgp.", (r_info[today_mon].level * 50 + 100) * o_ptr->number);
#endif
				p_ptr->au[SV_GOLD_MITHRIL] += (r_info[today_mon].level * 50 + 100) * o_ptr->number;
				p_ptr->update |= (PU_GOLD);
				update_stuff();
				p_ptr->redraw |= (PR_GOLD);
				inven_item_increase(i, -o_ptr->number);
				inven_item_describe(i);
				inven_item_optimize(i);
				change_your_alignment(ALI_GNE, 5);
			}
			change = TRUE;
		}
	}
	for (i = 0;i <= INVEN_LARM; i++)
	{
		o_ptr = &inventory[i];
		if ((o_ptr->tval == TV_CORPSE) && (o_ptr->sval == SV_SKELETON) && (streq(r_name + r_info[o_ptr->pval].name, r_name + r_info[today_mon].name)))
		{
			char buf[MAX_NLEN+20];
			object_desc(o_name, o_ptr, 0);
#ifdef JP
			sprintf(buf, "%s を換金しますか？",o_name);
#else
			sprintf(buf, "Convert %s into money? ",o_name);
#endif
			if (get_check(buf))
			{
#ifdef JP
				msg_format("賞金 %ld$を手に入れた。", (r_info[today_mon].level * 30 + 60) * o_ptr->number);
#else
				msg_format("You get %ldgp.", (r_info[today_mon].level * 30 + 60) * o_ptr->number);
#endif
				p_ptr->au[SV_GOLD_MITHRIL] += (r_info[today_mon].level * 30 + 60) * o_ptr->number;
				p_ptr->update |= (PU_GOLD);
				update_stuff();
				p_ptr->redraw |= (PR_GOLD);
				inven_item_increase(i, -o_ptr->number);
				inven_item_describe(i);
				inven_item_optimize(i);
				change_your_alignment(ALI_GNE, 5);
			}
			change = TRUE;
		}
	}

	for (j = 0; j < MAX_KUBI; j++)
	{
		for (i = 0;i <= INVEN_LARM; i++)
		{
			o_ptr = &inventory[i];
			if ((o_ptr->tval == TV_CORPSE) &&
				((o_ptr->sval == SV_CORPSE) || (o_ptr->sval == SV_SKELETON)) &&
				(o_ptr->pval == kubi_r_idx[j]))
			{
				char buf[MAX_NLEN+20];
				object_desc(o_name, o_ptr, 0);
#ifdef JP
				sprintf(buf, "%s を換金しますか？",o_name);
#else
				sprintf(buf, "Convert %s into money? ",o_name);
#endif
				if (get_check(buf))
				{
#ifdef JP
					msg_format("賞金 %ld$を手に入れた。", (r_info[kubi_r_idx[j]].level + 1) * 300 * o_ptr->number);
#else
					msg_format("You get %ldgp.", (r_info[kubi_r_idx[j]].level + 1) * 300 * o_ptr->number);
#endif
					p_ptr->au[SV_GOLD_MITHRIL] += (r_info[kubi_r_idx[j]].level+1) * 300 * o_ptr->number;
					p_ptr->update |= (PU_GOLD);
					update_stuff();
					p_ptr->redraw |= (PR_GOLD);
					inven_item_increase(i, -o_ptr->number);
					inven_item_describe(i);
					inven_item_optimize(i);
					change_your_alignment(ALI_GNE, 20);
					kubi_r_idx[j] += 10000;
				}
				change = TRUE;
			}
		}
	}

	if (!change)
	{
#ifdef JP
		msg_print("賞金を得られそうなものは持っていなかった。");
#else
		msg_print("You have nothing.");
#endif
		msg_print(NULL);
		return FALSE;
	}
	return TRUE;
}


/*
 * inn commands
 * Note that resting for the night was a perfect way to avoid player
 * ghosts in the town *if* you could only make it to the inn in time (-:
 * Now that the ghosts are temporarily disabled in 2.8.X, this function
 * will not be that useful.  I will keep it in the hopes the player
 * ghost code does become a reality again. Does help to avoid filthy urchins.
 * Resting at night is also a quick way to restock stores -KMW-
 */
static bool inn_comm(int cmd)
{
	int dawnval;

	switch (cmd)
	{
		case BACT_FOOD: /* Buy food & drink */
			if (p_ptr->no_digest)
			{
#ifdef JP
				msg_print("食べ物を頼む意味はない。");
#else
				msg_print("No means on ordering foods.");
#endif
				return FALSE;
			}
			else if (p_ptr->food >= PY_FOOD_FULL)
			{
#ifdef JP
				msg_print("今は満腹だ。");
#else
				msg_print("You are full now.");
#endif
				return FALSE;
			}

#ifdef JP
			msg_print("バーテンはいくらかの食べ物とビールをくれた。");
#else
			msg_print("The barkeep gives you some gruel and a beer.");
#endif

			(void)set_food(PY_FOOD_MAX - 1);
			break;

		case BACT_REST: /* Rest for the night */
			dawnval = ((turn % (TURNS_PER_TICK * TOWN_DAWN)));
			if (dawnval > (TURNS_PER_TICK * TOWN_DAWN)/4)
			{  /* nighttime */
				if (p_ptr->poisoned || p_ptr->cut || p_ptr->stoning)
				{
#ifdef JP
					msg_print("あなたに必要なのは部屋ではなく、治療者です。");
#else
					msg_print("You need a healer, not a room.");
#endif

					msg_print(NULL);
#ifdef JP
					msg_print("すみません、でもうちで誰かに死なれちゃ困りますんで。");
#else
					msg_print("Sorry, but don't want anyone dying in here.");
#endif

				}
				else
				{
					s32b oldturn = turn;
#ifdef JP
					do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "宿屋に泊まった。");
#else
					do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "stay over night at the inn");
#endif
					turn = (turn / (TURNS_PER_TICK*TOWN_DAWN/2) + 1) * (TURNS_PER_TICK*TOWN_DAWN/2);

					if (dungeon_turn < dungeon_turn_limit)
					{
						dungeon_turn += MIN(turn - oldturn, TURNS_PER_TICK*250);
						if (dungeon_turn > dungeon_turn_limit) dungeon_turn = dungeon_turn_limit;
					}

					prevent_turn_overflow();

					if (((oldturn + TURNS_PER_TICK * TOWN_DAWN / 4) % (TURNS_PER_TICK * TOWN_DAWN)) > TURNS_PER_TICK * TOWN_DAWN/4) do_cmd_write_nikki(NIKKI_HIGAWARI, 0, NULL);
					p_ptr->chp = p_ptr->mhp;

					change_your_alignment(ALI_LNC, -1);

					set_blind(0);
					set_confused(0);
					p_ptr->stun = 0;
					p_ptr->chp = p_ptr->mhp;
					p_ptr->csp = p_ptr->msp;

#ifdef JP
					msg_print("あなたはリフレッシュして目覚め、新たな日を迎えた。");
#else
					msg_print("You awake refreshed for the new day.");
#endif

#ifdef JP
					do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "すがすがしい朝をむかえた。");
#else
					do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "awake refreshed.");
#endif
				}
			}
			else
			{
#ifdef JP
				msg_print("部屋は夜だけ使用可能です。");
#else
				msg_print("The rooms are available only at night.");
#endif

				return (FALSE);
			}
			break;
		case BACT_RUMORS: /* Listen for rumors */
			{
				char Rumor[1024];

#ifdef JP
				if (!get_rnd_line_jonly("rumors_j.txt", 0, Rumor, 10))
#else
				if (!get_rnd_line("rumors.txt", 0, Rumor))
#endif

					msg_format("%s", Rumor);
				break;
			}
	}

	return (TRUE);
}


/*
 * Display quest information
 */
static void get_questinfo(int questnum)
{
	int     i;
	int     old_quest;
	char    tmp_str[80];


	/* Clear the text */
	for (i = 0; i < 10; i++)
	{
		quest_text[i][0] = '\0';
	}

	quest_text_line = 0;

	/* Set the quest number temporary */
	old_quest = p_ptr->inside_quest;
	p_ptr->inside_quest = questnum;

	/* Get the quest text */
	init_flags = INIT_SHOW_TEXT | INIT_ASSIGN;

	process_dungeon_file("q_info.txt", 0, 0, 0, 0);

	/* Reset the old quest number */
	p_ptr->inside_quest = old_quest;

	/* Print the quest info */
#ifdef JP
	sprintf(tmp_str, "クエスト情報 (危険度: %d 階相当)", quest[questnum].level);
#else
	sprintf(tmp_str, "Quest Information (Danger level: %d)", quest[questnum].level);
#endif

	prt(tmp_str, 5, 0);

	prt(quest[questnum].name, 7, 0);

	for (i = 0; i < 10; i++)
	{
		c_put_str(TERM_YELLOW, quest_text[i], i + 8, 0);
	}
}


/*
 * Request a quest from the Lord.
 */
static void castle_quest(void)
{
	int             q_index = 0;
	monster_race    *r_ptr;
	quest_type      *q_ptr;
	cptr            name;


	clear_bldg(4, 18);

	/* Current quest of the building */
	q_index = cave[py][px].special;

	/* Is there a quest available at the building? */
	if (!q_index)
	{
#ifdef JP
		put_str("今のところクエストはありません。", 8, 0);
#else
		put_str("I don't have a quest for you at the moment.", 8, 0);
#endif

		return;
	}

	q_ptr = &quest[q_index];

	/* Quest is completed */
	if (q_ptr->status == QUEST_STATUS_COMPLETED)
	{
		/* Rewarded quest */
		q_ptr->status = QUEST_STATUS_REWARDED;

		get_questinfo(q_index);

		reinit_wilderness = TRUE;
		switch (q_index)
		{
		case QUEST_MONTSALVAT:
		case QUEST_HOLY_KNIGHTS:
			change_chaos_frame(ETHNICITY_ZENOBIAN, -25);
			change_chaos_frame(ETHNICITY_LODIS, 80);
			break;

		case QUEST_RAMZEN:
			change_chaos_frame(ETHNICITY_ZENOBIAN, 80);
			change_chaos_frame(ETHNICITY_LODIS, -25);
			break;

		default:
			if (!dun_level && p_ptr->town_num && !(q_ptr->flags & QUEST_FLAG_NO_ETHNICITY))
				change_chaos_frame(town[p_ptr->town_num].ethnic, 60);
			break;
		}
	}
	/* Failed quest */
	else if (q_ptr->status == QUEST_STATUS_FAILED)
	{
		get_questinfo(q_index);

		/* Mark quest as done (but failed) */
		q_ptr->status = QUEST_STATUS_FAILED_DONE;

		reinit_wilderness = TRUE;

		if (!(q_ptr->flags & QUEST_FLAG_NO_ETHNICITY)) change_chaos_frame(town[p_ptr->town_num].ethnic, -60);
	}
	/* Quest is still unfinished */
	else if (q_ptr->status == QUEST_STATUS_TAKEN)
	{
#ifdef JP
		put_str("あなたは現在のクエストを終了させていません！", 8, 0);
		put_str("CTRL-Qを使えばクエストの状態がチェックできます。", 9, 0);
		put_str("クエストを終わらせたら戻って来て下さい。", 12, 0);
#else
		put_str("You have not completed your current quest yet!", 8, 0);
		put_str("Use CTRL-Q to check the status of your quest.", 9, 0);
		put_str("Return when you have completed your quest.", 12, 0);
#endif

	}
	/* No quest yet */
	else if (q_ptr->status == QUEST_STATUS_UNTAKEN)
	{
		/* Assign a new quest */
		if (q_ptr->type == QUEST_TYPE_KILL_ANY_LEVEL)
		{
			if (q_ptr->r_idx == 0)
			{
				/* Random monster at least 5 - 10 levels out of deep */
				q_ptr->r_idx = get_mon_num(q_ptr->level + 4 + randint1(6));
			}

			r_ptr = &r_info[q_ptr->r_idx];

			while ((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->rarity != 1))
			{
				q_ptr->r_idx = get_mon_num(q_ptr->level) + 4 + randint1(6);
				r_ptr = &r_info[q_ptr->r_idx];
			}

			if (q_ptr->max_num == 0)
			{
				/* Random monster number */
				if (randint1(10) > 7)
					q_ptr->max_num = 1;
				else
					q_ptr->max_num = randint1(3) + 1;
			}

			q_ptr->cur_num = 0;
			name = (r_name + r_ptr->name);
#ifdef JP
			msg_format("クエスト: %sを %d体倒す", name,q_ptr->max_num);
#else
			msg_format("Your quest: kill %d %s", q_ptr->max_num, name);
#endif

		}
		else
		{
			get_questinfo(q_index);
		}

		/* check */
#ifdef JP
		if (!get_check("このクエストの依頼を受けますか？"))
#else
		if (!get_check("Do you accept this quest? "))
#endif
		{
			int i;
			for (i = 5; i < 18; i++)
				prt ("", i, 0);
			return;
		}

#ifdef JP
		msg_print("クエストの依頼を受けました。");
#else
		msg_print("You accept this quest.");
#endif

		q_ptr->status = QUEST_STATUS_TAKEN;

		switch (q_index)
		{
		case QUEST_MONTSALVAT:
		case QUEST_HOLY_KNIGHTS:
			misc_event_flags |= EVENT_CANNOT_BE_WHITEKNIGHT;
			break;

		case QUEST_RAMZEN:
			misc_event_flags |= EVENT_CANNOT_BE_TEMPLEKNIGHT;
			break;

		default:
			break;
		}

		reinit_wilderness = TRUE;
	}
}


/*
 * Display town history
 */
static void town_history(void)
{
	/* Save screen */
	screen_save();

	/* Peruse the building help file */
#ifdef JP
	(void)show_file(TRUE, "jbldg.txt", NULL, 0, 0);
#else
	(void)show_file(TRUE, "bldg.txt", NULL, 0, 0);
#endif


	/* Load screen */
	screen_load();
}


/*
 * Display the damage figure of an object
 * (used by compare_weapon_aux1)
 *
 * Only accurate for the current weapon, because it includes
 * the current +dam of the player.
 */
static void compare_weapon_aux2(object_type *o_ptr, int numblows,
                                int r, int c, int mult, cptr attr, byte color)
{
	char tmp_str[80];
	int hack_dd = o_ptr->dd + p_ptr->to_dd[0];
	int hack_ds = o_ptr->ds + p_ptr->to_ds[0];

	/* Print the intro text */
	c_put_str(color, attr, r, c);

	/* Calculate the min and max damage figures */
#ifdef JP
	sprintf(tmp_str, "１ターン: %d-%d ダメージ",
#else
	sprintf(tmp_str, "Attack: %d-%d damage",
#endif

	    (numblows * (mult * hack_dd /60 + o_ptr->to_d + p_ptr->to_d[0])),
	    (numblows * (mult * hack_ds * hack_dd /60 + o_ptr->to_d + p_ptr->to_d[0])));

	/* Print the damage */
	put_str(tmp_str, r, c + 8);
}


/*
 * Show the damage figures for the various monster types
 *
 * Only accurate for the current weapon, because it includes
 * the current number of blows for the player.
 */
static void compare_weapon_aux1(object_type *o_ptr, int col, int r)
{
	int i, mult = 60;
	u32b flgs[TR_FLAG_SIZE];
	int blow = p_ptr->num_blow[0];
	
	/* Get the flags of the weapon */
	object_flags(o_ptr, flgs);

	/* Extract flags and store */
	player_flags(p_ptr->flags);

	for (i = 0; i < TR_FLAG_SIZE; i++) flgs[i] |= p_ptr->flags[i];

	if ((have_flag(flgs, TR_FORCE_WEAPON)) && (p_ptr->csp > (o_ptr->dd * o_ptr->ds / 5))) mult = mult * 7 / 2;

	/* Print the relevant lines */
#ifdef JP
	if (have_flag(flgs, TR_FORCE_WEAPON)) compare_weapon_aux2(o_ptr, blow, r++, col, 1*mult, "理力:", TERM_L_BLUE);
	if (have_flag(flgs, TR_KILL_ANIMAL)) compare_weapon_aux2(o_ptr, blow, r++, col, 4*mult, "動物:", TERM_YELLOW);
	 else if (have_flag(flgs, TR_SLAY_ANIMAL)) compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult/2, "動物:", TERM_YELLOW);
	if (have_flag(flgs, TR_KILL_EVIL))   compare_weapon_aux2(o_ptr, blow, r++, col, 7*mult/2, "邪悪:", TERM_YELLOW);
	 else if (have_flag(flgs, TR_SLAY_EVIL)) compare_weapon_aux2(o_ptr, blow, r++, col, 2*mult, "邪悪:", TERM_YELLOW);
	if (have_flag(flgs, TR_KILL_GOOD)) compare_weapon_aux2(o_ptr, blow, r++, col, 7*mult/2, "善良:", TERM_YELLOW);
	else if (have_flag(flgs, TR_SLAY_GOOD)) compare_weapon_aux2(o_ptr, blow, r++, col, 2*mult, "善良:", TERM_YELLOW);
	if (have_flag(flgs, TR_KILL_LIVING)) compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult, "生命:", TERM_YELLOW);
	 else if (have_flag(flgs, TR_SLAY_LIVING)) compare_weapon_aux2(o_ptr, blow, r++, col, 3*mult, "生命:", TERM_YELLOW);
	if (have_flag(flgs, TR_KILL_HUMAN)) compare_weapon_aux2(o_ptr, blow, r++, col, 4*mult, "人間:", TERM_YELLOW);
	 else if (have_flag(flgs, TR_SLAY_HUMAN)) compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult/2, "人間:", TERM_YELLOW);
	if (have_flag(flgs, TR_KILL_UNDEAD)) compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult, "不死:", TERM_YELLOW);
	 else if (have_flag(flgs, TR_SLAY_UNDEAD)) compare_weapon_aux2(o_ptr, blow, r++, col, 3*mult, "不死:", TERM_YELLOW);
	if (have_flag(flgs, TR_KILL_DEMON)) compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult, "悪魔:", TERM_YELLOW);
	else if (have_flag(flgs, TR_SLAY_DEMON)) compare_weapon_aux2(o_ptr, blow, r++, col, 3*mult, "悪魔:", TERM_YELLOW);
	if (have_flag(flgs, TR_KILL_ORC))    compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult, "オーク:", TERM_YELLOW);
	 else if (have_flag(flgs, TR_SLAY_ORC))    compare_weapon_aux2(o_ptr, blow, r++, col, 3*mult, "オーク:", TERM_YELLOW);
	if (have_flag(flgs, TR_KILL_TROLL))  compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult, "トロル:", TERM_YELLOW);
	 else if (have_flag(flgs, TR_SLAY_TROLL))  compare_weapon_aux2(o_ptr, blow, r++, col, 3*mult, "トロル:", TERM_YELLOW);
	if (have_flag(flgs, TR_KILL_GIANT))  compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult, "巨人:", TERM_YELLOW);
	 else if (have_flag(flgs, TR_SLAY_GIANT))  compare_weapon_aux2(o_ptr, blow, r++, col, 3*mult, "巨人:", TERM_YELLOW);
	if (have_flag(flgs, TR_KILL_DRAGON)) compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult, "竜:", TERM_YELLOW);
	else if (have_flag(flgs, TR_SLAY_DRAGON)) compare_weapon_aux2(o_ptr, blow, r++, col, 3*mult, "竜:", TERM_YELLOW);
	if (have_flag(flgs, TR_BRAND_ACID)) compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult/2, "酸属性:", TERM_RED);
	if (have_flag(flgs, TR_BRAND_ELEC)) compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult/2, "電属性:", TERM_RED);
	if (have_flag(flgs, TR_BRAND_FIRE)) compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult/2, "炎属性:", TERM_RED);
	if (have_flag(flgs, TR_BRAND_COLD)) compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult/2, "冷属性:", TERM_RED);
	if (have_flag(flgs, TR_BRAND_POIS)) compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult/2, "毒属性:", TERM_RED);
	if (have_flag(flgs, TR_TUNNEL) && (o_ptr->to_misc[OB_TUNNEL] > 0)) compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult, "掘削:", TERM_RED);
#else
	if (have_flag(flgs, TR_FORCE_WEAPON)) compare_weapon_aux2(o_ptr, blow, r++, col, 1*mult, "Force  :", TERM_L_BLUE);
	if (have_flag(flgs, TR_KILL_ANIMAL)) compare_weapon_aux2(o_ptr, blow, r++, col, 4*mult, "Animals:", TERM_YELLOW);
	 else if (have_flag(flgs, TR_SLAY_ANIMAL)) compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult/2, "Animals:", TERM_YELLOW);
	if (have_flag(flgs, TR_KILL_EVIL))   compare_weapon_aux2(o_ptr, blow, r++, col, 7*mult/2, "Evil:", TERM_YELLOW);
	 else if (have_flag(flgs, TR_SLAY_EVIL))   compare_weapon_aux2(o_ptr, blow, r++, col, 2*mult, "Evil:", TERM_YELLOW);
	if (have_flag(flgs, TR_KILL_GOOD)) compare_weapon_aux2(o_ptr, blow, r++, col, 7*mult/2, "Good:", TERM_YELLOW);
	 else if (have_flag(flgs, TR_SLAY_GOOD)) compare_weapon_aux2(o_ptr, blow, r++, col, 2*mult, "Good:", TERM_YELLOW);
	if (have_flag(flgs, TR_KILL_LIVING)) compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult, "Living:", TERM_YELLOW);
	 else if (have_flag(flgs, TR_SLAY_LIVING)) compare_weapon_aux2(o_ptr, blow, r++, col, 3*mult, "Living:", TERM_YELLOW);
	if (have_flag(flgs, TR_KILL_HUMAN))   compare_weapon_aux2(o_ptr, blow, r++, col, 4*mult, "Human:", TERM_YELLOW);
	 else if (have_flag(flgs, TR_SLAY_HUMAN))   compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult/2, "Human:", TERM_YELLOW);
	if (have_flag(flgs, TR_KILL_UNDEAD)) compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult, "Undead:", TERM_YELLOW);
	 else if (have_flag(flgs, TR_SLAY_UNDEAD)) compare_weapon_aux2(o_ptr, blow, r++, col, 3*mult, "Undead:", TERM_YELLOW);
	if (have_flag(flgs, TR_KILL_DEMON))  compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult, "Demons:", TERM_YELLOW);
	 else if (have_flag(flgs, TR_SLAY_DEMON))  compare_weapon_aux2(o_ptr, blow, r++, col, 3*mult, "Demons:", TERM_YELLOW);
	if (have_flag(flgs, TR_KILL_ORC))    compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult, "Orcs:", TERM_YELLOW);
	 else if (have_flag(flgs, TR_SLAY_ORC))    compare_weapon_aux2(o_ptr, blow, r++, col, 3*mult, "Orcs:", TERM_YELLOW);
	if (have_flag(flgs, TR_KILL_TROLL))  compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult, "Trolls:", TERM_YELLOW);
	 else if (have_flag(flgs, TR_SLAY_TROLL))  compare_weapon_aux2(o_ptr, blow, r++, col, 3*mult, "Trolls:", TERM_YELLOW);
	if (have_flag(flgs, TR_KILL_GIANT))  compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult, "Giants:", TERM_YELLOW);
	 else if (have_flag(flgs, TR_SLAY_GIANT))  compare_weapon_aux2(o_ptr, blow, r++, col, 3*mult, "Giants:", TERM_YELLOW);
	if (have_flag(flgs, TR_KILL_DRAGON)) compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult, "Dragons:", TERM_YELLOW);
	 else if (have_flag(flgs, TR_SLAY_DRAGON)) compare_weapon_aux2(o_ptr, blow, r++, col, 3*mult, "Dragons:", TERM_YELLOW);
	if (have_flag(flgs, TR_BRAND_ACID)) compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult/2, "Acid:", TERM_RED);
	if (have_flag(flgs, TR_BRAND_ELEC)) compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult/2, "Elec:", TERM_RED);
	if (have_flag(flgs, TR_BRAND_FIRE)) compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult/2, "Fire:", TERM_RED);
	if (have_flag(flgs, TR_BRAND_COLD)) compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult/2, "Cold:", TERM_RED);
	if (have_flag(flgs, TR_BRAND_POIS)) compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult/2, "Poison:", TERM_RED);
	if (have_flag(flgs, TR_TUNNEL) && (o_ptr->to_misc[OB_TUNNEL] > 0)) compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult, "Digging:", TERM_RED);
#endif

}

static int hit_chance(int to_h, int ac)
{
	int chance = 0;
	int meichuu = p_ptr->skill_thn + (p_ptr->to_h[0] + to_h) * BTH_PLUS_ADJ;

	if (meichuu <= 0) return 5;
	if (meichuu > (SKILL_LIKERT_MYTHICAL_MAX * SKILL_DIV_XTHN)) meichuu = SKILL_LIKERT_MYTHICAL_MAX * SKILL_DIV_XTHN;

	chance = 100 - ((ac * 75) / meichuu);

	if (chance > 95) chance = 95;
	if (chance < 5) chance = 5;
	return chance;
}

/*
 * Displays all info about a weapon
 *
 * Only accurate for the current weapon, because it includes
 * various info about the player's +to_dam and number of blows.
 */
static void list_weapon(object_type *o_ptr, int row, int col)
{
	char o_name[MAX_NLEN];
	char tmp_str[80];
	int hack_dd = o_ptr->dd + p_ptr->to_dd[0];
	int hack_ds = o_ptr->ds + p_ptr->to_ds[0];

	/* Print the weapon name */
	object_desc(o_name, o_ptr, OD_NAME_ONLY);
	c_put_str(TERM_YELLOW, o_name, row, col);

	/* Print the player's number of blows */
#ifdef JP
	sprintf(tmp_str, "攻撃回数: %d", p_ptr->num_blow[0]);
#else
	sprintf(tmp_str, "Number of Blows: %d", p_ptr->num_blow[0]);
#endif

	put_str(tmp_str, row+1, col);

	/* Print to_hit and to_dam of the weapon */
#ifdef JP
	sprintf(tmp_str, "命中率:  0  50 100 150 200 (敵のAC)");
#else
	sprintf(tmp_str, "To Hit:  0  50 100 150 200 (AC)");
#endif

	put_str(tmp_str, row+2, col);

	/* Print the weapons base damage dice */
#ifdef JP
	sprintf(tmp_str, "        %2d  %2d  %2d  %2d  %2d (%%)", hit_chance(o_ptr->to_h, 0), hit_chance(o_ptr->to_h, 50), hit_chance(o_ptr->to_h, 100), hit_chance(o_ptr->to_h, 150), hit_chance(o_ptr->to_h, 200));
#else
	sprintf(tmp_str, "        %2d  %2d  %2d  %2d  %2d (%%)", hit_chance(o_ptr->to_h, 0), hit_chance(o_ptr->to_h, 50), hit_chance(o_ptr->to_h, 100), hit_chance(o_ptr->to_h, 150), hit_chance(o_ptr->to_h, 200));
#endif

	put_str(tmp_str, row+3, col);

#ifdef JP
	c_put_str(TERM_YELLOW, "可能なダメージ:", row+5, col);
#else
	c_put_str(TERM_YELLOW, "Possible Damage:", row+5, col);
#endif


	/* Damage for one blow (if it hits) */
#ifdef JP
	sprintf(tmp_str, "攻撃一回につき %d-%d",
#else
	sprintf(tmp_str, "One Strike: %d-%d damage",
#endif

	    hack_dd + o_ptr->to_d + p_ptr->to_d[0],
	    hack_ds * hack_dd + o_ptr->to_d + p_ptr->to_d[0]);
	put_str(tmp_str, row+6, col+1);

	/* Damage for the complete attack (if all blows hit) */
#ifdef JP
	sprintf(tmp_str, "１ターンにつき %d-%d",
#else
	sprintf(tmp_str, "One Attack: %d-%d damage",
#endif

	    p_ptr->num_blow[0] * (hack_dd + o_ptr->to_d + p_ptr->to_d[0]),
	    p_ptr->num_blow[0] * (hack_ds * hack_dd + o_ptr->to_d + p_ptr->to_d[0]));
	put_str(tmp_str, row+7, col+1);
}


/*
 * Compare weapons
 *
 * Copies the weapons to compare into the weapon-slot and
 * compares the values for both weapons.
 */
static bool compare_weapons(void)
{
	int item, item2;
	object_type *o1_ptr, *o2_ptr;
	object_type orig_weapon;
	object_type *i_ptr;
	cptr q, s;
	int row = 2;

	screen_save();
	/* Clear the screen */
	clear_bldg(0, 22);

	/* Store copy of original wielded weapon */
	i_ptr = &inventory[INVEN_RARM];
	object_copy(&orig_weapon, i_ptr);

	item_tester_no_ryoute = TRUE;
	/* Only compare melee weapons */
	item_tester_hook = object_is_melee_weapon;

	/* Get the first weapon */
#ifdef JP
	q = "第一の武器は？";
	s = "比べるものがありません。";
#else
	q = "What is your first weapon? ";
	s = "You have nothing to compare.";
#endif

	if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN)))
	{
		screen_load();
		return (FALSE);
	}

	/* Get the item (in the pack) */
	o1_ptr = &inventory[item];

	/* Clear the screen */
	clear_bldg(0, 22);

	item_tester_no_ryoute = TRUE;
	/* Only compare melee weapons */
	item_tester_hook = object_is_melee_weapon;

	/* Get the second weapon */
#ifdef JP
	q = "第二の武器は？";
	s = "比べるものがありません。";
#else
	q = "What is your second weapon? ";
	s = "You have nothing to compare.";
#endif

	if (!get_item(&item2, q, s, (USE_EQUIP | USE_INVEN)))
	{
		screen_load();
		return (FALSE);
	}

	/* Get the item (in the pack) */
	o2_ptr = &inventory[item2];

	/* Clear the screen */
	clear_bldg(0, 22);

	/* Copy first weapon into the weapon slot (if it's not already there) */
	if (o1_ptr != i_ptr)
		object_copy(i_ptr, o1_ptr);

	/* Get the new values */
	calc_bonuses();

	/* List the new values */
	list_weapon(o1_ptr, row, 2);
	compare_weapon_aux1(o1_ptr, 2, row + 8);

	/* Copy second weapon into the weapon slot (if it's not already there) */
	if (o2_ptr != i_ptr)
		object_copy(i_ptr, o2_ptr);
	else
		object_copy(i_ptr, &orig_weapon);

	/* Get the new values */
	calc_bonuses();

	/* List the new values */
	list_weapon(o2_ptr, row, 40);
	compare_weapon_aux1(o2_ptr, 40, row + 8);

	/* Copy back the original weapon into the weapon slot */
	object_copy(i_ptr, &orig_weapon);

	/* Reset the values for the old weapon */
	calc_bonuses();

#ifdef JP
	put_str("(一番高いダメージが適用されます。複数の倍打効果は足し算されません。)", row + 4, 0);
#else
	put_str("(Only highest damage applies per monster. Special damage not cumulative.)", row + 4, 0);
#endif

#ifdef JP
	msg_print("現在の技量から判断すると、あなたの武器は以下のような威力を発揮します:");
#else
	msg_print("Based on your current abilities, here is what your weapons will do");
#endif


	flush();
	(void)inkey();
	screen_load();

	/* Done */
	return (TRUE);
}


/*
 * Evaluate AC
 *
 * ACから回避率、ダメージ減少率を計算し表示する
 * Calculate and display the dodge-rate and the protection-rate
 * based on AC
 */
static bool eval_ac(int iAC)
{
#ifdef JP
	const char memo[] =
		"ダメージ軽減率とは、敵の攻撃が当たった時そのダメージを\n"
		"何パーセント軽減するかを示します。\n"
		"ダメージ軽減は通常の直接攻撃(種類が「攻撃する」と「粉砕する」の物)\n"
		"に対してのみ効果があります。\n \n"
		"敵のレベルとは、その敵が通常何階に現れるかを示します。\n \n"
		"回避率は敵の直接攻撃を何パーセントの確率で避けるかを示し、\n"
		"敵のレベルとあなたのACによって決定されます。\n \n"
		"ダメージ期待値とは、敵の１００ポイントの通常攻撃に対し、\n"
		"回避率とダメージ軽減率を考慮したダメージの期待値を示します。\n";
#else
	const char memo[] =
		"'Protection Rate' means how much damage is reduced by your armor.\n"
		"Note that the Protection rate is effective only against normal "
		"'attack' and 'shatter' type melee attacks, "
		"and has no effect against any other types such as 'poison'.\n \n"
		"'Dodge Rate' indicates the success rate on dodging the "
		"monster's melee attacks.  "
		"It is depend on the level of the monster and your AC.\n \n"
		"'Average Damage' indicates the expected amount of damage "
		"when you are attacked by normal melee attacks with power=100.";
#endif

	int protection;
	int col, row = 2;
	int lvl;
	char buf[80*20], *t;

	/* AC lower than zero has no effect */
	if (iAC < 0) iAC = 0;

	/* ダメージ軽減率を計算 */
	protection = 100 * MIN(iAC, 150) / 250;

	screen_save();
	clear_bldg(0, 22);

#ifdef JP
	put_str(format("あなたの現在のAC: %3d", iAC), row++, 0);
	put_str(format("ダメージ軽減率  : %3d%%", protection), row++, 0);
	row++;

	put_str("敵のレベル      :", row + 0, 0);
	put_str("回避率          :", row + 1, 0);
	put_str("ダメージ期待値  :", row + 2, 0);
#else
	put_str(format("Your current AC : %3d", iAC), row++, 0);
	put_str(format("Protection rate : %3d%%", protection), row++, 0);
	row++;

	put_str("Level of Monster:", row + 0, 0);
	put_str("Dodge Rate      :", row + 1, 0);
	put_str("Average Damage  :", row + 2, 0);
#endif
    
	for (col = 17 + 1, lvl = 0; lvl <= 100; lvl += 10, col += 5)
	{
		int quality = 60 + lvl * 3; /* attack quality with power 60 */
		int dodge;   /* 回避率(%) */
		int average; /* ダメージ期待値 */

		put_str(format("%3d", lvl), row + 0, col);

		/* 回避率を計算 */
		dodge = 5 + (MIN(100, 100 * (iAC * 3 / 4) / quality) * 9 + 5) / 10;

		put_str(format("%3d%%", dodge), row + 1, col);

		/* 100点の攻撃に対してのダメージ期待値を計算 */
		average = (100 - dodge) * (100 - protection) / 100;
		put_str(format("%3d", average), row + 2, col);
	}

	/* Display note */
	roff_to_buf(memo, 70, buf, sizeof buf);
	for (t = buf; t[0]; t += strlen(t) + 1)
		put_str(t, (row++) + 4, 4);

#ifdef JP
	prt("現在のあなたの装備からすると、あなたの防御力は"
		   "これくらいです:", 0, 0);
#else
	prt("Defense abilities from your current Armor Class are evaluated below.", 0, 0);
#endif
  
	flush();
	(void)inkey();
	screen_load();

	/* Done */
	return (TRUE);
}


/*
 * Enchant item
 */
static bool enchant_item(int cost, int to_hit, int to_dam, int to_ac)
{
	int         i, item;
	bool        okay = FALSE;
	object_type *o_ptr;
	cptr        q, s;
	int         maxenchant = (p_ptr->town_num == 6) ? 15 : (p_ptr->lev / 5);
	char        tmp_str[MAX_NLEN];

	clear_bldg(4, 18);
#ifdef JP
	prt(format("現在のあなたの技量だと、+%d まで改良できます。", maxenchant), 5, 0);
	prt(format(" 改良の料金は一個につき$%d です。", cost), 7, 0);
#else
	prt(format("  Based on your skill, we can improve up to +%d.", maxenchant), 5, 0);
	prt(format("  The price for the service is %d gold per item.", cost), 7, 0);
#endif

	item_tester_no_ryoute = TRUE;

	/* Get an item */
#ifdef JP
	q = "どのアイテムを改良しますか？";
	s = "改良できるものがありません。";
#else
	q = "Improve which item? ";
	s = "You have nothing to improve.";
#endif

	if (!get_item(&item, q, s, (USE_INVEN | USE_EQUIP))) return (FALSE);

	/* Get the item (in the pack) */
	o_ptr = &inventory[item];

	/* Check if the player has enough money */
	if (p_ptr->au_sum < (cost * o_ptr->number))
	{
		object_desc(tmp_str, o_ptr, OD_NAME_ONLY);
#ifdef JP
		msg_format("%sを改良するだけのお金がありません！", tmp_str);
#else
		msg_format("You do not have the gold to improve %s!", tmp_str);
#endif

		return (FALSE);
	}

	if ((item == INVEN_RARM) && mw_diff_to_melee)
	{
		o_ptr->to_h -= mw_diff_to_melee;
		o_ptr->to_d -= mw_diff_to_melee;
	}

	/* Enchant to hit */
	for (i = 0; i < to_hit; i++)
	{
		if (o_ptr->to_h < maxenchant)
		{
			if (enchant(o_ptr, 1, (ENCH_TOHIT | ENCH_FORCE)))
			{
				okay = TRUE;
				break;
			}
		}
	}

	/* Enchant to damage */
	for (i = 0; i < to_dam; i++)
	{
		if (o_ptr->to_d < maxenchant)
		{
			if (enchant(o_ptr, 1, (ENCH_TODAM | ENCH_FORCE)))
			{
				okay = TRUE;
				break;
			}
		}
	}

	/* Enchant to AC */
	for (i = 0; i < to_ac; i++)
	{
		if (o_ptr->to_a < maxenchant)
		{
			if (enchant(o_ptr, 1, (ENCH_TOAC | ENCH_FORCE)))
			{
				okay = TRUE;
				break;
			}
		}
	}

	if ((item == INVEN_RARM) && mw_diff_to_melee)
	{
		o_ptr->to_h += mw_diff_to_melee;
		o_ptr->to_d += mw_diff_to_melee;
	}

	/* Failure */
	if (!okay)
	{
		/* Flush */
		if (flush_failure) flush();

		/* Message */
#ifdef JP
		msg_print("改良に失敗した。");
#else
		msg_print("The improvement failed.");
#endif

		return (FALSE);
	}
	else
	{
		object_desc(tmp_str, o_ptr, OD_NAME_AND_ENCHANT);
#ifdef JP
		msg_format("$%d で%sに改良しました。", cost * o_ptr->number, tmp_str );
#else
		msg_format("Improved into %s for %d gold.", tmp_str, cost * o_ptr->number);
#endif

		/* Charge the money */
		p_ptr->au_sum -= (cost * o_ptr->number);
		p_ptr->update |= (PU_GOLD);
		update_stuff();

		/* Something happened */
		return (TRUE);
	}
}


/*
 * Recharge rods, wands and staves
 *
 * The player can select the number of charges to add
 * (up to a limit), and the recharge never fails.
 *
 * The cost for rods depends on the level of the rod. The prices
 * for recharging wands and staves are dependent on the cost of
 * the base-item.
 */
static void building_recharge(void)
{
	int         item, lev;
	object_type *o_ptr;
	object_kind *k_ptr;
	cptr        q, s;
	int         price;
	int         charges;
	int         max_charges;
	char        tmp_str[MAX_NLEN];

	msg_flag = FALSE;

	/* Display some info */
	clear_bldg(4, 18);
#ifdef JP
	prt("  再充填の費用はアイテムの種類によります。", 6, 0);
#else
	prt("  The prices of recharge depend on the type.", 6, 0);
#endif


	/* Only accept legal items */
	item_tester_hook = object_is_recharge;

	/* Get an item */
#ifdef JP
	q = "どのアイテムに魔力を充填しますか? ";
	s = "魔力を充填すべきアイテムがない。";
#else
	q = "Recharge which item? ";
	s = "You have nothing to recharge.";
#endif

	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return;

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

	k_ptr = &k_info[o_ptr->k_idx];

	/*
	 * We don't want to give the player free info about
	 * the level of the item or the number of charges.
	 */
	/* The item must be "known" */
	if (!object_is_known(o_ptr))
	{
#ifdef JP
		msg_format("充填する前に鑑定されている必要があります！");
#else
		msg_format("The item must be identified first!");
#endif

		msg_print(NULL);

		if ((p_ptr->au_sum >= 50) &&
#ifdef JP
			get_check("$50で鑑定しますか？ "))
#else
			get_check("Identify for 50 gold? "))
#endif

		{
			/* Pay the price */
			p_ptr->au_sum -= 50;
			p_ptr->update |= (PU_GOLD);
			update_stuff();

			/* Identify it */
			identify_item(o_ptr);

			{
				int idx = is_autopick(o_ptr);
				(void)auto_inscribe_object(o_ptr, idx);
			}

			/* Description */
			object_desc(tmp_str, o_ptr, 0);

#ifdef JP
			msg_format("%s です。", tmp_str);
#else
			msg_format("You have: %s.", tmp_str);
#endif

			/* Update the gold display */
			building_prt_gold();
		}
		else
		{
			return;
		}
	}

	/* Extract the object "level" */
	lev = k_info[o_ptr->k_idx].level;

	/* Price for a rod */
	if (o_ptr->tval == TV_ROD)
	{
		if (o_ptr->timeout > 0)
		{
			/* Fully recharge */
			price = (lev * 50 * o_ptr->timeout) / k_ptr->pval;
		}
		else
		{
			/* No recharge necessary */
			price = 0;
#ifdef JP
			msg_format("それは再充填する必要はありません。");
#else
			msg_format("That doesn't need to be recharged.");
#endif

			return;
		}
	}
	else if (o_ptr->tval == TV_STAFF)
	{
		/* Price per charge ( = double the price paid by shopkeepers for the charge) */
		price = (k_info[o_ptr->k_idx].cost / 10) * o_ptr->number;

		/* Pay at least 10 gold per charge */
		price = MAX(10, price);
	}
	else
	{
		/* Price per charge ( = double the price paid by shopkeepers for the charge) */
		price = (k_info[o_ptr->k_idx].cost / 10);

		/* Pay at least 10 gold per charge */
		price = MAX(10, price);
	}

	/* Limit the number of charges for wands and staffs */
	if (o_ptr->tval == TV_WAND
		&& (o_ptr->pval / o_ptr->number >= k_ptr->pval))
	{
		if (o_ptr->number > 1)
		{
#ifdef JP
			msg_print("この魔法棒はもう充分に充填されています。");
#else
			msg_print("These wands are already fully charged.");
#endif
		}
		else
		{
#ifdef JP
			msg_print("この魔法棒はもう充分に充填されています。");
#else
			msg_print("This wand is already fully charged.");
#endif
		}
		return;
	}
	else if (o_ptr->tval == TV_STAFF && o_ptr->pval >= k_ptr->pval)
	{
		if (o_ptr->number > 1)
		{
#ifdef JP
			msg_print("この杖はもう充分に充填されています。");
#else
			msg_print("These staffs are already fully charged.");
#endif
		}
		else
		{
#ifdef JP
			msg_print("この杖はもう充分に充填されています。");
#else
			msg_print("This staff is already fully charged.");
#endif
		}
		return;
	}

	/* Check if the player has enough money */
	if (p_ptr->au_sum < price)
	{
		object_desc(tmp_str, o_ptr, OD_NAME_ONLY);
#ifdef JP
		msg_format("%sを再充填するには$%d 必要です！", tmp_str,price );
#else
		msg_format("You need %d gold to recharge %s!", price, tmp_str);
#endif

		return;
	}

	if (o_ptr->tval == TV_ROD)
	{
#ifdef JP
		if (get_check(format("そのロッドを$%d で再充填しますか？", price)))
#else
		if (get_check(format("Recharge the %s for %d gold? ",
			((o_ptr->number > 1) ? "rods" : "rod"), price)))
#endif

		{
			/* Recharge fully */
			o_ptr->timeout = 0;
		}
		else
		{
			return;
		}
	}
	else
	{
		if (o_ptr->tval == TV_STAFF)
			max_charges = k_ptr->pval - o_ptr->pval;
		else
			max_charges = o_ptr->number * k_ptr->pval - o_ptr->pval;

		/* Get the quantity for staves and wands */
#ifdef JP
		charges = get_quantity(format("一回分$%d で何回分充填しますか？",
#else
		charges = get_quantity(format("Add how many charges for %d gold? ",
#endif

		              price), MIN(p_ptr->au_sum / price, max_charges));

		/* Do nothing */
		if (charges < 1) return;

		/* Get the new price */
		price *= charges;

		/* Recharge */
		o_ptr->pval += charges;

		/* We no longer think the item is empty */
		o_ptr->ident &= ~(IDENT_EMPTY);
	}

	/* Give feedback */
	object_desc(tmp_str, o_ptr, 0);
#ifdef JP
	msg_format("%sを$%d で再充填しました。", tmp_str, price);
#else
	msg_format("%^s %s recharged for %d gold.", tmp_str, ((o_ptr->number > 1) ? "were" : "was"), price);
#endif

	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN);

	/* Pay the price */
	p_ptr->au_sum -= price;
	p_ptr->update |= (PU_GOLD);
	update_stuff();

	/* Finished */
	return;
}


/*
 * Recharge rods, wands and staves
 *
 * The player can select the number of charges to add
 * (up to a limit), and the recharge never fails.
 *
 * The cost for rods depends on the level of the rod. The prices
 * for recharging wands and staves are dependent on the cost of
 * the base-item.
 */
static void building_recharge_all(void)
{
	int         i;
	int         lev;
	object_type *o_ptr;
	object_kind *k_ptr;
	int         price = 0;
	int         total_cost = 0;


	/* Display some info */
	msg_flag = FALSE;
	clear_bldg(4, 18);
#ifdef JP
	prt("  再充填の費用はアイテムの種類によります。", 6, 0);
#else
	prt("  The prices of recharge depend on the type.", 6, 0);
#endif

	/* Calculate cost */
	for ( i = 0; i < INVEN_PACK; i++)
	{
		o_ptr = &inventory[i];
				
		/* skip non magic device */
		if (o_ptr->tval < TV_STAFF || o_ptr->tval > TV_ROD) continue;

		/* need identified */
		if (!object_is_known(o_ptr)) total_cost += 50;

		/* Extract the object "level" */
		lev = k_info[o_ptr->k_idx].level;

		k_ptr = &k_info[o_ptr->k_idx];

		switch (o_ptr->tval)
		{
		case TV_ROD:
			price = (lev * 50 * o_ptr->timeout) / k_ptr->pval;
			break;

		case TV_STAFF:
			/* Price per charge ( = double the price paid by shopkeepers for the charge) */
			price = (k_info[o_ptr->k_idx].cost / 10) * o_ptr->number;

			/* Pay at least 10 gold per charge */
			price = MAX(10, price);

			/* Fully charge */
			price = (k_ptr->pval - o_ptr->pval) * price;
			break;

		case TV_WAND:
			/* Price per charge ( = double the price paid by shopkeepers for the charge) */
			price = (k_info[o_ptr->k_idx].cost / 10);

			/* Pay at least 10 gold per charge */
			price = MAX(10, price);

			/* Fully charge */
			price = (o_ptr->number * k_ptr->pval - o_ptr->pval) * price;
			break;
		}

		/* if price <= 0 then item have enough charge */
		if (price > 0) total_cost += price;
	}

	if (!total_cost)
	{
#ifdef JP
		msg_print("充填する必要はありません。");
#else
		msg_print("No need to recharge.");
#endif

		msg_print(NULL);
		return;
	}

	/* Check if the player has enough money */
	if (p_ptr->au_sum < total_cost)
	{
#ifdef JP
		msg_format("すべてのアイテムを再充填するには$%d 必要です！", total_cost );
#else
		msg_format("You need %d gold to recharge all items!",total_cost);
#endif

		msg_print(NULL);
		return;
	}

#ifdef JP
	if (!get_check(format("すべてのアイテムを $%d で再充填しますか？",  total_cost))) return;
#else
	if (!get_check(format("Recharge all items for %d gold? ", total_cost))) return;
#endif

	for (i = 0; i < INVEN_PACK; i++)
	{
		o_ptr = &inventory[i];
		k_ptr = &k_info[o_ptr->k_idx];
				
		/* skip non magic device */
		if (o_ptr->tval < TV_STAFF || o_ptr->tval > TV_ROD) continue;

		/* Identify it */
		if (!object_is_known(o_ptr))
		{
			int idx;
			identify_item(o_ptr);

			idx = is_autopick(o_ptr);
			(void)auto_inscribe_object(o_ptr, idx);
		}

		/* Recharge */
		switch (o_ptr->tval)
		{
		case TV_ROD:
			o_ptr->timeout = 0;
			break;
		case TV_STAFF:
			if (o_ptr->pval < k_ptr->pval) o_ptr->pval = k_ptr->pval;
			/* We no longer think the item is empty */
			o_ptr->ident &= ~(IDENT_EMPTY);
			break;
		case TV_WAND:
			if (o_ptr->pval < o_ptr->number * k_ptr->pval)
				o_ptr->pval = o_ptr->number * k_ptr->pval;
			/* We no longer think the item is empty */
			o_ptr->ident &= ~(IDENT_EMPTY);
			break;
		}
	}

	/* Give feedback */
#ifdef JP
	msg_format("$%d で再充填しました。", total_cost);
#else
	msg_format("You pay %d gold.", total_cost);
#endif

	msg_print(NULL);

	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN);

	/* Pay the price */
	p_ptr->au_sum -= total_cost;
	p_ptr->update |= (PU_GOLD);
	update_stuff();

	/* Finished */
	return;
}


/* Locations of the tables on the screen */
#define HEADER_ROW		1
#define INSTRUCT_ROW	3
#define QUESTION_ROW	7
#define TABLE_ROW		10

#define QUESTION_COL	2
#define CLASS_COL		2
#define CLASS_AUX_COL   22

#define CLASS_WID		20


typedef struct cc_menu cc_menu;

struct cc_menu
{
	int  real;
	bool can_choose;
};

/*
 * Get the special blow of Temple-Knight
 */
static void get_temple_blow(void)
{
	cc_menu           blows[MAX_TEMPLE_SB];
	int               i, hgt;
	int               num = 0, top = 0, cur = 0;
	char              c;
	char              buf[80];
	bool              done = FALSE;
	byte              attr;
	char              s[80];
	special_blow_type *sb_ptr;

	/* Tabulate special blows */
	for (i = 0; i < MAX_TEMPLE_SB; i++)
	{
		if (p_ptr->special_blow & ((0x00000001L << MAX_SB) << i)) return;

		blows[num].real = i;
		blows[num++].can_choose = ((i != 1) || (p_ptr->psex == SEX_FEMALE));
	}

	/*** Instructions ***/

	/* Clear screen */
	Term_clear();

	/* Display some helpful information */
#ifdef JP
	Term_putstr(QUESTION_COL, HEADER_ROW, -1, TERM_L_BLUE,
	            "以下のメニューから覚えたい必殺技を選んでください。");
	Term_putstr(QUESTION_COL, INSTRUCT_ROW, -1, TERM_WHITE,
	            "移動キーで項目をスクロールさせ、Enterで決定します。");

	/* Hack - highlight the key names */
	Term_putstr(QUESTION_COL + 0, INSTRUCT_ROW, -1, TERM_L_GREEN, "移動キー");
	Term_putstr(QUESTION_COL + 32, INSTRUCT_ROW, -1, TERM_L_GREEN, "Enter");
#else
	Term_putstr(QUESTION_COL, HEADER_ROW, -1, TERM_L_BLUE,
	            "Please select new special blow from the menu below:");
	Term_putstr(QUESTION_COL, INSTRUCT_ROW, -1, TERM_WHITE,
 	            "Use the movement keys to scroll the menu, Enter to select the current");
	Term_putstr(QUESTION_COL, INSTRUCT_ROW + 1, -1, TERM_WHITE,
	            "menu item.");

	/* Hack - highlight the key names */
	Term_putstr(QUESTION_COL + 8, INSTRUCT_ROW, -1, TERM_L_GREEN, "movement keys");
	Term_putstr(QUESTION_COL + 42, INSTRUCT_ROW, -1, TERM_L_GREEN, "Enter");
#endif

	/* Choose */
	while (TRUE)
	{
		hgt = Term->hgt - TABLE_ROW - 1;

		/* Redraw the list */
		for (i = 0; ((i + top < num) && (i <= hgt)); i++)
		{
			sb_ptr = &temple_blow_info[blows[i + top].real];
			if (i + top < 26)
			{
				sprintf(buf, "%c) %s", I2A(i + top), sb_ptr->name);
			}
			else
			{
				/* ToDo: Fix the ASCII dependency */
				sprintf(buf, "%c) %s", 'A' + (i + top - 26), sb_ptr->name);
			}

			/* Clear */
			Term_erase(CLASS_COL, i + TABLE_ROW, CLASS_WID);

			/* Display */
			/* Highlight the current selection */
			if (i == (cur - top)) attr = blows[i + top].can_choose ? TERM_L_BLUE : TERM_BLUE;
			else attr = blows[i + top].can_choose ? TERM_WHITE : TERM_SLATE;

			Term_putstr(CLASS_COL, i + TABLE_ROW, CLASS_WID, attr, buf);
		}

		sb_ptr = &temple_blow_info[blows[cur].real];

		prt("", TABLE_ROW, CLASS_AUX_COL);
		strcpy(s, "対象武器:");
		for (i = 1; i <= MAX_WT; i++)
		{
			if (weapon_type_bit(i) & sb_ptr->weapon_type)
			{
				strcat(s, " ");
				strcat(s, weapon_skill_name[i]);
			}
		}
		Term_putstr(CLASS_AUX_COL, TABLE_ROW, -1, TERM_WHITE, s);
		prt("", TABLE_ROW + 1, CLASS_AUX_COL);
		sprintf(s, "レベル: %2d, コスト: %2d", sb_ptr->level, sb_ptr->cost);
		Term_putstr(CLASS_AUX_COL, TABLE_ROW + 1, -1, TERM_WHITE, s);

		if (blows[cur].can_choose)
		{
			strcpy(s, "                                  ");
			Term_putstr(CLASS_AUX_COL, TABLE_ROW + A_MAX + 5, -1, TERM_WHITE, s);
		}
		else
		{
			strcpy(s, "この必殺技は女性のみ習得可能です。");
			Term_putstr(CLASS_AUX_COL, TABLE_ROW + A_MAX + 5, -1, TERM_L_RED, s);
		}

		if (done)
		{
			char temp[80 * 9];
			cptr t;

			clear_from(TABLE_ROW);
			Term_putstr(CLASS_COL, TABLE_ROW, -1, TERM_L_BLUE, sb_ptr->name);

			roff_to_buf(sb_ptr->text, 74, temp, sizeof temp);
			t = temp;

			for (i = 0; i < 9; i++)
			{
				if (t[0] == 0)
					break; 
				else
				{
					prt(t, TABLE_ROW + 2 + i, CLASS_COL);
					t += strlen(t) + 1;
				}
			}

#ifdef JP
			if (get_check("よろしいですか？"))
#else
			if (get_check("Are you sure? "))
#endif
				break;

			clear_from(TABLE_ROW);
			done = FALSE;
			continue;
		}

		/* Move the cursor */
		put_str("", TABLE_ROW + cur - top, CLASS_COL);

		c = inkey();

		switch (c)
		{
		case '\n':
		case '\r':
		case ' ':
			/* Done */
			if (blows[cur].can_choose) done = TRUE;
			break;

		case '8':
			if (cur != 0)
			{
				/* Move selection */
				cur--;
			}

			if ((top > 0) && ((cur - top) < 4))
			{
				/* Scroll up */
				top--;
			}
			break;

		case '2':
			if (cur != (num - 1))
			{
				/* Move selection */
				cur++;
			}

			if ((top + hgt < (num - 1)) && ((top + hgt - cur) < 4))
			{
				/* Scroll down */
				top++;
			}
			break;

		default:
			if (isalpha(c))
			{
				int choice;

				if (islower(c))
				{
					choice = A2I(c);
				}
				else
				{
					choice = c - 'A' + 26;
				}

				/* Validate input */
				if ((choice > -1) && (choice < num))
				{
					cur = choice;

					/* Move it onto the screen */
					if ((cur < top) || (cur > top + hgt))
					{
						top = cur;
					}

					/* Done */
					if (blows[cur].can_choose) done = TRUE;
				}
				else
				{
					bell();
				}
			}

			/* Invalid input */
			else bell();
			break;
		}
	}

	sb_ptr = &temple_blow_info[blows[cur].real];

	/* Clear */
	clear_from(TABLE_ROW);

#ifdef JP
	sprintf(buf, "%sを習得した！", sb_ptr->name);
#else
	sprintf(buf, "You learned %s!", sb_ptr->name);
#endif

	p_ptr->special_blow |= ((0x00000001L << MAX_SB) << blows[cur].real);

	Term_putstr(QUESTION_COL, TABLE_ROW, -1, TERM_WHITE, buf);
	message_add(buf);
#ifdef JP
	Term_putstr(QUESTION_COL, TABLE_ROW + 2, -1, TERM_WHITE,
		"[ 何かキーを押してください ]");
#else
	Term_putstr(QUESTION_COL, TABLE_ROW + 2, -1, TERM_WHITE,
		"[Press any key to continue]");
#endif
	inkey();

	return;
}

static bool change_class(int cmd)
{
	int i, total_max_clev = 0, experienced_classes = 0;
	byte new_class = 0;
	byte old_pclass = p_ptr->pclass;
	cexp_info_type *cexp_ptr;
	char buf[80];

	/* Calculate character total class level */
	for (i = 0; i < max_c_idx; i++)
	{
		if (p_ptr->cexp_info[i].max_clev > 0)
		{
			total_max_clev += p_ptr->cexp_info[i].max_clev;
			experienced_classes++;
		}
	}

	switch (cmd)
	{
	case BACT_JOIN_LODIS_KNIGHTS:
		new_class = CLASS_TEMPLEKNIGHT;
		break;
	case BACT_JOIN_ZENOBIAN_KNIGHTS:
		new_class = CLASS_WHITEKNIGHT;
		break;
	case BACT_CHANGE_SUCCUBUS:
		new_class = CLASS_SUCCUBUS;
		break;
	}

	if (p_ptr->pclass == new_class)
	{
#ifdef JP
		msg_format("あなたはすでに%sです。", c_name + cp_ptr->name);
#else
		msg_format("You are already %s.", c_name + cp_ptr->name);
#endif
		return FALSE;
	}
	else if (!(can_choose_class(new_class, CLASS_CHOOSE_MODE_BLDGS)))
	{
#ifdef JP
		msg_print("クラスチェンジできない。");
#else
		msg_print("You cannot choose this class now.");
#endif
		return FALSE;
	}
	else
	{
		char temp[80 * 9];
		cptr t;

		Term_putstr(CLASS_COL, TABLE_ROW - 5, -1, TERM_L_BLUE, c_name + class_info[new_class].name);

		roff_to_buf(c_text + class_info[new_class].text, 74, temp, sizeof temp);
		t = temp;

		for (i = 0; i < 9; i++)
		{
			if (t[0] == 0)
				break; 
			else
			{
				prt(t, TABLE_ROW - 3 + i, CLASS_COL);
				t += strlen(t) + 1;
			}
		}

#ifdef JP
		if (!get_check("クラスチェンジしますか？"))
#else
		if (!get_check("Are you sure? "))
#endif
		{
			clear_bldg(4,18);
			return FALSE;
		}

	}

	/* Set class */
	p_ptr->pclass = new_class;
	cp_ptr = &class_info[p_ptr->pclass];
	mp_ptr = &m_info[p_ptr->pclass];
	p_ptr->s_ptr = &s_info[p_ptr->pclass];
	cexp_ptr = &p_ptr->cexp_info[p_ptr->pclass];

#ifdef JP
	msg_format("%sから%sへとクラスチェンジしました。",
#else
	msg_format("Your class has changed from %s to %s.",
#endif
		c_name + class_info[old_pclass].name, c_name + cp_ptr->name);

#ifdef JP
	sprintf(buf,"%sから%sへとクラスチェンジした。", c_name + class_info[old_pclass].name, c_name + cp_ptr->name);
#else
	sprintf(buf,"changed class from %s to %s.", c_name + class_info[old_pclass].name, c_name + cp_ptr->name);
#endif
	do_cmd_write_nikki(NIKKI_BUNSHOU, 0, buf);

	switch (p_ptr->pclass)
	{
		case CLASS_TEMPLEKNIGHT:
			change_level99_quest(TRUE);
			get_temple_blow();
			misc_event_flags |= EVENT_CANNOT_BE_WHITEKNIGHT;
			break;
		case CLASS_WHITEKNIGHT:
			misc_event_flags |= EVENT_CANNOT_BE_TEMPLEKNIGHT;
			break;
	}

	if (cp_ptr->c_flags & PCF_NO_DIGEST) p_ptr->food = PY_FOOD_FULL - 1;

	for (i = 0; i < MAX_REALM+1; i++)
		if (p_ptr->magic_exp[i] == 0) p_ptr->magic_exp[i] = p_ptr->s_ptr->s_eff[i];

	if (!cexp_ptr->max_clev)
	{
		cexp_ptr->max_clev = cexp_ptr->clev = 1;
		if (!cexp_ptr->max_max_clev) cexp_ptr->max_max_clev = 1;
		p_ptr->cexpfact[p_ptr->pclass] = class_info[p_ptr->pclass].c_exp + 50 * experienced_classes + total_max_clev / 5 * 10;
	}

	/* Notice stuff */
	notice_stuff();

	/* Update stuff */
	update_stuff();

	if (p_ptr->chp > p_ptr->mhp) p_ptr->chp = p_ptr->mhp;
	if (p_ptr->csp > p_ptr->msp) p_ptr->csp = p_ptr->msp;

	dispel_player();
	set_action(ACTION_NONE);
	init_realm_table();

	/* Update stuff */
	p_ptr->update |= (PU_BONUS | PU_HP | PU_MANA | PU_LITE | PU_SPELLS);

	/* Redraw stuff */
	p_ptr->redraw |= (PR_WIPE | PR_BASIC | PR_EXTRA | PR_EQUIPPY);

	/* Window stuff */
	p_ptr->window |= (PW_MESSAGE | PW_SPELL | PW_PLAYER);

	/* Combine / Reorder the pack */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	(void)combine_and_reorder_home(STORE_HOME);
	(void)combine_and_reorder_home(STORE_MUSEUM);

	/* Load the "pref" files */
	load_all_pref_files();

	clear_bldg(4,18);

	return TRUE;
}


bool tele_town(bool magic)
{
	int i, x, y;
	int num = 0;

	if (dun_level)
	{
#ifdef JP
		msg_print("この魔法は地上でしか使えない！");
#else
		msg_print("This spell can only be used on the surface!");
#endif
		return FALSE;
	}

	if (p_ptr->inside_arena)
	{
#ifdef JP
		msg_print("この魔法は外でしか使えない！");
#else
		msg_print("This spell can only be used outside!");
#endif
		return FALSE;
	}

	screen_save();
	clear_bldg(4, 14);

	for (i=1;i<max_towns;i++)
	{
		char buf[80];

		if ((!magic && ((i == TOWN_BARMAMUTHA) || (i == TOWN_LOST_ISLAND))) ||
			(i == p_ptr->town_num) || ((i == TOWN_ARMORICA) && (quest[QUEST_ARMORICA].status < QUEST_STATUS_COMPLETED)) ||
			!(p_ptr->visit & (1L << (i-1)))) continue;

		sprintf(buf,"%c) %-20s", I2A(i-1), town[i].name);
		prt(buf, 5+i, 5);
		num++;
	}

	if (!num)
	{
#ifdef JP
		msg_print("まだ行けるところがない。");
#else
		msg_print("You have not yet visited any town.");
#endif

		msg_print(NULL);
		screen_load();
		return FALSE;
	}

#ifdef JP
	prt("どこに行きますか:", 0, 0);
#else
	prt("Which town you go: ", 0, 0);
#endif
	while(1)
	{
		i = inkey();

		if (i == ESCAPE)
		{
			screen_load();
			return FALSE;
		}
		else if ((i < 'a') || (i > ('a'+max_towns-2))) continue;
		else if (((i-'a'+1) == p_ptr->town_num) ||
			(!magic && (((i-'a'+1) == TOWN_BARMAMUTHA) || ((i-'a'+1) == TOWN_LOST_ISLAND))) ||
			!(p_ptr->visit & (1L << (i-'a')))) continue;
		break;
	}

	for (y = 0; y < max_wild_y; y++)
	{
		for (x = 0; x < max_wild_x; x++)
		{
			if(wilderness[y][x].town == (i-'a'+1))
			{
				p_ptr->wilderness_y = y;
				p_ptr->wilderness_x = x;
			}
		}
	}

	p_ptr->leaving = TRUE;
	leave_bldg = TRUE;
	p_ptr->teleport_town = TRUE;
	screen_load();
	return TRUE;
}


/*
 *  research_mon
 *  -KMW-
 */
static bool research_mon(void)
{
	int i, n, r_idx;
	char sym, query;
	char buf[128];

	bool notpicked;

	bool recall = FALSE;

	u16b why = 0;

	u16b	*who;

	/* XTRA HACK WHATSEARCH */
	bool    all = FALSE;
	bool    uniq = FALSE;
	bool    norm = FALSE;
	char temp[80] = "";

	/* XTRA HACK REMEMBER_IDX */
	static int old_sym = '\0';
	static int old_i = 0;


	/* Save the screen */
	screen_save();

	/* Get a character, or abort */
#ifdef JP
	if (!get_com("モンスターの文字を入力して下さい(記号 or ^A全,^Uユ,^N非ユ,^M名前):", &sym, FALSE)) 
#else
	if (!get_com("Enter character to be identified(^A:All,^U:Uniqs,^N:Non uniqs,^M:Name): ", &sym, FALSE))
#endif

	{
		/* Restore */
		screen_load();

		return (FALSE);
	}

	/* Find that character info, and describe it */
	for (i = 0; ident_info[i]; ++i)
	{
		if (sym == ident_info[i][0]) break;
	}

		/* XTRA HACK WHATSEARCH */
	if (sym == KTRL('A'))
	{
		all = TRUE;
#ifdef JP
		strcpy(buf, "全モンスターのリスト");
#else
		strcpy(buf, "Full monster list.");
#endif
	}
	else if (sym == KTRL('U'))
	{
		all = uniq = TRUE;
#ifdef JP
		strcpy(buf, "ユニーク・モンスターのリスト");
#else
		strcpy(buf, "Unique monster list.");
#endif
	}
	else if (sym == KTRL('N'))
	{
		all = norm = TRUE;
#ifdef JP
		strcpy(buf, "ユニーク外モンスターのリスト");
#else
		strcpy(buf, "Non-unique monster list.");
#endif
	}
	else if (sym == KTRL('M'))
	{
		all = TRUE;
#ifdef JP
		if (!get_string("名前(英語の場合小文字で可)",temp, 70))
#else
		if (!get_string("Enter name:",temp, 70))
#endif
		{
			temp[0]=0;

			/* Restore */
			screen_load();

			return FALSE;
		}
#ifdef JP
		sprintf(buf, "名前:%sにマッチ",temp);
#else
		sprintf(buf, "Monsters with a name \"%s\"",temp);
#endif
	}
	else if (ident_info[i])
	{
		sprintf(buf, "%c - %s.", sym, ident_info[i] + 2);
	}
	else
	{
#ifdef JP
		sprintf(buf, "%c - %s", sym, "無効な文字");
#else
		sprintf(buf, "%c - %s.", sym, "Unknown Symbol");
#endif

	}

	/* Display the result */
	prt(buf, 16, 10);


	/* Allocate the "who" array */
	C_MAKE(who, max_r_idx, u16b);

	/* Collect matching monsters */
	for (n = 0, i = 1; i < max_r_idx; i++)
	{
		monster_race *r_ptr = &r_info[i];

		/* XTRA HACK WHATSEARCH */
		/* Require non-unique monsters if needed */
		if (norm && (r_ptr->flags1 & (RF1_UNIQUE))) continue;

		/* Require unique monsters if needed */
		if (uniq && !(r_ptr->flags1 & (RF1_UNIQUE))) continue;

		/* 名前検索 */
		if (temp[0]){
		  int xx;
		  char temp2[80];
  
		  for (xx=0; temp[xx] && xx<80; xx++){
#ifdef JP
		    if (iskanji( temp[xx])) { xx++; continue; }
#endif
		    if (isupper(temp[xx])) temp[xx]=tolower(temp[xx]);
		  }
  
#ifdef JP
		  strcpy(temp2, r_name+r_ptr->E_name);
#else
		  strcpy(temp2, r_name+r_ptr->name);
#endif
		  for (xx=0; temp2[xx] && xx<80; xx++)
		    if (isupper(temp2[xx])) temp2[xx]=tolower(temp2[xx]);
  
#ifdef JP
		  if (my_strstr(temp2, temp) || my_strstr(r_name + r_ptr->name, temp) )
#else
		  if (my_strstr(temp2, temp))
#endif
			  who[n++]=i;
		}
		else if (all || (r_ptr->d_char == sym)) who[n++] = i;
	}

	/* Nothing to recall */
	if (!n)
	{
		/* Free the "who" array */
		C_KILL(who, max_r_idx, u16b);

		/* Restore */
		screen_load();

		return (FALSE);
	}

	/* Sort by level */
	why = 2;
	query = 'y';

	/* Sort if needed */
	if (why)
	{
		/* Select the sort method */
		ang_sort_comp = ang_sort_comp_hook;
		ang_sort_swap = ang_sort_swap_hook;

		/* Sort the array */
		ang_sort(who, &why, n);
	}


	/* Start at the end */
	/* XTRA HACK REMEMBER_IDX */
	if (old_sym == sym && old_i < n) i = old_i;
	else i = n - 1;

	notpicked = TRUE;

	/* Scan the monster memory */
	while (notpicked)
	{
		/* Extract a race */
		r_idx = who[i];

		/* Save this monster ID */
		p_ptr->monster_race_idx = r_idx;

		/* Hack -- Handle stuff */
		handle_stuff();

		/* Hack -- Begin the prompt */
		roff_top(r_idx);

		/* Hack -- Complete the prompt */
#ifdef JP
		Term_addstr(-1, TERM_WHITE, " ['r'思い出, ' 'で続行, ESC]");
#else
		Term_addstr(-1, TERM_WHITE, " [(r)ecall, ESC, space to continue]");
#endif


		/* Interact */
		while (1)
		{
			/* Recall */
			if (recall)
			{
				/* Get maximal info about this monster */
				lore_do_probe(r_idx);
			
				/* know every thing mode */
				screen_roff(r_idx, 0x01);
				notpicked = FALSE;

				/* XTRA HACK REMEMBER_IDX */
				old_sym = sym;
				old_i = i;
			}

			/* Command */
			query = inkey();

			/* Normal commands */
			if (query != 'r') break;

			/* Toggle recall */
			recall = !recall;
		}

		/* Stop scanning */
		if (query == ESCAPE) break;

		/* Move to "prev" monster */
		if (query == '-')
		{
			if (++i == n)
			{
				i = 0;
				if (!expand_list) break;
			}
		}

		/* Move to "next" monster */
		else
		{
			if (i-- == 0)
			{
				i = n - 1;
				if (!expand_list) break;
			}
		}
	}


	/* Re-display the identity */
	/* prt(buf, 5, 5);*/

	/* Free the "who" array */
	C_KILL(who, max_r_idx, u16b);

	/* Restore */
	screen_load();

	return (!notpicked);
}


static bool calc_cost(object_type *dest_ptr, object_type *src_ptr)
{
	int price = 0;

	price += k_info[dest_ptr->k_idx].cost + flag_cost(dest_ptr);
	price += k_info[src_ptr->k_idx].cost + flag_cost(src_ptr);

	/* Check if the player has enough money */
	if (p_ptr->au_sum < price)
	{
#ifdef JP
		msg_format("合成するには$%d 必要です！", price );
#else
		msg_format("You need %d gold to conposite!", price);
#endif
		return FALSE;
	}

	else
	{
#ifdef JP
		if (!get_check(format("$%d で合成しますか？", price)))
#else
		if (!get_check(format("Composite objects for %d gold? ",price)))
#endif
		{
			return FALSE;
		}
	}

	/* Pay the price */
	p_ptr->au_sum -= price;
	p_ptr->update |= (PU_GOLD);
	update_stuff();

	return TRUE;
}


static void merge_flags1(object_type * dest_ptr, object_type * src_ptr)
{
	int i;

	if (dest_ptr->dd > src_ptr->dd) src_ptr->dd += (dest_ptr->dd - k_info[dest_ptr->k_idx].dd) / 2;
	if (dest_ptr->ds > src_ptr->ds) src_ptr->ds += (dest_ptr->ds - k_info[dest_ptr->k_idx].ds) / 2;
	if (dest_ptr->ac > src_ptr->ac) src_ptr->ac += (dest_ptr->ac - k_info[dest_ptr->k_idx].ac) / 2;
	if (dest_ptr->to_h > 0) src_ptr->to_h += (dest_ptr->to_h - k_info[dest_ptr->k_idx].to_h) / 3;
	else src_ptr->to_h += (dest_ptr->to_h - k_info[dest_ptr->k_idx].to_h);
	if (dest_ptr->to_d > 0) src_ptr->to_d += (dest_ptr->to_d - k_info[dest_ptr->k_idx].to_d) / 3;
	else src_ptr->to_d += (dest_ptr->to_d - k_info[dest_ptr->k_idx].to_d);
	if (dest_ptr->to_a > 0) src_ptr->to_a += (dest_ptr->to_a - k_info[dest_ptr->k_idx].to_a) / 3;
	else src_ptr->to_a += (dest_ptr->to_a - k_info[dest_ptr->k_idx].to_a);

	if ((src_ptr->to_h) && (src_ptr->to_d)) add_flag(src_ptr->art_flags, TR_SHOW_MODS);
	if (((src_ptr->to_h) || (src_ptr->to_d)) && (src_ptr->tval == TV_GLOVES)) add_flag(src_ptr->art_flags, TR_SHOW_MODS);

	src_ptr->dd = MIN(8, src_ptr->dd);
	src_ptr->ds = MIN(8, src_ptr->ds);
	src_ptr->to_h = MIN(30, src_ptr->to_h);
	src_ptr->to_d = MIN(30, src_ptr->to_d);
	src_ptr->to_a = MIN(40, src_ptr->to_a);


	for (i = 0; i < A_MAX; i++)
	{
		src_ptr->to_stat[i] += dest_ptr->to_stat[i];
		src_ptr->to_stat[i] = MIN(2, src_ptr->to_stat[i]);
	}

	for (i = 0; i < OB_MAX; i++) src_ptr->to_misc[i] += dest_ptr->to_misc[i];
	src_ptr->to_misc[OB_MAGIC_MASTERY] = MIN(9, src_ptr->to_misc[OB_MAGIC_MASTERY]);
	src_ptr->to_misc[OB_STEALTH] = MIN(9, src_ptr->to_misc[OB_STEALTH]);
	src_ptr->to_misc[OB_SEARCH] = MIN(10, src_ptr->to_misc[OB_SEARCH]);
	src_ptr->to_misc[OB_INFRA] = MIN(10, src_ptr->to_misc[OB_INFRA]);
	src_ptr->to_misc[OB_TUNNEL] = MIN(10, src_ptr->to_misc[OB_TUNNEL]);
	src_ptr->to_misc[OB_SPEED] = MIN(10, src_ptr->to_misc[OB_SPEED]);
	src_ptr->to_misc[OB_BLOWS] = MIN(2, src_ptr->to_misc[OB_BLOWS]);
	src_ptr->to_misc[OB_ANTI_MAGIC] = MIN(3, src_ptr->to_misc[OB_ANTI_MAGIC]);

	for (i = 0; i < ALI_MAX; i++) src_ptr->to_align[i] += dest_ptr->to_align[i];
}


static void merge_flags2(object_type * dest_ptr, object_type * src_ptr)
{
	int i;
	u32b mflgs[TR_FLAG_SIZE];
	u32b flgs1[TR_FLAG_SIZE];
	u32b flgs2[TR_FLAG_SIZE];

	/* Get the flags of the weapon */
	object_flags(dest_ptr, flgs1);
	object_flags(src_ptr, flgs2);
	object_flags(src_ptr, mflgs);

	for (i = 0; i < TR_FLAG_SIZE; i++) mflgs[i] |= flgs1[i];

	if ((have_flag(flgs1, TR_SLAY_EVIL)) && (have_flag(flgs2, TR_SLAY_EVIL)) && one_in_(7))
		add_flag(src_ptr->art_flags, TR_KILL_EVIL);
	else if (have_flag(mflgs, TR_KILL_EVIL)) add_flag(src_ptr->art_flags, TR_KILL_EVIL);
	else if (have_flag(mflgs, TR_SLAY_EVIL)) add_flag(src_ptr->art_flags, TR_SLAY_EVIL);
	if ((have_flag(flgs1, TR_SLAY_GOOD)) && (have_flag(flgs2, TR_SLAY_GOOD)) && one_in_(7))
		add_flag(src_ptr->art_flags, TR_KILL_GOOD);
	else if (have_flag(mflgs, TR_KILL_GOOD)) add_flag(src_ptr->art_flags, TR_KILL_GOOD);
	else if (have_flag(mflgs, TR_SLAY_GOOD)) add_flag(src_ptr->art_flags, TR_SLAY_GOOD);
	if ((have_flag(flgs1, TR_SLAY_LIVING)) && (have_flag(flgs2, TR_SLAY_LIVING)) && one_in_(7))
		add_flag(src_ptr->art_flags, TR_KILL_LIVING);
	else if (have_flag(mflgs, TR_KILL_LIVING)) add_flag(src_ptr->art_flags, TR_KILL_LIVING);
	else if (have_flag(mflgs, TR_SLAY_LIVING)) add_flag(src_ptr->art_flags, TR_SLAY_LIVING);

	if ((have_flag(flgs1, TR_SLAY_ANIMAL)) && (have_flag(flgs2, TR_SLAY_ANIMAL)))
		add_flag(src_ptr->art_flags, TR_KILL_ANIMAL);
	else if (have_flag(mflgs, TR_KILL_ANIMAL)) add_flag(src_ptr->art_flags, TR_KILL_ANIMAL);
	else if (have_flag(mflgs, TR_SLAY_ANIMAL)) add_flag(src_ptr->art_flags, TR_SLAY_ANIMAL);
	if ((have_flag(flgs1, TR_SLAY_HUMAN)) && (have_flag(flgs2, TR_SLAY_HUMAN)))
		add_flag(src_ptr->art_flags, TR_KILL_HUMAN);
	else if (have_flag(mflgs, TR_KILL_HUMAN)) add_flag(src_ptr->art_flags, TR_KILL_HUMAN);
	else if (have_flag(mflgs, TR_SLAY_HUMAN)) add_flag(src_ptr->art_flags, TR_SLAY_HUMAN);
	if ((have_flag(flgs1, TR_SLAY_UNDEAD)) && (have_flag(flgs2, TR_SLAY_UNDEAD)))
		add_flag(src_ptr->art_flags, TR_KILL_UNDEAD);
	else if (have_flag(mflgs, TR_KILL_UNDEAD)) add_flag(src_ptr->art_flags, TR_KILL_UNDEAD);
	else if (have_flag(mflgs, TR_SLAY_UNDEAD)) add_flag(src_ptr->art_flags, TR_SLAY_UNDEAD);
	if ((have_flag(flgs1, TR_SLAY_DEMON)) && (have_flag(flgs2, TR_SLAY_DEMON)))
		add_flag(src_ptr->art_flags, TR_KILL_DEMON);
	else if (have_flag(mflgs, TR_KILL_DEMON)) add_flag(src_ptr->art_flags, TR_KILL_DEMON);
	else if (have_flag(mflgs, TR_SLAY_DEMON)) add_flag(src_ptr->art_flags, TR_SLAY_DEMON);
	if ((have_flag(flgs1, TR_SLAY_ORC)) && (have_flag(flgs2, TR_SLAY_ORC)))
		add_flag(src_ptr->art_flags, TR_SLAY_ORC);
	else if (have_flag(mflgs, TR_SLAY_ORC)) add_flag(src_ptr->art_flags, TR_SLAY_ORC);
	else if (have_flag(mflgs, TR_SLAY_ORC)) add_flag(src_ptr->art_flags, TR_SLAY_ORC);
	if ((have_flag(flgs1, TR_SLAY_TROLL)) && (have_flag(flgs2, TR_SLAY_TROLL)))
		add_flag(src_ptr->art_flags, TR_KILL_TROLL);
	else if (have_flag(mflgs, TR_KILL_TROLL)) add_flag(src_ptr->art_flags, TR_KILL_TROLL);
	else if (have_flag(mflgs, TR_SLAY_TROLL)) add_flag(src_ptr->art_flags, TR_SLAY_TROLL);
	if ((have_flag(flgs1, TR_SLAY_GIANT)) && (have_flag(flgs2, TR_SLAY_GIANT)))
		add_flag(src_ptr->art_flags, TR_KILL_GIANT);
	else if (have_flag(mflgs, TR_KILL_GIANT)) add_flag(src_ptr->art_flags, TR_KILL_GIANT);
	else if (have_flag(mflgs, TR_SLAY_GIANT)) add_flag(src_ptr->art_flags, TR_SLAY_GIANT);
	if ((have_flag(flgs1, TR_SLAY_DRAGON)) && (have_flag(flgs2, TR_SLAY_DRAGON)))
		add_flag(src_ptr->art_flags, TR_KILL_DRAGON);
	else if (have_flag(mflgs, TR_KILL_DRAGON)) add_flag(src_ptr->art_flags, TR_KILL_DRAGON);
	else if (have_flag(mflgs, TR_SLAY_DRAGON)) add_flag(src_ptr->art_flags, TR_SLAY_DRAGON);

	if (have_flag(mflgs, TR_FORCE_WEAPON)) add_flag(src_ptr->art_flags, TR_FORCE_WEAPON);
	if (have_flag(mflgs, TR_CHAOTIC)) add_flag(src_ptr->art_flags, TR_CHAOTIC);
	if (have_flag(mflgs, TR_VAMPIRIC)) add_flag(src_ptr->art_flags, TR_VAMPIRIC);
	if (have_flag(mflgs, TR_IMPACT)) add_flag(src_ptr->art_flags, TR_IMPACT);
	if (have_flag(mflgs, TR_BRAND_ACID)) add_flag(src_ptr->art_flags, TR_BRAND_ACID);
	if (have_flag(mflgs, TR_BRAND_ELEC)) add_flag(src_ptr->art_flags, TR_BRAND_ELEC);
	if (have_flag(mflgs, TR_BRAND_FIRE)) add_flag(src_ptr->art_flags, TR_BRAND_FIRE);
	if (have_flag(mflgs, TR_BRAND_COLD)) add_flag(src_ptr->art_flags, TR_BRAND_COLD);
	if (have_flag(mflgs, TR_BRAND_POIS)) add_flag(src_ptr->art_flags, TR_BRAND_POIS);

	if ((have_flag(flgs1, TR_VORPAL)) && (have_flag(flgs2, TR_VORPAL)) && one_in_(7))
		add_flag(src_ptr->art_flags, TR_EXTRA_VORPAL);
	else if (have_flag(mflgs, TR_EXTRA_VORPAL)) add_flag(src_ptr->art_flags, TR_EXTRA_VORPAL);
	else if (have_flag(mflgs, TR_VORPAL)) add_flag(src_ptr->art_flags, TR_VORPAL);

	if ((have_flag(flgs1, TR_BLESSED)) && (have_flag(flgs2, TR_UNHOLY))) remove_flag(src_ptr->art_flags, TR_UNHOLY);
	else if (have_flag(mflgs, TR_BLESSED)) add_flag(src_ptr->art_flags, TR_BLESSED);
	if ((have_flag(flgs1, TR_UNHOLY)) && (have_flag(flgs2, TR_BLESSED))) remove_flag(src_ptr->art_flags, TR_BLESSED);
	else if (have_flag(mflgs, TR_UNHOLY)) add_flag(src_ptr->art_flags, TR_UNHOLY);
}


static void merge_flags3(object_type * dest_ptr, object_type * src_ptr)
{
	int i;
	u32b mflgs[TR_FLAG_SIZE];
	u32b flgs1[TR_FLAG_SIZE];

	/* Get the flags of the weapon */
	object_flags(dest_ptr, flgs1);
	object_flags(src_ptr, mflgs);

	for (i = 0; i < TR_FLAG_SIZE; i++) mflgs[i] |= flgs1[i];

	/* Sustain */
	if (have_flag(mflgs, TR_SUST_STR)) add_flag(src_ptr->art_flags, TR_SUST_STR);
	if (have_flag(mflgs, TR_SUST_INT)) add_flag(src_ptr->art_flags, TR_SUST_INT);
	if (have_flag(mflgs, TR_SUST_WIS)) add_flag(src_ptr->art_flags, TR_SUST_WIS);
	if (have_flag(mflgs, TR_SUST_DEX)) add_flag(src_ptr->art_flags, TR_SUST_DEX);
	if (have_flag(mflgs, TR_SUST_CON)) add_flag(src_ptr->art_flags, TR_SUST_CON);
	if (have_flag(mflgs, TR_SUST_CHR)) add_flag(src_ptr->art_flags, TR_SUST_CHR);

	/* Immunity */
	if (have_flag(mflgs, TR_IM_ACID)) add_flag(src_ptr->art_flags, TR_IM_ACID);
	if (have_flag(mflgs, TR_IM_ELEC)) add_flag(src_ptr->art_flags, TR_IM_ELEC);
	if (have_flag(mflgs, TR_IM_COLD)) add_flag(src_ptr->art_flags, TR_IM_COLD);
	if (have_flag(mflgs, TR_IM_FIRE)) add_flag(src_ptr->art_flags, TR_IM_FIRE);

	/* Resistance */
	if (have_flag(mflgs, TR_RES_ACID)) add_flag(src_ptr->art_flags, TR_RES_ACID);
	if (have_flag(mflgs, TR_RES_ELEC)) add_flag(src_ptr->art_flags, TR_RES_ELEC);
	if (have_flag(mflgs, TR_RES_COLD)) add_flag(src_ptr->art_flags, TR_RES_COLD);
	if (have_flag(mflgs, TR_RES_FIRE)) add_flag(src_ptr->art_flags, TR_RES_FIRE);
	if (have_flag(mflgs, TR_RES_POIS)) add_flag(src_ptr->art_flags, TR_RES_POIS);
	if (have_flag(mflgs, TR_RES_LITE)) add_flag(src_ptr->art_flags, TR_RES_LITE);
	if (have_flag(mflgs, TR_RES_DARK)) add_flag(src_ptr->art_flags, TR_RES_DARK);
	if (have_flag(mflgs, TR_RES_SHARDS)) add_flag(src_ptr->art_flags, TR_RES_SHARDS);
	if (have_flag(mflgs, TR_RES_BLIND)) add_flag(src_ptr->art_flags, TR_RES_BLIND);
	if (have_flag(mflgs, TR_RES_CONF)) add_flag(src_ptr->art_flags, TR_RES_CONF);
	if (have_flag(mflgs, TR_RES_SOUND)) add_flag(src_ptr->art_flags, TR_RES_SOUND);
	if (have_flag(mflgs, TR_RES_NETHER)) add_flag(src_ptr->art_flags, TR_RES_NETHER);
	if (have_flag(mflgs, TR_RES_STONE)) add_flag(src_ptr->art_flags, TR_RES_STONE);
	if (have_flag(mflgs, TR_RES_CHAOS)) add_flag(src_ptr->art_flags, TR_RES_CHAOS);
	if (have_flag(mflgs, TR_RES_DISEN)) add_flag(src_ptr->art_flags, TR_RES_DISEN);
	if (have_flag(mflgs, TR_RES_FEAR)) add_flag(src_ptr->art_flags, TR_RES_FEAR);

	if (have_flag(mflgs, TR_REFLECT)) add_flag(src_ptr->art_flags, TR_REFLECT);
	if (have_flag(mflgs, TR_SH_FIRE)) add_flag(src_ptr->art_flags, TR_SH_FIRE);
	if (have_flag(mflgs, TR_SH_ELEC)) add_flag(src_ptr->art_flags, TR_SH_ELEC);
	if (have_flag(mflgs, TR_SH_COLD)) add_flag(src_ptr->art_flags, TR_SH_COLD);

	if (have_flag(mflgs, TR_FREE_ACT)) add_flag(src_ptr->art_flags, TR_FREE_ACT);
	if (have_flag(mflgs, TR_SEE_INVIS)) add_flag(src_ptr->art_flags, TR_SEE_INVIS);
	if (have_flag(mflgs, TR_HOLD_LIFE)) add_flag(src_ptr->art_flags, TR_HOLD_LIFE);
	if (have_flag(mflgs, TR_WARNING)) add_flag(src_ptr->art_flags, TR_WARNING);
	if (have_flag(mflgs, TR_SLOW_DIGEST)) add_flag(src_ptr->art_flags, TR_SLOW_DIGEST);
	if (have_flag(mflgs, TR_REGEN)) add_flag(src_ptr->art_flags, TR_REGEN);
	if (have_flag(mflgs, TR_LEVITATION)) add_flag(src_ptr->art_flags, TR_LEVITATION);
	if (have_flag(mflgs, TR_LITE)) add_flag(src_ptr->art_flags, TR_LITE);

	if (have_flag(mflgs, TR_TELEPATHY)) add_flag(src_ptr->art_flags, TR_TELEPATHY);
	if (have_flag(mflgs, TR_REGEN_MANA)) add_flag(src_ptr->art_flags, TR_REGEN_MANA);
	if (have_flag(mflgs, TR_RES_MAGIC)) add_flag(src_ptr->art_flags, TR_RES_MAGIC);
	if (have_flag(mflgs, TR_DEC_MANA)) add_flag(src_ptr->art_flags, TR_DEC_MANA);
	if (have_flag(mflgs, TR_EASY_SPELL)) add_flag(src_ptr->art_flags, TR_EASY_SPELL);
	if (have_flag(mflgs, TR_WRAITH)) add_flag(src_ptr->art_flags, TR_WRAITH);
	if (have_flag(mflgs, TR_FEAR_FIELD)) add_flag(src_ptr->art_flags, TR_FEAR_FIELD);
	if (have_flag(mflgs, TR_DRAIN_EXP)) add_flag(src_ptr->art_flags, TR_DRAIN_EXP);
	if (have_flag(mflgs, TR_TELEPORT)) add_flag(src_ptr->art_flags, TR_TELEPORT);
	if (have_flag(mflgs, TR_NO_TELE)) add_flag(src_ptr->art_flags, TR_NO_TELE);
	if (have_flag(mflgs, TR_NO_MAGIC)) add_flag(src_ptr->art_flags, TR_NO_MAGIC);
	if (have_flag(mflgs, TR_AGGRAVATE)) add_flag(src_ptr->art_flags, TR_AGGRAVATE);
	if (have_flag(mflgs, TR_TY_CURSE)) add_flag(src_ptr->art_flags, TR_TY_CURSE);

	if (dest_ptr->curse_flags & TRC_CURSED) src_ptr->curse_flags |= TRC_CURSED;
	if (dest_ptr->curse_flags & TRC_HEAVY_CURSE) src_ptr->curse_flags |= TRC_HEAVY_CURSE;
	if (dest_ptr->curse_flags & TRC_PERMA_CURSE) src_ptr->curse_flags |= TRC_PERMA_CURSE;
}


static cptr compose_name(int dest_name, int src_name)
{
	cptr new_name;

	switch (dest_name)
	{
		/* Body Armor */
		case EGO_PERMANENCE:
			if (src_name == EGO_POWER) new_name = "永続のパワー";
			else if (src_name == EGO_BALANCE) new_name = "永続のバランス";
			else new_name = "永続の";
			break;
		case EGO_ARCH_MAGI:
			if (src_name == EGO_POWER) new_name = "大魔術師のパワー";
			else if (src_name == EGO_BALANCE) new_name = "大魔術師のバランス";
			else new_name = "大魔術師の";
			break;
		case EGO_BALANCE:
			if (src_name == EGO_PERMANENCE) new_name = "均衡の永続の";
			else if (src_name == EGO_ARCH_MAGI) new_name = "均衡の大";
			else new_name = "均衡の";
			break;
		case EGO_POWER:
			if (src_name == EGO_PERMANENCE) new_name = "完全耐性の永続の";
			else if (src_name == EGO_ARCH_MAGI) new_name = "完全耐性の大";
			else new_name = "完全耐性の";
			break;

		/* Weapon */
		case EGO_HA:
			switch (src_name)
			{
				case EGO_DF:
					new_name = "(聖防衛者)";
					break;
				case EGO_KILL_GOOD:
				case EGO_ASMODE:
				case EGO_MORGUL:
				case EGO_NETHERWORLD:
					new_name = "聖魔の";
					break;
				default:
					new_name = "(*聖戦者*)";
					break;
			}
			break;
		case EGO_DF:
			switch (src_name)
			{
				case EGO_HA:
				case EGO_KILL_EVIL:
				case EGO_ISHTALLE:
					new_name = "(聖防衛者)";
					break;
				case EGO_KILL_GOOD:
				case EGO_ASMODE:
				case EGO_MORGUL:
				case EGO_NETHERWORLD:
					new_name = "(魔防衛者)";
					break;
				default:
					new_name = "(*防衛者*)";
					break;
			}
			break;
		case EGO_ISHTALLE:
		case EGO_KILL_EVIL:
			switch (src_name)
			{
				case EGO_HA:
					new_name = "(*聖戦者*)";
					break;
				case EGO_DF:
					new_name = "(聖防衛者)";
					break;
				case EGO_ASMODE:
				case EGO_MORGUL:
				case EGO_NETHERWORLD:
					new_name = "聖魔の";
					break;
				default:
					new_name = "合成";
					break;
			}
			break;
		case EGO_KILL_GOOD:
		case EGO_ASMODE:
		case EGO_MORGUL:
		case EGO_NETHERWORLD:
			switch (src_name)
			{
				case EGO_HA:
				case EGO_ISHTALLE:
				case EGO_KILL_EVIL:
					new_name = "聖魔の";
					break;
				case EGO_DF:
					new_name = "(魔防衛者)";
					break;
				default:
					new_name = "(魔戦士)";
					break;
			}
			break;
		default:
			new_name = "合成";
			break;
	}

	return new_name;
}

static bool item_tester_composite(object_type *o_ptr)
{
	switch (o_ptr->tval)
	{
		case TV_HAFTED:
		case TV_POLEARM:
		case TV_SWORD:
		case TV_DIGGING:
		case TV_HARD_ARMOR:
		case TV_SOFT_ARMOR:
		case TV_SHIELD:
		case TV_CROWN:
		case TV_HELM:
		case TV_BOOTS:
		case TV_GLOVES:
		{
			return (TRUE);
		}
	}

	return (FALSE);
}


static bool composite_item(void)
{
	int src_item, dest_item;
	bool is_armor = FALSE;

	object_type *src_ptr, *dest_ptr;
	object_type tmp_obj;

	cptr q, s;

	item_tester_hook = item_tester_composite;

	/* Get an item */
#ifdef JP
	q = "第一のアイテムは？";
	s = "合成できるものがありません。";
#else
	q = "Fennel which item? ";
	s = "You have nothing to fennel.";
#endif
	if (!get_item(&src_item, q, s, (USE_EQUIP | USE_INVEN)))
		return FALSE;

	src_ptr = &inventory[src_item];
	if (object_is_armour(src_ptr)) is_armor = TRUE;

	if (is_armor) item_tester_tval = src_ptr->tval;
	else item_tester_wt = get_weapon_type(&k_info[src_ptr->k_idx]);

	/* Get an item */
#ifdef JP
	q = "第二のアイテムは？";
	s = "合成できるものがありません。";
#else
	q = "Into which other item? ";
	s = "You have nothing to fennel.";
#endif
	if (!get_item(&dest_item, q, s, (USE_EQUIP | USE_INVEN)))
		return FALSE;

	if (src_item == dest_item)
	{
#ifdef JP
		msg_print("それ自身には合成できない");
#else
		msg_print("You cannot fennel an object into itself.");
#endif
		return FALSE;
	}
	if (inventory[dest_item].number > 1)
	{
		object_copy(&tmp_obj, &inventory[dest_item]);

		tmp_obj.number = 1;

		dest_ptr = &tmp_obj;
	}
	else
		dest_ptr = &inventory[dest_item];

	if (object_is_artifact(src_ptr) || object_is_artifact(dest_ptr))
	{
#ifdef JP
		msg_print("アーティファクトは合成できません.");
#else
		msg_print("You cannot fennel artifacts.");
#endif
		return FALSE;
	}

	if (!calc_cost(dest_ptr, src_ptr)) return FALSE;

	merge_flags1(dest_ptr, src_ptr);
	merge_flags2(dest_ptr, src_ptr);
	merge_flags3(dest_ptr, src_ptr);

	add_flag(src_ptr->art_flags, TR_IGNORE_ACID);
	add_flag(src_ptr->art_flags, TR_IGNORE_FIRE);
	add_flag(src_ptr->art_flags, TR_IGNORE_ELEC);
	add_flag(src_ptr->art_flags, TR_IGNORE_COLD);

	src_ptr->art_name = quark_add(compose_name(dest_ptr->name2, src_ptr->name2));

	/* Nuke enchantments */
	src_ptr->name1 = 0;
	src_ptr->name2 = 0;

	/* Paranoia. This should never be needed. If it is, the weight
	 * gets screwed up. */
	dest_ptr->number = 1;

	/* Handle the case of there having been more than one copy
	 * of the destination object. */
	inven_item_increase(dest_item, -1);
	inven_item_optimize(dest_item);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Recalculate torch */
	p_ptr->update |= (PU_TORCH);

	/* Recalculate mana */
	p_ptr->update |= (PU_MANA);

	p_ptr->redraw |= (PR_EQUIPPY);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);

	return TRUE;
}


/*
 * Execute a building command
 */
static void bldg_process_command(building_type *bldg, int i)
{
	int bact = bldg->actions[i];
	int bcost = bldg->costs[i];
	bool paid = FALSE;
	int amt;

	/* Flush messages XXX XXX XXX */
	msg_flag = FALSE;
	msg_print(NULL);

	/* action restrictions */
	if ((bldg->action_restr[i] == 1) && !can_eat_food())
	{
#ifdef JP
		msg_print("それを選択することはできません！");
#else
		msg_print("You have no right to choose that!");
#endif
		return;
	}

	/* check gold (HACK - Recharge uses variable costs) */
	if ((bact != BACT_RECHARGE) && ((bldg->costs[i] > p_ptr->au_sum) && can_eat_food()))
	{
#ifdef JP
		msg_print("お金が足りません！");
#else
		msg_print("You do not have the gold!");
#endif
		return;
	}

	switch (bact)
	{
	case BACT_NOTHING:
		/* Do nothing */
		break;
	case BACT_RESEARCH_ITEM:
		paid = identify_fully(FALSE);
		break;
	case BACT_TOWN_HISTORY:
		town_history();
		break;
	case BACT_RACE_LEGENDS:
		race_legends();
		break;
	case BACT_QUEST:
		castle_quest();
		break;
	case BACT_KING_LEGENDS:
	case BACT_ARENA_LEGENDS:
	case BACT_LEGENDS:
		show_highclass();
		break;
	case BACT_POSTER:
	case BACT_ARENA_RULES:
	case BACT_ARENA:
		arena_comm(bact);
		break;
	case BACT_IN_BETWEEN:
	case BACT_CRAPS:
	case BACT_SPIN_WHEEL:
	case BACT_DICE_SLOTS:
	case BACT_GAMBLE_RULES:
	case BACT_POKER:
		gamble_comm(bact);
		break;
	case BACT_REST:
	case BACT_RUMORS:
	case BACT_FOOD:
		paid = inn_comm(bact);
		break;
	case BACT_RESEARCH_MONSTER:
		paid = research_mon();
		break;
	case BACT_COMPARE_WEAPONS:
		paid = compare_weapons();
		break;
	case BACT_ENCHANT_WEAPON:
		item_tester_hook = object_is_melee_weapon;
		enchant_item(bcost, 1, 1, 0);
		break;
	case BACT_ENCHANT_ARMOR:
		item_tester_hook = object_is_armour;
		enchant_item(bcost, 0, 0, 1);
		break;
	case BACT_RECHARGE:
		building_recharge();
		break;
	case BACT_RECHARGE_ALL:
		building_recharge_all();
		break;
	case BACT_IDENTS: /* needs work */
#ifdef JP
		if (!get_check("持ち物を全て鑑定してよろしいですか？")) break;
		identify_pack();
		msg_print("持ち物全てが鑑定されました。");
#else
		if (!get_check("Do you pay for identify all your possession? ")) break;
		identify_pack();
		msg_print("Your possessions have been identified.");
#endif

		paid = TRUE;
		break;
	case BACT_IDENT_ONE: /* needs work */
		paid = ident_spell(FALSE);
		break;
	case BACT_LEARN:
#ifdef JP
		msg_print("使用できません。");
#else
		msg_print("Obsoleted.");
#endif
		break;
	case BACT_HEALING: /* needs work */
		hp_player(200);
		set_poisoned(0);
		set_blind(0);
		set_confused(0);
		set_cut(0);
		set_stun(0);
		set_stoning(0);
		paid = TRUE;
		break;
	case BACT_RESTORE: /* needs work */
		if (do_res_stat(A_STR)) paid = TRUE;
		if (do_res_stat(A_INT)) paid = TRUE;
		if (do_res_stat(A_WIS)) paid = TRUE;
		if (do_res_stat(A_DEX)) paid = TRUE;
		if (do_res_stat(A_CON)) paid = TRUE;
		if (do_res_stat(A_CHR)) paid = TRUE;
		break;
	case BACT_ENCHANT_ARROWS:
		item_tester_hook = object_is_ammo;
		enchant_item(bcost, 1, 1, 0);
		break;
	case BACT_ENCHANT_BOW:
		item_tester_hook = object_is_bow;
		enchant_item(bcost, 1, 1, 0);
		break;
	case BACT_RECALL:
		if (recall_player(1)) paid = TRUE;
		break;
	case BACT_TELEPORT_LEVEL:
	{
		int select_dungeon;
		int i, num = 0;
		s16b *dun;
		int max_depth;

		/* Allocate the "dun" array */
		C_MAKE(dun, max_d_idx, s16b);

		screen_save();
		clear_bldg(4, 20);

		for(i = 1; i < max_d_idx; i++)
		{
			char buf[80];
			bool seiha = FALSE;

			if (d_info[i].flags1 & DF1_CLOSED) continue;
			if (!d_info[i].maxdepth) continue;
			if (!max_dlv[i]) continue;
			if (d_info[i].final_guardian)
			{
				if (!r_info[d_info[i].final_guardian].max_num) seiha = TRUE;
			}
			else if (max_dlv[i] == d_info[i].maxdepth) seiha = TRUE;

#ifdef JP
			sprintf(buf,"%c) %c%-12s : 最大 %d 階", 'a'+num, seiha ? '!' : ' ', d_name + d_info[i].name, max_dlv[i]);
#else
			sprintf(buf,"%c) %c%-12s : Max level %d", 'a'+num, seiha ? '!' : ' ', d_name + d_info[i].name, max_dlv[i]);
#endif
			put_str(buf, 4+num, 5);
			dun[num] = i;
			num++;
		}
#ifdef JP
		prt("どのダンジョンにテレポートしますか:", 0, 0);
#else
		prt("Which dungeon do you teleport?: ", 0, 0);
#endif
		while(1)
		{
			i = inkey();

			if (i == ESCAPE)
			{
				/* Free the "dun" array */
				C_KILL(dun, max_d_idx, s16b);

				screen_load();
				return;
			}
			if (i >= 'a' && i <('a'+num))
			{
				select_dungeon = dun[i-'a'];
				break;
			}
			else bell();
		}
		screen_load();

		/* Free the "dun" array */
		C_KILL(dun, max_d_idx, s16b);

		max_depth = d_info[select_dungeon].maxdepth;

		/* Limit depth in Death Palace */
		if (select_dungeon == DUNGEON_PALACE)
		{
			if ((quest[QUEST_LANCELOT].status == QUEST_STATUS_TAKEN) ||
			    (quest[QUEST_DENIM].status == QUEST_STATUS_TAKEN))
				max_depth = 98;
			else if(quest[QUEST_DOLGARUA].status != QUEST_STATUS_FINISHED) max_depth = 99;
		}
		else if (select_dungeon == DUNGEON_HEAVEN_WAY)
		{
			max_depth = d_info[select_dungeon].maxdepth - 1;
		}

#ifdef JP
		amt = get_quantity(format("%sの何階にテレポートしますか？", d_name + d_info[select_dungeon].name), max_depth);
#else
		amt = get_quantity(format("Teleport to which level of %s? ", d_name + d_info[select_dungeon].name), max_depth);
#endif

		if (amt > 0)
		{
			p_ptr->word_recall = 1;
			p_ptr->recall_dungeon = select_dungeon;
			max_dlv[p_ptr->recall_dungeon] = ((amt > d_info[select_dungeon].maxdepth) ? d_info[select_dungeon].maxdepth : ((amt < d_info[select_dungeon].mindepth) ? d_info[select_dungeon].mindepth : amt));
			if (record_maxdeapth)
#ifdef JP
				do_cmd_write_nikki(NIKKI_TRUMP, select_dungeon, "転送術の塔で");
#else
				do_cmd_write_nikki(NIKKI_TRUMP, select_dungeon, "at Transportation Tower");
#endif
#ifdef JP
			msg_print("回りの大気が張りつめてきた...");
#else
			msg_print("The air about you becomes charged...");
#endif

			paid = TRUE;
			p_ptr->redraw |= (PR_STATUS);
		}
		break;
	}
	case BACT_LOSE_MUTATION:
		if (p_ptr->muta1 || p_ptr->muta2 || p_ptr->muta3)
		{
			while(!lose_mutation(0));
			paid = TRUE;
		}
		else
		{
#ifdef JP
			msg_print("治すべき突然変異が無い。");
#else
			msg_print("You have no mutations.");
#endif
			msg_print(NULL);
		}
		break;
	case BACT_TSUCHINOKO:
		tsuchinoko();
		break;
	case BACT_KUBI:
		shoukinkubi();
		break;
	case BACT_TARGET:
		today_target();
		break;
	case BACT_KANKIN:
		kankin();
		break;
	case BACT_EQUALIZATION:
#ifdef JP
		msg_print("平衡化の儀式を行なった。");
#else
		msg_print("You received an equalization ritual.");
#endif
		p_ptr->align_self[ALI_LNC] = 0;
		p_ptr->update |= (PU_BONUS);
		paid = TRUE;
		break;
	case BACT_TELE_TOWN:
		paid = tele_town(FALSE);
		break;
	case BACT_INC_STR:
	case BACT_INC_INT:
	case BACT_INC_WIS:
	case BACT_INC_DEX:
	case BACT_INC_CON:
	case BACT_INC_CHR:
		if (randint1(100) < 11)
		{
			if (dec_stat((bact - BACT_INC_STR), randint1(10), one_in_(10)))
			{
#ifdef JP
				msg_print("逆に能力値が下がってしまった！");
#else
				msg_print("nanka!");
#endif
			}
		}
		else if (randint1(100) < 31)
		{
#ifdef JP
			msg_print("能力を高めるのに失敗した！");
#else
			msg_print("nanka!");
#endif
		}
		else
		{
			do_inc_stat(bact - BACT_INC_STR);
		}
		paid = TRUE;
		break;
	case BACT_DONATION:
	case BACT_DONATION_LODIS:
	case BACT_DONATION_ZENOBIAN:
		{
			bool reject = FALSE;

			if (bact == BACT_DONATION_LODIS)
			{
				if (chaos_frame[ETHNICITY_LODIS] <= CFRAME_LOWER_LIMIT) reject = TRUE;
			}
			else if (bact == BACT_DONATION_ZENOBIAN)
			{
				if (chaos_frame[ETHNICITY_ZENOBIAN] <= CFRAME_LOWER_LIMIT) reject = TRUE;
			}
			else if (!dun_level && p_ptr->town_num)
			{
				if (chaos_frame[(int)town[p_ptr->town_num].ethnic] <= CFRAME_LOWER_LIMIT) reject = TRUE;
			}

			if (reject)
			{
#ifdef JP
				msg_print("寄付の申し出は冷たくあしらわれた。");
#else
				msg_print("Donation is rejected cruelly.");
#endif
				return;
			}
		}

		bcost = (400 - ((int)adj_chr_give[p_ptr->stat_ind[A_CHR]] - 128) * 8) * p_ptr->lev * 2;
#ifdef JP
		msg_format("1回の寄付には$%d必要です。", bcost);
#else
		msg_format("Donation needs %d gold.", bcost);
#endif
		if (p_ptr->au_sum < bcost)
		{
#ifdef JP
			msg_print("お金が足りません。");
#else
			msg_print("You do not have enough gold.");
#endif
			return;
		}

#ifdef JP
		if (!get_check("寄付しますか？")) return;
#else
		if (!get_check("Do you donate? ")) return;
#endif
#ifdef JP
		msg_format("$%dを寄付しました。", bcost);
#else
		msg_format("You donated %d gold.", bcost);
#endif
		if (bact == BACT_DONATION_LODIS) change_chaos_frame(ETHNICITY_LODIS, 10);
		else if (bact == BACT_DONATION_ZENOBIAN) change_chaos_frame(ETHNICITY_ZENOBIAN, 10);
		else if (!dun_level && p_ptr->town_num) change_chaos_frame(town[p_ptr->town_num].ethnic, 10);
		change_your_alignment(ALI_LNC, 10);
		paid = TRUE;
		break;
	case BACT_EVAL_AC:
		paid = eval_ac(p_ptr->dis_ac + p_ptr->dis_to_a);
		break;
	case BACT_JOIN_LODIS_KNIGHTS:
	case BACT_JOIN_ZENOBIAN_KNIGHTS:
	case BACT_CHANGE_SUCCUBUS:
		change_class(bact);
		break;
	case BACT_COMPOSITE_ITEM:
		composite_item();
		break;
	}

	if (paid)
	{
		p_ptr->au_sum -= bcost;
		p_ptr->update |= (PU_GOLD);
		update_stuff();
	}
}


/*
 * Enter quest level
 */
void do_cmd_quest(void)
{
	energy_use = 100;

	if (cave[py][px].feat != FEAT_QUEST_ENTER)
	{
#ifdef JP
		msg_print("ここにはクエストの入口はない。");
#else
		msg_print("You see no quest level here.");
#endif

		return;
	}
	else
	{
#ifdef JP
		msg_print("ここにはクエストへの入口があります。");
		if (!get_check("クエストに入りますか？")) return;
#else
		msg_print("There is an entry of a quest.");
		if (!get_check("Do you enter? ")) return;
#endif

		/* Player enters a new quest */
		p_ptr->oldpy = 0;
		p_ptr->oldpx = 0;

		leave_quest_check();

		if (quest[p_ptr->inside_quest].type != QUEST_TYPE_RANDOM) dun_level = 1;
		p_ptr->inside_quest = cave[py][px].special;

		p_ptr->leaving = TRUE;
	}
}


/*
 * Do building commands
 */
void do_cmd_bldg(void)
{
	int             i, which;
	char            command;
	bool            validcmd;
	building_type   *bldg;
	bool            is_casino = FALSE;
	int             old_au = p_ptr->au_sum;


	energy_use = 100;

	if (!((cave[py][px].feat >= FEAT_BLDG_HEAD) &&
		  (cave[py][px].feat <= FEAT_BLDG_TAIL)))
	{
#ifdef JP
		msg_print("ここには建物はない。");
#else
		msg_print("You see no building here.");
#endif

		return;
	}

	which = (cave[py][px].feat - FEAT_BLDG_HEAD);

	if (dun_level)
	{
		process_dungeon_file("t0000000.txt", 0, 0, MAX_HGT, MAX_WID);
		bldg = &building[which];
	}
	else bldg = &building[which];

	/* Don't re-init the wilderness */
	reinit_wilderness = FALSE;

	/* Restriction by chaos frame */
	if (!dun_level && p_ptr->town_num && !p_ptr->inside_arena)
	{
		int ethnic = town[p_ptr->town_num].ethnic;
		if (ethnic != NO_ETHNICITY)
		{
			/* Special Restriction for Barmamutha Genocider! */
			if (p_ptr->town_num == TOWN_BARMAMUTHA)
			{
				if (misc_event_flags & EVENT_CLOSE_BARMAMUTHA)
				{
#ifdef JP
					msg_print("ドアに鍵がかかっている。");
#else
					msg_print("The doors are locked.");
#endif
					return;
				}
			}

			if (p_ptr->town_num == TOWN_ARMORICA)
			{
				if (!(misc_event_flags & EVENT_LIBERATION_OF_ARMORICA))
				{
#ifdef JP
					msg_print("ドアに鍵がかかっている。");
#else
					msg_print("The doors are locked.");
#endif
					return;
				}
			}

			if ((bldg->chaos_frame_restr && (chaos_frame[ethnic] <= CFRAME_CLOSE_BLDG)) ||
				(chaos_frame[ethnic] <= CFRAME_LOWER_LIMIT))
			{
#ifdef JP
				msg_print("ドアに鍵がかかっている。");
#else
				msg_print("The doors are locked.");
#endif
				return;
			}

			if ((p_ptr->town_num == TOWN_CORTANI) && (which == 7) && (misc_event_flags & EVENT_CANNOT_BE_TEMPLEKNIGHT))
			{
#ifdef JP
				msg_print("ドアに鍵がかかっている。");
#else
				msg_print("The doors are locked.");
#endif
				return;
			}

			if ((p_ptr->town_num == TOWN_RIME_CITY) && (which == 12) && (misc_event_flags & EVENT_CANNOT_BE_WHITEKNIGHT))
			{
#ifdef JP
				msg_print("ドアに鍵がかかっている。");
#else
				msg_print("The doors are locked.");
#endif
				return;
			}

			if ((p_ptr->town_num != p_ptr->smithy_town_num) && (which == 31))
			{
#ifdef JP
				msg_print("ドアに鍵がかかっている。");
#else
				msg_print("The doors are locked.");
#endif
				return;
			}

		}
	}

	if ((which == 2) && (p_ptr->arena_number < 0))
	{
#ifdef JP
		msg_print("「敗者に用はない。」");
#else
		msg_print("'There's no place here for a LOSER like you!'");
#endif
		return;
	}
	else if ((which == 2) && p_ptr->inside_arena)
	{
		if (!p_ptr->exit_bldg)
		{
#ifdef JP
			prt("ゲートは閉まっている。モンスターがあなたを待っている！",0,0);
#else
			prt("The gates are closed.  The monster awaits!", 0, 0);
#endif
		}
		else
		{
			/* Don't save the arena as saved floor */
			prepare_change_floor_mode(CFM_SAVE_FLOORS | CFM_NO_RETURN);

			p_ptr->inside_arena = FALSE;
			p_ptr->leaving = TRUE;

			/* Re-enter the arena */
			command_new = SPECIAL_KEY_BUILDING;

			/* No energy needed to re-enter the arena */
			energy_use = 0;
		}

		return;
	}
	else
	{
		p_ptr->oldpy = py;
		p_ptr->oldpx = px;
	}

	/* Forget the lite */
	forget_lite();

	/* Forget the view */
	forget_view();

	/* Hack -- Increase "icky" depth */
	character_icky++;

	command_arg = 0;
	command_rep = 0;
	command_new = 0;

	show_building(bldg);
	leave_bldg = FALSE;

	gamble_play_times = 0;

	for (i = 0; i < 8; i++)
	{
		switch (bldg->actions[i])
		{
		case BACT_IN_BETWEEN:
		case BACT_CRAPS:
		case BACT_SPIN_WHEEL:
		case BACT_DICE_SLOTS:
		case BACT_POKER:
			is_casino = TRUE;
			break;
		}
		if (is_casino) break;
	}

	while (!leave_bldg)
	{
		validcmd = FALSE;
		prt("", 1, 0);

		building_prt_gold();

		command = inkey();

		if (command == ESCAPE)
		{
			leave_bldg = TRUE;
			p_ptr->inside_arena = FALSE;
			break;
		}

		for (i = 0; i < 8; i++)
		{
			if (bldg->letters[i])
			{
				if (bldg->letters[i] == command)
				{
					validcmd = TRUE;
					break;
				}
			}
		}

		if (validcmd)
			bldg_process_command(bldg, i);

		/* Notice stuff */
		notice_stuff();

		/* Handle stuff */
		handle_stuff();
	}

	/* Flush messages XXX XXX XXX */
	msg_flag = FALSE;
	msg_print(NULL);

	/* Reinit wilderness to activate quests ... */
	if (reinit_wilderness)
	{
		p_ptr->leaving = TRUE;
	}

	/* Hack -- Decrease "icky" depth */
	character_icky--;

	/* Clear the screen */
	Term_clear();

	/* Update the visuals */
	p_ptr->update |= (PU_VIEW | PU_MONSTERS | PU_BONUS | PU_LITE | PU_MON_LITE);

	/* Redraw entire screen */
	p_ptr->redraw |= (PR_BASIC | PR_EXTRA | PR_EQUIPPY | PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

	if (is_casino)
	{
		if (gamble_play_times >= 10)
		{
			if ((p_ptr->au_sum - old_au) >= 100)
			{
				if (!dun_level && p_ptr->town_num)
				{
					change_chaos_frame(town[p_ptr->town_num].ethnic, -1);
					change_your_alignment(ALI_LNC, -1);
				}
			}
			else if ((old_au - p_ptr->au_sum) >= 100)
			{
				if (!dun_level && p_ptr->town_num)
				{
					change_chaos_frame(town[p_ptr->town_num].ethnic, 1);
					change_your_alignment(ALI_LNC, 1);
				}
			}
		}
	}
	gamble_play_times = 0;
}


/* Array of places to find an inscription */
static cptr find_quest[] =
{
#ifdef JP
	"床にメッセージが刻まれている:",
#else
	"You find the following inscription in the floor",
#endif

#ifdef JP
	"壁にメッセージが刻まれている:",
#else
	"You see a message inscribed in the wall",
#endif

#ifdef JP
	"メッセージを見つけた:",
#else
	"There is a sign saying",
#endif

#ifdef JP
	"何かが階段の上に書いてある:",
#else
	"Something is written on the staircase",
#endif

#ifdef JP
	"巻物を見つけた。メッセージが書いてある:",
#else
	"You find a scroll with the following message",
#endif

};


/*
 * Discover quest
 */
void quest_discovery(int q_idx)
{
	quest_type      *q_ptr = &quest[q_idx];
	monster_race    *r_ptr = &r_info[q_ptr->r_idx];
	int             q_num = q_ptr->max_num;
	char            name[80];

	/* No quest index */
	if (!q_idx) return;

	strcpy(name, (r_name + r_ptr->name));

	sound(SOUND_ENCOUNT);

	msg_print(find_quest[rand_range(0, 4)]);
	msg_print(NULL);

	if (q_num == 1)
	{
		/* Unique */

		/* Hack -- "unique" monsters must be "unique" */
		if ((r_ptr->flags1 & RF1_UNIQUE) &&
		    (0 == r_ptr->max_num))
		{
#ifdef JP
			msg_print("この階は以前は誰かによって守られていたようだ…。");
#else
			msg_print("It seems that this level was protected by someone before...");
#endif
			/* The unique is already dead */
			quest[q_idx].status = QUEST_STATUS_FINISHED;
		}
		else
		{
#ifdef JP
			msg_format("注意せよ！この階は%sによって守られている！", name);
#else
			msg_format("Beware, this level is protected by %s!", name);
#endif
		}
	}
	else
	{
		/* Normal monsters */
#ifdef JP
		msg_format("注意しろ！この階は%d体の%sによって守られている！", q_num, name);
#else
		plural_aux(name);
		msg_format("Be warned, this level is guarded by %d %s!", q_num, name);
#endif

	}
}


/*
 * Hack -- Check if a level is a "quest" level
 */
int quest_number(int level)
{
	int i;

	/* Check quests */
	if (p_ptr->inside_quest)
		return (p_ptr->inside_quest);

	for (i = 0; i < max_quests; i++)
	{
		if (quest[i].status != QUEST_STATUS_TAKEN) continue;

		if ((quest[i].type == QUEST_TYPE_KILL_LEVEL) &&
			!(quest[i].flags & QUEST_FLAG_PRESET) &&
		    (quest[i].level == level) &&
		    (quest[i].dungeon == dungeon_type))
			return (i);
	}

	/* Check for random quest */
	return (random_quest_number(level));
}


/*
 * Return the index of the random quest on this level
 * (or zero)
 */
int random_quest_number(int level)
{
	int i;
	int min_random_quest = astral_mode ? MIN_RANDOM_QUEST_ASTRAL: MIN_RANDOM_QUEST;
	int max_random_quest = astral_mode ? MAX_RANDOM_QUEST_ASTRAL: MAX_RANDOM_QUEST;

	if (dungeon_type != DUNGEON_PALACE) return 0;

	for (i = min_random_quest; i <= max_random_quest; i++)
	{
		if ((quest[i].type == QUEST_TYPE_RANDOM) &&
		    (quest[i].status == QUEST_STATUS_TAKEN) &&
		    (quest[i].level == level) &&
		    (quest[i].dungeon == DUNGEON_PALACE))
		{
			return i;
		}
	}

	/* Nope */
	return 0;
}
