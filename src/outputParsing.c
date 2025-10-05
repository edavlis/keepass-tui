#include <string.h>
#include <stdio.h>

// some of these functions are redundant (for now at least), im keeping them anyways bcuz i might  need them later

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



void removeFirstLastLine(char *originalString, char *output) {
  
  const char *firstLine = strchr(originalString, '\n'); 

  if (!firstLine) {
    printf("Not enough lines: there is only 1 line!");
    *output = '\0';
    return;
  }
  firstLine++;

  const char *lastLine = strrchr(originalString, '\n');
  lastLine++;

  if (lastLine == firstLine) {
    printf("Not enough lines: there are only 2 lines!");
    return;
  }
  int i = 0;


  size_t len = lastLine - (firstLine);
  strncpy(output, firstLine, len);
  output[len] = '\0';
}


