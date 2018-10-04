//
//  enums.h
//  tacticsclone
//
//  Created by asdfuiop on 6/27/18.
//  Copyright © 2018 asdfuiop. All rights reserved.
//

#ifndef enums_h
#define enums_h

#include <unordered_map>

enum class stage {initial, game};
enum class game_substage{strategy, tactic};
enum class use {start, end_turn, sub_but};
//enum class subwindow_stage {none};
enum class slot {RH, LH, sidearm, armor, accessory, helmet, gloves, boots, mount, NA};
enum class inside {no, partial, yes};
enum class subwindow_reason{none, crafting, hero_overview, hero, research, change_equip,
                            update_changed_hero, sent_party_info, event_info, roster};
enum class direction {left, right};
//missions that you start yourself as well > train, steal_tech, steal_message, investigate?, intrigue, bribe, deliver_message
//war special ones> raid, raze, interdict, flank, spearpoint, run-down
enum class event_type {rebellion, revolt, uprising, riot, creature, intrigue, escort, assassinate, defend, cult, investigate,
                        raid, raze, interdict, flank, infiltrate, spearpoint, train, run_down, steal_tech, steal_message,
                        deliver_message, bribe, single_combat
};


typedef std::unordered_map<float, std::unordered_map<int, std::vector<int>>> polygon_column_index_map;
typedef std::unordered_map<float, std::vector<int>> polygon_row_index_map;
typedef std::tuple<int, int> tuple_int;

struct coordinate
{
    int x;
    int y;
};

#endif /* enums_h */
