// COMP1521 18s2 mysh ... command history
// Implements an abstract data object

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ctype.h>
#include <assert.h>

#include "history.h"

// This is defined in string.h
// BUT ONLY if you use -std=gnu99
//extern char *strdup(const char *s);

// Command History
// array of command lines
// each is associated with a sequence number

#define MAXHIST 20
#define MAXSTR  200

#define HISTFILE ".mymysh_history"

#define FALSE 0
#define TRUE 1

void moveEverythingUp(void);

typedef struct _history_entry {
   int   seqNumber;
   char *commandLine;
} HistoryEntry;

typedef struct _history_list {
   int nEntries;
   HistoryEntry commands[MAXHIST];
} HistoryList;

HistoryList CommandHistory;

// initCommandHistory()
// - initialise the data structure
// - read from .history if it exists
// - returns next cmdNo

int initCommandHistory()
{
	// TODO
	int i = 0;
	FILE *f = fopen(HISTFILE, "r");

	// if file exists => return true
	if (f != NULL) {
		char buf[MAXSTR];
		int seqNo;
		while (fgets(buf, sizeof(buf), f) != NULL) {
			buf[strlen(buf)-1] = '\0';

			char *string = malloc(MAXSTR);
			sscanf(buf, " %3d  %s\n", &seqNo, string);
			
			CommandHistory.commands[i].seqNumber = seqNo;		
			CommandHistory.commands[i].commandLine = strdup(string);
			CommandHistory.nEntries++;
			
			free(string);
			i++;
		}
		seqNo++;
		fclose(f);
		return seqNo;
	} else {
		for (int i = 0; i < MAXHIST; i++) {
			CommandHistory.commands[i].commandLine = NULL;
		}
	}
	
	return 1; 	// return 1 if no file found
}

// addToCommandHistory()
// - add a command line to the history list
// - overwrite oldest entry if buffer is full

void addToCommandHistory(char *cmdLine, int seqNo)
{
   // TODO, criteria for adding

	if (seqNo-1 >= MAXHIST) {
		moveEverythingUp();
		CommandHistory.commands[MAXHIST-1].seqNumber = seqNo;
		CommandHistory.commands[MAXHIST-1].commandLine = strdup(cmdLine);
	} else {
		CommandHistory.commands[seqNo-1].seqNumber = seqNo;
		CommandHistory.commands[seqNo-1].commandLine = strdup(cmdLine);
		CommandHistory.nEntries++;
	}
}

// moveEverythingUp()
// - moves everything up
// - need to free the first one
void moveEverythingUp(void) {
	for (int i = 0; i < MAXHIST; i++) {
		CommandHistory.commands[i].seqNumber = 
			CommandHistory.commands[i+1].seqNumber;
		CommandHistory.commands[i].commandLine = 
			CommandHistory.commands[i+1].commandLine;
	}
}

// showCommandHistory()
// - display the list of 

void showCommandHistory(FILE *outf)
{
   // TODO - currently writes to STDOUT, not a file
	for (int i = 0; i < CommandHistory.nEntries; i++) {
		printf (" %3d  ", CommandHistory.commands[i].seqNumber);
		printf ("%s\n", CommandHistory.commands[i].commandLine);
	}
}

// getCommandFromHistory()
// - get the command line for specified command
// - returns NULL if no command with this number

char *getCommandFromHistory(int cmdNo)
{
	// TODO
	for (int i = 0; i < CommandHistory.nEntries; i++) {
		if (cmdNo == CommandHistory.commands[i].seqNumber) {
			return CommandHistory.commands[i].commandLine;
		}
	}
	return NULL;
}

// saveCommandHistory()
// - write history to $HOME/.mymysh_history

void saveCommandHistory()
{
	FILE *f = fopen(HISTFILE, "w");

	if (f != NULL) {
		for (int i = 0; i < CommandHistory.nEntries; i++) {
			fprintf (f, " %3d  %s\n", CommandHistory.commands[i].seqNumber, 
											  CommandHistory.commands[i].commandLine);
		}
		fclose(f);
	}

}

// cleanCommandHistory
// - release all data allocated to command history

void cleanCommandHistory()
{
   // TODO
	for (int i = 0; i < CommandHistory.nEntries; i++) {
		CommandHistory.commands[i].seqNumber = 0;
		free(CommandHistory.commands[i].commandLine);
		CommandHistory.commands[i].commandLine = NULL;
	}
}

