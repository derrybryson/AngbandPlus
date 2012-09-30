-- The god quest: find randomly placed relic in a randomly placed dungeon!

-- set some global variables (stored in the save file via the ["data"] key)
god_quest = {}

-- increase this number to make god quests more common, to a max value of 100 
god_quest.CHANCE_OF_GOD_QUEST = 20

-- increase this number to make more quests
god_quest.MAX_NUM_GOD_QUESTS = 5

-- d_idx of the god_quest (Lost Temple) dungeon
god_quest.DUNGEON_GOD = 30

add_quest
{
        ["global"] =    "GOD_QUEST",
        ["name"] =      "God quest",
        ["desc"] =      function()
                        local home, home_axis

                        if quest(GOD_QUEST).status == QUEST_STATUS_TAKEN then
                                
                                -- get the direction that the dungeon lies from lothlorien/angband 
                                player_axis, home, home_axis = get_god_quest_axes()
                        
                                print_hook("#####yGod quest!\n")
                                print_hook("Thou art to find the lost temple of thy God and\n");
                                print_hook("to retrieve the lost part of the relic for thy God! \n")
                                print_hook("The temple lies to the "..home_axis.." of "..home.." \n")
                                print_hook("and to the "..player_axis.." of thy position when thou were given the quest. \n")
                                print_hook("\n")
                        end
        end,
        ["level"] =     -1,
        ["data"] =      {
                ["god_quest.relic_num"] = 1,
                ["god_quest.quests_given"] = 0,
                ["god_quest.relics_identified"] = 0,
                ["god_quest.dun_mindepth"] = 1,
                ["god_quest.dun_maxdepth"] = 6,
                ["god_quest.dun_minplev"] = 1,
                ["god_quest.relic_gen_tries"] = 0,
                ["god_quest.relic_generated"] = FALSE,
                ["god_quest.dung_x"] = 1,
                ["god_quest.dung_y"] = 1,
                ["god_quest.player_x"] = 0,
                ["god_quest.player_y"] = 0,
        },
        ["hooks"] =     {
                        -- Start the game without the quest, given it by chance
                        [HOOK_BIRTH_OBJECTS] = function()
                                quest(GOD_QUEST).status = QUEST_STATUS_UNTAKEN

                                -- initialise save-file stored variables when new character is created
                                god_quest.relic_num = 1
                                god_quest.quests_given = 0
                                god_quest.relics_identified = 0
                                god_quest.dun_mindepth = 1
                                god_quest.dun_maxdepth = 6
                                god_quest.dun_minplev = 1
                                god_quest.relic_gen_tries = 0
                                god_quest.relic_generated = FALSE
                        end,
                        [HOOK_PLAYER_LEVEL] = function(gained)
                                local home_axis, home
                                
                                if gained > 0 then
                                        -- roll for chance of quest
                                        local give_god_quest = magik(god_quest.CHANCE_OF_GOD_QUEST)
                                
                                        -- check player is worshipping a god, not already on a god quest.
                                        if (player.pgod <= 0) or (quest(GOD_QUEST).status == QUEST_STATUS_TAKEN)
                                        or (god_quest.quests_given >= god_quest.MAX_NUM_GOD_QUESTS) or (give_god_quest == FALSE)
                                        or ((current_dungeon_idx == god_quest.DUNGEON_GOD) and (dun_level > 0)) then
                                                return
                                        else
                                                -- each god has different characteristics, so the quests are differnet depending on your god
                                                if player.pgod == GOD_ERU then
                                                        god_quest.relic_num = 7
                                                elseif player.pgod == GOD_MANWE then
                                                        god_quest.relic_num = 8
                                                elseif player.pgod == GOD_TULKAS then
                                                        god_quest.relic_num = 9
                                                elseif player.pgod == GOD_MELKOR then
                                                        god_quest.relic_num = 10
                                                elseif player.pgod == GOD_YAVANNA then
                                                        god_quest.relic_num =11
                                                end

                                                -- This var will need resetting
                                                god_quest.relic_generated = FALSE
                                                quest(GOD_QUEST).status = QUEST_STATUS_TAKEN
                                                god_quest.quests_given = god_quest.quests_given + 1

                                                -- actually place the dungeon in a random place
                                                place_rand_dung()
                                                
                                                -- store the variables of the coords where the player was given the quest
                                                god_quest.player_y, god_quest.player_x = player.get_wild_coord()
                                                
                                                -- establish direction of player and 'home' from dungeon
                                                player_axis, home, home_axis = get_god_quest_axes()

                                                -- God issues instructions
                                                cmsg_print(TERM_L_BLUE, "The voice of "..deity(player.pgod).name.." booms in your head:")

                                                cmsg_print(TERM_YELLOW, "'I have a task for thee.")
                                                cmsg_print(TERM_YELLOW, "Centuries ago an ancient relic of mine was broken apart.")
                                                cmsg_print(TERM_YELLOW, "The pieces of it hath been lost in fallen temples.")
                                                cmsg_print(TERM_YELLOW, "Thou art to find my lost temple and retrieve a piece of the relic.")
                                                cmsg_print(TERM_YELLOW, "When thy task is done, thou art to lift it in the air and call upon my name.")
                                                cmsg_print(TERM_YELLOW, "I shall then come to reclaim what is mine!")
                                                cmsg_print(TERM_YELLOW, "The temple lies to the "..player_axis.." of thy current position, and to the ")
                                                cmsg_print(TERM_YELLOW, home_axis.." of "..home..", I can feel it.'")
                                                
                                                -- Prepare depth of dungeon. If this was generated in set_god_dungeon_attributes(),
                                                -- then we'd have trouble if someone levelled up in the dungeon!
                                                god_quest.dun_mindepth = player.lev*2/3
                                                god_quest.dun_maxdepth = player.lev*2/3 + 5
                                                god_quest.dun_minplev = player.lev
                                        end
                                end
                        end,
                        [HOOK_LEVEL_END_GEN] = function()
                                local chance

                                -- Check for dungeon
                                if (current_dungeon_idx ~= god_quest.DUNGEON_GOD) or (god_quest.relic_generated == TRUE) 
                                or (quest(GOD_QUEST).status == QUEST_STATUS_UNTAKEN) then
                                        return
                                else
                                        -- Force relic generation on 5th attempt if others have been unsuccessful.
                                        if (god_quest.relic_gen_tries == 4) and (god_quest.relic_generated == FALSE) then
                                                generate_relic()
                                        else
                                                -- 1/5 chance of generation
                                                chance = randint(5)
                                                if (chance == 5) then
                                                        generate_relic()
                                                else
                                                        god_quest.relic_gen_tries = god_quest.relic_gen_tries + 1
                                                end
                                        end
                                end
                        end,
                        [HOOK_ENTER_DUNGEON] = function(d_idx)
                                -- call the function to set the dungeon variables (dependant on pgod) the first time we enter the dungeon
                                if d_idx ~= god_quest.DUNGEON_GOD then 
                                        return
                                else
                                        set_god_dungeon_attributes()
                                end
                        end,
                        [HOOK_GEN_LEVEL_BEGIN] = function()
                                -- call the function to set the dungeon variables (dependant on pgod) when we WoR back into the dungeon
                                if current_dungeon_idx ~= god_quest.DUNGEON_GOD then 
                                        return 
                                else
                                        set_god_dungeon_attributes()
                                end
                        end,
                        [HOOK_STAIR] = function(down)
                                -- call the function to set the dungeon variables (dependant on pgod) every time we go down a level
                                if current_dungeon_idx ~= god_quest.DUNGEON_GOD then 
                                        return 
                                else
                                        set_god_dungeon_attributes()
                                end
                        end,
                        [HOOK_GET] = function(o_ptr, item)
                                        -- Is it the relic, and check to make sure the relic hasn't already been identified
                                if (o_ptr.tval == TV_JUNK) and (o_ptr.sval == god_quest.relic_num) and (o_ptr.pval ~= TRUE) 
                                and (god_quest.relics_identified < god_quest.quests_given) and (quest(GOD_QUEST).status == QUEST_STATUS_TAKEN) then

                                        -- more God talky-talky
                                        cmsg_print(TERM_L_BLUE, deity(player.pgod).name.." speaks to you:")

                                        -- Is it the last piece of the relic?
                                        if (god_quest.quests_given == god_quest.MAX_NUM_GOD_QUESTS) then
                                                cmsg_print(TERM_YELLOW, "'At last! Thou hast found all of the relic pieces.")

                                                -- reward player by increasing prayer skill
                                                cmsg_print(TERM_YELLOW, "Thou hast done exceptionally well! I shall increase thy prayer skill even more!'")
                                                skill(SKILL_PRAY).value = skill(SKILL_PRAY).value + (10 * (skill(SKILL_PRAY).mod))

                                                -- Take the relic piece
                                                floor_item_increase(item, -1)
                                                floor_item_optimize(item)
                                        else
                                                cmsg_print(TERM_YELLOW, "'Well done! Thou hast found part of the relic.")
                                                cmsg_print(TERM_YELLOW, "I shall surely ask thee to find more of it later!")
                                                cmsg_print(TERM_YELLOW, "I will take it from thee for now'")

                                                -- Take the relic piece
                                                floor_item_increase(item, -1)
                                                floor_item_optimize(item)

                                                -- reward player by increasing prayer skill
                                                cmsg_print(TERM_YELLOW, "'As a reward, I shall teach thee how to pray better'")
                                                skill(SKILL_PRAY).value = skill(SKILL_PRAY).value + (5 * (skill(SKILL_PRAY).mod))
                                        end

                                        -- relic piece has been identified
                                        o_ptr.pval = TRUE
                                        god_quest.relics_identified = god_quest.relics_identified + 1

                                        -- Make sure quests can be given again if neccesary
                                        quest(GOD_QUEST).status = QUEST_STATUS_UNTAKEN
                                        return TRUE
                                end
                        end,
        },
}

