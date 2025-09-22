#include <stdio.h>
#include <string.h>


int main() {
  char write_to_me[100] = "";
  while (strcmp(write_to_me , "STOP") != 0) {
    scanf(" %[^\n]", write_to_me);
    printf("%s\n", write_to_me);
  }
}
