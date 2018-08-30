#ifndef classes_hpp
#define classes_hpp

#include "enums.h"
#include "UI_classes.hpp"
#include <vector>
#include <string>

struct material
{
    std::string name;
    int number;
};
struct equipment
{
    equipment(int in_mod, std::string in_name, slot which_slot)
    {
        modifier = in_mod;
        name = in_name;
        where_go = which_slot;
    }
    bool operator==(const equipment& other) const
    {
        return (name == other.name && modifier == other.modifier && where_go==other.where_go);
    }
    equipment()
    {
        modifier=20;
        name="fk";
    } //this is so fucking dumb
    int modifier;
    std::string name;
    slot where_go;
    bool equipped;
    int cost;
    //need more things to represent cost etc. maybe make each one the product of crafting
};

struct recipe
{
    equipment makes;
    std::vector<std::tuple<material, int>> material_costs;
};

struct research
{
    int tier;
    int required_research_points;
    int gotten_research_points;
    bool finished;
    std::string name;
    std::vector<std::string> leads_to;
    std::vector<std::string> requires;
};

class unit
{
public:
    unit(bool in_ranged, int in_range, bool in_magical, std::string in_name)
    {
        ranged=in_ranged;
        range =in_range;
        magical=in_magical;
        name=in_name;
    }
    float hp, stamina, mana;
    int accuracy, defense, DR, reaction;
    int damage;
    bool ranged;
    bool magical;
    int range;
    int gives_exp;
    std::string name;
private:
};
//maybe implement wounds? i.e. caravaneer style

//martyr, lictor-BG, imperator-Offensive, palatine-Defensive, Trapper-cunning trapper,
//sicarius-assassin, hierophant- leader,rector-defensive, ambusher, spymaster, champion, wanderer, marksman
struct archetype
//struct because it will eventually have default weapons etc
{
    archetype(std::string in_name, std::vector<equipment> in_equip)
    {
        name = in_name;
        archetype_equipment = in_equip;
    }
    archetype()
    {
        
    }//fuk this
    std::string name;
    std::string abbrv;
    std::vector<equipment> archetype_equipment;
    int mod_cun, mod_int, mod_wis, mod_tena, mod_agi, mod_cha, mod_str;
};

class hero : public unit
{
public:
    hero(int in_cun, int in_intel, int in_wis, int in_tena, int in_agi,
         int in_cha, int in_str, bool ranged, int range, bool magical, std::string name, archetype& in_archetype):
    unit (ranged, range, magical, name)
    {
        std::tie(cun, intel, wis) = std::tie(in_cun, in_intel, in_wis);
        std::tie(tena, agi, cha, str) = std::tie(in_tena, in_agi, in_cha, in_str);
        hp = tena*2.3 + str*.5;
        stamina = tena*1.9;
        mana = intel*1.4 + wis*.3 + cha*.2;
        if (ranged)
        {
            accuracy = agi * 1.3 + str * 1.7 + tena * .3;
        }
        else
        {
            accuracy = agi * 1.7 + str * 1.3 + tena * .6;
        }
        defense = agi * .5 + tena * .3;
        reaction = agi * 1 + tena * .4; //can turn a crit to a normal, normal to glances
        hero_type=in_archetype;
        damage=5;
    }
    void change_exp(int delta)
    {
        has_exp+=delta;
        if (has_exp<0)
        {
            has_exp=0;
        }
        if (has_exp>100)
        {
            level+=1;
        }
    }
    archetype get_archetype() {return hero_type;}
    int has_exp=0;
    int level = 1;
private:
    int cun, intel, wis;
    int tena, agi, cha, str;
    int loyalty, honesty, upright;
    int seen_loyalty, seen_honesty, seen_upright;
    archetype hero_type;
    std::vector<equipment> wearing;
};


struct role
//struct because it will eventually have default weapons etc
{
    role(std::string in_name, std::vector<equipment> in_equip)
    {
        name = in_name;
        role_equipment = in_equip;
    }
    role()
    {
        
    }//fuk this
    std::string name;
    std::vector<equipment> role_equipment;
};
/*
(int in_damage, int in_health, int in_DR, int in_exp, int in_unit_num, int in_unique_id, role in_role, std::string& in_name)
    {
        damage = in_damage;
        health = in_health;
        DR = in_DR;
        exp = in_exp;
        unit_number = in_unit_num;
        name = in_name;
        unique_id = in_unique_id;
        in_role;
		base_damage = in_damage;
		base_health = in_health;
		base_DR = in_DR;
    }

    void change_equipment(equipment& original, equipment& new_equip)
    {
		bool found=false;
        for (equipment & old : wearing)
        {
            if (old == original)
            {
				found = true;
                old = new_equip;
				break;
            }
        }
		if (!found)
		{
			wearing.push_back(new_equip);
		}
    }
	//unequip
	void change_equipment(equipment & original)
	{
		bool first_one = true;
		std::vector<equipment> new_wearing;
		for (equipment & old : wearing)
		{
			if (!first_one)
			{
				new_wearing.push_back(old);
			}
			else if (old == original)
			{
				first_one = false;
			}
			else
			{
				new_wearing.push_back(old);
			}
		}
		wearing = new_wearing;
	}
    void apply_equipment(void)
    {
		int highest_damage = 0;
		damage = base_damage;
		DR = base_DR;
        for (equipment & piece: wearing)
        {
            switch (piece.where_go)
            {
                case (slot::wep):
					if (highest_damage < piece.modifier)
					{
						highest_damage = piece.modifier;
					}
                    break;
                case (slot::armor):
                    DR = piece.modifier;
                    break;
                default:
                    break;
            }
        }
		damage = highest_damage;
    }
    void apply_role(std::vector<equipment_stores> & available_equipment)
    {
        wearing.clear();
        for (equipment & desired : .role_equipment)
        {
            for (equipment_stores & available : available_equipment)
            {
                if (available.make_up == desired)
                {
                    if (available.total_number > available.distributed_number)
                    {
                        available.distributed_number += 1;
                        wearing.push_back(available.make_up);
                    }
                }
            }
        }
        apply_equipment();
    }
	void apply_location(std::string loc_name)
	{
		location = loc_name;
	}
    std::vector<equipment> wearing;
    //private: haha i'm a bad programmer. rules r 4 suckas
    int damage;
    int health;
    int DR;
    int exp;
    int unit_number;
    std::string name;
    int unique_id;
	int base_damage;
	int base_health;
	int base_DR;
	std::string location = "Homeworld";
    //std::string location;
    //add equipment l8er
};
 */

#endif /* classes_hpp */
