#include <string.h>
#include <stdio.h>

// some of these functions are redundant (for now at least), im keeping them anyways bcuz i might  need them later

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void stringToLines(char *originalString, char **linedOutput) {
  if (!originalString  || !linedOutput )  return; 

  int lineIndex = 0;
  char *start = originalString;
  char *newline;

  while ((newline = strchr(start, '\n')) != NULL) {
    size_t len = newline - start + 1; // include newline char
    linedOutput[lineIndex] = malloc(len + 1); // for null term
    if (!linedOutput[lineIndex]) {
      printf("malloc");
      return;
    }
    strncpy(linedOutput[lineIndex], start, len );
    linedOutput[lineIndex][len] = '\0';
    lineIndex++;

    start = newline + 1;
        }

  if (*start != '\0') {
    linedOutput[lineIndex] = strdup(start);
    lineIndex++;
  }
  linedOutput[lineIndex] = NULL;
}





//void checkForDatabase(char **lines, char *databasePrompt) { // i know you can check just hte first and last lines but i want to be sure, ill write a second function that does the afformentioned
//  int i = 0;
//  while (lines[i] != NULL) {
//    
//    if (strstr(lines[i], databasePrompt)  ) {
//      lines[i] = "";
//    }
//    i++;
//  }
//}
//
//
//
//void removeFirstLastLine(char *originalString, char *output) {
//  
//  const char *firstLine = strchr(originalString, '\n'); 
//
//  if (!firstLine) {
//    printf("Not enough lines: there is only 1 line!");
//    *output = '\0';
//    return;
//  }
//  firstLine++;
//
//  const char *lastLine = strrchr(originalString, '\n');
//  lastLine++;
//
//  if (lastLine == firstLine) {
//    printf("Not enough lines: there are only 2 lines!");
//    return;
//  }
//  int i = 0;
//
//
//  size_t len = lastLine - (firstLine);
//  strncpy(output, firstLine, len);
//  output[len] = '\0';
//}


