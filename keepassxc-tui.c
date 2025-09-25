#include <stdio.h>
#include <termios.h>
#include <poll.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#include <unistd.h>

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

  // usage
  if (argc < 2) {
    printf("Usage: keepassxc-tui <path-to-database>\n");
    return 1;
  }
  
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
    } else { // parent

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
      do {
        
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
      fgets(buffer, sizeof(buffer), stdin);
      if (write(in_to_child[1], buffer, strlen(buffer)) == -1) {
        printf("Failed to write to child write pipe");
      };
      }


      } while (1);

     close(in_to_child[1]);
     close(out_of_child[0]);

      }
      
    }








      //// read from the output of keepassxc-cli
      //int size_read = read(out_of_child[0],read_buffer,sizeof(read_buffer) -1); 
      //if (size_read > 0) {
      //  read_buffer[size_read] = '\0';
      //printf("%s", read_buffer);
      //fflush(stdout);
      //}
      //if (size_read == -1) {
      //printf("Failed to read from child read pipe");
      //}
      //if (size_read == 0) {
      //  printf("Reached end of input stream");
      //}
