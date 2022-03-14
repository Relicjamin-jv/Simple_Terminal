/*
 Author: Collin Campbell
 Course: COMP 340, Operating Systems
 Date: 11 Feb. 2022
 Description: This file implements the
 Shell program
 Compile with: gcc -o shell shell.c
 Run with: ./shell
*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <signal.h>

struct stat stats; //can i use the same buffer or do I need a new buffer each time?

int shell_change_dir(char *dir_path)
{
  // use the chdir() system call to change the current directory
  int success = chdir(dir_path);
  return success;
}

int shell_file_exists(char *file_path)
{
  // use the stat() system call to check if a file exists
  if (stat(file_path, &stats) != 0)
  {
    return -1;
  }
  return 0; //success
}

/**
 * WORKS, tok var has each var in path. can check for file in each one. 
 * NEED - append filename to the end of each path var, and check to see if the file exist, and then return it to the filepath pointer given. 
 * 		  I have no idea what the file_path_size if for?
 */
int shell_find_file(char *file_name, char *file_path, char file_path_size)
{
  // traverse the PATH environment variable to find the absolute path of a file/command
  char pathToSearch[500];              //should never reach this and go over buffer limit
  char *path = strdup(getenv("PATH")); //saves the copy of the path var

  char *tok = path; 
  char *end = path;

  while (tok != NULL)
  {
    //getting it formatted correctly
    strsep(&end, ":");
    strcpy(pathToSearch, tok);
    strcat(pathToSearch, "/");
    strcat(pathToSearch, file_name);
    if (shell_file_exists(pathToSearch) == 0)
    {
      strcpy(file_path, pathToSearch);
      return 0;
    }
    tok = end;
    //checking to see where the file exist or not
  }

  free(path);
  return 1;
}

int shell_execute(char *file_path, char **argv)
{
  // execute the file with the command line arguments
  // use the fork() and exec() system call
  pid_t childpid;
  if ((childpid = fork()) == 0)
  { //child running code
    execvp(file_path, argv);
  }
  wait(NULL);
  return 0;
}

int main(int argc, char *argv[])
{
  // shell_find_file("hello", filePath, 'a');
  // printf("filePATH: %s\n", filePath);
  // printf("running file\n");
  // shell_execute(filePath, NULL);
  int exit = 0;
  char input[1000];        //500 bytes for input
  char filePath[1000];     //buffer of 500 bytes
  char currFilePath[1000]; //to output to terminal about curr path
  char *inputargs[500];

  //run the shell

  /*
   while (!exit)
   {
	1. display prompt and wait for user input
		// generate some prompt 
		// e.g. username@hostname:~/david$ ping 
	
	2. filter out whitespace command 
	
	3. if command line contains non-whitespace characters
	
	4. parse command line
		if the specified command is “exit”, terminate the program taking care to release any allocated resources.
		if the specified command is “cd”
			change the current working directory to the specified directory path using shell_change_dir()
		if the command is specified using an absolute path, exists in the user’s PATH or exists in the current folder
			execute the command using shell_execute()
		else
			report an error message
    }
   */

  while (exit == 0)
  {
    getcwd(currFilePath, 1000);
    int index = 0;
    printf("username@hostname:%s$", currFilePath);
    gets(input);

    if(input[0] == NULL){ //back handed way to just make enter work
      input[0] = 10;
    }

    char *tok;

    tok = strtok(input, " ");

    while (tok != NULL)
    { 
      inputargs[index] = tok;
      tok = strtok(NULL, " "); //still more to the input and telling it to pick up the next token
      index++;
    }

    if (strncmp(inputargs[0], "exit", 4) == 0)
    {
      exit = 1;
      printf("Exiting the shell\n");
    }
    else if (strcmp(inputargs[0], "cd") == 0)
    {
      if (shell_change_dir(inputargs[1]) != 0)
      {
        printf("File path %s does not exist\n", inputargs[1]);
      }
    }else if(input[0] == 10){
    
    }
    else
    {
      if (shell_find_file(inputargs[0], filePath, 'a') == 0 && inputargs[0][0] != '.')
      {
        shell_execute(filePath, inputargs); //run code from the PATH
      }
      else if (shell_file_exists(inputargs[0]) == 0)
      {
        shell_execute(inputargs[0], inputargs); //run code absolute path
      }
    }
  }
}
