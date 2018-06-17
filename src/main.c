#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#include <npong.h>

int main(int argc, char **argv)
{
  // initialize ncurses
  initscr();

  // create our main windows
  WINDOW *game_win = newwin(game_win_height, game_win_width, game_win_start_y, game_win_start_x);
  WINDOW *score_win = newwin(score_win_height, score_win_width, 0, 0);

  set_ncurses_options(game_win, score_win);

  player_t *player_one = create_player(2, 8, 1, 4, 0);
  player_t *player_two = create_player(57, 8, 1, 4, 0);
  ball_t *ball = create_ball('O', game_win_width / 2, game_win_height / 2, 0.003, 0.003);

  while (running) {
    /* Menu */
    if (gamestate == 0) {
      wclear(game_win);

      int result = draw_and_update_menu(game_win);

      if (result == 1) {
        nodelay(game_win, TRUE);
        gamestate = 1;
      }
      else if (result == 2) {
        gamestate = 2;
      }
      else if (result == 3) {
        gamestate = 3;
      }

      wrefresh(game_win);
    }

    /* Play */
    if (gamestate == 1) {
      wclear(game_win);
      wclear(score_win);

      update_players(player_one, player_two);
      int winner = update_and_draw_ball(game_win, ball, player_one, player_two);
      draw_players_and_arena(game_win, player_one, player_two);
      draw_score_and_window(score_win, &player_one->score, &player_two->score);

      key_pressed = wgetch(game_win);
      wrefresh(game_win);
      wrefresh(score_win);

      victory_screen(game_win, score_win, player_one, player_two, winner);
    }

    /* Help */
    if (gamestate == 2) {
      wclear(game_win);

      draw_help(game_win);
      wrefresh(game_win);

      wgetch(game_win);
      gamestate = 0;
    }

    if (gamestate == 3) {
      running = 0;
    }
  }

  clean_up(game_win, score_win, player_one, player_two, ball);
  endwin(); // deconstructs the stdscr and shuts down the ncurses library

  return 0;
}
