#include <stdio.h>
#include <string.h>
#include <unistd.h>


int main(int argc, char *argv[]) {

  
  // create pipes
    int in_to_child[2];
    int out_of_child[2];

    pipe(in_to_child);
    pipe(out_of_child);


  // handle arguments
   // argv[1] = path 
   // use getopt but we'll deal with that later 

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

      dup2(out_of_child[1], STDIN_FILENO);
      dup2(out_of_child[1], STDERR_FILENO);
      dup2(in_to_child[0], STDIN_FILENO);

    
      execlp("./outputter","./outputter");
      close(in_to_child[0]);
      close(out_of_child[1]);
    } else { // parent

      close(in_to_child[0]);
      close(out_of_child[1]);

      char read_buffer[64] = "";
      char buffer[64] = "Hello!\n";

      
      if (write(in_to_child[1], buffer, sizeof(buffer)) == -1) {
        printf("Failed to write to child write pipe");
      };


      if (read(out_of_child[0],read_buffer,sizeof(read_buffer)) == -1) {
        printf("Failed to read from child read pipe");
      } else if (read(out_of_child[0],read_buffer,sizeof(read_buffer)) == EOF) {
        printf("Reached end of input stream");
      }

     close(in_to_child[1]);
     close(out_of_child[0]);

      }
      
    }








