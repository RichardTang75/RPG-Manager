//
//  main.cpp
//  tacticsclone
//
//  Created by asdfuiop on 6/26/18.
//  Copyright © 2018 asdfuiop. All rights reserved.
//

//FEATURE FLAG>USE_PROVINCES

#ifdef _WIN32
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL.h>
#else
#include <SDL2_image/SDL_image.h>
#include <SDL2/SDL.h>
#include <SDL2_ttf/SDL_ttf.h>
#endif
#include <iostream>
#include <vector>
#include <tuple>
#include <unordered_map>
#include "enums.h"
#include "utilities.hpp"
#include "units.hpp"
#include "UI_classes.hpp"
#include "window_manager.hpp"
#include "delaunay-triangulation/vector2.h"
#include "delaunay-triangulation/triangle.h"
#include "delaunay-triangulation/delaunay.h"
#include "FastNoiseSIMD/FastNoiseSIMD/FastNoiseSIMD.h"
#include "ThreadPool/ThreadPool.h"
#include <random>
#include <memory>
#include <thread>
#include "test_funcs.hpp"
#include "FastNoise.h"
#undef main
#include "events.h"
#include <chrono>

int event_ids=0;

SDL_Color font_color_white {255, 255, 255};
SDL_Color font_color_darker {75, 75, 75};
TTF_Font* general_font;
TTF_Font* title_font;
TTF_Font* stress_font;
int window_height = 800;
int window_width=1280;
int camera_x=0;
int camera_y=0;
int world_height = 512*3;
int world_width = 512*3;
SDL_Rect camera_view;
int year = 17;
int month = 5;
int day = 30;
int hour = 8;
std::string reign_name = "Antony";
//dryseed-march
//greenshoot -april
//paleflower -may
//drysun -june
//goldharvest-july
//fullfruit -august
//grayfog - september
//deadleaf-october
//barebranch-november
//snowmist-december
//whitefrost-january
//highwind-february
std::vector<std::string> month_names = {"Newseed", "Greenshoot", "Paleflower",
                                         "Drysun", "Goldharvest", "Fullfruit",
                                         "Grayfog", "Deadleaf", "Barebranch",
                                         "Snowmist", "Whitefrost", "Highwind"};

stage current_stage = stage::game;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

int y_pixels_ratio=16;
int x_pixels_ratio=9;

std::unordered_map<std::string, texture_holder> archetype_icons;


std::random_device rd;
std::mt19937 eng(rd());
std::uniform_int_distribution<int> rand_y(0, world_height);
std::uniform_int_distribution<int> rand_x(0, world_width);

int get_random_int(int min, int max)
{
    return (std::uniform_int_distribution<int>(min, max)(eng));
}

std::vector<tuple_int> to_draw_directions =
{
    tuple_int(-1,-1), tuple_int(0,-1), tuple_int(1, -1),
    tuple_int(-1, 0), tuple_int(0,0), tuple_int(1, 0),
    tuple_int(-1, 1), tuple_int(0,1), tuple_int(1,1)
};

void close()
{
    TTF_CloseFont(general_font);
    TTF_CloseFont(stress_font);
    TTF_CloseFont(title_font);
    SDL_DestroyRenderer(renderer);
    renderer = NULL;
    SDL_DestroyWindow(window);
    window = NULL;
    IMG_Quit();
    SDL_Quit();
}

void update_time(std::vector<event> & active_events, std::vector<province> & provinces)
{
    hour+=1;
    if (hour>24)
    {
        hour=1;
        day+=1;
        std::vector<event> to_remove;
        for (auto & happening : active_events)
        {
            if (happening.update())
            {
                to_remove.push_back(happening);
            }
        }
        for (event & no_longer_active : to_remove)
        {
            auto iterator = std::find(active_events.begin(), active_events.end(), no_longer_active);
            active_events.erase(iterator);
        }
        for (int i=0; i<get_random_int(0, 5); i++)
        {
            coordinate temp_coord = {rand_x(eng), rand_y(eng)};
            event temp_event (temp_coord, event_ids, provinces);
            active_events.push_back(temp_event);
        }
    }
    if (day>30)
    {
        day=1;
        month+=1;
    }
    if (month>12)
    {
        month=1;
        year+=1;
    }
}

