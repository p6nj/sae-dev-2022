#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
  char title[] = "Fiche d'appel des RT2FA";

  initscr();     /* initialize NCurses */
  addstr(title); // add the title
  addch('\n');
  refresh();
  getch(); /* wait until next keypress */

  endwin(); /* clean up NCurses */
  return EXIT_SUCCESS;
}
