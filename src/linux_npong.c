#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ncurses.h>

#define internal static
#define global static

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float r32;
typedef double r64;

typedef u32 b32;

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
  gamestate_controls,
};

typedef struct
{
  i32 x;
  i32 y;
  i32 width;
  i32 height;
  i32 score;
  i32 can_add_speed;
} player_t;

typedef struct
{
  i32 flag_x;
  i32 flag_y;
  char glyph;
  // NOTE(Rami): Turn into i32
  r32 x;
  r32 y;
  r32 vx;
  r32 vy;
} ball_t;

i32 running = 1;
i32 gamestate = gamestate_main_menu;

i32 game_win_start_x = 0;
i32 game_win_start_y = 3;
i32 game_win_width = 60;
i32 game_win_height = 20;

i32 score_win_width = 60;
i32 score_win_height = 3;

i32 key_pressed = 0;
i32 menu_choice;
i32 highlighted = 0;
i32 menu_item_amount = 3;
char *menu_items[] = {"Play", "Controls", "Quit"};

internal void
render_ball(WINDOW *game_win, ball_t *ball)
{
  wattron(game_win, COLOR_PAIR(white_pair));
  mvwprintw(game_win, ball->y, ball->x, "%c", ball->glyph);
  wattroff(game_win, COLOR_PAIR(white_pair));
}

internal void
render_players(WINDOW *game_win, player_t *player_one, player_t *player_two)
{
  wattron(game_win, COLOR_PAIR(white_pair));
  for(i32 y = 0; y < player_one->height; y++) {mvwaddstr(game_win, player_one->y + y, player_one->x, "|");}
  for(i32 y = 0; y < player_two->height; y++) {mvwaddstr(game_win, player_two->y + y, player_two->x, "|");}
  wattroff(game_win, COLOR_PAIR(white_pair));
}

internal i32
update_ball(ball_t *ball, player_t *player_one, player_t *player_two)
{
  /*
   * Ball->flag_x:
   * 0 = left, 1 = right
   *
   * Ball->flag_y:
   * 0 = up, 1 = down
   */
  
  i32 winner = 0;

  // move the ball
  if(ball->flag_x == 0)
  {
    ball->x -= ball->vx;
  }
  else if(ball->flag_x == 1)
  {
    ball->x += ball->vx;
  }

  if(ball->flag_y == 0)
  {
    ball->y -= ball->vy;
  }
  else if(ball->flag_y == 1)
  {
    ball->y += ball->vy;
  }

  // check if the ball is past the screen on the x axis
  if(ball->x < 0)
  {
    player_two->score++;

    if(player_two->score >= 5)
    {
      winner = 2;
    }

    ball->flag_x = rand() % 2;
    ball->flag_y = rand() % 2;
    ball->x = game_win_width / 2;
    ball->y = game_win_height / 2;
    ball->vx = 0.0002;
    ball->vy = 0.0002;
    player_one->can_add_speed = 1;
    player_two->can_add_speed = 1;
  }
  else if(ball->x > game_win_width)
  {
    player_one->score++;

    if(player_one->score >= 5)
    {
      winner = 1;
    }

    ball->flag_x = rand() % 2;
    ball->flag_y = rand() % 2;
    ball->x = game_win_width / 2;
    ball->y = game_win_height / 2;
    ball->vx = 0.0002;
    ball->vy = 0.0002;
    player_one->can_add_speed = 1;
    player_two->can_add_speed = 1;
  }

  // check if the ball is past the screen on the y axis
  if(ball->y < 1)
  {
    ball->flag_y = 1;
  }
  else if(ball->y > game_win_height - 1)
  {
    ball->flag_y = 0;
  }

  // player one to ball collision
  if(ball->x > player_one->x && ball->x < player_one->x + 1 && ball->y > player_one->y - 1 && ball->y < (player_one->y + player_one->height))
  {
    // reverse x
    if(ball->flag_x == 1)
    {
      ball->flag_x = 0;
    }
    else if(ball->flag_x == 0)
    {
      ball->flag_x = 1;
    }

    if(player_one->can_add_speed == 1)
    {
      // add speed to the ball if it's below the max
      if(ball->vx && ball->vy < 0.002)
      {
        ball->vx += 0.0001;
        ball->vy += 0.0001;
      }

      player_one->can_add_speed = 0;
      player_two->can_add_speed = 1;
    }
  }

  // player two to ball collision
  if(ball->x < player_two->x && ball->x > player_two->x - 1 && ball->y > player_two->y - 1 && ball->y < (player_two->y + player_two->height))
  {
    // reverse x
    if(ball->flag_x == 1)
      ball->flag_x = 0;
    else if(ball->flag_x == 0)
      ball->flag_x = 1;

    if(player_two->can_add_speed == 1)
    {
      // add speed to the ball if it's below the max
      if(ball->vx && ball->vy < 0.002)
      {
        ball->vx += 0.0001;
        ball->vy += 0.0001;
      }

      player_one->can_add_speed = 1;
      player_two->can_add_speed = 0;
    }
  }

  return winner;
}

