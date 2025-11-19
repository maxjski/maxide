#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

static struct termios orig;

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
  printf("TESTETS");
  atexit(disable_raw);

  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

  printf("%d", w.ws_col);
  //
  char width[20];
  char height[20];
  //
  sprintf(width, "%d", w.ws_col);
  sprintf(height, "%d", w.ws_row);

  printf(width);
  printf(height);
  // char *doc = malloc(sizeof(char) * 1000);
  //
  // for (int i = 0; i < 1000; i++) {
  //   doc[i] = 'z';
  // }
  //
  // doc[999] = '\0';
  // char str[100];
  // int i = 0;
  // short f = 0;
  // int j;
  // while (i < 100) {
  //   if (f == 0) {
  //     if (width[i] != '\0') {
  //       str[i] = width[i];
  //     } else {
  //       f = 1;
  //       j = i;
  //       str[i] = ' ';
  //       i++;
  //     }
  //   }
  //
  //   if (f == 1) {
  //     if (height[i - j] != '\0') {
  //       str[i] = height[i - j];
  //     } else {
  //       str[i] = '\0';
  //       i = 101;
  //     }
  //   }
  // }

  // while (c != 'q') {
  //   read(STDIN_FILENO, &c, 1);
  //   doc[0] = c;
  //   out("\x1b[2J\x1b[H"); // screen cleared
  //   out(str);
  // }
  disable_raw();
  // return 0;
}
