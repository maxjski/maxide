#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

static struct termios orig;

char *get_window_sizes_string();

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
  printf("TESTETS\n");
  atexit(disable_raw);

  char width[20];
  char height[20];

  enable_raw();
  // out("\x1b[2J\x1b[H"); // screen cleared
  char c = ' ';
  char *doc = malloc(sizeof(char) * 1000);

  for (int i = 0; i < 1000; i++) {
    doc[i] = 'z';
  }

  doc[999] = '\0';

  char **page;

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

  int x = 0;
  int y = 1;
  printf("HERE\n");
  get_cursor_pos(&x, &y);
  free(str);

  disable_raw();
  return 0;
}

// gotta free it
char *get_window_sizes_string() {
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  char *width = malloc(sizeof(char) * 20);
  char height[20];

  sprintf(width, "%d", w.ws_col);
  sprintf(height, "%d", w.ws_row);

  strcat(width, " ");
  strcat(width, height);
  return width;
}

// gotta free answer bruh
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
