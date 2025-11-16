#include <stdlib.h>
#include <string.h>
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
  atexit(disable_raw);
  enable_raw();
  out("\x1b[2J\x1b[H"); // screen cleared
  char c = ' ';
  char *doc = malloc(sizeof(char) * 1000);

  for (int i = 0; i < 1000; i++) {
    doc[i] = 'z';
  }

  doc[999] = '\0';

  while (c != 'q') {
    read(STDIN_FILENO, &c, 1);
    doc[0] = c;
    out("\x1b[2J\x1b[H"); // screen cleared
    out(doc);
  }
  disable_raw();
  return 0;
}
