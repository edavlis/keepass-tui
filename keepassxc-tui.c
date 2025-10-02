// INCLUDES
#include <stdio.h>
#include <termios.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <string.h>
#include <sys/poll.h>
#include <unistd.h>



// FUNCTION PROTOTYPES
void enable_raw_mode(struct termios *new_terminal_settings) ;
void reset_terminal(struct termios *old_terminal_settings) ;
void get_terminal_dimensions( int *width, int *height) ;
void moveCursor(int x, int y) ;
void clearscreen() ;
char* readFromFileDescriptor(int filedescriptor, char* buffer, int buffersize);
void stringToLines(char *originalString, char **linedOutput) ;
void checkForDatabase(char **lines, char *databasePrompt);
                                                            


// MAIN FUNCTION
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


        struct pollfd STDIN_CHILD_filedescriptors[2]; // 0  is child 1 is stdin

        STDIN_CHILD_filedescriptors[0].fd = out_of_child[0];
        STDIN_CHILD_filedescriptors[0].events = POLLIN;
        STDIN_CHILD_filedescriptors[1].fd = STDIN_FILENO;
        STDIN_CHILD_filedescriptors[1].events = POLLIN;

        

      // close file descriptors
        close(in_to_child[0]);
        close(out_of_child[1]);

      // make buffers to write to
        char keyboard_input_buffer[64*8] = "";
        char childOutputDataBuffer[64*8];
        char *linedChildOutputDataBuffer[64*8];




    moveCursor(1,1);
    clearscreen();
    // main part of main function
      while (1) {

        int CHILDOUT_ready=poll(STDIN_CHILD_filedescriptors, 2, -1);
        if (CHILDOUT_ready == -1) {
          printf("poll failed");
        }

        if (STDIN_CHILD_filedescriptors[0].revents & POLLIN) {
          int i = 0;
          memset(childOutputDataBuffer,0,sizeof(childOutputDataBuffer));
          readFromFileDescriptor(out_of_child[0], childOutputDataBuffer,128*128);
          printf("%s", childOutputDataBuffer);
          //stringToLines(childOutputDataBuffer,linedChildOutputDataBuffer);
          //checkForDatabase(linedChildOutputDataBuffer,"> ");

         // while (linedChildOutputDataBuffer[i] != NULL) {
         //   if (strcmp(linedChildOutputDataBuffer[i], "") != 0) {
         //     printf("%s\n",linedChildOutputDataBuffer[i]);
         //   }
         //   i++;
         // }
          fflush(stdout);
        }

        // get ketboard input
        if (STDIN_CHILD_filedescriptors[1].revents & POLLIN) {
          memset(keyboard_input_buffer,0,sizeof(keyboard_input_buffer));
          read(STDIN_FILENO, keyboard_input_buffer, sizeof(keyboard_input_buffer));

          if (write(in_to_child[1], keyboard_input_buffer, strlen(keyboard_input_buffer)) == -1) {
            printf("Failed to write to child write pipe");
            

        };
        }


        } 

       close(in_to_child[1]);
       close(out_of_child[0]);

      }



// FUNCTIONS 
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
char* readFromFileDescriptor(int filedescriptor, char* buffer, int buffersize) {  
  
 
  int amountRead = read(filedescriptor,buffer,buffersize-1);
    if (amountRead > 0 ) {
      buffer[amountRead]  = '\0';   
    }
    if (amountRead == -1) 
    {
      printf("read fail, returned = -1");
      return NULL;
    }
    if (amountRead == 0) {
      printf("read fail, read returned= 0");
    
      return NULL;
    }
    fflush(stdout);

    return buffer;
    
  }

void stringToLines(char *originalString, char **linedOutput) {
  
  char *line;
  line = strtok(originalString, "\n"); 
  linedOutput[0] = line;
  int i = 0;
  while (line != NULL) {
    linedOutput[i] = line;
    i++;
    line = strtok(NULL, "\n"); 
  }
  linedOutput[i] = NULL;
}

void checkForDatabase(char **lines, char *databasePrompt) { // i know you can check just hte first and last lines but i want to be sure, ill write a second function that does the afformentioned
  int i = 0;
  while (lines[i] != NULL) {
    
    if (strstr(lines[i], databasePrompt)  ) {
      lines[i] = "";
    }
    i++;
  }
}


// show --show-protected
