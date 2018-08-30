//
//  window_manager.hpp
//  tacticsclone
//
//  Created by asdfuiop on 7/6/18.
//  Copyright © 2018 asdfuiop. All rights reserved.
//

#ifndef window_manager_hpp
#define window_manager_hpp
#include "units.hpp"
#include "UI_classes.hpp"



class window_UI
{
public:
    //offset can be for the main affair, i.e. titles
    window_UI(texture_holder* & in_background, texture_holder* & in_button_img,
              int x1, int y1, int x2, int y2, int z, int x_off, int y_off, int x_inc, int y_inc)
    {
        std::tie(start_x, start_y, end_x, end_y, z_index) = std::tie(x1, y1, x2, y2, z);
        x_offset = x_off;
        y_offset = y_off;
        x_increment = x_inc;
        y_increment = y_inc;
        int x_width = x2-x1;
        int y_width = y2-y1;
        background_img=in_background;
        button_img = in_button_img;
        background = {start_x, start_y, x_width, y_width};
        from_background= {0,0, x_width, y_width};
    }
    window_UI(texture_holder* & in_button_img,
              int x1, int y1, int x2, int y2, int z, int x_off, int y_off, int x_inc, int y_inc)
    {
        std::tie(start_x, start_y, end_x, end_y, z_index) = std::tie(x1, y1, x2, y2, z);
        x_offset = x_off;
        y_offset = y_off;
        x_increment = x_inc;
        y_increment = y_inc;
        int x_width = x2-x1;
        int y_width = y2-y1;
        button_img = in_button_img;
        background = {start_x, start_y, x_width, y_width};
        from_background= {0,0, x_width, y_width};
    }
    ~window_UI()
    {
        
    }
    int get_z(void)
    {
        return z_index;
    }
    SDL_Rect dimensions(void)
    {
        int dim_width = end_x-start_x;
        int dim_height=end_y-start_y;
        return SDL_Rect{start_x, start_y, dim_width, dim_height};
    }
    void draw_outline(SDL_Renderer* to_use, int r=255, int g=255, int b=255)
    {
        SDL_SetRenderDrawColor(to_use, r, g, b, 0xFF);
        SDL_RenderDrawRect(to_use, &background);
    }
    void draw_base(SDL_Renderer* to_use)
    {
        background_img->draw_this(to_use, background);
        draw_outline(to_use);
    }
    void draw_base(SDL_Renderer* to_use, int out_r, int out_g, int out_b)
    {
        background_img->draw_this(to_use, background);
        draw_outline(to_use, out_r, out_g, out_b);
    }
    bool in_bounds(int where_x, int where_y)
    {
        return (where_x > start_x && where_x < end_x && where_y>start_y && where_y < end_y);
    }
    virtual std::pair<subwindow_reason, int> handle_click(int mouse_x, int mouse_y)
    {
        return std::make_pair(subwindow_reason::none,-1);
    }
    virtual void draw_full(SDL_Renderer* to_use, TTF_Font* font_use, SDL_Color what_color)
    {
    }
    void border_drawer(texture_holder* corner, texture_holder* center, texture_holder* line, SDL_Renderer* to_use, int sub_section_x=0, int sub_section_y=0)
    {
        int x0=start_x;
        int x1=end_x;
        int y0=start_y;
        int y1=end_y;
        if (sub_section_x<0)
        {
            x0-=sub_section_x;
        }
        else
        {
            x1+=sub_section_x;
        }
        if (sub_section_y<0)
        {
            y0-=sub_section_y;
        }
        else
        {
            y1+=sub_section_y;
        }

        int width=x1-x0;
        int height=y1-y0;
        if (width < 100 || height < 100)
        {
            
        }
        else
        {
            corner->draw_this_rotate(to_use, SDL_Rect{x0, y0, 50, 50}, 0);
            corner->draw_this_rotate(to_use, SDL_Rect{x1-50, y0, 50, 50}, 0, SDL_FLIP_HORIZONTAL);
            corner->draw_this_rotate(to_use, SDL_Rect{x0, y1-50, 50, 50}, 0, SDL_FLIP_VERTICAL);
            corner->draw_this_rotate(to_use, SDL_Rect{x1-50, y1-50, 50, 50}, 180);
            
            if (width>250)
            {
                int center_x = x0+width/2;
                center->draw_this(to_use, SDL_Rect{center_x-25, y0, 50, 50});
                center->draw_this_rotate(to_use, SDL_Rect{center_x-25, y1-50, 50, 50}, 0, SDL_FLIP_VERTICAL);
                int width_to_center = center_x - (x0+50) - 25;
                line->draw_this(to_use, SDL_Rect{x0+50, y0, width_to_center, 50});
                line->draw_this(to_use, SDL_Rect{center_x+25, y0, width_to_center, 50});
                line->draw_this_rotate(to_use, SDL_Rect{x0+50, y1-50, width_to_center, 50}, 0, SDL_FLIP_VERTICAL);
                line->draw_this_rotate(to_use, SDL_Rect{center_x+25, y1-50, width_to_center, 50}, 0, SDL_FLIP_VERTICAL);
            }
            else if (width>100)
            {
                line->draw_this(to_use, SDL_Rect{x0+50, y0, width-100, 50});
                line->draw_this_rotate(to_use, SDL_Rect{x0+50, y1-50, width-100, 50}, 0, SDL_FLIP_VERTICAL);
            }
            
            if(height>250)
            {
                int center_y = y0+height/2;
                int height_to_center = center_y - (y0+50) - 25;
                center->draw_this_rotate(to_use, SDL_Rect{x0, center_y-25, 50, 50}, -90);
                center->draw_this_rotate(to_use, SDL_Rect{x1-50, center_y-25, 50, 50}, 90);
                line->draw_this_rotate(to_use, SDL_Rect{x0, y0+51, 50, height_to_center}, -90);
                line->draw_this_rotate(to_use, SDL_Rect{x0, center_y+26, 50, height_to_center}, -90);
                line->draw_this_rotate(to_use, SDL_Rect{x1-50, y0+50, 50, height_to_center}, 90);
                line->draw_this_rotate(to_use, SDL_Rect{x1-50, center_y+25, 50, height_to_center}, 90);
                
            }
            else if (height>100)
            {
                line->draw_this_rotate(to_use, SDL_Rect{x0, y0+50, 50, height-100}, -90);
                line->draw_this_rotate(to_use, SDL_Rect{x1-50, y0+50, 50, height-100}, 90);
            }
        }
    }
    void title_drawer(std::string title, SDL_Renderer* to_use, TTF_Font* font_use, SDL_Color what_color, int down, int x_off=0)
    {
        int width=end_x-start_x+x_off;
        int middle=(width)/2+start_x;
        write_text_centered(middle, down+start_y, title, to_use, font_use, what_color);
    }
    bool shown = false;
    subwindow_reason get_unique(void) { return unique; }
    bool changed = false;
    int index_changed = 0;
    int index_hero=0;
protected:
    int start_x, start_y, end_x, end_y; //base window
    int z_index;                        //does what it sounds like
    int x_offset, y_offset, x_increment, y_increment;   //offset>
    SDL_Rect background;
    SDL_Rect from_background;
    //dependent upon what you want bby
    std::vector<button> sub_rects;
    //buttons. first two declare first start of buttons, end_x is end_x of button,
    //sub_height is height of buttons
    int sub_x_start, sub_y_start, sub_end_x_offset, sub_height;
    int start_write_x;
    int start_write_y;
    subwindow_reason unique;
    texture_holder* background_img;
    texture_holder* button_img;
    //std::vector<button> click_inside;
};


