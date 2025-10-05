#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>


void enable_raw_mode(struct termios *new_terminal_settings) {
  new_terminal_settings->c_lflag &= ~( ICANON | ECHO);
  tcsetattr(STDIN_FILENO,TCSANOW, new_terminal_settings);
}

void reset_terminal(struct termios *old_terminal_settings) {
  tcsetattr(STDIN_FILENO,TCSANOW,old_terminal_settings);
}

void get_terminal_dimensions( int *width, int *height) {
   
  struct winsize windowSize;
  ioctl(STDIN_FILENO,TIOCGWINSZ, &windowSize);

  *width = windowSize.ws_col;
  *height = windowSize.ws_row;

}

void moveCursor(int x, int y) {
  printf("\033[%d;%dH", y, x);
}

void clearscreen() {
  printf("\033[2J");
    
}


