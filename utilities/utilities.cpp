//
//  utilities.cpp
//  tacticsclone
//
//  Created by asdfuiop on 7/7/18.
//  Copyright © 2018 asdfuiop. All rights reserved.
//
#include <math.h>
#include <string>
#include <vector>
#ifdef _WIN32
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL.h>
#else
#include <SDL2_image/SDL_image.h>
#include <SDL2/SDL.h>
#include <SDL2_ttf/SDL_ttf.h>
#endif
#include "enums.h"
float distance(int x1, int y1, int x2, int y2)
{
    return powf((x2 - x1), 2) + powf((y2 - y1), 2);
}

bool in_bounds(int where_x, int where_y, int lower_bound_x, int lower_bound_y, int upper_bound_x, int upper_bound_y)
{
    return (where_x > lower_bound_x) && (where_x < upper_bound_x) && (where_y > lower_bound_y) && (where_y < (upper_bound_y));
}


//maybe save these textures?
void write_text(int start_x, int start_y, std::string what_to_write, SDL_Renderer* use_renderer,
                TTF_Font* use_font, SDL_Color font_color, int wrap_length = 9000)
{
    SDL_Surface* text_surface = TTF_RenderText_Blended_Wrapped(use_font, what_to_write.c_str(), font_color, wrap_length);
    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(use_renderer, text_surface);
    int text_width = text_surface->w;
    int text_height = text_surface->h;
    SDL_Rect text_rect{ start_x, start_y, text_width, text_height };
    SDL_RenderCopy(use_renderer, text_texture, NULL, &text_rect);
    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);
}

void write_text_centered(int center_x, int center_y, std::string what_to_write, SDL_Renderer* use_renderer,
                         TTF_Font* use_font, SDL_Color font_color, bool override_y_centering=false)
{
    SDL_Surface* text_surface = TTF_RenderText_Blended(use_font, what_to_write.c_str(), font_color);
    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(use_renderer, text_surface);
    int text_width;
    int text_height;
    TTF_SizeText(use_font, what_to_write.c_str(), &text_width, &text_height);
    int start_x=center_x-text_width/2;
    int start_y=center_y-text_height/2;
    if (override_y_centering)
    {
        start_y=center_y;
    }
    SDL_Rect text_rect{ start_x, start_y, text_width, text_height };
    SDL_RenderCopy(use_renderer, text_texture, NULL, &text_rect);
    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);
}

std::vector<int> remove_negatives(std::vector<int>& pre_removed)
{
    std::vector<int> to_return;
    for (int hp : pre_removed)
    {
        if (hp > 0)
        {
            to_return.push_back(hp);
        }
    }
    return (to_return);
}
void ensure_bounds(int& to_bind_x, int& to_bind_y, int x1, int y1, int x2, int y2)
{
    if (to_bind_x<x1)
    {
        to_bind_x=x1;
    }
    else if(to_bind_x>x2)
    {
        to_bind_x=x2;
    }
    if (to_bind_y<y1)
    {
        to_bind_y=y1;
    }
    else if (to_bind_y>y2)
    {
        to_bind_y=y2;
    }
}
inside in_set_bounds(SDL_Rect object, int x_bound_start, int x_bound_end, int y_bound_start, int y_bound_end)
{
    bool start_x = (object.x>x_bound_start);
    bool end_x = (object.x+object.w)<x_bound_end;
    bool start_y = (object.y>y_bound_start);
    bool end_y = (object.y+object.h)<y_bound_end;
    bool overlap_x = (object.x<x_bound_end || object.x+object.w >x_bound_start);
    bool overlap_y = (object.y<y_bound_end || object.y+object.h > y_bound_start);
    if ((start_x+end_x+start_y+end_y)==4)
    {
        return inside::yes;
    }
    else if (overlap_x && overlap_y)
    {
        return inside::partial;
    }
    else
    {
        return inside::no;
    }
}

inside in_set_bounds(SDL_Rect object, SDL_Rect bounds)
{
    bool start_x = (object.x>bounds.x);
    bool end_x = (object.x+object.w)<(bounds.x+bounds.w);
    bool start_y = (object.y>bounds.y);
    bool end_y = (object.y+object.h)< (bounds.y+bounds.h);
    bool overlap_x = (object.x< (bounds.x+bounds.w) || object.x+object.w > bounds.x);
    bool overlap_y = (object.y< (bounds.y+bounds.h) || object.y+object.h > bounds.y);
    if ((start_x+end_x+start_y+end_y)==4)
    {
        return inside::yes;
    }
    else if (overlap_x && overlap_y)
    {
        return inside::partial;
    }
    else
    {
        return inside::no;
    }
}

bool in_bounds(SDL_Rect object, SDL_Rect bounds)
{
    if ((object.x< (bounds.x+bounds.w) || object.x+object.w > bounds.x) &&
        (object.y< (bounds.y+bounds.h) || object.y+object.h > bounds.y))
    {
        return true;
    }
    else
    {
        return false;
    }
}
bool in_rect_bounds(int click_x, int click_y, SDL_Rect& of_interest)
{
    return (click_x>of_interest.x && click_x<of_interest.x+of_interest.w && click_y>of_interest.y && click_y<of_interest.y+of_interest.h);
}