std::string ordinal_indicator(int number)
{
    int last_digit = number%10;
    int second_digit = (number/10)%10;
    if (second_digit==1)
    {
        return "th";
    }
    else
    {
        if (last_digit == 1)
        {
            return "st";
        }
        else if (last_digit == 2)
        {
            return "nd";
        }
        else if (last_digit == 3)
        {
            return "rd";
        }
        else
        {
            return "th";
        }
    }
}

void left_mouse_click(const int & mouse_x, const int & mouse_y,
                      std::vector<event>& active_events,
                      std::vector<window_UI*>& all_windows,
                      SDL_Rect & minimap, SDL_Rect & current_screen, const float& mini_size,
                      bool& in_mini_click, bool& drag, int & drag_x, int & drag_y, int & pre_drag_cam_x, int & pre_drag_cam_y , std::vector<std::unique_ptr<icon>>& icon_holder)
{
    bool active_click=false;
    std::vector<int> indices_of_interest;
    subwindow_reason window_to_deal_with;
    std::vector<int> already_selected_indices;
    int index_to_pass;
    for (auto & window : all_windows)
    {
        if (window->shown && window->in_bounds(mouse_x, mouse_y) && (!active_click))
        {
            active_click=true;
            switch (window->get_unique())
            {
                case subwindow_reason::roster:
                    window->handle_click(mouse_x, mouse_y); //submit button should push them all to the important window
                    break;
                case subwindow_reason::hero:
                    std::tie(window_to_deal_with, index_to_pass) = window->handle_click(mouse_x, mouse_y);
                    //need to pass info that hero was changed, changing screen
                    break;
                case subwindow_reason::hero_overview:
                    std::tie(window_to_deal_with, index_to_pass) = window->handle_click(mouse_x, mouse_y);
                    for (auto & find_to_deal_with : all_windows)
                    {
                        if (find_to_deal_with->get_unique()==window_to_deal_with)
                        {
                            find_to_deal_with->index_hero=index_to_pass;
                        }
                    }
                    break;
            };
            if (window->get_unique()==subwindow_reason::roster)
            {
            }
        }
    }
    if (in_rect_bounds(mouse_x, mouse_y, current_screen))
    {
        in_mini_click=true;
        drag=true;
        drag_x=mouse_x;
        drag_y=mouse_y;
        pre_drag_cam_x=camera_x;
        pre_drag_cam_y=camera_y;
    }
     else if (in_rect_bounds(mouse_x, mouse_y, minimap))
    {
        int equiv_x = float(mouse_x-minimap.x)/(mini_size);
        int equiv_y = float(mouse_y-minimap.y)/(mini_size);
        camera_x=equiv_x-window_width/2; //centers it around the point
        camera_y=equiv_y-window_height/2;
        in_mini_click=true;
        active_click=true;
        drag = true;
        drag_x=mouse_x;
        drag_y=mouse_y;
        pre_drag_cam_x=camera_x;
        pre_drag_cam_y=camera_y;
    }
    else
    {
        in_mini_click=false;
        drag=false;
    }
    if (!active_click)
    {
        for (auto & window : all_windows)
        {
            window->shown=false;
        }
    }
    for (auto & is_clicked : icon_holder)
    {
        if(is_clicked->in_bounds(mouse_x, mouse_y))
        {
            for (auto & assoc: all_windows)
            {
                if (assoc->get_unique()==is_clicked->assoc_subwindow)
                {
                    assoc->shown=true;
                }
            }
        }
    }
    for (auto& event_clicked : active_events)
    {
        event_clicked.handle_click(mouse_x, mouse_y, camera_x, camera_y, all_windows); //deals with loading the window with info
    }
}



//------------------------------------------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------------------------------------------//


