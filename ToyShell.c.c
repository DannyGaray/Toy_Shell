/***************************************************************************//**
purpose of this program is a toy shell or console that allows the user to interact with a shell, and to interact with the shell, the user can interact with the shell by typing in commands.
************************************************************************/




#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char* shellname = "myshell";
char* terminator = "<";
char* newNames[10] = {};
char* oldNames[10] = {};
char* storeArgs[10][10] = {};//to store mulitple arguments in seperate occasions
int num = 0;
char* SV_NN[10][10]; //SaveNewNames (SV_NN) stores the path of the executables
int SV_NUM = 0;  //stores the number of executables
/*
Functio Declarations for builtin shell commands:
*/
int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);
int setshellname(char **args); //sets the shell name
int setterminator(char **args); // sets terminator
int setNewNames(char **args);  //sets new names
int listNewNames(char **args); //lists new names
int saveNewNames(char **args);// save New names
int Stop(char **args);
/*
List of builtin commands, followed by their corresponding functions.
*/
char *builtin_str[] = 
{
        "Stop",
        "cd",
        "help",
        "exit",
        "setshellname",
        "setterminator",
        "setNewNames",
        "listNewNames",
        "saveNewNames"
};
int (*builtin_func[]) (char **) = {
        &Stop,
        &lsh_cd,
        &lsh_help,
        &lsh_exit,
        &setshellname,
        &setterminator,
        &setNewNames,
        &listNewNames,
        &saveNewNames
};

int lsh_num_builtins() {
        return sizeof(builtin_str) / sizeof(char *);
}


/*
Builtin function implementations.
*/
/**
@brief Bultin command: change directory.
@param args List of args. args[0] is "cd". args[1] is the directory.
@return Always returns 1, to continue executing.
*/
int lsh_cd(char **args)
{
  if (args[1] == NULL)
  {
    fprintf(stderr, "lsh: expected argument to \"cd\"\n");
  }
  else
  {
    if (chdir(args[1]) != 0)
    {
      perror("lsh");
    }
  }
  return 1;
}
/**
@brief Builtin command: print help.
@param args List of args. Not examined.
@return Always returns 1, to continue executing.
*/
int lsh_help(char **args)
{
        int i;
        printf("Stephen Brennan's LSH\n");
        printf("Type program names and arguments, and hit enter.\n");
        printf("The following are built in:\n");
        for (i = 0; i < lsh_num_builtins(); i++)
        {
                printf(" %s\n", builtin_str[i]);
        }
        printf("Use the man command for information on other programs.\n");
        return 1;
}

/**
   @brief Builtin command: exit.
   @param args List of args.  Not examined.
   @return Always returns 0, to terminate execution.
 */

int Stop (char **args)
{
  return 0;
}
int lsh_exit(char **args)
{
  return 0;
}

int setshellname(char **args)
{
    if (args[1] == NULL)
            shellname = "myshell";
    else
            shellname = args[1];
    return 1;

}

int setterminator(char **args)
{
        if (args[1] == NULL)
                terminator = "<";
        else
                terminator = args[1];
        return 1;
}

int setNewNames(char **args)
{
  if (args[1]== NULL) // if the argument only has 1 it will output an error because there needs to be more than 1
  {
    fprintf(stderr, "To set new name needs at least two argumen\n");
    return 1;
  }
  if (args[2] ==NULL) // user only provides one argument
  {
    int i = 0;
    while (i < num && strcmp(newNames[i], args [1]) !=0) // if the new name is not in the array it will add it
    {
      i++; // increment i
    }
    if (i<num) //newNames[i] match with args[1] which need to delete
    {
      newNames[i]= newNames[num-1]; // replace the newNames[i] with the last element of the array
      oldNames[i]= oldNames[num-1]; // replace the oldNames[i] with the last element of the array
      for (int j = 0; storeArgs[i][j] != NULL; j++) 
      {
        storeArgs[i][j] = storeArgs[num - 1][j];  // replace the storeArgs[i][j] with the last element of the array
      }
      num = num - 1;
    }
  }
  else{
    newNames[num] = args [1]; // if the user provides 2 arguments it will add the first argument to the newNames array
    oldNames[num] = args [2]; // if the user provides 2 arguments it will add
    for (int i = 0; args[i] != NULL; i++)
    {
      storeArgs[num][i] = args[i]; // Store arguments in the 2D array
    }
    num++;
  }
  return 1;
}
 