internal void
render_main_menu(WINDOW *game_win)
{
  // calculate the amount to print for the title text
  i32 current_width = getmaxx(game_win);
  i32 amount_to_remove = 0;
  i32 amount_to_print = 44;

  if(current_width < 52)
  {
    amount_to_remove = 52 - current_width;
    amount_to_print = amount_to_print - amount_to_remove;
  }

  // render game title
  wattron(game_win, COLOR_PAIR(white_pair));
  mvwaddnstr(game_win, 1, 8, " _   _   _____   _____   _   _   _____ ", amount_to_print);
  mvwaddnstr(game_win, 2, 8, "/ \\ / \\ /  __ \\ /  _  \\ / \\ / \\ /  __ \\", amount_to_print);
  mvwaddnstr(game_win, 3, 8, "|  \\| | | /_/ | | | | | |  \\  | | |  \\/", amount_to_print);
  mvwaddnstr(game_win, 4, 8, "| . ` | |  __/  | | | | | . ` | | | ___", amount_to_print);
  mvwaddnstr(game_win, 5, 8, "| |\\  | | |     | |_| | | |\\  | | |_| |", amount_to_print);
  mvwaddnstr(game_win, 6, 8, "\\_/ \\_/ \\_/     \\_____/ \\_/ \\_/ \\_____/", amount_to_print);
  wattroff(game_win, COLOR_PAIR(white_pair));

  // render the menu options
  char menu_item[32] = "> ";
  amount_to_print = 6;

  // calculate the amount to print for the menu item text
  if(current_width < 12)
  {
    amount_to_remove = 12 - current_width;
    amount_to_print = amount_to_print - amount_to_remove;
  }

  for(i32 i = 0; i < menu_item_amount; i++)
  {
    if(i == highlighted && highlighted == 0)
    {
      strcpy(menu_item + 2, menu_items[i]);

      wattron(game_win, COLOR_PAIR(blue_pair));
      mvwaddnstr(game_win, 10, 6, menu_item, amount_to_print);
      wattroff(game_win, COLOR_PAIR(blue_pair));

      wattron(game_win, COLOR_PAIR(white_pair));
      mvwaddnstr(game_win, 11, 8, menu_items[1], amount_to_print - 2);

      mvwaddnstr(game_win, 12, 8, menu_items[2], amount_to_print - 2);
      wattroff(game_win, COLOR_PAIR(white_pair));
    }
    else if(i == highlighted && highlighted == 1)
    {
      strcpy(menu_item + 2, menu_items[i]);

      wattron(game_win, COLOR_PAIR(white_pair));
      mvwaddnstr(game_win, 10, 8, menu_items[0], amount_to_print - 2);
      wattroff(game_win, COLOR_PAIR(white_pair));

      wattron(game_win, COLOR_PAIR(blue_pair));
      mvwaddnstr(game_win, 11, 6, menu_item, amount_to_print);
      wattroff(game_win, COLOR_PAIR(blue_pair));

      wattron(game_win, COLOR_PAIR(white_pair));
      mvwaddnstr(game_win, 12, 8, menu_items[2], amount_to_print - 2);
      wattroff(game_win, COLOR_PAIR(white_pair));
    }
    else if(i == highlighted && highlighted == 2)
    {
      strcpy(menu_item + 2, menu_items[i]);

      wattron(game_win, COLOR_PAIR(white_pair));
      mvwaddnstr(game_win, 10, 8, menu_items[0], amount_to_print - 2);

      mvwaddnstr(game_win, 11, 8, menu_items[1], amount_to_print - 2);
      wattroff(game_win, COLOR_PAIR(white_pair));

      wattron(game_win, COLOR_PAIR(blue_pair));
      mvwaddnstr(game_win, 12, 6, menu_item, amount_to_print);
      wattroff(game_win, COLOR_PAIR(blue_pair));
    }
  }
}