void draw_everything
(
                     std::vector<std::unique_ptr<texture_holder>>& maps, int map_width, int map_height, std::vector<event>& active_events,
                     SDL_Rect& minimap, float mini_size, int mini_equiv_x, int mini_equiv_y, int mini_equiv_w, int mini_equiv_h,
                     SDL_Rect& current_screen, SDL_Rect& bottom_bar, bool& paused, std::vector<window_UI*>& all_windows,
                     texture_holder& center, texture_holder& corner, texture_holder& line, texture_holder& equipment_subrect,
                     std::vector<province> provinces, std::vector<std::unique_ptr<icon>>& icon_holder
)
{
    for (int i=0; i<maps.size(); i++)
    {
        tuple_int dir = to_draw_directions[i];
        int to_draw_x =(std::get<0>(dir)+1)*map_width - camera_x;
        int to_draw_y=(std::get<1>(dir)+1)*map_height - camera_y;
        SDL_Rect draw_rect=  {to_draw_x, to_draw_y, map_width, map_height};
        if (in_bounds(draw_rect, camera_view))
        {
            maps[i]->draw_this(renderer, draw_rect);
        }
    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 225);
    for (event & happening : active_events)
    {
        if (happening.in_bounds(camera_view))
        {
            happening.draw(camera_x, camera_y, renderer, general_font, font_color_white);
        }
    }
    for (int i=0; i<maps.size(); i++)
    {
        tuple_int dir = to_draw_directions[i];
        int mini_map_x = 980 + (std::get<0>(dir)+1)* 100;
        int mini_map_y = 0 + (std::get<1>(dir)+1)*100;
        SDL_Rect mini_map_rect = {mini_map_x, mini_map_y, 100, 100};
        maps[i]->change_alpha(renderer, 230);
        maps[i]->draw_this(renderer, mini_map_rect);
        maps[i]->change_alpha(renderer, 255);
    }
    
    
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &minimap);
    mini_equiv_x=camera_x*mini_size+980;
    mini_equiv_y=camera_y*mini_size;
    current_screen = SDL_Rect{mini_equiv_x, mini_equiv_y, mini_equiv_w, mini_equiv_h};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 30);
    SDL_RenderFillRect(renderer, &current_screen);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
    SDL_RenderFillRect(renderer, &bottom_bar);
    std::string fancy_date = " Today is the " + std::to_string(day) + ordinal_indicator(day) +  " of "
    + month_names[(month-1)] + ", year " +
    std::to_string(year) + " of Thearch " + reign_name + "'s reign.\n It is the " +
    std::to_string(hour) + ordinal_indicator(hour) + " hour of the day.";
    std::string time_and_date = "Year: " + std::to_string(year) + " Month: " + std::to_string(month)
    + " Day: " + std::to_string(day) + " Hour: " + std::to_string(hour);
    
    write_text(bottom_bar.x, bottom_bar.y, fancy_date, renderer, general_font, font_color_white);
    
    paused=false;
    
    for (window_UI* & to_draw : all_windows)
    {
        if (to_draw->shown)
        {
            paused=true;
            if (to_draw->get_unique()==subwindow_reason::hero_overview)
            {
                hero_window* cur_window = dynamic_cast<hero_window*>(to_draw);
                to_draw->draw_full(renderer, general_font, font_color_darker);
                to_draw->border_drawer(&corner, &center, &line, renderer, -(cur_window->subwindow_width));
                to_draw->title_drawer("Overview", renderer, title_font, font_color_white, 60);
                cur_window->held_subwindow->shown=true;
                equipment_subrect.draw_this(renderer, SDL_Rect {160, 120, 200, 215});
            }
            else if (to_draw->get_unique()==subwindow_reason::hero)
            {
                to_draw->draw_full(renderer, stress_font, font_color_white);
                to_draw->border_drawer(&corner, &center, &line, renderer);
            }
            else if (to_draw->get_unique()==subwindow_reason::roster)
            {
                to_draw->draw_full(renderer, general_font, font_color_darker);
                to_draw->border_drawer(&corner, &center, &line, renderer);
                to_draw->title_drawer("Roster", renderer, stress_font, font_color_white, 30);
            }
            else if (to_draw->get_unique()==subwindow_reason::event_info)
            {
                event_infoscreen* cur_window = dynamic_cast<event_infoscreen*>(to_draw);
                to_draw->draw_full(renderer, general_font, font_color_white);
                to_draw->border_drawer(&corner, &center, &line, renderer, -(cur_window->subwindow_width));
                cur_window->held_subwindow->shown=true;
            }
            else
            {
                to_draw->draw_full(renderer, general_font, font_color_white);
                to_draw->border_drawer(&corner, &center, &line, renderer);
            }
        }
    }
    for (auto & drawing : icon_holder)
    {
        drawing->draw_this(renderer);
    }
    for (auto & province : provinces)
    {
        province.draw_province(renderer, camera_x, camera_y);
    }
}
//difficulties of governing, clearly a pre-information age game
void game_loop(bool& quit, std::vector<province> & provinces)
{
    texture_holder window_background=texture_holder(renderer, "background.png");
    texture_holder box_background=texture_holder(renderer, "box.png");
    SDL_Surface* t_surf = IMG_Load("tempterrain.png");
    SDL_Texture* terrain_text = SDL_CreateTextureFromSurface(renderer, t_surf);
    SDL_FreeSurface(t_surf);
    SDL_Rect characters_draw_loc{1040, 720, 80, 80};
    SDL_Rect crafting_draw_loc{1120, 720, 80, 80};
    SDL_Rect research_draw_loc{1200, 720, 80, 80};
    texture_holder corner = texture_holder(renderer, "corner.png");
    texture_holder line = texture_holder(renderer, "line.png");
    texture_holder center = texture_holder(renderer, "center.png");
    std::vector<std::unique_ptr<icon>> icon_holder;
    std::vector<hero> all_heros;
    std::vector<equipment> martyr_equip;
    
    coordinate cool{250, 250};
    event temp_event (cool, event_ids, provinces);
    
    bool paused = false;
    
    std::vector<event> active_events =  {temp_event};
    std::chrono::system_clock::time_point last_update;
    std::chrono::system_clock::time_point now;
    last_update = std::chrono::system_clock::now();
    double time_to_next = 1000;

    archetype martyr = archetype("Martyr", martyr_equip);
    archetype hierophant = archetype("Hierophant", martyr_equip);
    archetype imperator = archetype("Imperator", martyr_equip);
    imperator.abbrv = "Imp";
    std::unordered_map<std::string, texture_holder> archetype_icons;
    hero temp = hero(5, 5, 5, 5, 5, 5, 5, true, 5, true, "Aurelian of the Golden Fist", imperator);
    all_heros.push_back(temp);
    //border_drawer window_borders = border_drawer(&corner, &center, &line);
    /*hero_window(<#texture_holder *background#>, <#texture_holder *button_img#>, <#int x1#>, <#int y1#>, <#int x2#>, <#int y2#>, <#int z#>, <#int x_off#>, <#int y_off#>, <#int x_inc#>, <#int y_inc#>, <#std::vector<hero> *every_hero_ref#>, <#int sub_x#>, <#int sub_y#>, <#int sub_e_x#>, <#int sub_h#>, <#std::unordered_map<std::string, texture_holder> &archetype_icons#>)
     */
    hero_infoscreen detailed_view = hero_infoscreen(&box_background, 60, 40, 460, 700, 3, 50, 30, 0, 50, &all_heros, 50, 75, 350, 50, archetype_icons);
    hero_window org_window = hero_window(&window_background, &box_background, &detailed_view, direction::left, 60, 40, 1220, 700, 2, 20, 80, 40, 80, &all_heros, 80, 100, 80, 50, archetype_icons);
    roster_screen roster = roster_screen(&window_background, &box_background, 160, 240, 460, 600, 1, 40, 40, 40, 80, &all_heros, 30, 60, 30, 38, archetype_icons);
    event_infoscreen event_window = event_infoscreen(&window_background, &box_background, &roster, direction::left, 160, 240, 1000, 600, 1, 40, 40, 0,0, &all_heros, 40, 40, 1000-40, 600-240-80, archetype_icons);
    //split the event_infoscreen in half for selected?
    texture_holder equipment_subrect (renderer, "equipment.png");
    std::vector<window_UI*> all_windows = {&org_window, &detailed_view, &event_window, &roster};
    //names make them more distiguishable incase I decide to separate
    std::vector<std::unique_ptr<texture_holder>> maps;

    for (tuple_int& dir: to_draw_directions)
    {
        std::string map_name="maps/cplus " + std::to_string(std::get<0>(dir)) + " , " + std::to_string(std::get<1>(dir)) + " .png";
        maps.push_back(std::make_unique<texture_holder>(renderer, map_name));
    }
    auto characters_icon = std::make_unique<icon>(renderer, "icons/visored-helm.png",
                                                characters_draw_loc, subwindow_reason::hero_overview);
    auto crafting_icon = std::make_unique<icon>(renderer, "icons/anvil-impact.png",
                                                crafting_draw_loc, subwindow_reason::crafting);
    //crafting, materials, equipment, all one screen
    auto research_icon = std::make_unique<icon>(renderer, "icons/enlightenment.png",
                                                research_draw_loc, subwindow_reason::research);
    icon_holder.push_back(std::move(characters_icon));
    icon_holder.push_back(std::move(crafting_icon));
    icon_holder.push_back(std::move(research_icon));
    SDL_Event e;
    int mouse_x, mouse_y;
    int map_height=512;
    int map_width=512;
    
    //can query texture instead to declare these
    SDL_Rect minimap {980, 0, 300, 300};
    SDL_Rect bottom_bar {800, 680, 480, 120};
    float mini_size = float(300)/float(1536);
    int mini_equiv_w=window_width*mini_size;
    int mini_equiv_h=window_height*mini_size;
    bool in_mini_click=false;
    int mini_equiv_x=camera_x*mini_size+980;
    int mini_equiv_y=camera_y*mini_size;
    SDL_Rect current_screen = SDL_Rect{mini_equiv_x, mini_equiv_y, mini_equiv_w, mini_equiv_h};
    
    int drag_x;
    int drag_y;
    bool dragging=false;
    int pre_drag_cam_x;
    int pre_drag_cam_y;
    
    while (!quit && (current_stage==stage::game))
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
                close();
                break;
            }
            switch (e.type)
            {
                case SDL_MOUSEBUTTONDOWN:
                    switch (e.button.button)
                {
                    case SDL_BUTTON_LEFT:
                        SDL_GetMouseState(&mouse_x, &mouse_y);
                        left_mouse_click(mouse_x, mouse_y, active_events, all_windows,
                                         minimap, current_screen, mini_size, in_mini_click,
                                         dragging, drag_x, drag_y, pre_drag_cam_x, pre_drag_cam_y, icon_holder);
                        break;
                }
                    break;
                case SDL_MOUSEMOTION:
                    SDL_GetMouseState(&mouse_x, &mouse_y);
                    if (in_mini_click && in_rect_bounds(mouse_x, mouse_y, minimap) && dragging)
                    {
                        int change_x = float(drag_x-mouse_x)/mini_size;
                        int change_y = float(drag_y-mouse_y)/mini_size;
                        camera_x=pre_drag_cam_x-change_x;
                        camera_y=pre_drag_cam_y-change_y;
                    }
                    break;
                case SDL_MOUSEBUTTONUP:
                    in_mini_click=false;
                    dragging=false;
                    break;
                case SDL_KEYDOWN:
                {
                    switch (e.key.keysym.sym)
                    {
                        case SDLK_UP:
                            camera_y -=15;
                            break;
                        case SDLK_DOWN:
                            camera_y +=15;
                            break;
                        case SDLK_LEFT:
                            camera_x-=15;
                            break;
                        case SDLK_RIGHT:
                            camera_x+=15;
                            break;
                    }
                }
            }
        }
        ensure_bounds(camera_x, camera_y, 0, 0, (3*map_width-window_width), (3* map_height-window_height));
        camera_view = SDL_Rect {camera_x-window_width/2, camera_y-window_height/2, window_width, window_height};
        //141, 217, 180, 230
        if (!quit)
        {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            draw_everything(maps, map_width, map_height, active_events, minimap, mini_size, mini_equiv_x, mini_equiv_y, mini_equiv_w, mini_equiv_h, current_screen, bottom_bar, paused, all_windows, center, corner, line, equipment_subrect, provinces, icon_holder);
            SDL_RenderPresent(renderer);
            SDL_Delay(10);
        }
        now = std::chrono::system_clock::now();
        if (std::chrono::duration<double, std::milli>(now-last_update).count()>time_to_next && (!paused))
        {
            last_update=now;
            update_time(active_events, provinces);
        }
    }
}

