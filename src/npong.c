#include <npong.h>

int running = 1;
int gamestate = gamestate_main_menu;

int game_win_start_x = 0;
int game_win_start_y = 3;
int game_win_width = 60;
int game_win_height = 20;

int score_win_width = 60;
int score_win_height = 3;

int key_pressed = 0;
int menu_choice;
int highlighted = 0;
int menu_item_amount = 3;
char *menu_items[] = {"Play", "Help", "Quit"};

void init_renderer_and_options()
{
  // initialize ncurses
  initscr();

  if (has_colors() == FALSE)
  {
    endwin();
    printf("Your terminal does not support colors.\nFind a more modern system/terminal.");
    exit(1);
  }

  start_color();

  // disable the use of CTRL+* shortcuts
  raw();

  // won't print pressed characters on the screen
  noecho();

  // make the cursor invisible
  curs_set(0);

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
}

void render_ball(WINDOW *game_win, ball_t *ball)
{
  wattron(game_win, COLOR_PAIR(white_pair));
  mvwprintw(game_win, ball->y, ball->x, "%c", ball->symbol);
  wattroff(game_win, COLOR_PAIR(white_pair));
}

void render_players(WINDOW *game_win, player_t *player_one, player_t *player_two)
{
  wattron(game_win, COLOR_PAIR(white_pair));

  // player one
  for (int y = 0; y < player_one->height; y++)
  {
    mvwaddstr(game_win, player_one->y + y, player_one->x, "|");
  }
  
  // player two
  for (int y = 0; y < player_two->height; y++)
  {
    mvwaddstr(game_win, player_two->y + y, player_two->x, "|");
  }

  wattroff(game_win, COLOR_PAIR(white_pair));
}