internal i32
update_main_menu(WINDOW *game_win)
{
  // record user movement between menu items
  menu_choice = wgetch(game_win);
  switch(menu_choice)
  {
    case KEY_UP:
    {
      if(highlighted > 0)
      {
        (highlighted)--;
      }
    } break;

    case KEY_DOWN:
    {
      if(highlighted < 2)
      {
        (highlighted)++;
      }
    } break;

    default:
    {

    } break;
  }

  // return 1 if choice was "Play"
  if(menu_choice == 10 && highlighted == 0)
  {
    return 1;
  }

  // return 2 if choice was "Controls"
  else if(menu_choice == 10 && highlighted == 1)
  {
    return 2;
  }

  // return 3 if choice was "Quit"
  else if(menu_choice == 10 && highlighted == 2)
  {
    return 3;
  }

  return 4;
}

internal void
render_controls(WINDOW *game_win)
{
  werase(game_win);

  wattron(game_win, COLOR_PAIR(white_pair));
  mvwaddstr(game_win, 1, 8, " _   _   _____   _       _____");
  mvwaddstr(game_win, 2, 8, "| | | | |  ___| | |     /  __ \\");
  mvwaddstr(game_win, 3, 8, "| |_| | | |__   | |     | |_/ /");
  mvwaddstr(game_win, 4, 8, "|  _  | |  __|  | |     |  __/");
  mvwaddstr(game_win, 5, 8, "| | | | | |___  | |____ | |");
  mvwaddstr(game_win, 6, 8, "\\_| |_/ \\____/  \\_____/ \\_/");

  mvwaddstr(game_win, 10, 8, "NPONG is a Pong clone made using ncurses");
  mvwaddstr(game_win, 12, 8, "Player 1: up and down using \"w\" and \"s\"");
  mvwaddstr(game_win, 13, 8, "Player 2: up and down using arrow keys");
  mvwaddstr(game_win, 15, 8, "You can press 'q' to exit the game if needed ingame");
  mvwaddstr(game_win, 17, 8, "First player to score 5 points wins");
  wattroff(game_win, COLOR_PAIR(white_pair));

  wgetch(game_win);
}

internal void
render_score_window(WINDOW *score_win, i32 player_one_score, i32 player_two_score)
{
  wattron(score_win, COLOR_PAIR(white_pair));
  wresize(score_win, score_win_height, score_win_width);
  box(score_win, 0, 0);

  mvwprintw(score_win, 1, 1, "Player 1: %d", player_one_score);
  mvwprintw(score_win, 1, 48, "Player 2: %d", player_two_score);

  wattroff(score_win, COLOR_PAIR(white_pair));
}

