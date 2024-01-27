#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
size_t BUFFERSIZE = 64;

char *PATH[128] = {
    "/bin/"};

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
  // s = strdup(myargv[0]);
  // myargv[0] = strdup(PATH[0]);
  // myargv[0] = strcat(myargv[0], s);

  return myargc;
}


int get_path(char* myargv[]){
  char temp_path[64];
  int i = 0;
  while (1)
  {
    if (PATH[i] = NULL)
      break;
    else {
      strcpy(temp_path, PATH[i]);
      strcat(temp_path, myargv[0]);
      int acc_rc = access(temp_path, X_OK);
      if (acc_rc == 0)  
      {
        myargv[0] = strdup(temp_path);
        return 0;
      }
        }
        i++;
  }
  return -1;
}

int main(int argc, char *argv[]){
  while (1){
    printf("wish> ");
    char *buffer = (char *)malloc(sizeof(BUFFERSIZE));
    size_t n = getline(&buffer, &BUFFERSIZE, stdin);
    char *myargv[128];
    int myargc = parse_input(myargv, buffer, n);
    // handle exit
    if (strcmp(myargv[0], "exit") == 0)
    {
      if (myargc != 1)
      {
        fprintf(stderr, "An error has occurred\n");
      }
      else
      {
        exit(0);
      }
    }
    // handle path
    if (strcmp(myargv[0], "path") == 0)
    {
      for (size_t i = 0; i < myargc - 1; i++)
      {
        PATH[i] = myargv[i + 1];
      }
    }

    // handle cd

    if (strcmp(myargv[0], "cd") == 0)
    {
      if (myargc > 2)
      {
        fprintf(stderr, "An error has occurred\n");
        continue;
      }
      int ret = chdir("$HOME/");
      if (ret == -1)
      {
        fprintf(stderr, "An error has occurred\n");
        continue;
      }
      
    }

    if (get_path(myargv) == -1){
      fprintf(stderr, "An error has occurred\n");
      continue;
    }

    int rc = fork();
    if (rc == 0) { // child process
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
