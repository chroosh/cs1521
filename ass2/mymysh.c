// mysh.c ... a small shell
// Started by John Shepherd, September 2018
// Completed by Christopher Shi <z5165244>, September/October 2018

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <glob.h>
#include <assert.h>
#include <fcntl.h>
#include "history.h"

// This is defined in string.h
// BUT ONLY if you use -std=gnu99
//extern char *strdup(char *);

// Function forward references

void trim(char *);
int strContains(char *, char *);
char **tokenise(char *, char *);
char **fileNameExpand(char **);
void freeTokens(char **);
char *findExecutable(char *, char **);
int isExecutable(char *);
void prompt(void);

// Global Constants

#define MAXLINE 200
#define FALSE 0
#define TRUE 1

// Global Data

/* none ... unless you want some */


// Main program
// Set up enviroment and then run main loop
// - read command, execute command, repeat

int main(int argc, char *argv[], char *envp[])
{
	pid_t pid;   // pid of child process
	int status;    // return status of child
   char **path; // array of directory names
	int cmdNo;   // command number
   int i;       // generic index

   // set up command PATH from environment variable
   for (i = 0; envp[i] != NULL; i++) {
      if (strncmp(envp[i], "PATH=", 5) == 0) break;
   }
   if (envp[i] == NULL)
      path = tokenise("/bin:/usr/bin",":");
   else
      // &envp[i][5] skips over "PATH=" prefix
      path = tokenise(&envp[i][5],":");
#ifdef DBUG
   for (i = 0; path[i] != NULL;i++)
      printf("path[%d] = %s\n",i,path[i]);
#endif

   // initialise command history
   // - use content of ~/.mymysh_history file if it exists
	cmdNo = initCommandHistory();
	

   // main loop: print prompt, read line, execute command
   char line[MAXLINE];
   prompt();
   while (fgets(line, MAXLINE, stdin) != NULL) {
      trim(line); // remove leading/trailing space
		// DONE if empty command, ignore
		if (strcmp(line, "") == 0) { prompt(); continue; }
		if (strcmp(line, "exit") == 0) break;

		// DONE handle ! history substitution 
		if (line[0] == '!') {
			if (line[1] == '!') {
				strcpy(line, getCommandFromHistory(cmdNo-1));
				trim(line);
				printf ("%s\n", line);
			} else {
				char num_c[MAXLINE];
				for (int i = 0; line[i] != '\0'; i++)
					num_c[i] = line[i+1];

				int num_i = atoi(num_c);
				if (num_i <= 0 || isdigit(line[1]) == FALSE) {
					printf ("Invalid history substitution\n");
					prompt();
					continue;
				}

				if (getCommandFromHistory(num_i) == NULL) {
					printf ("No command #%d\n", num_i);
					prompt();
					continue;
				} else {
					strcpy(line, getCommandFromHistory(num_i));
					trim(line);
					printf ("%s\n", line);
				}
			}
		}
		
		// DONE tokenise
		char **args;
		args = tokenise(line, " ");

		// DONE handle *?[~ filename expansion
		if (line[0] != '?' && fileNameExpand(args) != NULL) {
			args = fileNameExpand(args);
		}

		// DONE check for input/output redirections
		int last_token = 0;
		while (args[last_token] != NULL) {
			last_token++;
		}
		last_token--;

		// if line contains both '>' and '<', invalid i/o
		if (strchr(line, '>') != NULL && strchr(line, '<') != NULL) {
			printf ("Invalid i/o redirection\n");
			freeTokens(args);
			prompt();
			continue;
		}

		// if line contains '>'
		if (strchr(line, '>') != NULL) {
			// if second last arg is not ">" invalid i/o
			if (strcmp(args[last_token-1], ">") != 0) {
				printf ("Invalid i/o redirection\n");
				freeTokens(args);
				prompt();
				continue;
			}

			// if last token is unwritable, permission denied
			char *output = strdup(args[last_token]);
			FILE *f = fopen(output, "w");
			if (f == NULL) {
				printf ("Output redirection: Permission denied\n");
				free(output);
				freeTokens(args);
				prompt();
				continue;
			} else {
				fclose(f);
			}
			free(output);
		}

		// if line contains '<'
		if (strchr(line, '<') != NULL) {
			// if second last arg is not "<", invalid i/o
			if (strcmp(args[last_token-1], "<") != 0) {
				printf ("Invalid i/o redirection\n");
				freeTokens(args);
				prompt();
				continue;
			}

			char *input = strdup(args[last_token]);
			// if last token does not exist in directory
			struct stat buffer;
			if (stat(input, &buffer) == -1) {
				printf ("Input redirection: No such file or directory\n");
				free(input);
				prompt();
				continue;
			} else {
				// if last token is unreadable
				FILE *f = fopen(input, "r");
				if (f == NULL) {
					printf ("Input redirection: Permission denied\n");
					free(input);
					freeTokens(args);
					prompt();
					continue;
				} else {
					fclose(f);
				}
			} 
			free(input);
		}

		// DONE add to command history
		if (findExecutable(*args, path) != NULL) {
			addToCommandHistory(line, cmdNo);
			cmdNo++;
		}
			
		// DONE handle shell built-ins
		if (strcmp(line, "h") == 0 || strcmp(line, "history") == 0) {
			showCommandHistory();
			addToCommandHistory(line, cmdNo);
			cmdNo++;
			freeTokens(args);
			prompt();
			continue;
		}

		if (strcmp(line, "pwd") == 0) {
			char buf[MAXLINE];
			printf ("%s\n", getcwd(buf,sizeof(buf)));
			freeTokens(args);
			prompt();
			continue;
		}
		
		if (strcmp(args[0], "cd") == 0) {
			// if line = "cd"
			if (args[1] == '\0') {
				chdir(getenv("HOME"));
				char buf[MAXLINE];
				printf ("%s\n", getcwd(buf, sizeof(buf)));
				addToCommandHistory(line, cmdNo);
				cmdNo++;

			} else {
				// change to dir
				if (chdir(args[1]) == 0) {
					char buf[MAXLINE];
					printf ("%s\n", getcwd(buf, sizeof(buf)));
					addToCommandHistory(line, cmdNo);
					cmdNo++;
				} else {
					// if chdir failed - no directory under args[1] exists
					printf ("%s: No such file or directory\n", args[1]);
				}
			}
			freeTokens(args);
			prompt();
			continue;
		}


		// DONE run the command
		if ((pid = fork() != 0)) {
			// parent waits
			wait(&status);

			if (findExecutable(*args, path) != NULL) {
				printf ("--------------------\n");
				printf ("Returns %d\n", WEXITSTATUS(status));	
			}

			freeTokens(args);
			prompt();
		} else {
			// child executes
			char *exec;
			if ((exec = findExecutable(*args, path)) == NULL) {
				printf ("%s: Command not found\n", line);
			} else {
				// TODO sort out any redirections
				printf ("Running %s ...\n", exec);
				printf ("--------------------\n");

				int f;
				for (int i = 0; args[i] != NULL; i++) {
					// redirect to output
					if (strcmp(args[i], ">") == 0) {
						f = open(args[i+1], O_WRONLY | O_CREAT);
						dup2(f, 1);
						free(args[i]); args[i] = NULL;
						free(args[i+1]); args[i+1] = NULL;

						if (execve(exec, args, envp) == -1) {
							printf ("%s: unknown type of executable\n", exec);
							return -1;
						}
						return status;
					}
					
					// redirect to input
					if (strcmp(args[i], "<") == 0) {
						f = open(args[i+1], O_RDONLY);
						dup2(f, 0);
						free(args[i]); args[i] = NULL;
						free(args[i+1]); args[i+1] = NULL;

						if (execve(exec, args, envp) == -1) {
							printf ("%s: unknown type of executable\n", exec);
							return -1;
						}
						return status;
					}
				}

				if (execve(exec, args, envp) == -1) {
					printf ("%s: unknown type of executable\n", exec);
					return -1;
				}
			}
			return status;
		}
   }
   saveCommandHistory();
   cleanCommandHistory();
   printf("\n");
   return(EXIT_SUCCESS);
}

