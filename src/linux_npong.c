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
  key_enter = 10
};

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

typedef enum
{
  state_main_menu,
  state_play,
  state_controls,
  state_quit,

  state_count
} game_state_e;

typedef struct
{
  game_state_e state;
} game_t;

typedef struct
{
  i32 x;
  i32 y;
  i32 w;
  i32 h;
  i32 glyph;
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

typedef struct
{
  WINDOW *win;
  i32 x;
  i32 y;
  i32 w;
  i32 h;
} window_t;

// NOTE(Rami): 
i32 key_pressed;
i32 input;
i32 highlighted;
i32 menu_item_amount = 3;
char *menu_items[] = {"Play", "Controls", "Quit"};

global game_t game;

global window_t game_win;
global window_t score_win;

global player_t player_one;
global player_t player_two;

global ball_t ball;

internal void
render_ball()
{
  wattron(game_win.win, COLOR_PAIR(white_pair));
  mvwprintw(game_win.win, ball.y, ball.x, "%c", ball.glyph);
  wattroff(game_win.win, COLOR_PAIR(white_pair));
}

internal void
render_players()
{
  wattron(game_win.win, COLOR_PAIR(white_pair));
  for(i32 y = 0; y < player_one.h; y++) {mvwprintw(game_win.win, player_one.y + y, player_one.x, "%c", player_one.glyph);}
  for(i32 y = 0; y < player_two.h; y++) {mvwprintw(game_win.win, player_two.y + y, player_two.x, "%c", player_two.glyph);}
  wattroff(game_win.win, COLOR_PAIR(white_pair));
}

internal i32
update_ball()
{
  /*
   * ball.flag_x:
   * 0 = left, 1 = right
   *
   * ball.flag_y:
   * 0 = up, 1 = down
   */
  
  i32 winner = 0;

  // move the ball
  if(ball.flag_x == 0)
  {
    ball.x -= ball.vx;
  }
  else if(ball.flag_x == 1)
  {
    ball.x += ball.vx;
  }

  if(ball.flag_y == 0)
  {
    ball.y -= ball.vy;
  }
  else if(ball.flag_y == 1)
  {
    ball.y += ball.vy;
  }

  // check if the ball is past the screen on the x axis
  if(ball.x < 1)
  {
    player_two.score++;

    if(player_two.score >= 5)
    {
      winner = 2;
    }

    ball.flag_x = rand() % 2;
    ball.flag_y = rand() % 2;
    ball.x = game_win.w / 2;
    ball.y = game_win.h / 2;
    ball.vx = 0.0002;
    ball.vy = 0.0002;
    player_one.can_add_speed = 1;
    player_two.can_add_speed = 1;
  }
  else if(ball.x > (game_win.w - 1))
  {
    player_one.score++;

    if(player_one.score >= 5)
    {
      winner = 1;
    }

    ball.flag_x = rand() % 2;
    ball.flag_y = rand() % 2;
    ball.x = game_win.w / 2;
    ball.y = game_win.h / 2;
    ball.vx = 0.0002;
    ball.vy = 0.0002;
    player_one.can_add_speed = 1;
    player_two.can_add_speed = 1;
  }

  // check if the ball is past the screen on the y axis
  if(ball.y < 1)
  {
    ball.flag_y = 1;
  }
  else if(ball.y > game_win.h - 1)
  {
    ball.flag_y = 0;
  }

  // player one to ball collision
  if(ball.x > player_one.x && ball.x < player_one.x + 1 && ball.y > player_one.y - 1 && ball.y < (player_one.y + player_one.h))
  {
    // reverse x
    if(ball.flag_x == 1)
    {
      ball.flag_x = 0;
    }
    else if(ball.flag_x == 0)
    {
      ball.flag_x = 1;
    }

    if(player_one.can_add_speed == 1)
    {
      // add speed to the ball if it's below the max
      if(ball.vx && ball.vy < 0.002)
      {
        ball.vx += 0.0001;
        ball.vy += 0.0001;
      }

      player_one.can_add_speed = 0;
      player_two.can_add_speed = 1;
    }
  }

  // player two to ball collision
  if(ball.x < player_two.x && ball.x > player_two.x - 1 && ball.y > player_two.y - 1 && ball.y < (player_two.y + player_two.h))
  {
    // reverse x
    if(ball.flag_x == 1)
    {
      ball.flag_x = 0;
    }
    else if(ball.flag_x == 0)
    {
      ball.flag_x = 1;
    }

    if(player_two.can_add_speed == 1)
    {
      // add speed to the ball if it's below the max
      if(ball.vx && ball.vy < 0.002)
      {
        ball.vx += 0.0001;
        ball.vy += 0.0001;
      }

      player_one.can_add_speed = 1;
      player_two.can_add_speed = 0;
    }
  }

  return winner;
}

internal void
render_main_menu()
{
  i32 window_width = getmaxx(game_win.win);
  i32 print_count = 44;
  i32 remove_count = 0;

  if(window_width < 52)
  {
    remove_count = 52 - window_width;
    print_count = print_count - remove_count;
  }

  wattron(game_win.win, COLOR_PAIR(white_pair));
  mvwaddnstr(game_win.win, 1, 8, " _   _   _____   _____   _   _   _____ ", print_count);
  mvwaddnstr(game_win.win, 2, 8, "/ \\ / \\ /  _  \\ /  _  \\ / \\ / \\ /  __ \\", print_count);
  mvwaddnstr(game_win.win, 3, 8, "|  \\| | | |_| | | | | | |  \\  | | |  \\/", print_count);
  mvwaddnstr(game_win.win, 4, 8, "| . ` | |  __/  | | | | | . ` | | | ___", print_count);
  mvwaddnstr(game_win.win, 5, 8, "| |\\  | | |     | |_| | | |\\  | | |_| |", print_count);
  mvwaddnstr(game_win.win, 6, 8, "\\_/ \\_/ \\_/     \\_____/ \\_/ \\_/ \\_____/", print_count);
  wattroff(game_win.win, COLOR_PAIR(white_pair));

  char menu_item[16] = "> ";
  print_count = 10;

  if(window_width < 16)
  {
    remove_count = 16 - window_width;
    print_count -= remove_count;
  }

  for(i32 i = 0; i < menu_item_amount; i++)
  {
    if(i == highlighted && highlighted == 0)
    {
      strcpy(menu_item + 2, menu_items[i]);

      wattron(game_win.win, COLOR_PAIR(blue_pair));
      mvwaddnstr(game_win.win, 10, 6, menu_item, print_count);
      wattroff(game_win.win, COLOR_PAIR(blue_pair));

      wattron(game_win.win, COLOR_PAIR(white_pair));
      mvwaddnstr(game_win.win, 11, 8, menu_items[1], print_count - 2);

      mvwaddnstr(game_win.win, 12, 8, menu_items[2], print_count - 2);
      wattroff(game_win.win, COLOR_PAIR(white_pair));
    }
    else if(i == highlighted && highlighted == 1)
    {
      strcpy(menu_item + 2, menu_items[i]);

      wattron(game_win.win, COLOR_PAIR(white_pair));
      mvwaddnstr(game_win.win, 10, 8, menu_items[0], print_count - 2);
      wattroff(game_win.win, COLOR_PAIR(white_pair));

      wattron(game_win.win, COLOR_PAIR(blue_pair));
      mvwaddnstr(game_win.win, 11, 6, menu_item, print_count);
      wattroff(game_win.win, COLOR_PAIR(blue_pair));

      wattron(game_win.win, COLOR_PAIR(white_pair));
      mvwaddnstr(game_win.win, 12, 8, menu_items[2], print_count - 2);
      wattroff(game_win.win, COLOR_PAIR(white_pair));
    }
    else if(i == highlighted && highlighted == 2)
    {
      strcpy(menu_item + 2, menu_items[i]);

      wattron(game_win.win, COLOR_PAIR(white_pair));
      mvwaddnstr(game_win.win, 10, 8, menu_items[0], print_count - 2);

      mvwaddnstr(game_win.win, 11, 8, menu_items[1], print_count - 2);
      wattroff(game_win.win, COLOR_PAIR(white_pair));

      wattron(game_win.win, COLOR_PAIR(blue_pair));
      mvwaddnstr(game_win.win, 12, 6, menu_item, print_count);
      wattroff(game_win.win, COLOR_PAIR(blue_pair));
    }
  }
}

internal i32
update_main_menu()
{
  i32 result = state_count;

  input = wgetch(game_win.win);
  switch(input)
  {
    case KEY_UP:
    {
      if(highlighted > 0)
      {
        highlighted--;
      }
    } break;

    case KEY_DOWN:
    {
      if(highlighted < 2)
      {
        highlighted++;
      }
    } break;

    case key_enter:
    {
      if(highlighted == 0)
      {
        result = state_play;
      }
      else if(highlighted == 1)
      {
        result = state_controls;
      }
      else if(highlighted == 2)
      {
        result = state_quit;
      }
    } break;

    default: break;
  }

  return result;
}

internal void
render_controls()
{
  i32 window_width = getmaxx(game_win.win);
  i32 print_count = 63;
  i32 remove_count = 0;

  if(window_width < 71)
  {
    remove_count = 71 - window_width;
    print_count -= remove_count;
  }

  wattron(game_win.win, COLOR_PAIR(white_pair));
  mvwaddnstr(game_win.win, 1, 8, " _____   _____   _   _   _____   _____   _____   _      ______", print_count);
  mvwaddnstr(game_win.win, 2, 8, "/  __ \\ /  _  \\ / \\ / \\ /_   _\\ /  _  \\ /  _  \\ / |    /  ____\\", print_count);
  mvwaddnstr(game_win.win, 3, 8, "| /  \\/ | | | | |  \\| |   | |   | |_| / | | | | | |    | |____", print_count);
  mvwaddnstr(game_win.win, 4, 8, "| |     | | | | | . ` |   | |   |    /  | | | | | |    \\____  \\", print_count);
  mvwaddnstr(game_win.win, 5, 8, "| \\__/\\ | |_| | | |\\  |   | |   | |\\ \\  | |_| | | |___ _____| |", print_count);
  mvwaddnstr(game_win.win, 6, 8, "\\_____/ \\_____/ \\_/ \\_/   \\_/   \\_/ \\_/ \\_____/ \\____/ \\______/", print_count);

  print_count = 46;
  if(window_width < 54)
  {
    remove_count = 54 - window_width;
    print_count -= remove_count;
  }

  mvwaddnstr(game_win.win, 10, 8, "Player 1: up and down using \"w\" and \"s\"", print_count);
  mvwaddnstr(game_win.win, 11, 8, "Player 2: up and down using arrow keys", print_count);
  mvwaddnstr(game_win.win, 13, 8, "You can press Q to quit the game while playing", print_count);
  mvwaddnstr(game_win.win, 15, 8, "First player to score 5 points wins", print_count);
  wattroff(game_win.win, COLOR_PAIR(white_pair));
}

internal void
render_score()
{
  wattron(score_win.win, COLOR_PAIR(white_pair));
  wresize(score_win.win, score_win.h, score_win.w);
  box(score_win.win, 0, 0);
  mvwprintw(score_win.win, 1, 1, "Player 1: %d", player_one.score);
  mvwprintw(score_win.win, 1, 63, "Player 2: %d", player_two.score);
  wattroff(score_win.win, COLOR_PAIR(white_pair));
}

internal void
update_players()
{
  switch(key_pressed)
  {
    case 'Q':
    case 'q':
    {
      game.state = state_quit;
    } break;

    case 'W':
    case 'w':
    {
      if(player_one.y > 1)
      {
        player_one.y--;
      }
    } break;

    case 'S':
    case 's':
    {
      if((player_one.y + player_one.h) < (game_win.h - 1))
      {
        player_one.y++;
      }
    } break;

    case KEY_UP:
    {
      if(player_two.y > 1)
      {
        player_two.y--;
      }
    } break;

    case KEY_DOWN:
    {
      if((player_two.y + player_two.h) < (game_win.h - 1))
      {
        player_two.y++;
      }
    } break;
  }
}

internal void
render_arena()
{
  wattron(game_win.win, COLOR_PAIR(white_pair));
  wresize(game_win.win, game_win.h, game_win.w);
  box(game_win.win, 0, 0);
  wattroff(game_win.win, COLOR_PAIR(white_pair));
}

internal void
render_victory(i32 winner)
{
  nodelay(game_win.win, 0);

  werase(game_win.win);
  werase(score_win.win);
  wattron(game_win.win, COLOR_PAIR(white_pair));

  mvwprintw(game_win.win, 5, 30, "Player %d wins!", winner);
  mvwaddstr(game_win.win, 11, 26, "[R] Rematch");
  mvwaddstr(game_win.win, 12, 26, "[Q] Main Menu");

  wattroff(game_win.win, COLOR_PAIR(white_pair));
  wrefresh(game_win.win);
  wrefresh(score_win.win);

  while(1)
  {
    i32 input = wgetch(game_win.win);
    if(input == 'Q' || input == 'q')
    {
      game.state = state_main_menu;
      break;
    }
    else if(input == 'R' || input == 'r')
    {
      nodelay(game_win.win, 1);
      break;
    }
  }

  player_one.y = (game_win.h / 2) - 2;
  player_one.score = 0;
  player_two.y = (game_win.h / 2) - 2;
  player_two.score = 0;
}

internal void
run_game()
{
  while(game.state != state_quit)
  {
    if(game.state == state_main_menu)
    {
      werase(game_win.win);

      render_main_menu(game_win.win);
      i32 result = update_main_menu(game_win.win);

      if(result == state_play)
      {
        nodelay(game_win.win, 1);
        game.state = state_play;
      }
      else if(result == state_controls)
      {
        game.state = state_controls;
      }
      else if(result == state_quit)
      {
        game.state = state_quit;
      }

      wrefresh(game_win.win);
    }
    else if(game.state == state_play)
    {
      key_pressed = wgetch(game_win.win);
      werase(game_win.win);
      werase(score_win.win);

      update_players();
      i32 winner = update_ball();

      render_arena();
      render_players();
      render_ball();
      render_score();

      if(winner)
      {
        render_victory(winner);
      }

      wrefresh(game_win.win);
      wrefresh(score_win.win);
    }
    else if(game.state == state_controls)
    {
      werase(game_win.win);

      render_controls(game_win.win);

      if(wgetch(game_win.win) == key_enter)
      {
        game.state = state_main_menu;
      }

      wrefresh(game_win.win);
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

  game_win.y = 3;
  game_win.w = 75;
  game_win.h = 20;
  game_win.win = newwin(game_win.h, game_win.w, game_win.y, game_win.x);

  score_win.w = 75;
  score_win.h = 3;
  score_win.win = newwin(score_win.h, score_win.w, score_win.y, score_win.x);

  wbkgd(stdscr, COLOR_PAIR(cyan_pair));
  wbkgd(game_win.win, COLOR_PAIR(cyan_pair));
  wbkgd(score_win.win, COLOR_PAIR(cyan_pair));

  keypad(game_win.win, 1);  // enable F1, F2, arrow keys etc.
  nodelay(game_win.win, 0); // getch will block execution
  wrefresh(stdscr);

  player_one.x = 2;
  player_one.y = 8;
  player_one.w = 1;
  player_one.h = 4;
  player_one.glyph = '|';
  player_one.can_add_speed = 1;

  player_two.x = 72;
  player_two.y = 8;
  player_two.w = 1;
  player_two.h = 4;
  player_two.glyph = '|';
  player_two.can_add_speed = 1;

  ball.glyph = 'O';
  ball.x = game_win.w / 2;
  ball.y = game_win.y / 2;
  ball.vx = 0.0002;
  ball.vy = 0.0002;

  return 1;
}

i32
main()
{
  if(init_game())
  {
    run_game();
    endwin();
    return EXIT_SUCCESS;
  }

  endwin();
  return EXIT_FAILURE;
}