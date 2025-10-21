// INCLUDES
#include <malloc.h>
#include <pty.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <string.h>
#include <termios.h>
#include <stdio.h>
#include <sys/poll.h>
#include <unistd.h>
#include "../include/outputParsing.h"
#include "../include/termiosFunctions.h"


// FUNCTION PROTOTYPES
char* readFromFileDescriptor(int filedescriptor, char* buffer, int buffersize);

char* getDatabaseName(char *line) 
{
  int found = 0;
  char* wordBegining;
  char* lineEnd;
  if (((wordBegining = strstr(line, "Name ")) != NULL) && ((lineEnd = strstr(line, ">\n")) != NULL))  {
    found = 1;
        wordBegining += 5;
      } else {
      found = 0;
      }

  if (found == 1) {
  return wordBegining;
  } else {
  return NULL;
  }
}


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

    int masterFd;
  
  
  // create pipes
    int in_to_child[2];
    int out_of_child[2];

    pipe(in_to_child);
    pipe(out_of_child);



 // create child and parent proccess
    pid_t pid = forkpty(&masterFd, NULL, NULL, NULL);

    if ( (pid = fork()) == -1) {
      printf("Failed to fork process");
      return (-1);
    }

    // child proc
    if (pid == 0) {
      close(in_to_child[1]);
      close(out_of_child[0]);

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
        char keyboard_input_buffer[64*8] = {0};
        char childOutputDataBuffer[64*8] = {0};
        char *linedChildOutputDataBuffer[64*8] = {0};




    moveCursor(1,1);
    clearscreen();
    // main while loop
   int setup = 0; 
      while (1) {


        int CHILDOUT_ready=poll(STDIN_CHILD_filedescriptors, 2, -1);
        if (CHILDOUT_ready == -1) {
          printf("poll failed");
       }

        if (STDIN_CHILD_filedescriptors[0].revents & POLLIN) {
          // printing
            memset(childOutputDataBuffer,0,sizeof(childOutputDataBuffer));
            memset(linedChildOutputDataBuffer,0,sizeof(linedChildOutputDataBuffer));
            read(masterFd, childOutputDataBuffer, sizeof(childOutputDataBuffer));
            stringToLines(childOutputDataBuffer,linedChildOutputDataBuffer);

          int i = 0;
               while (linedChildOutputDataBuffer[i] != NULL ) {
                  printf("line %d: %s",i, linedChildOutputDataBuffer[i]);
                  fflush(stdout);
                  free(linedChildOutputDataBuffer[i]);
                  i++;
                 
          }

            
        }

        // get ketboard input
        if (STDIN_CHILD_filedescriptors[1].revents & POLLIN) {
          memset(keyboard_input_buffer,0,sizeof(keyboard_input_buffer));
          read(STDIN_FILENO, keyboard_input_buffer, sizeof(keyboard_input_buffer));

          if (write(masterFd, keyboard_input_buffer, strlen(keyboard_input_buffer)) == -1) {
            printf("Failed to write to child write pipe");
            

        };
        }




       close(in_to_child[1]);
       close(out_of_child[0]);

      }

}

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

    return buffer;
    
  }

