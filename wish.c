#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
size_t BUFFERSIZE = 64;

char *PATH[128] = {"/bin"};
/*
TODO: handle output redirection
      parallel command
*/
int parse_input(char *myargv[], char *buffer, size_t n)
{
  int i = 0;
  int myargc = 0;
  char *s = (char *)malloc(sizeof(strlen(buffer)));

  // remove trailing newline character
  if (buffer[n - 1] == '\n')
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

  return myargc;
}

char *get_path(char *myargv[])
{
  char *temp_path;
  int i = 0;
  while (1)
  {
    if (PATH[i] == NULL)
      break;
    else
    {
      temp_path = strdup(PATH[i]);
      strcat(temp_path, "/");
      strcat(temp_path, myargv[0]);
      int acc_rc = access(temp_path, X_OK);
      if (acc_rc == 0)
      {
        return temp_path;
      }
    }
    i++;
  }
  return NULL;
}

int main(int argc, char *argv[])
{
  if (argc > 3)
  {
    fprintf(stderr, "An error has occurred\n");
    return 1;
  }

  if (argc == 2)
  {
    FILE *f = fopen(argv[1], "r");
    dup2(fileno(f), STDIN_FILENO);
  }
  while (1)
  {
    if (argc == 1)
    {
      printf("wish> ");
    }

    char *buffer = (char *)malloc(sizeof(BUFFERSIZE));
    size_t n = getline(&buffer, &BUFFERSIZE, stdin);
    char *myargv[128];

    // reach EOF
    if (n == -1)
    {
      exit(0);
    }

    // empty input
    if (n == 1)
    {
      continue;
    }

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
      continue;
    }
    // handle path
    if (strcmp(myargv[0], "path") == 0)
    {
      size_t i = 0;
      while (PATH[i] != NULL)
      {
        PATH[i] = NULL;
        i++;
      }

      for (i = 0; i < myargc - 1; i++)
      {
        PATH[i] = strdup(myargv[i + 1]);
      }

      continue;
    }

    // handle cd

    if (strcmp(myargv[0], "cd") == 0)
    {
      if (myargc != 2)
      {
        fprintf(stderr, "An error has occurred\n");
        continue;
      }
      int ret = chdir(myargv[1]);
      if (ret == -1)
      {
        fprintf(stderr, "An error has occurred\n");
      }
      continue;
    }

    char *command_path = get_path(myargv);
    if (command_path == NULL)
    {
      fprintf(stderr, "An error has occurred\n");
      continue;
    }

    int rc = fork();
    if (rc == 0)
    { // child process
      rc = execv(command_path, myargv);
      fprintf(stderr, "An error has occurred\n");
    }
    else
    {
      // parent
      wait(NULL);
    }
  }
}