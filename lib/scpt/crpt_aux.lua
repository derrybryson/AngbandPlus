-- Core functions for corruptions

__corruptions = {}
__corruptions_max = 0
__corruptions_callbacks_max = 0

-- Get the corruption
function player.corruption(c, set)
        if set then
                player.corruptions_aux[c + 1] = set
                player.redraw = bor(player.redraw, PR_BASIC)
                player.update = bor(player.update, PU_BONUS, PU_TORCH, PU_BODY, PU_POWERS)
                if (set == TRUE) and (__corruptions[c].gain) then
                        __corruptions[c].gain()
                end
                if (set == FALSE) and (__corruptions[c].lose) then
                        __corruptions[c].lose()
                end
        else
                return player.corruptions_aux[c + 1]
        end
end

-- Test if we have that corruption
-- We must:
-- 1) have it or be willing to get it
-- 2) have all its dependancies
-- 3) have none of its opposing corruptions
function test_depend_corrupt(corrupt, can_gain)
        local i, c

        if not can_gain then can_gain = FALSE end

        if can_gain == TRUE then
                if (player.corruption(corrupt) ~= FALSE) then
                        return FALSE
                end
        else
                if (player.corruption(corrupt) ~= TRUE) then
                        return FALSE
                end
        end

        for c, i in __corruptions[corrupt].depends do
                if test_depend_corrupt(c) ~= TRUE then
                        return FALSE
                end
        end

        for c, i in __corruptions[corrupt].oppose do
                if test_depend_corrupt(c) ~= FALSE then
                        return FALSE
                end
        end
        return TRUE
end

-- Gain a new corruption
function gain_corruption()
        local i, max
        local pos = {}

        -- Get the list of all possible ones
        max = 0
        for i = 0, __corruptions_max - 1 do
                if test_depend_corrupt(i, TRUE) == TRUE then
                        pos[max] = i
                        max = max + 1
                       end
        end

        -- Ok now get one of them
        if (max > 0) then
                local ret = rand_int(max)

                player.corruption(pos[ret], TRUE)
                cmsg_print(TERM_L_RED, __corruptions[pos[ret]].get_text)

                return pos[ret]
        else
                return -1
        end
end

-- Lose an existing corruption
function lose_corruption()
        local i, max
        local pos = {}

        -- Get the list of all possible ones
        max = 0
        for i = 0, __corruptions_max - 1 do
                if test_depend_corrupt(i) == TRUE and __corruptions[i].removable == TRUE then
                        pos[max] = i
                        max = max + 1
                end
        end

        -- Ok now get one of them
        if (max > 0) then
                local ret = rand_int(max)

                player.corruption(pos[ret], FALSE)
                cmsg_print(TERM_L_RED, __corruptions[pos[ret]].lose_text)

                -- Ok now lets see if it broke some dependancies
                for i = 0, max - 1 do
                        if player.corruption(pos[i]) ~= test_depend_corrupt(pos[i]) then
                                player.corruption(pos[i], FALSE)
                                cmsg_print(TERM_L_RED, __corruptions[pos[i]].lose_text)
                        end
                end

                return pos[ret]
        else
                return -1
        end
end

-- Lose all corruptions (for e.g. Potion of New Life)
function lose_all_corruptions()
        local i;
        for i = 0, __corruptions_max - 1 do
                lose_corruption()
        end
        return -1
end

-- Creates a new corruption
function add_corruption(c)
        assert(c.color, "No corruption color")
        assert(c.name, "No corruption name")
        assert(c.get_text, "No corruption get_text")
        assert(c.lose_text, "No corruption lose_text")
        assert(c.desc, "No corruption desc")
        assert(c.hooks, "Nothing to do for corruption")
        if not c.removable then c.removable = TRUE end

        if c.depends == nil then c.depends = {} end
        if c.oppose == nil then c.oppose = {} end

        -- We must make sure the other ones opposes too
        local o, i
        for o, i in c.oppose do
                __corruptions[o].oppose[__corruptions_max] = TRUE
        end

        local index, h
        for index, h in c.hooks do
                add_hook_script(index, "__lua__corrupt_callback"..__corruptions_callbacks_max, "__lua__corrupt_callback"..__corruptions_callbacks_max)
                setglobal("__lua__corrupt_callback"..__corruptions_callbacks_max,
                        function (...)
                                if test_depend_corrupt(%__corruptions_max) == TRUE then
                                        return call(%h, arg)
                                end
                        end
                )
                __corruptions_callbacks_max = __corruptions_callbacks_max + 1
        end

        if type(c.desc) == "table" then
                local new_desc = ""
                for index, h in c.desc do
                        new_desc = new_desc..h.."\n"
                end
                c.desc = new_desc
        end

        __corruptions[__corruptions_max] = c
        __corruptions_max = __corruptions_max + 1
        return (__corruptions_max - 1)
end



---------- Corruption spoiler generator -----------
function corruption_spoiler_generate()
	make_temp_file()
        print_hook(
[[#####R=== ToME Corruptions Spoiler ===

Sometimes adventurers become exposed to the dark powers of Morgoth. If they 
are unable to resist these powers, they become corrupted. Corruptions can 
change their physical or mental abilities, some of which can be good, and
some bad. Most corruptions will affect you permanently, although some only 
operate when they are activated (whether by player choice or as a random 
event). You can check which corruptions do you have in the knowledge screen 
6 (accessed through the '~' menu) or in a character dump.

#####GGaining and losing corruptions
There are many ways how to gain or lose corruptions.

You can select the *****rm_corru.txt*0[corrupted race] on birth. Such foul
adventurers will gain and lose corruptions as they gain experience and
levels. There is a 1 in 3 chance to lose a corruption instead
of gaining it on these occasions.

You can become corrupted by quaffing a Potion of Corruption or by drinking
from Fountain of Corruption. Also some strange items can be activated
for corruption.

There is 1 in 3 chance you will get corrupted when hit by a chaos attack 
while lacking a source of chaos resistance.

You can also gain and lose corruptions when polymorphing yourself.

]])
        local i, e
        for i = 0, __corruptions_max - 1 do
                print_hook("[[[[[B"..__corruptions[i].name.."]\n")
                print_hook(__corruptions[i].desc)
                print_hook("[[[[[GGain message: "..__corruptions[i].get_text.."]\n")
                print_hook("[[[[[RLose message: "..__corruptions[i].lose_text.."]\n")

                local ok
                ok = nil
	        for e, _ in __corruptions[i].depends do ok = not nil end
                if ok then
	                print_hook("It depends on:\n")
	        	for e, _ in __corruptions[i].depends do
        	                print_hook("  "..__corruptions[e].name.."\n")
                	end
                end
                ok = nil
	        for e, _ in __corruptions[i].oppose do ok = not nil end
                if ok then
                        print_hook("It is opposed to:\n")
	                for e, _ in __corruptions[i].oppose do
        	                print_hook("  "..__corruptions[e].name.."\n")
                	end
                end
                print_hook("\n\n")
        end
        close_temp_file()
        msg_print("File created as: "..get_temp_name())
end