-- this function places the lost temple at a randomly determined place.
function place_rand_dung()
        local tries, grid

        -- erase old dungeon
        if (god_quest.quests_given > 0) then place_dungeon(god_quest.dung_y, god_quest.dung_x) end

        -- initialise tries variable
        tries = 0
                                                
        while (tries == 0) do

                -- get grid coordinates, within a range which prevents dungeon being generated at the very edge of the wilderness (would crash the game).
                god_quest.dung_x = rand_range(1, max_wild_x-2)
                god_quest.dung_y = rand_range(1, max_wild_y-2)
                
                -- Is there a town/dungeon/potentially impassable feature there, ?
                if (wild_map(god_quest.dung_y, god_quest.dung_x).entrance ~= 0) 
                or (wild_feat(wild_map(god_quest.dung_y, god_quest.dung_x)).entrance ~= 0)
                or (wild_feat(wild_map(god_quest.dung_y, god_quest.dung_x)).feature_idx == TERRAIN_EDGE)
                or (wild_feat(wild_map(god_quest.dung_y, god_quest.dung_x)).feature_idx == TERRAIN_DEEP_WATER)
                or (wild_feat(wild_map(god_quest.dung_y, god_quest.dung_x)).feature_idx == TERRAIN_TREES) 
                or (wild_feat(wild_map(god_quest.dung_y, god_quest.dung_x)).feature_idx == TERRAIN_SHALLOW_LAVA) 
                or (wild_feat(wild_map(god_quest.dung_y, god_quest.dung_x)).feature_idx == TERRAIN_DEEP_LAVA) 
                or (wild_feat(wild_map(god_quest.dung_y, god_quest.dung_x)).feature_idx == TERRAIN_MOUNTAIN) then

                        -- try again
                        tries = 0
                else
                        --neither player, nor wall, then stop this 'while'
                        tries = 1

                end
        end

        -- create god dungeon in that place
        place_dungeon(god_quest.dung_y, god_quest.dung_x, god_quest.DUNGEON_GOD)