void start_loop(bool& quit)
{
    SDL_Event e;
    int mouse_x, mouse_y;
    while (!quit && (current_stage==stage::initial))
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
                close();
                break;
            }
            switch (e.type)
            {
                case SDL_MOUSEBUTTONDOWN:
                    switch (e.button.button)
                {
                    case SDL_BUTTON_LEFT:
                        SDL_GetMouseState(&mouse_x, &mouse_y);
                        break;
                }
            }
        }
        if (!quit)
        {
            SDL_SetRenderDrawColor(renderer, 218, 224, 179, 255);
            SDL_RenderClear(renderer);
            SDL_RenderPresent(renderer);
            SDL_Delay(10);
        }
    }
}
bool init(int& width, int& height)
{
    bool success = true;
    if (SDL_Init(SDL_INIT_VIDEO)<0)
    {
        success = false;
        std::cout << "Error Code:" << SDL_GetError() << "\n";
    }
    else {
        TTF_Init();
        general_font = TTF_OpenFont("Dosis-Light.ttf", 14);
        title_font = TTF_OpenFont("Dosis-SemiBold.ttf", 30);
        stress_font = TTF_OpenFont("Dosis-SemiBold.ttf", 20);
        Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
        window = SDL_CreateWindow("Tactics Clone", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
        if (window == NULL)
        {
            success = false;
            std::cout << "Error Code:" << SDL_GetError() << "\n";
        }
        else
        {
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
            if (renderer == NULL)
            {
                success = false;
                std::cout << "Error Code:" << SDL_GetError() << "\n";
            }
            else
            {
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                int img_flags = IMG_INIT_PNG;
                if (!(IMG_Init(img_flags) & img_flags))
                {
                    success = false;
                    std::cout << "Error Code(sdl_image):" << IMG_GetError() << "\n";
                }
            }
        }
    }
    return success;
}

std::vector<SDL_Rect> get_sub_rects (int size, std::vector<int> & number_of_sprites)
{
    std::vector<SDL_Rect> sprite_rects;
    for (int i=0; i<number_of_sprites.size();i++)
    {
        for (int j=0; j<number_of_sprites[i]; j++)
        {
            SDL_Rect subrect {i*size, j*size, size, size};
            sprite_rects.push_back(subrect);
        }
    }
    return sprite_rects;
}
std::vector<int> number_of_sprites = {7, 8, 4, 9,4,10,10,10,10,10,3,10};

void combine_starts_and_ends
    (polygon_column_index_map& into,
     polygon_column_index_map& to_add)
{
    for (auto & value_and_row : to_add)
    {
        float value = value_and_row.first;
        for (auto & row_and_indices: value_and_row.second)
        {
            int row = row_and_indices.first;
            into[value][row] = row_and_indices.second;
        }
    }
}

//look through polygon_column_index_map to choose a city
bool acceptable_placement(polygon_column_index_map & into_map, std::vector<std::vector<float>> & noise_map, tuple_int coord, int radius)
{
    for (auto & province : into_map)
    {
        
    }
    return true;
}

polygon_column_index_map starts_and_ends_cell_value (std::vector<float>& row, int row_number)
{
	polygon_column_index_map to_return;
	int start_index = 0;
	//use the bool and the float to avoid having to initialize the float to a value like -9999 and set it back everytime a chain ends
	float current_value = row[0];
	for (int i = 0; i < row.size(); i++)
	{
		float value = row[i];
		if (value != current_value)
		{
			to_return[current_value][row_number].push_back(start_index);
			to_return[current_value][row_number].push_back(i);
			start_index = i;
			current_value = value;
		}
	}
	//for the final ones
	to_return[current_value][row_number].push_back(start_index);
	to_return[current_value][row_number].push_back(row.size());
	return to_return;
}
//sorts the column numbers and places them in a vector, as opposed to having to go through
//all of them using the map. can later do the whole index thingy;
polygon_row_index_map starts_and_ends_column_value (polygon_column_index_map & to_process)
{
    polygon_row_index_map to_return;
    for (auto & value_and_columns : to_process)
    {
        float value = value_and_columns.first;
        std::vector<int> column_indices;
        for (auto & column_and_row_indices: value_and_columns.second)
        {
            int column_number = column_and_row_indices.first;
            column_indices.push_back(column_number);
        }
        std::sort(column_indices.begin(), column_indices.end());
        to_return[value]=column_indices;
        
    }
    return to_return;
}
//hopefully won't have to create a noise_creator for each
float* noise_creation_wrapper(FastNoiseSIMD* noise_creator, int xstart, int ystart, int xend, int yend)
{
    return noise_creator->GetCellularSet(0, xstart, ystart, 1, xend, yend);
}

int main(int argc, const char * argv[]) {
    if (!init(window_width, window_height))
    {
        std::cout << "Failed to start \n";
    }


    
    int hard_conc = 1.5 * std::thread::hardware_concurrency();
    if (hard_conc==0)
    {
        hard_conc=3;
    }
    ThreadPool pool(hard_conc);

    std::vector<province> provinces; //Gotta initalize ~\_(?)_/~
#ifdef USE_PROVINCE
    FastNoiseSIMD* noiseynoise = FastNoiseSIMD::NewFastNoiseSIMD();
    noiseynoise->SetSeed(98435);
	noiseynoise->SetPerturbType(FastNoiseSIMD::GradientFractal);
    noiseynoise->SetPerturbAmp(20);//20 - 10
    noiseynoise->SetCellularJitter(.3);
    noiseynoise->SetPerturbFractalOctaves(12); //12 - 16
    noiseynoise->SetPerturbFractalGain(.45); //.55 -.6
    noiseynoise->SetPerturbFractalLacunarity(2); //2 - 2.5
    noiseynoise->SetPerturbFrequency(.01); //.01 - .005
    noiseynoise->SetCellularNoiseLookupType(FastNoiseSIMD::SimplexFractal);
    noiseynoise->SetCellularDistanceFunction(FastNoiseSIMD::Natural);
    noiseynoise->SetFrequency(.004); //.004
    noiseynoise->SetCellularReturnType(FastNoiseSIMD::CellValue); //Add
    //evens are starts, odds are ends
    std::unordered_map<float, std::unordered_map<int, int>> value_row_starts_and_ends;
    int cell_noise_x_size = 512;
    int cell_noise_y_size = 512;
    float* noise_set = noiseynoise->GetCellularSet(0, 0, 0, 1, world_width, world_height);
//    std::vector<std::future<float*>> noise_sets_worktasks;
//    std::vector<float*> noise_sets;
//    FastNoise my_noise;
//    my_noise.SetNoiseType(FastNoise::Cellular);
//    my_noise.SetSeed(98435);
//    my_noise.SetGradientPerturbAmp(2);
    //2 dumb this library is
//    noise_sets_worktasks.push_back(pool.enqueue(noise_creation_wrapper, noiseynoise, 0, 0,
//                                                1024, 1024));
//    noise_sets_worktasks.push_back(pool.enqueue(noise_creation_wrapper, noiseynoise, 512, 512,
//                                                1024, 1024));
//    noise_sets_worktasks.push_back(pool.enqueue(noise_creation_wrapper, noiseynoise, 0, 0,
//                                                512, 512));
//    noise_sets_worktasks.push_back(pool.enqueue(noise_creation_wrapper, noiseynoise, 512, 0,
//                                                1024, 512));
//    noise_sets_worktasks.push_back(pool.enqueue(noise_creation_wrapper, noiseynoise, 1024, 1024,
//                                                1536, 1536));
//    for (int i=0; i< world_width/cell_noise_x_size; i++)
//    {
//        for (int j=0; j< world_height/cell_noise_y_size; j++)
//        {
////            FastNoiseSIMD* new_noise = FastNoiseSIMD::NewFastNoiseSIMD();
////            new_noise->SetSeed(98435);
////            new_noise->SetPerturbType(FastNoiseSIMD::GradientFractal);
////            new_noise->SetPerturbAmp(20);//20 - 10
////            new_noise->SetCellularJitter(.3);
////            new_noise->SetPerturbFractalOctaves(12); //12 - 16
////            new_noise->SetPerturbFractalGain(.45); //.55 -.6
////            new_noise->SetPerturbFractalLacunarity(2); //2 - 2.5
////            new_noise->SetPerturbFrequency(.01); //.01 - .005
////            new_noise->SetCellularNoiseLookupType(FastNoiseSIMD::SimplexFractal);
////            new_noise->SetCellularDistanceFunction(FastNoiseSIMD::Natural);
////            new_noise->SetFrequency(.004); //.004
////            new_noise->SetCellularReturnType(FastNoiseSIMD::CellValue); //Add
////            new_noise->SetSIMDLevel(3);
//
//        }
//    }
//    for (auto& task : noise_sets_worktasks)
//    {
//        noise_sets.push_back(task.get());
//    }
    float max = *std::max_element(noise_set, noise_set+(world_width*world_height));
    float min = *std::min_element(noise_set, noise_set+(world_width*world_height));
    std::cout<<max <<"\n"<< min<<"\n";
    
    std::vector<std::future<polygon_column_index_map>> all_the_polygon_column_index_maps_worktasks;
    std::vector<polygon_column_index_map> all_the_polygon_column_index_maps;
    
    std::vector<std::vector<float>> noise_vec;
    noise_vec.reserve(world_height); //no reallocation
    for (int j=0; j<world_height; j++)
    {
        std::vector<float> row_of_noise(noise_set+(world_width*j), noise_set+(world_width*(j+1)));
        noise_vec.push_back(row_of_noise);
        all_the_polygon_column_index_maps_worktasks.push_back(pool.enqueue(starts_and_ends_cell_value, std::ref(noise_vec[j]), j));
    }
    
    for (auto & polygon_finisher : all_the_polygon_column_index_maps_worktasks)
    {
        all_the_polygon_column_index_maps.push_back(polygon_finisher.get());
    }
    //SEPARATE THIS FUNCTION
    int increment=2;
    std::vector<std::future<void>> adding_them_up_threads;
    while (increment <= 2*world_height)
    {
        int overflow = world_height%increment;
        for (int i=0; i<world_height; i+=increment)
        {
            auto & into = all_the_polygon_column_index_maps[i];
            if (overflow != 0 && i==0)
            {
                auto remainder = all_the_polygon_column_index_maps[(all_the_polygon_column_index_maps.size()-overflow)];
                combine_starts_and_ends(into, remainder);
            }
            int index_to_add = i+increment/2;
            auto & to_add = all_the_polygon_column_index_maps[index_to_add];
            adding_them_up_threads.push_back(pool.enqueue(combine_starts_and_ends, std::ref(into), to_add));
        }
        for (auto & finisher : adding_them_up_threads)
        {
            finisher.get();
        }
        increment*=2;
        adding_them_up_threads.clear();
    }
    auto final_polygon_column_index_map = all_the_polygon_column_index_maps[0];
    polygon_row_index_map final_polygon_row_index_map = starts_and_ends_column_value(final_polygon_column_index_map);

    write_polygon_index_map(final_polygon_column_index_map);
    //if province is too small absorb it into surroundings or make it an important city
    for (auto & pair : final_polygon_column_index_map)
    {
        float current_value = pair.first;
        province new_prov = province(final_polygon_column_index_map, final_polygon_row_index_map, current_value);
        provinces.push_back(new_prov);
    }
    
    for (auto & province : provinces)
    {
        province.load_province_for_drawing(renderer);
    }
    
    checkthis(noise_vec, world_width, world_height, min, max);
#endif
    std::vector<Vector2<float>> points;
    while (points.size()<25)
    {
        points.push_back(Vector2<float>(rand_x(eng), rand_y(eng)));
    }
    
    Delaunay<float> triangulation;
    const std::vector<Triangle<float> > triangles = triangulation.triangulate(points);
    std::cout << triangles.size() << " triangles generated\n";
    const std::vector<Edge<float> > edges = triangulation.getEdges();
    bool quit = false;
    while (quit == false)
    {
        switch (current_stage)
        {
            case stage::game:
                game_loop(quit, provinces);
                break;
            default:
                start_loop(quit);
        }
    }
	return 0;
}
