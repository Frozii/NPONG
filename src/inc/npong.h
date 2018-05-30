#ifndef NPONG_H
#define NPONG_H

typedef struct {
  int x;
  int y;
  int width;
  int height;
  int score;
} player_t;

typedef struct {
  int flag_x;
  int flag_y;
  char symbol;
  float x;
  float y;
  float vx;
  float vy;
} ball_t;

/*
* Gamestates:
* 0 = Menu
* 1 = Play
* 2 = Help
* 3 = Quit
*/

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

player_t* create_player(int x, int y, int width, int height, int score);
ball_t* create_ball(char symbol, int x, int y, float vx, float vy);
int update_and_draw_ball(WINDOW *game_win, ball_t *ball, player_t *player_one, player_t *player_two);
int draw_and_update_menu(WINDOW *game_win);
void clean_up(WINDOW *game_win, WINDOW *score_win, player_t *p_one, player_t *p_two, ball_t *b);
void draw_help(WINDOW *game_win);
void draw_score_and_window(WINDOW *score_win, int *player_one_score, int *player_two_score);
void update_players(player_t *player_one, player_t *player_two);
void draw_players_and_arena(WINDOW *game_win, player_t *player_one, player_t *player_two);
void set_ncurses_options(WINDOW *game_win, WINDOW *score_win);
void victory_screen(WINDOW *game_win, WINDOW *score_win, player_t *player_one, player_t *player_two, int winner);

#endif // NPONG_H