internal void
update_players(player_t *player_one, player_t *player_two)
{
  switch(key_pressed)
  {
    case 'q':
    {
      running = 0;
    } break;

    case 'w':
    {
      if(player_one->y > 1)
      {
        player_one->y--;
      }
    } break;

    case 's':
    {
      if((player_one->y + player_one->height) < (game_win_height - 1))
      {
        player_one->y++;
      }
    } break;

    case KEY_UP:
    {
      if(player_two->y > 1)
      {
        player_two->y--;
      }
    } break;

    case KEY_DOWN:
    {
      if((player_two->y + player_two->height) < (game_win_height - 1))
      {
        player_two->y++;
      }
    } break;
  }
}

internal void
render_arena(WINDOW *game_win)
{
  // corners
  wattron(game_win, COLOR_PAIR(white_pair));
  mvwaddstr(game_win, 0, 0, "+");
  mvwaddstr(game_win, 0, game_win_width - 1, "+");
  mvwaddstr(game_win, game_win_height - 1, 0, "+");
  mvwaddstr(game_win, game_win_height - 1, game_win_width - 1, "+");

  // left / right border
  for(i32 y = 1; y < game_win_height - 1; y++)
  {
    mvwaddstr(game_win, y, 0, "|");
    mvwaddstr(game_win, y, game_win_width - 1, "|");
  }

  // top / bottom border
  for(i32 x = 1; x < game_win_width - 1; x++)
  {
    mvwaddstr(game_win, 0, x, "-");
    mvwaddstr(game_win, game_win_height - 1, x, "-");
  }

  wattroff(game_win, COLOR_PAIR(white_pair));
}

internal player_t*
create_player(i32 x, i32 y, i32 width, i32 height)
{
  player_t *p = malloc(sizeof(player_t));

  p->x = x;
  p->y = y;
  p->width = width;
  p->height = height;
  p->score = 0;
  p->can_add_speed = 1;

  return p;
}

internal ball_t*
create_ball(char glyph, i32 x, i32 y, r32 vx, r32 vy)
{
  ball_t *b = malloc(sizeof(ball_t));

  b->flag_x = rand() % 2;
  b->flag_y = rand() % 2;
  b->glyph = glyph;
  b->x = x;
  b->y = y;
  b->vx = vx;
  b->vy = vy;

  return b;
}

internal void
render_victory_screen(WINDOW *game_win, WINDOW *score_win, i32 winner)
{
  nodelay(game_win, 0);

  werase(game_win);
  werase(score_win);

  wattron(game_win, COLOR_PAIR(white_pair));
  mvwprintw(game_win, 5, game_win_width / 2, "Player %d wins!", winner);
  mvwaddstr(game_win, 10, 26, "[Q] for main menu");
  mvwaddstr(game_win, 11, 26, "[R] for a rematch");

  for(i32 y = 3; y < 8; y++)
  {
    mvwaddstr(game_win, y, (game_win_width / 2) - 4, "|");
    mvwaddstr(game_win, y, (game_win_width / 2) + 17, "|");
  }

  for(i32 x = 26; x < 48; x++)
  {
    mvwaddstr(game_win, 2, x, "-");
    mvwaddstr(game_win, 8, x, "-");
  }

  wattroff(game_win, COLOR_PAIR(white_pair));
  wrefresh(game_win);
  wrefresh(score_win);

  while(1)
  {
    i32 input = wgetch(game_win);

    if(input == 'Q' || input == 'q')
    {
      gamestate = gamestate_main_menu;
      break;
    }
    else if(input == 'R' || input == 'r')
    {
      nodelay(game_win, 1);
      break;
    }
  }
}

internal void
exit_game(player_t *player_one, player_t *player_two, ball_t *ball)
{
  free(player_one);
  player_one = NULL;

  free(player_two);
  player_two = NULL;

  free(ball);
  ball = NULL;

  endwin();
}

