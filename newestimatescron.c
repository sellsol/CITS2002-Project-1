#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <ctype.h>
//You're going to have to trim these libraries later, but this is what the old project had

#define ALLVALUES		-1
#define ERRORVAL		-2

#define MAX_CMDLINES		20
#define MAX_CMDNAME_LEN		41
#define MAX_FILENAME_LEN	101

//'weekday' is a bad way to decribe this, and vague. meed a better way to describe days of the week
int weekdayToInt(char *day_name) {
	char days[7][4] = {"sun", "mon", "tue", "wed", "thu", "fri", "sat"};
	for (int dayNum = 0; dayNum < 7; dayNum++) {
		if (strcmp(days[dayNum], day_name)) {
			return dayNum;
		}
	}

	return ERRORVAL;
}

//Converts month string to int code
int monthToInt(char *month_name) {
	char months[12][4] = {"jan", "feb", "mar", "apr", "may", "jun", "jul", "aug", "sep", "oct", "nov", "dec"};
	for (int monthNum = 0; monthNum < 12; monthNum++) {
		if (strcmp(months[monthNum], month_name) == 0) {
			return monthNum;
		}
	}

	return ERRORVAL;
}

//Get's the first weekday of the month
int firstDayOfMonth(int month) {	
	struct tm tm;

	memset(&tm, 0, sizeof(tm));

	tm.tm_mday = 1;
	tm.tm_mon = month;
	tm.tm_year = (2022 - 1900);

	mktime(&tm);

	return tm.tm_wday;
}

#define FEBUARY		1
#define APRIL		3
#define JUNE		5
#define SEPTEMBER	8
#define NOVEMBER	10

//Returns the number of days in a month
int GetDaysInMonth(int month) {
	if (month == FEBUARY) {
		return 28;
	} else if (month == APRIL || month == JUNE || month == SEPTEMBER || month == NOVEMBER) {
		return 30;
	} else {
		return 31;
	}
}

//Converts a passed string of data into an integer form
//The mode indicates that this string is a month/week, which pertains to how a string should be deciphered
#define MODE_NONE 	0
#define MODE_WEEK	1
#define MODE_MONTH 	2


int my_atoi(char *datastring, int mode) {
	if (isdigit(datastring[0])) {
		return atoi(datastring);
	} else if (datastring[0] == '*') {
		return ALLVALUES;
	} else {
		//At this point, convert a 3-char string to an integer based off the mode
		switch (mode) {
			case MODE_WEEK:
				return weekdayToInt(datastring);
			case MODE_MONTH:
				return monthToInt(datastring);
			default:
				return ERRORVAL;
		}
	}
}

//
struct {
	int minute;
	int hour;
	int date;
	int month;
	int weekday;
	char name[MAX_CMDNAME_LEN];
	int estimatesID;
} crontabFile[MAX_CMDLINES];

struct {
	char name[MAX_CMDNAME_LEN];
	int runtime;
	int runcount;
} estimatesFile[MAX_CMDLINES];

//Returns whether it succeed or failed to find an available space to put the process
int invokeProcess(int endTime, int *processEndTimes) {
	//Find available space for the process to run
	for (int i = 0; i < MAX_CMDLINES; i++) {
		if (processEndTimes[i] == -1) {
			processEndTimes[i] = endTime;
			return 1;
		}
	}
	return 0;
}

#define CRONTABFILE		0
#define ESTIMATESFILE		1

int readfiles(char *filename, int filetype) {
	FILE *dict = fopen(filename, "r");
	if (dict == NULL) {
		fprintf(stderr, "error: cannot open '%s'\n", filename);
		exit(EXIT_FAILURE);
	}

	char line[MAX_FILENAME_LEN];
	char *token;
	
	int lineIndex = 0;
	while (fgets(line, sizeof line, dict) != NULL) {
		line[strcspn(line, "\r\n")] = 0;

		//Ignore empty lines
		if (strlen(line) == 0) {
			continue;
		}

		token = strtok(line, " ");
		char* timeEstimate;
		//Ignore comment lines
		if (token[0] == '#') {
			continue;
		}

		switch (filetype) {
			case CRONTABFILE:
				crontabFile[lineIndex].minute = my_atoi(token, MODE_NONE);
				token = strtok(NULL, " ");
				crontabFile[lineIndex].hour = my_atoi(token, MODE_NONE);
				token = strtok(NULL, " ");
				crontabFile[lineIndex].date = my_atoi(token, MODE_NONE);
				token = strtok(NULL, " ");
				crontabFile[lineIndex].month = my_atoi(token, MODE_MONTH);
				token = strtok(NULL, " ");
				crontabFile[lineIndex].weekday = my_atoi(token, MODE_WEEK);
				token = strtok(NULL, " ");
				strcpy(crontabFile[lineIndex].name, token);

				//Test if there is any errors when reading the file
				
				if (crontabFile[lineIndex].minute == ERRORVAL ||
					crontabFile[lineIndex].hour == ERRORVAL ||
					crontabFile[lineIndex].date == ERRORVAL ||
					crontabFile[lineIndex].month == ERRORVAL ||
					crontabFile[lineIndex].weekday == ERRORVAL) {
					
					printf("%d %d %d %d %d", crontabFile[lineIndex].minute, crontabFile[lineIndex].hour, crontabFile[lineIndex].date,
							crontabFile[lineIndex].month, crontabFile[lineIndex].weekday);	
					fprintf(stderr, "Error reading non-comment line '%d': Make sure data is valid\n", lineIndex);
					exit(EXIT_FAILURE);
				}

				break;
			case ESTIMATESFILE:
				strcpy(estimatesFile[lineIndex].name, token);
				token = strtok(NULL, " ");
				estimatesFile[lineIndex].runtime = atoi(timeEstimate);

				for (int j = 0; j < sizeof(estimatesFile)/sizeof(estimatesFile[0]); j++) {
					if (strcmp(crontabFile[lineIndex].name, token) == 0) {
						crontabFile[j].estimatesID = lineIndex;
					}
				}
				break;
		}
		lineIndex++;
	}

	fclose(dict);
	return lineIndex;
}

