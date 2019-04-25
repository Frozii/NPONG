#ifndef NPONG_H
#define NPONG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ncurses.h>

enum
{
  black_pair,
  red_pair,
  green_pair,
  yellow_pair,
  blue_pair,
  magenta_pair,
  cyan_pair,
  white_pair,

  cyan_background_pair
};

enum
{
  gamestate_main_menu,
  gamestate_play,
  gamestate_help,
};

typedef struct
{
  int x;
  int y;
  int width;
  int height;
  int score;
  int can_add_speed;
} player_t;

typedef struct
{
  int flag_x;
  int flag_y;
  char symbol;
  float x;
  float y;
  float vx;
  float vy;
} ball_t;

extern int running;
extern int gamestate;

// window variables
extern int game_win_start_x;
extern int game_win_start_y;
extern int game_win_width;
extern int game_win_height;

extern int score_win_width;
extern int score_win_height;

// menu variables
extern int key_pressed;
extern int menu_choice;
extern int highlighted;
extern int menu_item_amount;
extern char *menu_items[];

player_t* create_player(int x, int y, int width, int height);
ball_t* create_ball(char symbol, int x, int y, float vx, float vy);
int update_ball(ball_t *ball, player_t *player_one, player_t *player_two);
int update_main_menu(WINDOW *game_win);
void init_renderer_and_options();
void set_window_options(WINDOW *game_win, WINDOW *score_win);
void clean_up(WINDOW *game_win, WINDOW *score_win, ball_t *ball, player_t *player_one, player_t *player_two);
void render_help(WINDOW *game_win);
void update_players(player_t *player_one, player_t *player_two);
void render_victory_screen(WINDOW *game_win, WINDOW *score_win, int winner);
void render_arena(WINDOW *game_win);
void render_players(WINDOW *game_win, player_t *player_one, player_t *player_two);
void render_score_window(WINDOW *score_win, int player_one_score, int player_two_score);
void render_ball(WINDOW *game_win, ball_t *ball);
void render_main_menu(WINDOW *game_win);

#endif // NPONG_H