internal void
run_game(WINDOW *game_win, WINDOW *score_win, player_t *player_one, player_t *player_two, ball_t *ball)
{
  while(running)
  {
    if(gamestate == gamestate_main_menu)
    {
      werase(game_win);

      render_main_menu(game_win);
      i32 result = update_main_menu(game_win);

      if(result == 1)
      {
        nodelay(game_win, 1);
        gamestate = gamestate_play;
      }
      else if(result == 2)
      {
        gamestate = gamestate_controls;
      }
      else if(result == 3)
      {
        running = 0;
      }

      wrefresh(game_win);
    }

    if(gamestate == gamestate_play)
    {
      key_pressed = wgetch(game_win);
      werase(game_win);
      werase(score_win);

      update_players(player_one, player_two);
      i32 winner = update_ball(ball, player_one, player_two);

      render_arena(game_win);
      render_players(game_win, player_one, player_two);
      render_ball(game_win, ball);
      render_score_window(score_win, player_one->score, player_two->score);

      if(winner)
      {
        render_victory_screen(game_win, score_win, winner);

        player_one->score = 0;
        player_two->score = 0;

        player_one->y = (game_win_height / 2) - 2;
        player_two->y = (game_win_height / 2) - 2;
      }

      wrefresh(game_win);
      wrefresh(score_win);
    }

    if(gamestate == gamestate_controls)
    {
      render_controls(game_win);

      gamestate = gamestate_main_menu;
    }
  }
}

internal i32
init_game()
{
  srand(time(NULL));

  initscr();

  if(!has_colors())
  {
    printf("Your terminal does not support colors.\n");
    return 0;
  }

  start_color();
  curs_set(0);        // cursor off
  noecho();           // getch character will not be printed on screen
  cbreak();           // getch will return user input immediately

  init_color(COLOR_BLACK, 0, 0, 0);
  init_color(COLOR_RED, 1000, 0, 0);
  init_color(COLOR_GREEN, 420, 600, 140);
  init_color(COLOR_YELLOW, 1000, 1000, 0);
  init_color(COLOR_BLUE, 130, 490, 720);
  init_color(COLOR_MAGENTA, 1000, 0, 1000);
  init_color(COLOR_CYAN, 0, 200, 250);
  init_color(COLOR_WHITE, 1000, 1000, 1000);

  init_pair(black_pair, COLOR_BLACK, COLOR_BLACK);
  init_pair(red_pair, COLOR_RED, COLOR_CYAN);
  init_pair(green_pair, COLOR_GREEN, COLOR_CYAN);
  init_pair(yellow_pair, COLOR_YELLOW, COLOR_CYAN);
  init_pair(blue_pair, COLOR_BLUE, COLOR_CYAN);
  init_pair(magenta_pair, COLOR_MAGENTA, COLOR_CYAN);
  init_pair(cyan_pair, COLOR_CYAN, COLOR_CYAN);
  init_pair(white_pair, COLOR_WHITE, COLOR_CYAN);

  return 1;
}

i32
main()
{
  // create our main windows
  WINDOW *game_win = newwin(game_win_height, game_win_width, game_win_start_y, game_win_start_x);
  WINDOW *score_win = newwin(score_win_height, score_win_width, 0, 0);

  wbkgd(stdscr, COLOR_PAIR(cyan_pair));
  wbkgd(game_win, COLOR_PAIR(cyan_pair));
  wbkgd(score_win, COLOR_PAIR(cyan_pair));

  keypad(game_win, 1);  // enable F1, F2, arrow keys etc.
  nodelay(game_win, 0); // getch will block execution
  wrefresh(stdscr);

  // create players and the ball
  player_t *player_one = create_player(2, 8, 1, 4);
  player_t *player_two = create_player(57, 8, 1, 4);
  ball_t *ball = create_ball('O', game_win_width / 2, game_win_height / 2, 0.0002, 0.0002);

  if(init_game())
  {
    run_game(game_win, score_win, player_one, player_two, ball);
    exit_game(player_one, player_two, ball);
    return EXIT_SUCCESS;
  }

  exit_game(player_one, player_two, ball);
  return EXIT_FAILURE;
}
