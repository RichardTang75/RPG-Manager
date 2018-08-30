//
//  events.h
//  tacticsclone
//
//  Created by asdfuiop on 8/7/18.
//  Copyright © 2018 asdfuiop. All rights reserved.
//

#ifndef events_h
#define events_h

#include "enums.h"

//coountry or one big empire that's weakly connected?
class province
{
public:
    province(polygon_column_index_map & in_column_index_area, polygon_row_index_map & in_row_index_area, float value)
    {
        unique_value = value;
        column_index_area = in_column_index_area[value];
        row_index_area = in_row_index_area[value];
        assert(column_index_area.size() > 0);
        assert(row_index_area.size()>0);
        unrest = 10;
        wealth = 10;
        population = 10;
    }
    void load_province_for_drawing(SDL_Renderer* to_use)
    {
        bool first = true;
        int left = 0;
        int right = 0;
        int top = row_index_area.front();
        int down= row_index_area.back();
        for (auto & row_and_column_indices : column_index_area)
        {
            std::vector<int> column_indices = row_and_column_indices.second;
            int current_rightmost = *std::max_element(column_indices.begin(), column_indices.end());
            int current_leftmost = *std::min_element(column_indices.begin(), column_indices.end());
            if (first)
            {
                first=false;
                right = current_rightmost;
                left = current_leftmost;
            }
            else
            {
                if (current_rightmost > right)
                {
                    right = current_rightmost;
                }
                if (current_leftmost < left)
                {
                    left = current_leftmost;
                }
            }
        }
        draw_x = left;
        draw_y = top;
        draw_width = right-left;
        draw_height = down-top;
        SDL_Surface* temp_surf = SDL_CreateRGBSurfaceWithFormat(0, draw_width, draw_height, 32, SDL_PIXELFORMAT_RGBA32);
        for (auto & row_and_column_indices : column_index_area)
        {
            int row = row_and_column_indices.first;
            std::vector<int> column_indices = row_and_column_indices.second;
            for (int i=0; i<column_indices.size(); i+=2)
            {
                int line_start = column_indices[i]-draw_x;
                int line_end = column_indices[i+1]-draw_x;
                int line_width = line_end-line_start;
                SDL_Rect draw_line {line_start, row-draw_y, line_width, 1};
                int grayness = unique_value*128 + 128;
                SDL_FillRect(temp_surf,  &draw_line, SDL_MapRGBA(temp_surf->format, grayness, 255, grayness, 255));
            }
        }
//        std::string filename = std::to_string(unique_value) + ".bmp";
//        SDL_SaveBMP(temp_surf, filename.c_str());
        province_texture = SDL_CreateTextureFromSurface(to_use, temp_surf);
        SDL_FreeSurface(temp_surf);
        SDL_SetTextureAlphaMod(province_texture, 0);
    }
    void draw_province(SDL_Renderer* to_use, int camera_x, int camera_y)
    {
        SDL_Rect draw_province_rect {draw_x-camera_x, draw_y-camera_y, draw_width, draw_height};
        SDL_RenderCopy(to_use, province_texture, NULL, &draw_province_rect);
    }
    bool in_bound_box(coordinate in)
    {
        if (in.x>=draw_x && in.x<=(draw_x+draw_width) && in.y>=draw_y && in.y<=(draw_y+draw_height))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    bool inside(coordinate in)
    {
        if (std::find(row_index_area.begin(), row_index_area.end(), in.y) != row_index_area.end())
        {
            std::vector<int> & column_indices = column_index_area[in.y];
            for (int i=0; i<column_indices.size(); i++)
            {
                int start_index = column_indices[i];
                int end_index = column_indices[i+1];
                if (in.x>=start_index && in.x<=end_index) //look into whether the equality is needed or wanted
                {
                    return true;
                }
            }
        }
        return false;
    }
    friend class event;
private:
    std::unordered_map<int, std::vector<int>> column_index_area;
    std::vector<int> row_index_area;
    float unrest, wealth, population, wealth_growth, pop_growth;
    int draw_x, draw_y, draw_width, draw_height;
    SDL_Texture* province_texture;
    float unique_value;
};


//failure effect applies on event start, success restores some?
//failure effect applies on time out, stop event before, success does nothing
//failure does basically nothing, success increases stuff (patrolling)

class event
{
public:
    event(coordinate in_location, int & event_ids, std::vector<province> & provinces)
    {
        location=in_location;
        timer=2;
        unique_id=event_ids++;
        active=true;
#ifdef USE_PROVINCES
        for (auto & prov : provinces)
        {
            if (prov.in_bound_box(location))
            {
                std::cout<<"yo";
                if (prov.inside(location))
                {
                    associated_province = &prov;
                    std::cout<<"yo";
                    break;
                }
            }
        }
        assert(associated_province);
#endif
        std::tie(wealth_effect, wealth_growth_effect, population_effect, pop_growth_effect, unrest_effect) =
        std::make_tuple(10, 10, 10, 10, 10);
        std::tie(fail_wealth_effect, fail_wealth_growth_effect, fail_pop_effect, fail_pop_growth_effect, fail_unrest_effect) =
        std::make_tuple(-5, -5, -5, -5, -5);
    }
    bool operator== (const event& other)
    {
        return (unique_id==other.unique_id);
    }
    void draw(int camera_x, int camera_y, SDL_Renderer* to_use, TTF_Font* use_font, SDL_Color font_color)
    {
        int draw_x = location.x-camera_x;
        int draw_y = location.y-camera_y;
        int y= draw_y-radius;
        SDL_RenderDrawLine(to_use, draw_x, draw_y, draw_x, y);
        SDL_Rect event_rect {draw_x-radius, y-radius, 2*radius, radius};
        SDL_RenderFillRect(to_use, &event_rect);
        std::string text = "Days left: " + std::to_string(timer);
        write_text_centered(draw_x, y-radius/2, text, to_use, use_font, font_color);
    }
    void handle_click(int click_x, int click_y, int camera_x, int camera_y, std::vector<window_UI*> & all_windows)
    {
        if (click_x>location.x-radius-camera_x && click_x<location.x+radius-camera_x &&
            click_y>location.y-2*radius-camera_y && click_y<location.y-radius-camera_y)
        {
            for (auto & window : all_windows)
            {
                if (window->get_unique()==subwindow_reason::event_info)
                {
                    window->shown=true;
                }
            }
        }
    }
    bool update()
    {
        timer-=1;
        if (timer<1)
        {
            active=false;
            resolve(true);
            //apply effects
            return true;
        }
        return false;
    }
    void resolve(bool failure=false)
    {
#ifdef USE_PROVINCES
        if (failure)
        {
            associated_province->wealth+=fail_wealth_effect;
            associated_province->wealth_growth+=fail_wealth_growth_effect;
            associated_province->population+=fail_pop_effect;
            associated_province->pop_growth+=fail_pop_growth_effect;
            associated_province->unrest+=fail_unrest_effect;
        }
        else
        {
            associated_province->wealth+=wealth_effect;
            associated_province->wealth_growth+=wealth_growth_effect;
            associated_province->population+=population_effect;
            associated_province->pop_growth+=pop_growth_effect;
            associated_province->
            unrest+=unrest_effect;
        }
#endif
        active=false;
    }
    bool in_bounds(SDL_Rect& bounds)
    {
        int y=location.y-radius;
        SDL_Rect event_rect = {location.x-radius, y-radius, 2*radius, radius};
        inside is_it = in_set_bounds(event_rect, bounds);
        if (is_it == inside::no)
        {
            return false;
        }
        else
        {
            return true;
        }
    }
private:
    coordinate location;
    int priority; //???
    //success effects
    //failure effects
    //maybe another class handles this based on event type?
    int timer;
    //load in event text
    //each event has a radius, text moves around that radius, natural state is above
    int unique_id;
    bool active;
    int radius=50;
    province* associated_province;
    float wealth_effect, wealth_growth_effect, population_effect, pop_growth_effect, unrest_effect;
    float fail_wealth_effect, fail_wealth_growth_effect, fail_pop_effect, fail_pop_growth_effect, fail_unrest_effect;
};

#endif /* events_h */
