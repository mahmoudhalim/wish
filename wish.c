#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
size_t BUFFERSIZE = 64;

char *PATH[128] = {"/bin"};
int parse_commands(char *commands[], char *buffer, size_t n) {
  char *s = (char *)malloc(sizeof(strlen(buffer)));
  int i = 0;
  int command_count = 0;
  // remove trailling newline character
  if (buffer[n - 1] == '\n') {
    buffer[n - 1] = '\0';
    n--;
  }
  while (buffer != NULL) {
    s = strsep(&buffer, "&");
    if (*s != '\0') {
      commands[i++] = strdup(s);
      command_count++;
    }
  }
  return command_count;
}
int parse_input(char *myargv[], char *buffer, size_t n) {
  int i = 0;
  int myargc = 0;
  char *s = (char *)malloc(sizeof(strlen(buffer)));

  while (buffer != NULL) {
    s = strsep(&buffer, " ");
    if (*s != '\0') {
      myargv[i++] = strdup(s);
      myargc++;
    }
  }
  myargv[i] = NULL;
  return myargc;
}

char *get_path(char *myargv[]) {
  char *temp_path;
  int i = 0;
  while (1) {
    if (PATH[i] == NULL)
      break;
    else {
      temp_path = strdup(PATH[i]);
      strcat(temp_path, "/");
      strcat(temp_path, myargv[0]);
      int acc_rc = access(temp_path, X_OK);
      if (acc_rc == 0) {
        return temp_path;
      }
    }
    i++;
  }
  return NULL;
}

void handle_redirection(char *myargv[]) {
  for (int i = 0; myargv[i] != NULL; i++) {
    if (strcmp(myargv[i], ">")) {
      continue;
    }
    if (myargv[i + 1] == NULL || myargv[i + 2] != NULL) {
      fprintf(stderr, "An error has occurred\n");
      exit(1);
    }
    FILE *output = fopen(myargv[i + 1], "w+");
    dup2(fileno(output), STDOUT_FILENO);
    myargv[i] = NULL;
    myargv[i + 1] = NULL;
    break;
  }
}

int handle_builtin(char *myargv[], int myargc) {
  // handle exit
  if (strcmp(myargv[0], "exit") == 0) {
    if (myargc != 1) {
      fprintf(stderr, "An error has occurred\n");
      return 0;
    } else {
      exit(0);
    }
  }
  // handle path
  if (strcmp(myargv[0], "path") == 0) {
    size_t i = 0;
    while (PATH[i] != NULL) {
      PATH[i] = NULL;
      i++;
    }

    for (i = 0; i < myargc - 1; i++) {
      PATH[i] = strdup(myargv[i + 1]);
    }

    return 0;
  }

  // handle cd
  if (strcmp(myargv[0], "cd") == 0) {
    if (myargc != 2) {
      fprintf(stderr, "An error has occurred\n");
      return 0;
    }
    int ret = chdir(myargv[1]);
    if (ret == -1) {
      fprintf(stderr, "An error has occurred\n");
      return 0;
    }
    return 0;
  }
  return 1;
}

int main(int argc, char *argv[]) {
  if (argc > 2) {
    fprintf(stderr, "An error has occurred\n");
    exit(1);
  }

  if (argc == 2) {
    FILE *f = fopen(argv[1], "r");
    if (f == NULL) {
      fprintf(stderr, "An error has occurred\n");
      exit(1);
    }
    dup2(fileno(f), STDIN_FILENO);
  }
  char *buffer = (char *)malloc(sizeof(BUFFERSIZE));

  while (1) {
    if (argc == 1) {
      printf("wish> ");
    }

    size_t n = getline(&buffer, &BUFFERSIZE, stdin);
    char *myargv[128];
    char *commands[128];
    // reach EOF
    if (n == -1) {
      exit(0);
    }
    int command_count = parse_commands(commands, buffer, n);
    pid_t rc[command_count];

    for (int i = 0; i < command_count; i++) {
      int myargc = parse_input(myargv, commands[i], n);
      // empty input
      if (myargc == 0) {
        continue;
      }
      if (handle_builtin(myargv, myargc) == 0)
        continue;
      char *command_path = get_path(myargv);
      if (command_path == NULL) {
        fprintf(stderr, "An error has occurred\n");
        continue;
      }
      rc[i] = fork();
      if (rc[i] == 0) {
        // handle redirection
        handle_redirection(myargv);
        execv(command_path, myargv);
        fprintf(stderr, "An error has occurred\n");
      }
    }
    for (int i = 0; i < command_count; i++) {
      waitpid(rc[i], NULL, 0);
    }
  }
}
