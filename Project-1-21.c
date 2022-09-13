// code cleanup and skeleton, below funcs gets all 3 reqs
// not finished - need the simulate month process

// CITS2002 Project 1 2022
// Student1: 	23643117	Leow	Hai Ning
// Student2: 	TBD		Frayne	James

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include <time.h>

#define MAX_CMDLINES 		20
#define MAX_CMDNAME_LEN		40 + 1
#define MAX_FILENAME_LEN	100 + 1
#define ALLVALUES 		-1
#define ERRORRETURN		-2

struct cronLine {
	int minute;
	int hour;
	int date;
	int month;
	int weekday;
	char name[MAX_CMDNAME_LEN];
	int runtime;
} fileinfo[MAX_CMDLINES];

struct cmdStatus {
	time_t nextEventtime;
	int runStatus;
	int runCount;
} cmdStatuses[MAX_CMDLINES];

// declarations for readfiles process
void readfiles(char *filename, int filetype);
int myAtoi(char *datastring, int datafield);
int monthToInt(char *monthname);
int weekdayToInt(char *dayname); 
// declarations for simulate month process
void simMonth(char *mostFreq, int *totalCount, int *maxCount, int monthProvided);
void monthBounds(int monthProvided, time_t *monthStart, time_t *monthEnd);
time_t getNextStarttime(struct cronline info, time_t currenttime);
time_t getNextEndtime(int runtime, time_t startTime);
void mostFreqCommand(char mostFreq);

int main(int argcount, char *argvalue[])
{
  	int monthProvided = myAtoi(argvalue[0], 1);
	readfiles(argvalue[2], 1);
	readfiles(argvalue[3], 2);

	char[MAX_CMDNAME_LEN] mostFreq;
	int totalCount = 0;
	int maxCount = 0;
	simMonth(*mostFreq, *totalCount, *maxCount, monthProvided);

	printf("%s %d %d", mostFreq, totalCount, maxCount);

	exit(EXIT_SUCCESS);
}

// functions for readfiles process
void readfiles(char *filename, int filetype) 
{
	// opening text file
	FILE *dict = fopen(filename, "r");
	if (dict == NULL) {
		// error handling: cannot open file
		printf("Error: cannot open '%s.'\n", filename);
		exit(EXIT_FAILURE);
	}

	// loop over file, convert data values and store into fileinfo struct array
	char line[MAX_FILENAME_LEN];
	char *token;
	int n = 0; // to keep track of place in the fileinfo array
	while (fgets(line, sizeof line, dict) != NULL) {
		line[strcspn(line, "\r\n")] = 0;

		// ignore if empty or comment line
		if (strlen(line) == 0) {
			continue;
		}

		token = strtok(line, " ");
		if (token[0] == '#') {
			continue;
		}

		// switch storing technique based on if crontab or estimates
		switch (filetype) {
			case 1: // crontab file
				fileinfo[n].minute = myAtoi(token, 0);
				token = strtok(NULL, " ");
				fileinfo[n].hour = myAtoi(token, 0);
				token = strtok(NULL, " ");
				fileinfo[n].date = myAtoi(token, 0);
				token = strtok(NULL, " ");
				fileinfo[n].month = myAtoi(token, 1);
				token = strtok(NULL, " ");
				fileinfo[n].weekday = myAtoi(token, 2);
				token = strtok(NULL, " ");
				strcpy(fileinfo[n].name, token);
				break;
			case 2: // estimates file
				for (int i = 0; i < sizeof fileinfo / sizeof fileinfo[0]; i++) {
					if (strcmp(fileinfo[i].name, token) == 0) {
						token = strtok(NULL, " ");
						fileinfo[i].runtime = atoi(token);
						break;
					}
				}
				break;
		}
		n++;
	}

	// close file after loop ends
	fclose(dict);
}

int myAtoi(char *datastring, int datafield)
{
	// convert from string digit or *
	if (isdigit(datastring[0])) {
		return atoi(datastring);
	}
	if (datastring[0] == '*') {
		return ALLVALUES;
	}
	
	// convert from word
	switch (datafield) {
		case 1: // month field
			return monthToInt(datastring);
		case 2: // weekday field
			return weekdayToInt(datastring);
	}

	return ERRORRETURN;
}

int monthToInt(char *monthName) 
{
	// convert from month name to corresponding index
	char months[12][3] = {"jan", "feb", "mar", "apr", "may", "jun", 
		"jul", "aug", "sep", "oct", "nov", "dec"};
	for (int monthNum = 0; monthNum < 12; monthNum++) {
		if (strcmp(months[monthNum], monthName) == 0) {
			return monthNum;
		}
	}

	return ERRORRETURN;
}

int weekdayToInt(char *dayName) 
{
	// convert from weekday name to corresponding index
	char days[7][3] = {"sun", "mon", "tue", "wed", "thu", "fri", "sat"};
	for (int dayNum = 0; dayNum < 7; dayNum++) {
		if (strcmp(days[dayNum], dayName)) {
			return dayNum;
		}
	}

	return ERRORRETURN;
}

// functions to simulate month and get required calculations
void simMonth(char *mostFreq, int *totalCount, int *maxCount, int monthProvided)
{	
	// create struct tm and/or time_t for start of month, know when it ends
	getMonthBounds() // do you need a whole func for this?
	// then increment the tm by a month and get the time_t for both - start and end of loop

	// loop to initialise all the first starttimes (and endttimes?)
	getNextStarttime()
	
	// loop to simulate the month's time and check against the time
	// if times match and runStatus = 1 (running), getNextStarttime and change to idle etc.,
	// if times match and runStatus = 0 (idle), getNextEndtime and change to running etc.
	int currentCount = 0;
	getNextStarttime()
	getNextEndtime()

	// get the most frequently executed command - deal w/ this later
	mostFreqCommand()
}

struct tm getMonthBounds(int monthProvided) 
{
	// plug in start of month details into tm and return it
}

time_t getNextStarttime(struct cronline info, time_t currentTime) // cronline is used for fileinfo
{
	// convert currentTime into struct tm
	// increment struct tm based on cronline info for next starttime
	// convert back into time_t and return
}

time_t getNextEndtime(int duration, time_t startTime) 
{
	// either convert startTime into tm and increment and convert back
	// OR just add number of seconds (from duration mins) and return
}

void mostFreqCommand(char *mostFreq) {}




