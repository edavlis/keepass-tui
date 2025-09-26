#include <stdio.h>
#include <termios.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <string.h>
#include <sys/poll.h>
#include <unistd.h>

// termios functions
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

// input output functions
int  readFromFileDescriptor(int filedescriptor) {
  char buffer[4095] = "";
  int amountRead = read(filedescriptor, buffer,4095 -1);
    if (amountRead > 0 ) {
      buffer[amountRead]  = '\0';   
    }
    if (amountRead == -1) 
    {
      return (-1);
    }
    if (amountRead == 0) 
    {
      return(-1);
    }

    printf("%s",buffer);
    fflush(stdout);
    
    return 0;
  }



int main(int argc, char *argv[]) {
  
  // usage, refactor later
  if (argc < 2) {
    printf("Usage: keepassxc-tui <path-to-database>\n");
    return 1;
  }

  //termios stuff
  // get terminal dimentsions
    int height_terminal;
    int width_terminal;
    get_terminal_dimensions(&width_terminal, &height_terminal);
  // get new and old terminal settings
    struct termios new_terminal_settings, old_terminal_settings; 
    tcgetattr(STDIN_FILENO, &new_terminal_settings);
    tcgetattr(STDIN_FILENO, &old_terminal_settings);
    enable_raw_mode(&new_terminal_settings);
  
  
  // create pipes
    int in_to_child[2];
    int out_of_child[2];

    pipe(in_to_child);
    pipe(out_of_child);



 // create child and parent proccess
    int pid;

    if ( (pid = fork()) == -1) {
      printf("Failed to fork process");
      return (-1);
    }

    // child proc
    if (pid == 0) {
      close(in_to_child[1]);
      close(out_of_child[0]);

      dup2(out_of_child[1], STDOUT_FILENO); // redirect child STDOUT with out of child pipe
      dup2(out_of_child[1], STDERR_FILENO); // redirect child STDOUT with out of child pipe
      dup2(in_to_child[0], STDIN_FILENO); // CHILD STDINPUT now comes froem the intochild pipe

    
      execlp("keepassxc-cli","keepassxc-cli", "open" , argv[1] , NULL);
      close(in_to_child[0]);
      close(out_of_child[1]);
    }  

      moveCursor((width_terminal / 2), (height_terminal / 2));

        struct pollfd STDIN_CHILD_filedescriptors[2]; // 0  is child 1 is stdin

        STDIN_CHILD_filedescriptors[0].fd = out_of_child[0];
        STDIN_CHILD_filedescriptors[0].events = POLLIN;
        STDIN_CHILD_filedescriptors[1].fd = STDIN_FILENO;
        STDIN_CHILD_filedescriptors[1].events = POLLIN;

        

      // close file descriptors
      close(in_to_child[0]);
      close(out_of_child[1]);

      // make buffers to write to
      char buffer[64*64] = "";
      while (1) {
        moveCursor((width_terminal / 2), (height_terminal / 2));
        clearscreen();

        int CHILDOUT_ready=poll(STDIN_CHILD_filedescriptors, 2, -1);
        if (CHILDOUT_ready == -1) {
          printf("poll failed");
        }

        if (STDIN_CHILD_filedescriptors[0].revents & POLLIN) {
        readFromFileDescriptor(out_of_child[0]);
        fflush(stdout);
        }

        // get ketboard input
        if (STDIN_CHILD_filedescriptors[1].revents & POLLIN) {
          reset_terminal(&old_terminal_settings);
        fgets(buffer, sizeof(buffer), stdin);
        if (write(in_to_child[1], buffer, strlen(buffer)) == -1) {
          printf("Failed to write to child write pipe");
        };
        }


        } 

       close(in_to_child[1]);
       close(out_of_child[0]);

      }
