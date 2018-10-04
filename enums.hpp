//
//  enums.hpp
//  project4
//
//  Created by asdfuiop on 6/19/18.
//  Copyright © 2018 asdfuiop. All rights reserved.
//

#ifndef enums_h

#define enums_h
//so many things are initialized in the game loop, this may as well just be the loading screen+everything else
enum class stage {initial, game};
enum class game_substage {none, ground_war};
//add creation later. space + ground warfare can be drawn in a box, no need for stages for them<-earlier comment
enum class use {start, end_turn, start_atk, open_subwindow, sub_but};
enum class special_states {}; //equipment
enum class subwindow_stage {none, dudes};
enum class slot {wep, armor, special, mobility, NA};
enum class inside {no, partial, yes};
enum class window_UI_ID {dude, armory, dude_inset};
enum class subwindow_reason {none, armory, dude_overview, dude, change_equip, update_changed_dude, 
								fleet_info, switch_location, ships_info};
enum return_slot {primary, secondary, armor, special, mobility};

#endif /* enums_h */