// fileNameExpand: expand any wildcards in command-line args
// - returns a possibly larger set of tokens
char **fileNameExpand(char **tokens)
{
	int wildcard_found = FALSE;
	glob_t results;
	
	char string[MAXLINE];
	string[0] = '\0';

	for (int i = 0; tokens[i] != NULL; i++) {
		if (strchr(tokens[i], '*') != NULL || strchr(tokens[i], '?') != NULL ||
			 strchr(tokens[i], '[') != NULL || strchr(tokens[i], '~') != NULL) {

			wildcard_found = TRUE;
			glob(tokens[i], GLOB_NOCHECK|GLOB_TILDE, 0, &results);

			// greater than or equal to 1
			if (results.gl_pathc >= 1 && strcmp(results.gl_pathv[0], tokens[i]) != 0) {
				for (int i = 0; i < results.gl_pathc; i++) {
					strcat(string, results.gl_pathv[i]);
					strcat(string, " ");
				}
		   // less than 1 (0)
			} else {
				continue;
			}
			globfree(&results);
		} else {
			strcat(string, tokens[i]);
			strcat(string, " ");
		}
	}

	if (wildcard_found == TRUE) {
		trim(string);
		char **new_tokens;
		new_tokens = tokenise(string, " ");
		return new_tokens;
	} else {
		return NULL;
	}
}