int listNewNames(char **args)
{
  for (int i = 0; i < num; i++)
  {
    for (int j = 1; storeArgs[i][j] != NULL; j++)
    {
      printf("%s ", storeArgs[i][j]);
    }
    printf("\n");

  for(int j = 1; storeArgs[i][j] != NULL; j++) // storing arguments from storeArgs to SV_NN
    {
      SV_NN[SV_NUM][j] = storeArgs[i][j];
    }
    SV_NUM++;
  }
  return 1;
}
int saveNewNames(char **args)
{
  if (args[1] == NULL)
    {
      fprintf(stderr, "function needs a file to save it in");
    }
      FILE *file = fopen(args[1], "w");
      if (file == NULL)
      {
        perror("unable to open the file");
        return 1;
      }

        for (int i = 0; i < SV_NUM; i++)
          {
            for(int j = 1; SV_NN[i][j] != NULL; j++)
              {
                fprintf(file, "%s", SV_NN[i][j]);
              }
            fprintf(file, "\n");
          }
  fclose(file);

  return 1;
}

  

/**
  @brief Launch a program and wait for it to terminate.
  @param args Null terminated list of arguments (including program).
  @return Always returns 1, to continue execution.
 */


int lsh_launch(char **args)
{
  pid_t pid, wpid;
  int status;

  pid = fork();
  if (pid == 0) 
  {
    // Child process
    if (execvp(args[0], args) == -1) 
    {
      perror("lsh");
    }
    exit(EXIT_FAILURE);
  } 
  else if (pid < 0) {
    // Error forking
    perror("lsh");
  } 
  else 
  {// Parent process
    do 
    {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

/**
   @brief Execute shell built-in or launch program.
   @param args Null terminated list of arguments.
   @return 1 if the shell should continue running, 0 if it should terminate
 */
int lsh_execute(char **args)
{
  int i;

  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }


  for (i=0; i < num; i++)
  {
    if (strcmp(args[0], newNames[i]) ==0)
      args[0]=oldNames[i];
  }

  for (i = 0; i < lsh_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return lsh_launch(args);
}

#define LSH_RL_BUFSIZE 1024
/**
   @brief Read a line of input from stdin.
   @return The line from stdin.
 */
char *lsh_read_line(void)
  {
    int bufsize = LSH_RL_BUFSIZE;
    int position = 0;
    char *buffer = malloc(sizeof(char) * bufsize);
    int c;

    if (!buffer) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

    while (1) {// Read a character
      c = getchar();
    
    // If we hit EOF, replace it with a null character and return.
    if (c == EOF || c == '\n')
    {
      buffer[position] = '\0';
      return buffer;
    }
    else 
    {
      buffer[position] = c;
    }
    position++;

    // If we have exceeded the buffer, reallocate.
    if (position >= bufsize) 
    {
      bufsize += LSH_RL_BUFSIZE;
      buffer = realloc(buffer, bufsize);
      if (!buffer) 
      {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}

#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"
/**
   @brief Split a line into tokens (very naively).
   @param line The line.
   @return Null-terminated array of tokens.
 */
char **lsh_split_line(char *line)
{
  int bufsize = LSH_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token;

  if (!tokens) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, LSH_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;
     if (position >= bufsize) {
      bufsize += LSH_TOK_BUFSIZE;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, LSH_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

/**
   @brief Loop getting input and executing it.
 */
void lsh_loop(void)
{
  char *line;
  char **args;
  int status;

  do {
    printf("%s %s", shellname, terminator);
    line = lsh_read_line();
    args = lsh_split_line(line);
    status = lsh_execute(args);

    //free(line);
    //free(args);
  } while (status);
}

/**
   @brief Main entry point.
   @param argc Argument count.
   @param argv Argument vector.
   @return status code
 */
int main(int argc, char **argv)
{
    // Load config files, if any.
  char command[50];

  //strcpy(command, "ls -1");
  //system(command);
  // Run command loop.
  lsh_loop();
  strcpy(command, "ls -1");
  system(command);
  // Perform any shutdown/cleanup.

  return EXIT_SUCCESS;
}
