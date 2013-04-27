/* File: xtra4.c */

/*
 * Copyright (c) 2007 Kenneth Boyd.  This file is subject to the Boost license.

	http://www.boost.org/more/license_info.html
	Boost Software License - Version 1.0 - August 17th, 2003

	Permission is hereby granted, free of charge, to any person or organization
	obtaining a copy of the software and accompanying documentation covered by
	this license (the "Software") to use, reproduce, display, distribute,
	execute, and transmit the Software, and to prepare derivative works of the
	Software, and to permit third-parties to whom the Software is furnished to
	do so, all subject to the following:

	The copyright notices in the Software and this entire statement, including
	the above license grant, this restriction and the following disclaimer,
	must be included in all copies of the Software, in whole or in part, and
	all derivative works of the Software, unless such copies or derivative
	works are solely in the form of machine-executable object code generated by
	a source language processor.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
	SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
	FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
	ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
	DEALINGS IN THE SOFTWARE.
 */

#include "angband.h"

int
agent_type::ticks_to_move(int move, int diag) const
{
	return (50*(diag + 1) + 100*move - (int)energy)/(int)(extract_energy[speed]);
}

int
agent_type::moves_in_ticks(int ticks, int diag) const
{
	return ((int)energy + ticks*(int)(extract_energy[speed]) - 50*(diag + 1))/100;
}

int
agent_type::energy_in_ticks(int ticks) const
{
	return (int)energy + ticks*(int)(extract_energy[speed]);
}

void
agent_type::move_ratio(int& threat_moves, int& my_moves, const agent_type& threat, int threat_diag, int my_diag) const
{
	int ticks_to_my_move = ticks_to_move(2,my_diag);
	int ticks_to_threat_move = threat.ticks_to_move(1,threat_diag);

	my_moves = 1;
	threat_moves = 1;

	if		(ticks_to_my_move>ticks_to_threat_move)
	{	/* threat is faster, check for double-move */
		threat_moves = threat.moves_in_ticks(ticks_to_my_move,threat_diag);
		/* but I win energy ties (actually true only for player) */
		if (	(1 < threat_moves)
			&&	(threat.ticks_to_move(threat_moves,threat_diag) == ticks_to_my_move)
			&&	(energy_in_ticks(ticks_to_my_move)>=threat.energy_in_ticks(ticks_to_my_move)))
			--threat_moves;
	}
	else if	(ticks_to_my_move<ticks_to_threat_move)
	{	/* I am faster, check for double-move */
		my_moves = moves_in_ticks(ticks_to_threat_move,my_diag);
	}
	else if (energy_in_ticks(ticks_to_my_move)>=threat.energy_in_ticks(ticks_to_threat_move))
	{	/* I win energy ties (actually only true for player) */
		my_moves = 2;
	}
}

/**
 * \return nonstrict upper bound on health that player could calculate from monster health bar.
 */
int
agent_type::apparent_health() const
{
	if (chp>=mhp || smart_cheat) return chp;

	{
	int stars_sub_1 = (10L * (long) chp / (long)mhp);

	if (9 <= stars_sub_1) return mhp-1;

	return ((stars_sub_1+1)*(long)mhp)/10L;
	}	
}


/**
 * Test to see if the character acts on a moronic command.
 * \return true if the character does act on a moronic command.
 */
bool player_type::allow_moron() const	/* Strictly Boost-licensed */
{
	s16b INT = stat_use[A_INT];
	s16b WIS = stat_use[A_WIS];
	s16b STAT_PRODUCT = INT*WIS;
	s16b STAT_MAX = (INT<WIS) ? WIS : INT;

	/* animal intellect or wisdom...always */
	if (5>INT || 5>WIS) return TRUE;
	/* clinically moron or fool: linear check against better stat */
	else if (9>INT || 9>WIS) return 4>rand_int(STAT_MAX);

	/* reasonable INT and WIS: check against product */
	return 16>rand_int(STAT_PRODUCT);
}

