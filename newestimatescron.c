#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <ctype.h>
//You're going to have to trim these libraries later, but this is what the old project had

#define ALLVALUES	-1

#define MAX_CMDLINES		20
#define MAX_CMDNAME_LEN		41
#define MAX_FILENAME_LEN	101

//Tells us how many processes are stored in fileinfo
int fileinfoLength;

struct {
	int minute;
	int hour;
	int date;
	int month;
	int weekday;
	char name[MAX_CMDNAME_LEN];
	int runtime;
} fileinfo[MAX_CMDLINES];

//Need a process array - make sure it's initialised to 0
int processes[MAX_CMDLINES];

//Returns whether it succeeded or failed to find an available space to put the process
bool invokeProcess(int processTypeId) {
	//Find available space for the process to run
	for (int i = 0; i < MAX_CMDLINES; i++) {
		if (processes[i] == 0) {
			processes[i] = processTypeId;	
			return true;
		}
	}
	return false;
}

int main(int argcount, char *argvalue[]) {
	if (argcount != 4) {
		//stderr/print? I dunno
		exit(EXIT_FAILURE);
	}

	for () {

	}
}