/*-------------------------------------------------------------*/
/*-------------------------------------------------------------*/
/*-------------------------------------------------------------*/


class hero_window : public window_UI
{
public:
    hero_window(texture_holder* background, texture_holder* button_img, window_UI* assoc_sub, direction side,
                int x1, int y1, int x2, int y2, int z, int x_off, int y_off, int x_inc,
                int y_inc, std::vector<hero>* every_hero_ref, int sub_x, int sub_y,
                int sub_e_x, int sub_h, std::unordered_map<std::string, texture_holder>& archetype_icons): window_UI (background, button_img, x1, y1, x2, y2, z, x_off, y_off, x_inc, y_inc)
    {
        sub_x_start = sub_x;
        sub_y_start = sub_y;
        sub_end_x_offset = sub_e_x;
        sub_height = sub_h;
        hero_ref = every_hero_ref;
        unique = subwindow_reason::hero_overview;
        archetype_icon_ref=&archetype_icons;
        held_subwindow= assoc_sub;
        SDL_Rect subwindow_dimensions = held_subwindow->dimensions();
        subwindow_width = subwindow_dimensions.w;
        subwindow_height = subwindow_dimensions.h;
        subwindow_side = side;
    }
    virtual void draw_full(SDL_Renderer* to_use, TTF_Font* font_use, SDL_Color what_color)
    {
        draw_base(to_use);
        std::vector<hero> every_hero = *hero_ref;
        int button_x_start = sub_x_start + start_x;
        int button_y_start = sub_y_start + start_y;
        if (subwindow_side == direction::left)
        {
            button_x_start+=subwindow_width;
        }
        else if (subwindow_side == direction::right)
        {
            button_x_start-=subwindow_width;
        }
        int button_width = end_x - sub_end_x_offset - button_x_start;
        if (changed==true)
        {
            changed = false;
            hero current_hero = every_hero[index_changed];
            std::string text = current_hero.name + "\n" + std::to_string(current_hero.hp) + "\t" + std::to_string(current_hero.damage);
            button new_button = button(button_img, button_x_start, button_y_start + index_changed * y_increment, button_width, sub_height, text, use::sub_but, 0, 0);
            sub_rects[index_changed] = new_button;
        }
        else if (sub_rects.size()==every_hero.size())
        {
            
        }
        else
        {
            sub_rects.clear();
            for (int i=0; i<every_hero.size(); i++)
            {
                hero current_hero = every_hero[i];
                std::string text = "    " + current_hero.name + "\n        Archetype: " + current_hero.get_archetype().name + "    Level: " +
                std::to_string(current_hero.level) +  "     Health: " +std::to_string(int(current_hero.hp)) + "     Damage: " + std::to_string(int(current_hero.damage));
                button new_button = button(button_img, button_x_start, button_y_start + i* y_increment, button_width, sub_height, text, use::sub_but, 0, 0);
                sub_rects.push_back(new_button);
            }
        }
        for (button sub : sub_rects)
        {
            sub.draw(to_use, font_use, what_color, 255, 255, 255);
//            if (sub.in_set_bounds(start_x, end_x, start_y + scroll_y, end_y + scroll_y) == inside::yes)
//            {
//                sub.draw(renderer, smaller_font, font_color_white, 95, 63, 132, 255, 255, 255, 0, scroll_y);
//            }
        }
        SDL_SetRenderDrawColor(to_use, 200, 200, 200, 255);
        SDL_RenderDrawLine(to_use, start_x+x_offset+subwindow_width, start_y+y_offset, end_x-x_offset, start_y+y_offset);
    }
    virtual std::pair<subwindow_reason, int> handle_click(int mouse_x, int mouse_y)
    {
        for (int i = 0; i < sub_rects.size(); i++)
        {
            button is_click = sub_rects[i];
            if (is_click.in_bounds(mouse_x, mouse_y/*+scroll_y*/))
            {
                return std::make_pair(subwindow_reason::hero, i);
            }
        }
        return std::make_pair(subwindow_reason::none, -1);
    }
    int subwindow_width;
    int subwindow_height;
    window_UI* held_subwindow;
private:
    std::vector<hero>* hero_ref;
    std::unordered_map<std::string, texture_holder>* archetype_icon_ref;
    direction subwindow_side;
};
class hero_infoscreen : public window_UI
{
public:
    hero_infoscreen (texture_holder* button_img, int x1, int y1, int x2, int y2, int z, int x_off, int y_off, int x_inc,
                     int y_inc, std::vector<hero>* every_hero_ref, int sub_x, int sub_y,
                     int sub_e_x, int sub_h, std::unordered_map<std::string, texture_holder>& archetype_icons):
                     window_UI (button_img, button_img, x1, y1, x2, y2, z, x_off, y_off, x_inc, y_inc)
    {
        sub_x_start = sub_x;
        sub_y_start = sub_y;
        sub_end_x_offset = sub_e_x;
        sub_height = sub_h;
        hero_ref = every_hero_ref;
        unique = subwindow_reason::hero;
        archetype_icon_ref=&archetype_icons;
    }
    virtual void draw_full(SDL_Renderer* to_use, TTF_Font* font_use, SDL_Color what_color)
    {
        //draw_base(to_use);
        draw_outline(to_use);
        hero of_interest = (*hero_ref)[index_changed];
        std::string text = of_interest.name;
        write_text(start_x + x_offset, start_y + y_offset, text, to_use, font_use, what_color);
    }
private:
    std::vector<hero>* hero_ref;
    std::unordered_map<std::string, texture_holder>* archetype_icon_ref;
};
class roster_screen: public window_UI
{
public:
    roster_screen (texture_holder* background, texture_holder* button_img, int x1, int y1, int x2, int y2, int z, int x_off, int y_off, int x_inc,
                   int y_inc, std::vector<hero>* every_hero_ref, int sub_x, int sub_y,
                   int sub_e_x, int sub_h, std::unordered_map<std::string, texture_holder>& archetype_icons):
                    window_UI (background, button_img, x1, y1, x2, y2, z, x_off, y_off, x_inc, y_inc)
    {
        sub_x_start = sub_x;
        sub_y_start = sub_y;
        sub_end_x_offset = sub_e_x;
        sub_height = sub_h;
        hero_ref = every_hero_ref;
        unique = subwindow_reason::roster;
        archetype_icon_ref=&archetype_icons;
        button_x_start = sub_x_start + start_x;
        button_y_start = sub_y_start + start_y;
        sub_width = end_x - sub_end_x_offset - button_x_start;
    }
    virtual std::pair<subwindow_reason, int> handle_click(int mouse_x, int mouse_y)
    {
        if (mouse_x>button_x_start && mouse_x<button_x_start + sub_width && mouse_y>button_y_start)
        {
        }
        return std::make_pair(subwindow_reason::none,-1);
    }
    virtual void draw_full(SDL_Renderer* to_use, TTF_Font* font_use, SDL_Color what_color)
    {
        draw_outline(to_use);
        std::vector<hero> every_hero = *hero_ref;
        if (changed==true)
        {
            changed = false;
            hero current_hero = every_hero[index_changed];
            std::string text = current_hero.name + "\n" + std::to_string(current_hero.hp) + "\t" + std::to_string(current_hero.damage);
            button new_button = button(button_img, button_x_start, button_y_start + index_changed * y_increment, sub_width, sub_height, text, use::sub_but, 0, 0);
            sub_rects[index_changed] = new_button;
        }
        else if (sub_rects.size()==every_hero.size())
        {
        }
        else
        {
            sub_rects.clear();
            for (int i=0; i<every_hero.size(); i++)
            {
                hero current_hero = every_hero[i];
                std::string text = "    " + current_hero.name + "\n    Arch: " + current_hero.get_archetype().abbrv + "    Lvl: " +
                std::to_string(current_hero.level) +  "    Hp: " +std::to_string(int(current_hero.hp)) + "     Dmg: " + std::to_string(int(current_hero.damage));
                button new_button = button(button_img, button_x_start, button_y_start + i* y_increment, sub_width, sub_height, text, use::sub_but, 0, 0);
                sub_rects.push_back(new_button);
            }
        }
        for (int i=0; i<sub_rects.size(); i++)
        {
            button sub = sub_rects[i];
            if (std::find(selected_hero_indices.begin(), selected_hero_indices.end(), i) != selected_hero_indices.end())
            {
                
            }
            sub.draw(to_use, font_use, what_color, 255, 255, 255);
            //            if (sub.in_set_bounds(start_x, end_x, start_y + scroll_y, end_y + scroll_y) == inside::yes)
            //            {
            //                sub.draw(renderer, smaller_font, font_color_white, 95, 63, 132, 255, 255, 255, 0, scroll_y);
            //            }
        }
        SDL_SetRenderDrawColor(to_use, 200, 200, 200, 255);
        SDL_RenderDrawLine(to_use, start_x+x_offset, start_y+y_offset, end_x-x_offset, start_y+y_offset);
    }
    
