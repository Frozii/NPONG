#include <npong.h>

int main(void)
{
  // seed pgen
  srand(time(NULL));

  init_renderer_and_options();

  // create our main windows
  WINDOW *game_win = newwin(game_win_height, game_win_width, game_win_start_y, game_win_start_x);
  WINDOW *score_win = newwin(score_win_height, score_win_width, 0, 0);

  set_window_options(game_win, score_win);

  // create players and the ball
  player_t *player_one = create_player(2, 8, 1, 4);
  player_t *player_two = create_player(57, 8, 1, 4);
  ball_t *ball = create_ball('O', game_win_width / 2, game_win_height / 2, 0.0004, 0.0004);

  while (running)
  {
    if (gamestate == gamestate_main_menu)
    {
      werase(game_win);

      render_main_menu(game_win);
      int result = update_main_menu(game_win);

      if (result == 1)
      {
        nodelay(game_win, TRUE);
        gamestate = gamestate_play;
      }
      else if (result == 2)
      {
        gamestate = gamestate_help;
      }
      else if (result == 3)
      {
        running = 0;
      }

      wrefresh(game_win);
    }

    if (gamestate == gamestate_play)
    {
      key_pressed = wgetch(game_win);
      werase(game_win);
      werase(score_win);

      update_players(player_one, player_two);
      int winner = update_ball(ball, player_one, player_two);

      render_arena(game_win);
      render_players(game_win, player_one, player_two);
      render_ball(game_win, ball);
      render_score_window(score_win, player_one->score, player_two->score);

      if (winner)
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

    if (gamestate == gamestate_help)
    {
      werase(game_win);
      render_help(game_win);
      wgetch(game_win);

      gamestate = gamestate_main_menu;
    }
  }

  clean_up(game_win, score_win, ball, player_one, player_two);
  return 0;
}
