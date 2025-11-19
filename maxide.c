#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

static struct termios orig;

void get_window_size(unsigned short *wd, unsigned short *hg, struct winsize w);

void get_cursor_pos(int *col, int *row);
// setting for the terminal
void enable_raw(void) {
  tcgetattr(STDIN_FILENO, &orig);
  struct termios raw = orig;
  raw.c_lflag &= ~(ICANON | ECHO); // no line buffering
  raw.c_lflag &= ~(IEXTEN | ISIG); // disables ^Z ^C
  raw.c_lflag &= ~(IXON | ICRNL);  // no Ctrl-S
  raw.c_lflag &= ~(OPOST);         // no post processing

  // blocking read, so terminal only updates if there is input
  raw.c_cc[VMIN] = 1;
  raw.c_cc[VTIME] = 0; // no timeout for blocking read
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void disable_raw(void) { tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig); }

void out(const char *s) { write(STDOUT_FILENO, s, strlen(s)); }

int main(int argc, char **argv) {
  atexit(disable_raw);

  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  unsigned short terminalHeight = w.ws_row;
  unsigned short terminalWidth = w.ws_col;

  enable_raw();
  // out("\x1b[2J\x1b[H"); // screen cleared
  char c = ' ';
  char *doc = malloc(sizeof(char) * 1000);

  for (int i = 0; i < 1000; i++) {
    doc[i] = 'z';
  }

  doc[999] = '\0';

  char *page = malloc(sizeof(char) * terminalWidth * terminalHeight + 1);

  for (int i = 0; i < terminalWidth * terminalHeight; i++) {
    page[i] = ' ';
  }
  page[terminalWidth * terminalHeight] = '\0';
  // page[terminalWidth * terminalHeight - 1] =
  //     'X'; // this will print on the bottom right
  // page[((terminalHeight - 1) * terminalWidth) - 1] =
  //     'H'; // this will print on the right of the second last line

  // draw mode on the bottom left, insert or move.
  int cursorX = 0;
  int cursorY = 0;
  page[(terminalHeight - 1) * terminalWidth] = 'M';
  page[(terminalHeight - 1) * terminalWidth + 1] = 'O';
  page[(terminalHeight - 1) * terminalWidth + 2] = 'V';
  page[(terminalHeight - 1) * terminalWidth + 3] = 'E';

  while (c != 'q') {
    read(STDIN_FILENO, &c, 1);
    // page[0] = c;
    page[cursorX + cursorY] = ' ';
    if (c == 'h' && cursorX > 0) {
      cursorX--;
      c = ' ';
    } else if (c == 'j' && cursorY < (terminalHeight - 2) * terminalWidth) {
      cursorY += terminalWidth;
      c = ' ';
    } else if (c == 'k' && (cursorY - terminalWidth) >= 0) {
      cursorY -= terminalWidth;
      c = ' ';
    } else if (c == 'l' && cursorX < terminalWidth - 1) {
      cursorX += 1;
      c = ' ';
    }
    page[cursorX + cursorY] = 'X';
    // out("\x1b[2J\x1b[H"); // screen cleared
    out(page);
  }

  disable_raw();
  return 0;
}

void get_cursor_pos(int *col, int *row) {
  char buf[32];
  unsigned int i = 0;
  write(STDOUT_FILENO, "\x1B[6n", 5);

  while (i < 30 && read(STDIN_FILENO, &buf[i], 1) == 1) {
    // printf("Byte %d: '%c' (Decimal: %d)\r\n", i, buf[i], buf[i]);
    if (buf[i] == 'R') {
      break;
    }
    i++;
  }
  buf[i] = '\0';
  // Parse the result (Expected: ESC [ rows ; cols R)
  if (buf[0] == 27 && buf[1] == '[') {
    if (sscanf(&buf[2], "%d;%d", row, col) == 2) {
      // printf("SUCCESS: Rows=%d, Cols=%d\r\n", *row, *col);
    }
  }
}
