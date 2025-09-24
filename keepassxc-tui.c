#include <stdio.h>
#include <string.h>
#include <unistd.h>

void read_from_keepassxc(char read_buffer[4096], int read_buffer_size,  char buffer[4096], int buffer_size, int out_of_child[2], int in_to_child[2]) {

      int size_read = read(out_of_child[0],read_buffer,read_buffer_size -1); 
      if (size_read > 0) {
        read_buffer[size_read] = '\0';
      printf("%s", read_buffer);
      fflush(stdout);
      }
      if (size_read == -1) {
      printf("Failed to read from child read pipe");
      }
      if (size_read == 0) {
        printf("Reached end of input stream");
      }
}


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

      dup2(out_of_child[1], STDOUT_FILENO); // redirect child STDOUT with out of child pipe
      dup2(out_of_child[1], STDERR_FILENO); // redirect child STDOUT with out of child pipe
      dup2(in_to_child[0], STDIN_FILENO); // CHILD STDINPUT now comes froem the intochild pipe

    
      execlp("keepassxc-cli","keepassxc-cli", "open" , "./Passwords.kbdx" , NULL);
      close(in_to_child[0]);
      close(out_of_child[1]);
    } else { // parent

      // close file descriptors
      close(in_to_child[0]);
      close(out_of_child[1]);

      // make buffers to write to
      char read_buffer[64*64] = "";
      char buffer[64*64] = "";
      // read_from_keepassxc(read_buffer,sizeof(read_buffer), buffer,sizeof(buffer), out_of_child, in_to_child);
      //
      do {
        
      // read from the output of keepassxc-cli
      read_from_keepassxc(read_buffer,sizeof(read_buffer), buffer,sizeof(buffer), out_of_child, in_to_child);
     

      // get ketboard input
      fgets(buffer, sizeof(buffer), stdin);
      if (write(in_to_child[1], buffer, strlen(buffer)) == -1) {
        printf("Failed to write to child write pipe");
      };
      fflush(stdin);

      read_from_keepassxc(read_buffer,sizeof(read_buffer), buffer,sizeof(buffer), out_of_child, in_to_child);

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