int update_ball(ball_t *ball, player_t *player_one, player_t *player_two)
{
  /*
   * Ball->flag_x:
   * 0 = left, 1 = right
   *
   * Ball->flag_y:
   * 0 = up, 1 = down
   */
  
  int winner = 0;

  // move the ball
  if (ball->flag_x == 0)
  {
    ball->x -= ball->vx;
  }
  else if (ball->flag_x == 1)
  {
    ball->x += ball->vx;
  }

  if (ball->flag_y == 0)
  {
    ball->y -= ball->vy;
  }
  else if (ball->flag_y == 1)
  {
    ball->y += ball->vy;
  }

  // check if the ball is past the screen on the x axis
  if (ball->x < 0)
  {
    player_two->score++;

    if (player_two->score >= 5)
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
  else if (ball->x > game_win_width)
  {
    player_one->score++;

    if (player_one->score >= 5)
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
  if (ball->y < 1)
  {
    ball->flag_y = 1;
  }
  else if (ball->y > game_win_height - 1)
  {
    ball->flag_y = 0;
  }

  // player one to ball collision
  if (ball->x > player_one->x && ball->x < player_one->x + 1 && ball->y > player_one->y - 1 && ball->y < (player_one->y + player_one->height))
  {
    // reverse x
    if (ball->flag_x == 1)
    {
      ball->flag_x = 0;
    }
    else if (ball->flag_x == 0)
    {
      ball->flag_x = 1;
    }

    if (player_one->can_add_speed == 1)
    {
      // add speed to the ball if it's below the max
      if (ball->vx && ball->vy < 0.002)
      {
        ball->vx += 0.0001;
        ball->vy += 0.0001;
      }

      player_one->can_add_speed = 0;
      player_two->can_add_speed = 1;
    }
  }

  // player two to ball collision
  if (ball->x < player_two->x && ball->x > player_two->x - 1 && ball->y > player_two->y - 1 && ball->y < (player_two->y + player_two->height))
  {
    // reverse x
    if (ball->flag_x == 1)
      ball->flag_x = 0;
    else if (ball->flag_x == 0)
      ball->flag_x = 1;

    if (player_two->can_add_speed == 1)
    {
      // add speed to the ball if it's below the max
      if (ball->vx && ball->vy < 0.002)
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

void render_main_menu(WINDOW *game_win)
{
  // calculate the amount to print for the title text
  int current_width = getmaxx(game_win);
  int amount_to_remove = 0;
  int amount_to_print = 44;

  if (current_width < 52)
  {
    amount_to_remove = 52 - current_width;
    amount_to_print = amount_to_print - amount_to_remove;
  }

  // draw game title
  wattron(game_win, COLOR_PAIR(white_pair));
  mvwaddnstr(game_win, 1, 8, " _   _   _____   _____   _   _   _____ ", amount_to_print);
  mvwaddnstr(game_win, 2, 8, "/ \\ / \\ /  __ \\ /  _  \\ / \\ / \\ /  __ \\", amount_to_print);
  mvwaddnstr(game_win, 3, 8, "|  \\| | | /_/ | | | | | |  \\  | | |  \\/", amount_to_print);
  mvwaddnstr(game_win, 4, 8, "| . ` | |  __/  | | | | | . ` | | | ___", amount_to_print);
  mvwaddnstr(game_win, 5, 8, "| |\\  | | |     | |_| | | |\\  | | |_| |", amount_to_print);
  mvwaddnstr(game_win, 6, 8, "\\_/ \\_/ \\_/     \\_____/ \\_/ \\_/ \\_____/", amount_to_print);
  wattroff(game_win, COLOR_PAIR(white_pair));

  // draw the menu options
  char menu_item[7] = "> ";
  amount_to_print = 6;

  // calculate the amount to print for the menu item text
  if (current_width < 12)
  {
    amount_to_remove = 12 - current_width;
    amount_to_print = amount_to_print - amount_to_remove;
  }

  for (int i = 0; i < menu_item_amount; i++)
  {
    if (i == highlighted && highlighted == 0)
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
    else if (i == highlighted && highlighted == 1)
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
    else if (i == highlighted && highlighted == 2)
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

int update_main_menu(WINDOW *game_win)
{
  // record user movement between menu items
  menu_choice = wgetch(game_win);
  switch (menu_choice)
  {
    case KEY_UP:
    {
      if (highlighted > 0)
      {
        (highlighted)--;
      }
    } break;

    case KEY_DOWN:
    {
      if (highlighted < 2)
      {
        (highlighted)++;
      }
    } break;

    default:
    {

    } break;
  }

  // return 1 if choice was "Play"
  if (menu_choice == 10 && highlighted == 0)
  {
    return 1;
  }

  // return 2 if choice was "Help"
  else if (menu_choice == 10 && highlighted == 1)
  {
    return 2;
  }

  // return 3 if choice was "Quit"
  else if (menu_choice == 10 && highlighted == 2)
  {
    return 3;
  }

  return 4;
}

void render_help(WINDOW *game_win)
{
  // draw help text
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
}

void render_score_window(WINDOW *score_win, int player_one_score, int player_two_score)
{
  wattron(score_win, COLOR_PAIR(white_pair));

  // resize window to keep it in place
  wresize(score_win, score_win_height, score_win_width);

  // draw box border around the window
  box(score_win, 0, 0);

  mvwprintw(score_win, 1, 1, "Player 1: %d", player_one_score);
  mvwprintw(score_win, 1, 48, "Player 2: %d", player_two_score);

  wattroff(score_win, COLOR_PAIR(white_pair));
}

void update_players(player_t *player_one, player_t *player_two)
{
  // do collision detection and move player
  switch (key_pressed)
  {
    case 'q':
    {
      running = 0;
    } break;

    case 'w':
    {
      if (player_one->y > 1)
      {
        player_one->y--;
      }
    } break;

    case 's':
    {
      if ((player_one->y + player_one->height) < (game_win_height - 1))
      {
        player_one->y++;
      }
    } break;

    case KEY_UP:
    {
      if (player_two->y > 1)
      {
        player_two->y--;
      }
    } break;

    case KEY_DOWN:
    {
      if ((player_two->y + player_two->height) < (game_win_height - 1))
      {
        player_two->y++;
      }
    } break;

    default:
    {
    } break;
  }
}

void render_arena(WINDOW *game_win)
{
  // corners
  wattron(game_win, COLOR_PAIR(white_pair));
  mvwaddstr(game_win, 0, 0, "+");
  mvwaddstr(game_win, 0, game_win_width - 1, "+");
  mvwaddstr(game_win, game_win_height - 1, 0, "+");
  mvwaddstr(game_win, game_win_height - 1, game_win_width - 1, "+");

  // left and right border
  for (int y = 1; y < game_win_height - 1; y++)
  {
    mvwaddstr(game_win, y, 0, "|");
    mvwaddstr(game_win, y, game_win_width - 1, "|");
  }

  // top and bottom border
  for (int x = 1; x < game_win_width - 1; x++)
  {
    mvwaddstr(game_win, 0, x, "-");
    mvwaddstr(game_win, game_win_height - 1, x, "-");
  }

  wattroff(game_win, COLOR_PAIR(white_pair));
}

void set_window_options(WINDOW *game_win, WINDOW *score_win)
{
  // enable the use of F1, F2.. and arrow keys
  keypad(game_win, TRUE);

  // change the bg colors for the windows
  wbkgd(stdscr, COLOR_PAIR(cyan_pair));
  wbkgd(game_win, COLOR_PAIR(cyan_pair));
  wbkgd(score_win, COLOR_PAIR(cyan_pair));

  // refresh all the windows
  wrefresh(stdscr);
  wrefresh(game_win);
  wrefresh(score_win);
}

player_t* create_player(int x, int y, int width, int height)
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

ball_t* create_ball(char symbol, int x, int y, float vx, float vy)
{
  ball_t *b = malloc(sizeof(ball_t));

  b->flag_x = rand() % 2;
  b->flag_y = rand() % 2;
  b->symbol = symbol;
  b->x = x;
  b->y = y;
  b->vx = vx;
  b->vy = vy;

  return b;
}

// NOTE(Rami): 
void render_victory_screen(WINDOW *game_win, WINDOW *score_win, int winner)
{
  nodelay(game_win, FALSE);

  werase(game_win);
  werase(score_win);

  wattron(game_win, COLOR_PAIR(white_pair));
  mvwprintw(game_win, 5, game_win_width / 2, "Player %d wins!", winner);
  mvwaddstr(game_win, 10, 26, "[Q] for main menu");
  mvwaddstr(game_win, 11, 26, "[R] for a rematch");

  for (int y = 3; y < 8; y++)
  {
    mvwaddstr(game_win, y, (game_win_width / 2) - 4, "|");
    mvwaddstr(game_win, y, (game_win_width / 2) + 17, "|");
  }

  for (int x = 26; x < 48; x++)
  {
    mvwaddstr(game_win, 2, x, "-");
    mvwaddstr(game_win, 8, x, "-");
  }

  wattroff(game_win, COLOR_PAIR(white_pair));
  wrefresh(game_win);
  wrefresh(score_win);

  while (1)
  {
    int input = wgetch(game_win);

    if (input == 'Q' || input == 'q')
    {
      gamestate = gamestate_main_menu;
      break;
    }
    else if (input == 'R' || input == 'r')
    {
      nodelay(game_win, TRUE);
      break;
    }
  }
}

void clean_up(WINDOW *game_win, WINDOW *score_win, ball_t *ball, player_t *player_one, player_t *player_two)
{
  free(player_one);
  player_one = NULL;

  free(player_two);
  player_two = NULL;

  free(ball);
  ball = NULL;

  // delete game window
  delwin(game_win);

  // delete score window
  delwin(score_win);

  // deconstructs the stdscr and shuts down the ncurses library
  endwin();
}