//Invalid end times are -1. -1 is returned if there are no processes running
int GetNextEndingTime(int *processEndTimes) {
	int min = -1;
	for (int i = 0; i < MAX_CMDLINES; i++) {
		if (min == -1 || processEndTimes[i] < min) {
			min = i;
		}
	}
	return min;
}

int main(int argcount, char *argvalue[]) {
	printf("asda");
	if (argcount != 4) {
		fprintf(stderr, "Error: received %d arguments when there should have been 4", argcount);
		exit(EXIT_FAILURE);
	}

	int crontabLines = readfiles(argvalue[2], CRONTABFILE);
	int estimateLines = readfiles(argvalue[3], ESTIMATESFILE);

	int month = my_atoi(argvalue[1], MODE_MONTH); //Should error check here
	int minutesInMonth = 60 * 24 * GetDaysInMonth(month);

	int currentRunningProcesses = 0;
	int maxRunningProcesses = 0;

	int firstDay = firstDayOfMonth(month);
	
	int processEndTimes[MAX_CMDLINES];

	//Initalise processEndTimes with -1, as no process has begun yet
	for (int i = 0; i < MAX_CMDLINES; i++) {
		processEndTimes[i] = -1;
	}

	//Check every minute in month to see if anything starts, if anything ends
	for (int i = 0; i < minutesInMonth; i++) {

		//Ending processes
		for (int j = 0; j < MAX_CMDLINES; j++) {
			if (processEndTimes[j] == i) {
				processEndTimes[j] = -1;
				currentRunningProcesses -= 1;
				printf("Ended a process at point '%d' at time %d \n", j, i);
			}
		}

		//Can only end a process
		if (currentRunningProcesses == MAX_CMDLINES) {
			continue;
		}
	
		//Checking if any process can be starting
		for (int j = 0; j < crontabLines; j++) {
			int valid = 1; //Default true
			int hour = i / 60;
			int day = i / (60 * 24);
			if (crontabFile[j].date != ALLVALUES) {
				//Test if the day in the current minute is in fileinfo[j].date
				if (crontabFile[j].date != day) {
					valid = 0;
				}
			}
			if (crontabFile[j].weekday != ALLVALUES) {
				//Test if the day in the current minute is a valid weekday
				if ((firstDay + day) % 7 != crontabFile[j].weekday) {
					valid = 0;
				}
			}
			if (crontabFile[j].month != ALLVALUES && crontabFile[j].month != month) {
				valid = 0;
			}
			if (crontabFile[j].hour != ALLVALUES) {
				if (hour % 24 != crontabFile[j].hour) {
					valid = 0;
				}
			}
			if (crontabFile[j].minute != ALLVALUES) {
				if (i % 60 != crontabFile[j].minute) {
					valid = 0;
				}
			}
			if (valid == 1) {
				//Invoke process
				int returnval = invokeProcess(i + estimatesFile[crontabFile[j].estimatesID].runtime, processEndTimes);
				printf("Invoked process at point '%d' at time %d with endtime %d. Process id: %d \n", returnval, i, i + estimatesFile[crontabFile[j].estimatesID].runtime, j);
				currentRunningProcesses += 1;
				estimatesFile[crontabFile[j].estimatesID].runcount += 1;


				if (currentRunningProcesses > maxRunningProcesses) {
					maxRunningProcesses = currentRunningProcesses;
				}
			}
		}
	}

	//We've accumulated the run counts for every 
	int mostRanProgram = 0;
	int cumulativeProcesses = 0;
	for (int i = 0; i < estimateLines; i++) {
		cumulativeProcesses += estimatesFile[i].runcount;
		if (estimatesFile[mostRanProgram].runcount < estimatesFile[i].runcount) {
			mostRanProgram = i;
		}
	}

	printf("%s %d %d", estimatesFile[mostRanProgram].name, cumulativeProcesses, maxRunningProcesses);
}
