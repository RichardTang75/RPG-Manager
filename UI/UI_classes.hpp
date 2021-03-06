//
//  UI_classes.hpp
//  tacticsclone
//
//  Created by asdfuiop on 6/27/18.
//  Copyright © 2018 asdfuiop. All rights reserved.
//

#ifndef UI_classes_hpp
#define UI_classes_hpp

#ifdef _WIN32
#include <SDL_image.h>
#include <SDL.h>
#else
#include <SDL2_image/SDL_image.h>
#include <SDL2/SDL.h>
#endif
#include "utilities.hpp"

struct texture_holder
{
public:
    texture_holder(SDL_Renderer* to_use, std::string path)
    {
        SDL_Surface* loading = IMG_Load(path.c_str());
        held_texture = SDL_CreateTextureFromSurface(to_use, loading);
        SDL_FreeSurface(loading);
        SDL_SetTextureBlendMode(held_texture, SDL_BLENDMODE_BLEND);
        assert(held_texture != 0);
    }
    texture_holder(SDL_Renderer* to_use, std::string path, SDL_Rect destination)
    {
        SDL_Surface* loading = IMG_Load(path.c_str());
        held_texture = SDL_CreateTextureFromSurface(to_use, loading);
        SDL_FreeSurface(loading);
        where_draw=destination;
        SDL_SetTextureBlendMode(held_texture, SDL_BLENDMODE_BLEND);
        assert(held_texture != 0);
    }
    texture_holder()
    {
        //never use this
    }
    ~texture_holder()
    {
        SDL_DestroyTexture(held_texture);
    }
    void change_alpha(SDL_Renderer* to_use, int alpha_value)
    {
        SDL_SetTextureAlphaMod(held_texture, alpha_value);
        Uint8 current_alpha_value;
        SDL_GetTextureAlphaMod(held_texture, &current_alpha_value);
        assert(current_alpha_value==alpha_value);
    }
    void draw_this_slice(SDL_Renderer* to_use, SDL_Rect draw_here)
    {
        assert(held_texture != 0);
        SDL_RenderCopy(to_use, held_texture, &draw_here, &draw_here);
    }
    void draw_this(SDL_Renderer* to_use, SDL_Rect draw_here)
    {
        assert(held_texture != 0);
        SDL_RenderCopy(to_use, held_texture, NULL, &draw_here);
    }
    void draw_this(SDL_Renderer* to_use)
    {
        assert(held_texture != 0);
        //should check that the rect exists. not sure if this throws an exception or not
        SDL_RenderCopy(to_use, held_texture, NULL, &where_draw);
    }
    void draw_this_rotate(SDL_Renderer* to_use, SDL_Rect draw_here, int angle, SDL_RendererFlip flip = SDL_FLIP_NONE)
    {
        assert(held_texture != 0);
        //old center==new center

        if (angle==90 || angle==-90)
        {
            int half_w = draw_here.w/2;
            int half_h=draw_here.h/2;
            int desired_center_x = draw_here.x+half_w;
            int desired_center_y = draw_here.y+half_h;
            SDL_Rect use_rect;
            use_rect.x=desired_center_x-half_h;
            use_rect.y=desired_center_y-half_w;
            use_rect.w=draw_here.h;
            use_rect.h=draw_here.w;
            SDL_RenderCopyEx(to_use, held_texture, NULL, &use_rect, angle, NULL, flip);
        }
        else
        {
            SDL_RenderCopyEx(to_use, held_texture, NULL, &draw_here, angle, NULL, flip);
        }

        //turns 60, 110, 300, 50 to 185, -15, 50, 300
        // 60+300/2 - 50/2, 110
//        int actual_center_x =
    }
    SDL_Texture* get_texture()
    {
        return held_texture;
    }
private:
    SDL_Texture* held_texture;
    SDL_Rect where_draw;
};

class icon : public texture_holder
{
public:
    icon(SDL_Renderer* to_use, std::string path, SDL_Rect destination, subwindow_reason why) : texture_holder (to_use, path, destination)
    {
        start_x = destination.x;
        end_x=destination.x+destination.w;
        start_y=destination.y;
        end_y=destination.y+destination.h;
        assoc_subwindow=why;
    }
    bool in_bounds(int click_x, int click_y)
    {
        return (start_x < click_x && start_y < click_y && end_x > click_x && end_y > click_y);
    };
    subwindow_reason assoc_subwindow;
private:
    std::string hover_text;
    int start_x, end_x, start_y, end_y;
    //deal with hover text, click events
};


class button
{
public:
    button(texture_holder* in_image, int in_start_x, int in_start_y, int in_width, int in_height, std::string in_text,
           use in_purpose, int in_text_offset_x = 5, int in_text_offset_y = 10)
    {
        std::tie(width, height, position_x, position_y, text, purpose) =
        std::tie(in_width, in_height, in_start_x, in_start_y, in_text, in_purpose);
        std::tie(text_offset_x, text_offset_y) = std::tie(in_text_offset_x, in_text_offset_y);
        button_img = in_image;
    };
    void on_click();
    bool in_bounds(int click_x, int click_y)
    {
        return (position_x < click_x && position_y < click_y && position_x + width>click_x && position_y + height>click_y);
    };
    void draw(SDL_Renderer* to_use, TTF_Font* font_use, SDL_Color font_color = SDL_Color{ 0, 0 ,0 , 255 },
              int out_r = 0, int out_g = 0, int out_b = 0,
              int override_x = -1, int override_y=-1)
    {
        int draw_x = position_x;
        int draw_y = position_y;
        if (override_x != -1)
        {
            draw_x = draw_x - override_x;
        }
        if (override_y != -1)
        {
            draw_y = draw_y - override_y;
        }
        SDL_Rect image_rect {0,0,width,height};
        SDL_Rect to_draw_rect{ draw_x, draw_y, width, height};
        SDL_Texture* button_draw = button_img->get_texture();
        SDL_RenderCopy(to_use, button_draw, &image_rect, &to_draw_rect);
        SDL_SetRenderDrawColor(to_use, out_r, out_b, out_g, 0xFF);
        SDL_RenderDrawRect(to_use, &to_draw_rect);
        write_text(draw_x + text_offset_x, draw_y + text_offset_y, text, to_use, font_use, font_color);
    };
private:
    int width;
    int height;
    int position_x;
    int position_y;
    use purpose;
    std::string text;
    int text_offset_x = 5;
    int text_offset_y = 10;
    texture_holder* button_img;
};

