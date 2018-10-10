#ifndef utilities_hpp
#define utilities_hpp

float distance(int x1, int y1, int x2, int y2);

bool in_bounds(int where_x, int where_y, int lower_bound_x, int lower_bound_y, int upper_bound_x, int upper_bound_y);
tuple_int get_text_size_wrapped(std::string what_to_write, SDL_Renderer* use_renderer,
                                TTF_Font* use_font, int wrap_length = 9000);
void write_text(int start_x, int start_y, std::string what_to_write, SDL_Renderer* use_renderer,
                TTF_Font* use_font, SDL_Color font_color, int wrap_length = 9000);

void write_text_centered(int center_x, int center_y, std::string what_to_write, SDL_Renderer* use_renderer,
                         TTF_Font* use_font, SDL_Color font_color, bool override_y_centering=false);

std::vector<int> remove_negatives(std::vector<int>& pre_removed);

void ensure_bounds(int& to_bind_x, int& to_bind_y, int x1, int y1, int x2, int y2);

inside in_set_bounds(SDL_Rect object, int x_bound_start, int x_bound_end, int y_bound_start, int y_bound_end);

inside in_set_bounds(SDL_Rect object, SDL_Rect bounds);

bool in_bounds(SDL_Rect object, SDL_Rect bounds);

bool in_rect_bounds(int click_x, int click_y, SDL_Rect& of_interest);

#endif /* utilities_hpp */