// findExecutable: look for executable in PATH
char *findExecutable(char *cmd, char **path)
{
      char executable[MAXLINE];
      executable[0] = '\0';
      if (cmd[0] == '/' || cmd[0] == '.') {
         strcpy(executable, cmd);
         if (!isExecutable(executable))
            executable[0] = '\0';
      }
      else {
         int i;
         for (i = 0; path[i] != NULL; i++) {
            sprintf(executable, "%s/%s", path[i], cmd);
            if (isExecutable(executable)) break;
         }
         if (path[i] == NULL) executable[0] = '\0';
      }
      if (executable[0] == '\0')
         return NULL;
      else
         return strdup(executable);
}

// isExecutable: check whether this process can execute a file
int isExecutable(char *cmd)
{
   struct stat s;
   // must be accessible
   if (stat(cmd, &s) < 0)
      return 0;
   // must be a regular file
   //if (!(s.st_mode & S_IFREG))
   if (!S_ISREG(s.st_mode))
      return 0;
   // if it's owner executable by us, ok
   if (s.st_uid == getuid() && s.st_mode & S_IXUSR)
      return 1;
   // if it's group executable by us, ok
   if (s.st_gid == getgid() && s.st_mode & S_IXGRP)
      return 1;
   // if it's other executable by us, ok
   if (s.st_mode & S_IXOTH)
      return 1;
   return 0;
}

// tokenise: split a string around a set of separators
// create an array of separate strings
// final array element contains NULL
char **tokenise(char *str, char *sep)
{
   // temp copy of string, because strtok() mangles it
   char *tmp;
   // count tokens
   tmp = strdup(str);
   int n = 0;
   strtok(tmp, sep); n++;
   while (strtok(NULL, sep) != NULL) n++;
   free(tmp);
   // allocate array for argv strings
   char **strings = malloc((n+1)*sizeof(char *));
   assert(strings != NULL);
   // now tokenise and fill array
   tmp = strdup(str);
   char *next; int i = 0;
   next = strtok(tmp, sep);
   strings[i++] = strdup(next);
   while ((next = strtok(NULL,sep)) != NULL)
      strings[i++] = strdup(next);
   strings[i] = NULL;
   free(tmp);
   return strings;
}

// freeTokens: free memory associated with array of tokens
void freeTokens(char **toks)
{
   for (int i = 0; toks[i] != NULL; i++)
      free(toks[i]);
   free(toks);
}

// trim: remove leading/trailing spaces from a string
void trim(char *str)
{
   int first, last;
   first = 0;
   while (isspace(str[first])) first++;
   last  = strlen(str)-1;
   while (isspace(str[last])) last--;
   int i, j = 0;
   for (i = first; i <= last; i++) str[j++] = str[i];
   str[j] = '\0';
}

// strContains: does the first string contain any char from 2nd string?
int strContains(char *str, char *chars)
{
   for (char *s = str; *s != '\0'; s++) {
      for (char *c = chars; *c != '\0'; c++) {
         if (*s == *c) return 1;
      }
   }
   return 0;
}

// prompt: print a shell prompt
// done as a function to allow switching to $PS1
void prompt(void)
{
   printf("mymysh$ ");
}