    void change_position(int change_x, int change_y)
    {
        
    }
    void change_size(int new_width, int new_height)
    {
        
    }
    
private:
    std::vector<hero>* hero_ref;
    std::unordered_map<std::string, texture_holder>* archetype_icon_ref;
    std::vector<int> selected_hero_indices;
    int button_x_start;
    int button_y_start;
    int sub_width;
};
class event_infoscreen: public window_UI
{
public:
    event_infoscreen (texture_holder* background, texture_holder* button_img, window_UI* assoc_sub, direction side,
                      int x1, int y1, int x2, int y2, int z, int x_off, int y_off, int x_inc,
                      int y_inc, std::vector<hero>* every_hero_ref, int sub_x, int sub_y,
                      int sub_e_x, int sub_h, std::unordered_map<std::string, texture_holder>& archetype_icons): window_UI (background, button_img, x1, y1, x2, y2, z, x_off, y_off, x_inc, y_inc)
    {
        sub_x_start = sub_x;
        sub_y_start = sub_y;
        sub_end_x_offset = sub_e_x;
        sub_height = sub_h;
        hero_ref = every_hero_ref;
        unique = subwindow_reason::event_info;
        archetype_icon_ref=&archetype_icons;
        held_subwindow= assoc_sub;
        SDL_Rect subwindow_dimensions = held_subwindow->dimensions();
        subwindow_width = subwindow_dimensions.w;
        subwindow_height = subwindow_dimensions.h;
        subwindow_side = side;
    }
    int subwindow_width;
    int subwindow_height;
    window_UI* held_subwindow;
    virtual void draw_full(SDL_Renderer* to_use, TTF_Font* font_use, SDL_Color what_color)
    {
        draw_base(to_use);
        draw_outline(to_use);
//        hero of_interest = (*hero_ref)[index_changed];
//        std::string text = of_interest.name;
//        write_text(start_x + x_offset, start_y + y_offset, text, to_use, font_use, what_color);
    }
private:
    std::vector<hero>* hero_ref;
    std::unordered_map<std::string, texture_holder>* archetype_icon_ref;
    direction subwindow_side;
};
class research_window : public window_UI
{
    
};
class crafting_window: public window_UI
{
    
};

#endif /* window_manager_hpp */