end

-- this function generates the relic at a randomly determined place in the temple.
function generate_relic()
        local tries, grid, relic

        -- initialise tries variable
        tries = 0
                                                
        while (tries == 0) do

                -- get grid coordinates from current height/width, minus one to prevent relic being generated in outside wall. (would crash the game)
                y = randint(cur_hgt-1)
                x = randint(cur_wid-1)
                grid = cave(y, x)

                -- are the coordinates in a wall, ?
                if (cave_is(grid, FF1_FLOOR) == 0) then

                        -- try again
                        tries = 0

                else
                        -- neither player, nor wall, then stop this 'while'
                        tries = 1

                end
        end

        -- create relic
        relic = create_object(TV_JUNK, god_quest.relic_num)
        
        -- inscribe it to prevent automatizer 'accidents'
        relic.note = quark_add("quest")
        
        -- drop it
        drop_near(relic, -1, y, x)

        -- Only generate once!
        god_quest.relic_generated = TRUE

        -- Reset some variables
        god_quest.relic_gen_tries = 0

end




function set_god_dungeon_attributes()

        -- dungeon properties altered according to which god player is worshipping, 
        if player.pgod == GOD_ERU then

                -- The Eru temple is based on Meneltarma.
                -- W: Not too many monsters (they'll be tough though, with big levels)
                dungeon(god_quest.DUNGEON_GOD).min_m_alloc_level = 14
                dungeon(god_quest.DUNGEON_GOD).min_m_alloc_chance = 200

                -- L: Dirt and grass. More dirt at bottom, more grass at top. rocky ground would be nice
                dungeon(god_quest.DUNGEON_GOD).floor1 = 88
                dungeon(god_quest.DUNGEON_GOD).floor2 = 89
                dungeon(god_quest.DUNGEON_GOD).floor_percent1[1] = 70
                dungeon(god_quest.DUNGEON_GOD).floor_percent2[1] = 30
                dungeon(god_quest.DUNGEON_GOD).floor_percent1[2] = 10
                dungeon(god_quest.DUNGEON_GOD).floor_percent2[2] = 90

                -- A: Outer wall mountain chain. other walls granite
                dungeon(god_quest.DUNGEON_GOD).fill_type1 = 97
                dungeon(god_quest.DUNGEON_GOD).fill_percent1[1] = 100
                dungeon(god_quest.DUNGEON_GOD).outer_wall = 57
                dungeon(god_quest.DUNGEON_GOD).inner_wall = 97
                dungeon(god_quest.DUNGEON_GOD).fill_method = 2

                -- O: "At Meneltarma no weapon or tool had ever been borne" (but invaders would have left a small number)
                dungeon(god_quest.DUNGEON_GOD).objs.treasure = 45
                dungeon(god_quest.DUNGEON_GOD).objs.combat = 5
                dungeon(god_quest.DUNGEON_GOD).objs.magic = 45
                dungeon(god_quest.DUNGEON_GOD).objs.tools = 5
                
                -- F: A large pillar, with stairs created at edges. (You can't climb a rock through the middle, can you?)
                dungeon(god_quest.DUNGEON_GOD).flags1 = bor(DF1_BIG, DF1_NO_DOORS, DF1_CIRCULAR_ROOMS, DF1_EMPTY, DF1_TOWER, DF1_FLAT, DF1_ADJUST_LEVEL_2)
                dungeon(god_quest.DUNGEON_GOD).flags2 = DF2_NO_SHAFT
                
                -- R:
                dungeon(god_quest.DUNGEON_GOD).rules[1].mode = 3
                dungeon(god_quest.DUNGEON_GOD).rules[1].percent = 50

                -- M: We want evil or flying characters
                dungeon(god_quest.DUNGEON_GOD).rules[1].mflags3 = RF3_EVIL

                dungeon(god_quest.DUNGEON_GOD).rules[2].mode = 3
                dungeon(god_quest.DUNGEON_GOD).rules[2].percent = 50

                -- M: We want evil or flying characters
                dungeon(god_quest.DUNGEON_GOD).rules[2].mflags7 = RF7_CAN_FLY
                

        elseif player.pgod == GOD_MANWE then

                -- Manwe's lost temple is high in the clouds
                -- W: Has average number of monsters.
                dungeon(god_quest.DUNGEON_GOD).min_m_alloc_level = 18
                dungeon(god_quest.DUNGEON_GOD).min_m_alloc_chance = 160


                -- L: floor will be 'cloud-like vapour' and pools of 'condensing water'
                dungeon(god_quest.DUNGEON_GOD).floor1 = 208
                dungeon(god_quest.DUNGEON_GOD).floor2 = 209
                dungeon(god_quest.DUNGEON_GOD).floor_percent1[1] = 85
                dungeon(god_quest.DUNGEON_GOD).floor_percent2[1] = 15
                
                -- A: Outer wall is 'hail stone wall', inner wall 'dense fog'. FIlled at max smoothing, like islands.
                dungeon(god_quest.DUNGEON_GOD).fill_type1 = 211
                dungeon(god_quest.DUNGEON_GOD).fill_percent1[1] = 100
                dungeon(god_quest.DUNGEON_GOD).outer_wall = 210
                dungeon(god_quest.DUNGEON_GOD).inner_wall = 211
                dungeon(god_quest.DUNGEON_GOD).fill_method = 4

                -- O: Can't imagine Manwe having much treasure. Little need for tools in a cloud temple. lots of magical stuff though...
                dungeon(god_quest.DUNGEON_GOD).objs.treasure = 15
                dungeon(god_quest.DUNGEON_GOD).objs.combat = 25
                dungeon(god_quest.DUNGEON_GOD).objs.magic = 55
                dungeon(god_quest.DUNGEON_GOD).objs.tools = 5

                -- F: It's open, goes up like a tower, give it a few interesting rooms, make the monsters hard(ish).
                dungeon(god_quest.DUNGEON_GOD).flags1 = bor(DF1_NO_DOORS, DF1_TOWER, DF1_CAVERN,  DF1_ADJUST_LEVEL_1)
                dungeon(god_quest.DUNGEON_GOD).flags2 = bor(DF2_ADJUST_LEVEL_1_2, DF2_NO_SHAFT)

                -- R:
                dungeon(god_quest.DUNGEON_GOD).rules[1].mode = 3
                dungeon(god_quest.DUNGEON_GOD).rules[1].percent = 20
                dungeon(god_quest.DUNGEON_GOD).rules[2].mode = 3
                dungeon(god_quest.DUNGEON_GOD).rules[2].percent = 20
                dungeon(god_quest.DUNGEON_GOD).rules[3].mode = 3
                dungeon(god_quest.DUNGEON_GOD).rules[3].percent = 20
                dungeon(god_quest.DUNGEON_GOD).rules[4].mode = 3
                dungeon(god_quest.DUNGEON_GOD).rules[4].percent = 20
                dungeon(god_quest.DUNGEON_GOD).rules[5].mode = 3
                dungeon(god_quest.DUNGEON_GOD).rules[5].percent = 20

                -- M: We want air(poison-type) or flying characters. Orcs too. They would have ransacked his elf-loving temple :)
                dungeon(god_quest.DUNGEON_GOD).rules[1].mflags2 = RF2_INVISIBLE
                dungeon(god_quest.DUNGEON_GOD).rules[2].mflags3 = bor(RF3_ORC, RF3_IM_POIS)
                dungeon(god_quest.DUNGEON_GOD).rules[3].mflags4 = bor(RF4_BR_POIS, RF4_BR_GRAV)
                dungeon(god_quest.DUNGEON_GOD).rules[4].mflags5 = RF5_BA_POIS
                dungeon(god_quest.DUNGEON_GOD).rules[5].mflags7 = RF7_CAN_FLY

                
        elseif player.pgod == GOD_TULKAS then
                
                -- Tulkas dungeon is quite normal, possibly a bit boring to be honest. Maybe I should add something radical to it.
                -- 'The house of Tulkas in the midmost of  Valmar was a house of mirth and revelry. It sprang into the air with many storeys,
                -- and had a tower of bronze and pillars of copper in a wide arcade'
                -- W: but with lots of monsters
                dungeon(god_quest.DUNGEON_GOD).min_m_alloc_level = 20
                dungeon(god_quest.DUNGEON_GOD).min_m_alloc_chance = 120

                -- L: floor is normal
                dungeon(god_quest.DUNGEON_GOD).floor1 = 1
                dungeon(god_quest.DUNGEON_GOD).floor_percent1[1] = 100

                -- A: Granite walls
                dungeon(god_quest.DUNGEON_GOD).fill_type1 = 56
                dungeon(god_quest.DUNGEON_GOD).fill_percent1[1] = 100
                dungeon(god_quest.DUNGEON_GOD).outer_wall = 58
                dungeon(god_quest.DUNGEON_GOD).inner_wall = 57
                dungeon(god_quest.DUNGEON_GOD).fill_method = 0

                -- O: Loads of combat drops
                dungeon(god_quest.DUNGEON_GOD).objs.treasure = 10
                dungeon(god_quest.DUNGEON_GOD).objs.combat = 70
                dungeon(god_quest.DUNGEON_GOD).objs.magic = 5
                dungeon(god_quest.DUNGEON_GOD).objs.tools = 15

                -- F: fairly standard
                dungeon(god_quest.DUNGEON_GOD).flags1 = bor(DF1_NO_DESTROY, DF1_ADJUST_LEVEL_1)

                -- R:
                dungeon(god_quest.DUNGEON_GOD).rules[1].mode = 3
                dungeon(god_quest.DUNGEON_GOD).rules[1].percent = 100

                -- M: plenty demons please
                dungeon(god_quest.DUNGEON_GOD).rules[1].mflags3 = bor(RF3_DEMON, RF3_EVIL)
                

        elseif player.pgod == GOD_MELKOR then

                -- Melkors dungeon will be dark, fiery and stuff
                -- Many many monsters! (but prob ADJUST_LEVEL_1_2)
                dungeon(god_quest.DUNGEON_GOD).min_m_alloc_level = 24
                dungeon(god_quest.DUNGEON_GOD).min_m_alloc_chance = 80


                -- L: floor is dirt/mud/nether
                dungeon(god_quest.DUNGEON_GOD).floor1 = 88 
                dungeon(god_quest.DUNGEON_GOD).floor2 = 94 
                dungeon(god_quest.DUNGEON_GOD).floor3 = 102 
                dungeon(god_quest.DUNGEON_GOD).floor_percent1[1] = 45
                dungeon(god_quest.DUNGEON_GOD).floor_percent2[1] = 45
                dungeon(god_quest.DUNGEON_GOD).floor_percent3[1] = 10
                dungeon(god_quest.DUNGEON_GOD).floor_percent1[2] = 35
                dungeon(god_quest.DUNGEON_GOD).floor_percent2[2] = 35
                dungeon(god_quest.DUNGEON_GOD).floor_percent3[2] = 30

                -- A: Granite walls to fill but glass walls for room perimeters (you can see the nasty monsters coming)
                dungeon(god_quest.DUNGEON_GOD).fill_type1 = 188 
                dungeon(god_quest.DUNGEON_GOD).fill_percent1[1] = 100
                dungeon(god_quest.DUNGEON_GOD).outer_wall = 188
                dungeon(god_quest.DUNGEON_GOD).inner_wall = 57
                dungeon(god_quest.DUNGEON_GOD).fill_method = 1

                -- O: Even drops
                dungeon(god_quest.DUNGEON_GOD).objs.treasure = 25
                dungeon(god_quest.DUNGEON_GOD).objs.combat = 25
                dungeon(god_quest.DUNGEON_GOD).objs.magic = 25
                dungeon(god_quest.DUNGEON_GOD).objs.tools = 25

                -- F: Small, lava rivers, nasty monsters hehehehehe
                dungeon(god_quest.DUNGEON_GOD).flags1 = bor(DF1_SMALL, DF1_LAVA_RIVERS, DF1_ADJUST_LEVEL_2)

                -- R: No restrictions on monsters here
                dungeon(god_quest.DUNGEON_GOD).rules[1].mode = 0
                dungeon(god_quest.DUNGEON_GOD).rules[1].percent = 80

                -- R: Apart from making sure we have some GOOD ones
                dungeon(god_quest.DUNGEON_GOD).rules[2].mode = 3
                dungeon(god_quest.DUNGEON_GOD).rules[2].percent = 20

                -- M: 
                dungeon(god_quest.DUNGEON_GOD).rules[2].mflags3 = RF3_GOOD

        elseif player.pgod == GOD_YAVANNA then

                -- Yavannas dungeon will be very natural, tress and stuff.
                dungeon(god_quest.DUNGEON_GOD).min_m_alloc_level = 22
                dungeon(god_quest.DUNGEON_GOD).min_m_alloc_chance = 100

                -- L: floor is grass/flowers, plus dirt so not always regenerating quick!
                dungeon(god_quest.DUNGEON_GOD).floor1 = 89 
                dungeon(god_quest.DUNGEON_GOD).floor2 = 199 
                dungeon(god_quest.DUNGEON_GOD).floor3 = 88 
                dungeon(god_quest.DUNGEON_GOD).floor_percent1[1] = 40
                dungeon(god_quest.DUNGEON_GOD).floor_percent2[1] = 15
                dungeon(god_quest.DUNGEON_GOD).floor_percent3[1] = 45

                -- A: Tree walls to fill, small trees for inner walls
                dungeon(god_quest.DUNGEON_GOD).fill_type1 = 96
                dungeon(god_quest.DUNGEON_GOD).fill_percent1[1] = 100
                dungeon(god_quest.DUNGEON_GOD).outer_wall = 202
                dungeon(god_quest.DUNGEON_GOD).inner_wall = 96
                dungeon(god_quest.DUNGEON_GOD).fill_method = 1

                -- O: nt much combat.. tools where ransackers have tried to chop trees down.
                dungeon(god_quest.DUNGEON_GOD).objs.treasure = 20
                dungeon(god_quest.DUNGEON_GOD).objs.combat = 10
                dungeon(god_quest.DUNGEON_GOD).objs.magic = 30
                dungeon(god_quest.DUNGEON_GOD).objs.tools = 40

                -- F: Natural looking
                dungeon(god_quest.DUNGEON_GOD).flags1 = bor(DF1_NO_DOORS, DF1_WATER_RIVERS, DF1_NO_DESTROY, DF1_ADJUST_LEVEL_1)
                dungeon(god_quest.DUNGEON_GOD).flags2 = bor(DF2_ADJUST_LEVEL_1_2, DF2_NO_SHAFT, DF2_NO_GENO)

                -- R: Demons, Undead, non-living
                dungeon(god_quest.DUNGEON_GOD).rules[1].mode = 3
                dungeon(god_quest.DUNGEON_GOD).rules[1].percent = 100

                -- M: 
                dungeon(god_quest.DUNGEON_GOD).rules[1].mflags3 = bor(RF3_DEMON, RF3_UNDEAD, RF3_NONLIVING)

        end

        -- W: All dungeons are 5 levels deep, and created at 2/3 of the player clvl when the quest is given
        dungeon(god_quest.DUNGEON_GOD).mindepth = god_quest.dun_mindepth
        dungeon(god_quest.DUNGEON_GOD).maxdepth = god_quest.dun_maxdepth
        dungeon(god_quest.DUNGEON_GOD).minplev = god_quest.dun_minplev
        
end

-- Calling this function returns the direction the dungeon is in from the players position at the time 
-- the quest was given, and also the direction from angband (if the player is worshipping angband) or lothlorien.
function get_god_quest_axes()
        local play_y_coord, play_x_coord, y_axis, x_axis
        
        player_axis = compass(god_quest.player_y, god_quest.player_x, god_quest.dung_y, god_quest.dung_x)
                
        -- different values for different gods...
        if player.pgod ~= GOD_MELKOR then
        
                -- one of the valar, "home" is lothlorien
                home = "Lothlorien"
                home_y_coord = 34
                home_x_coord = 50
        else 
                -- Melkor, "home" is angband
                home = "the Pits of Angband"
                home_y_coord = 7
                home_x_coord = 34
        end
        
        home_axis = compass(home_y_coord, home_x_coord, god_quest.dung_y, god_quest.dung_x)
        
        return player_axis, home, home_axis 
end