struct scroll_helper
{
    int current_scroll;
    int min_scroll;
    int max_scroll;
};

struct event_message
{
    bool operator==(const event_message& other) const
    {
        return (message == other.message && zoom_to == other.zoom_to && priority==other.priority && time == other.time);
    }
    std::string message;
    coordinate zoom_to;
    int priority;
    int time;
};

bool compare_event_messages(const event_message& lhs, const event_message& rhs)
{
    return lhs.priority<rhs.priority;
}

void insert_sorted_event_message(std::vector<event_message>& messages, event_message& to_put_in)
{
    auto iterator = std::lower_bound(messages.begin(), messages.end(), to_put_in, compare_event_messages);
    messages.insert(iterator, to_put_in);
}

//alpha fade in and fade out, high priority go on top, last longer. low-6 hrs, normal-12 hrs, high-24 hrs, highest-168
//on delete do full recheck
//on add do basic best
class message_box
{
public:
    message_box(SDL_Rect in_bounds, std::vector<SDL_Color> in_priority_font_colors)
    {
        bounds=in_bounds;
        priority_font_colors= in_priority_font_colors;
    }
    void feed_info_in(SDL_Renderer* to_use, TTF_Font* use_font, std::string& message, int priority, coordinate& location, bool ending)
    {
        int time_message_lasts=0;
        switch (priority)
        {
            case 1:
                time_message_lasts=168;
                break;
            case 2:
                time_message_lasts=24;
                break;
            case 3:
                time_message_lasts=12;
                break;
            case 4:
                time_message_lasts=6;
                break;
        };
        if (ending)
        {
            time_message_lasts/=2;
        }
        event_message add_message = event_message{message, location, priority, time_message_lasts};
        insert_sorted_event_message(active_event_messages, add_message);
        int text_width, text_height;
        std::tie(text_width, text_height) = get_text_size_wrapped(message, to_use, use_font, bounds.w);
        if (text_height > y_increment)
        {
            y_increment = text_height;
        }
    }
    void update(SDL_Renderer* to_use, TTF_Font* use_font)
    {
        std::vector<event_message> messages_to_erase;
        bool message_expire=false;
        for (event_message & current_event_message : active_event_messages)
        {
            current_event_message.time-=1;
            if (current_event_message.time<=0)
            {
                message_expire=true;
                messages_to_erase.push_back(current_event_message);
            }
        }
        for (event_message & to_erase : messages_to_erase)
        {
            auto iterator = std::find(active_event_messages.begin(), active_event_messages.end(), to_erase);
            event_message is_it = *iterator;
            if (iterator != active_event_messages.end())
            {
                active_event_messages.erase(iterator);
            }
        }
        if (message_expire)
        {
            y_increment=0;
            for (event_message & remaining : active_event_messages)
            {
                int text_width, text_height;
                std::tie(text_width, text_height) = get_text_size_wrapped(remaining.message, to_use, use_font, bounds.w);
                if (text_height > y_increment)
                {
                    y_increment = text_height;
                }
            }
        }
    }
    void draw_message_box(SDL_Renderer* to_use, TTF_Font* font_use)
    {
        //std::vector<SDL_Color> priorities
        //use /n?, won't work, priorities
        SDL_SetRenderDrawColor(to_use, 0, 0, 0, 255);
        SDL_RenderDrawRect(to_use, &bounds);
        for (int i=0; i<active_event_messages.size(); i++)
        {
            write_text(bounds.x, bounds.y+y_increment*i, active_event_messages[i].message, to_use, font_use, priority_font_colors[active_event_messages[i].priority-1], bounds.w);
        }
    }
    void handle_click(int mouse_x, int mouse_y, int& camera_x, int& camera_y)
    {
        int relative_x; //don't think this matters
        int relative_y;
        if (in_rect_bounds(mouse_x, mouse_y, bounds))
        {
            relative_x = mouse_x-bounds.x;
            relative_y = mouse_y-bounds.y;
        }
        int index = (relative_y+message_box_scroll_aid.current_scroll)/y_increment;
    }
    void scroll(int change)
    {
        message_box_scroll_aid.current_scroll+=change;
        if (message_box_scroll_aid.current_scroll > message_box_scroll_aid.max_scroll)
        {
            message_box_scroll_aid.current_scroll = message_box_scroll_aid.max_scroll;
        }
        else if (message_box_scroll_aid.current_scroll < message_box_scroll_aid.min_scroll)
        {
            message_box_scroll_aid.current_scroll = message_box_scroll_aid.min_scroll;
        }
    }
private:
    SDL_Rect bounds;
    std::vector<event_message> active_event_messages;
    std::vector<SDL_Color> priority_font_colors;
    std::vector<button> message_interactions; //remove this, write interactions manually
    scroll_helper message_box_scroll_aid;
    int y_increment=0;
};

//lasts for 30-90 days? only show
class event_log
{
    std::vector<event_message> text;
};

#endif /* UI_classes_hpp */
