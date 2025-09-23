#include <stdio.h>
#include <string.h>


int main() {
  while (1) {
  char write_to_me[100] = "";
  while (strcmp(write_to_me , "STOP") != 0) {
    scanf(" %[^\n]", write_to_me);
    printf("Output: %s\n", write_to_me);
    fflush(stdout);
  }
}
}
