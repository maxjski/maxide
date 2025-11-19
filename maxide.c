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
  raw.c_cc[VMIN] = 0;              // non blocking read
  raw.c_cc[VTIME] = 1;             // with 100 ms timeout
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

  char **page = malloc(sizeof(char) * terminalWidth * terminalHeight);

  char *str;
  char *cursor;

  // while (c != 'q') {
  //   read(STDIN_FILENO, &c, 1);
  //   str = get_window_sizes_string();
  //   doc[0] = c;
  //   out("\x1b[2J\x1b[H"); // screen cleared
  //
  //   cursor = get_thingy();
  //   strcat(str, "\n");
  //   strcat(str, cursor);
  //   out(str);
  // }

  int cursorRow;
  int cursorCol;
  printf("HERE\n");
  get_cursor_pos(&cursorRow, &cursorCol);
  printf("%d row, %d col positions", cursorRow, cursorCol);
  free(str);

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
