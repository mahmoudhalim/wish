#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
size_t BUFFERSIZE = 64;

char *PATH[] = {
    "/bin/",
    "/usr/bin/"};

int parse_input(char *myargv[], char *buffer, size_t n)
{
  int i = 0;
  int myargc = 0;
  char *s = (char *)malloc(sizeof(strlen(buffer)));

  // remove trailling newline character
  if(buffer[n - 1] == '\n')
  { 
    buffer[n - 1] = '\0';
    n--;
  }
  while (buffer != NULL)
  {
    s = strsep(&buffer, " ");
    if (*s != '\0')
    {
      myargv[i++] = strdup(s);
      myargc++;
    }
  }
  myargv[i] = NULL;
  s = strdup(myargv[0]);
  myargv[0] = strdup(PATH[0]);
  myargv[0] = strcat(myargv[0], s);

  return myargc;
}

int main(int argc, char *argv[]){
  while (1){
    printf("wish> ");
    char *buffer = (char *)malloc(sizeof(BUFFERSIZE));
    size_t n = getline(&buffer, &BUFFERSIZE, stdin);

    int rc = fork();
    if (rc == 0) { // child process
      char *myargv[128];
      int argc = parse_input(myargv, buffer, n);

      if(strcmp(myargv[0],"exit") == 0){
        if(argc > 1)
          fprintf(stderr, "An error has occurred\n");
        else 
          exit(0);
      }
      
      rc = execv(myargv[0], myargv);
      printf("%s ",myargv[0]);
      fprintf(stderr, "An error has occurred\n");
    }
  else{
    // parent 
    rc = wait(NULL);
  }
  }
}
