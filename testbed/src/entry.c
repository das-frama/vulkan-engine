#include "game.h"

#include <entry.h>
#include <platform/platform.h>

// Define the funciton to create a game.
b8 create_game(game* out_game) {
    // Application config.
    out_game->app_config.start_pos_x  = 100;
    out_game->app_config.start_pos_y  = 100;
    out_game->app_config.start_width  = 1920;
    out_game->app_config.start_height = 1080;
    out_game->app_config.name = "Vulkan Game Engine";
    out_game->update = game_update;
    out_game->render = game_render;
    out_game->init = game_init;
    out_game->on_resize = game_on_resize;

    // Create the game state.
    out_game->state = platform_alloc(sizeof(game_state), false);

    return true;
}