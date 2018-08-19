#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <npong.h>

int main(void)
{
  init_renderer_and_options();

  // create our main windows
  WINDOW *game_win = newwin(game_win_height, game_win_width, game_win_start_y, game_win_start_x);
  WINDOW *score_win = newwin(score_win_height, score_win_width, 0, 0);

  set_window_options(game_win, score_win);

  // create players and the ball
  player_t *player_one = create_player(2, 8, 1, 4);
  player_t *player_two = create_player(57, 8, 1, 4);
  ball_t *ball = create_ball('O', game_win_width / 2, game_win_height / 2, 0.002, 0.002);

  while (running)
  {
    /* Menu */
    if (gamestate == 0)
    {
      wclear(game_win);

      int result = draw_and_update_menu(game_win);

      if (result == 1)
      {
        nodelay(game_win, TRUE);
        gamestate = 1;
      }
      else if (result == 2)
      {
        gamestate = 2;
      }
      else if (result == 3)
      {
        running = 0;
      }

      wrefresh(game_win);
    }

    /* Play */
    if (gamestate == 1)
    {
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
    if (gamestate == 2)
    {
      wclear(game_win);

      draw_help(game_win);
      wrefresh(game_win);

      wgetch(game_win);
      gamestate = 0;
    }
  }

  clean_up(game_win, score_win, player_one, player_two, ball);
  return 0;
}
