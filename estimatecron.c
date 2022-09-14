// CITS2002 Project 1 2022
// Student1: 	23643117	Leow	Hai Ning
// Student2:	TBD		Frayne	James

// program name: estimatescron.c
// program description: 
// given a month, a crontab file, and an estimates file,
// simulate the execution of the cron processes throughout the month and
// output the name of the most run command, the total number of processes run, and the maximum simultaneous processes run

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <ctype.h>

#define MAX_CMDLINES 		20
#define MAX_CMDNAME_LEN 	40 + 1
#define MAX_FILENAME_LEN	100 + 1

#define ALLVALUES 		-1
#define ERRORVALUE		-2
#define FEBRUARY 		1
#define APRIL 			3
#define JUNE			5
#define SEPTEMBER		8
#define NOVEMBER		10

#define MODE_ESTIMATES		1
#define MODE_CRONTAB		2
#define MODE_NONE 		0
#define MODE_MONTH		1
#define MODE_WEEKDAY		2

struct {
	int minute;
	int hour;
	int date;
	int month;
	int weekday;
	int estimatesID;
} crontabFile[MAX_CMDLINES];
int crontabLines = 0;

struct {
	char name[MAX_CMDNAME_LEN];
	int runTime;
	int runCount;
} estimatesFile[MAX_CMDLINES];
int estimatesLines = 0;

int readFiles(char *fileName, int mode);
int myAtoi(char *dataString, int mode, int lowerBound, int upperBound);
int monthToInt(char *monthName);
int weekdayToInt(char *dayName);

void simMonth(int *mostRunProgram, int *totalRunProcs, int *maxRunningProcs, int monthProvided);
int daysInMonth(int month);
int firstDayOfMonth(int month);
int invokeProcess(int endTime, int *processEndTimes);

int main(int argcount, char *argvalue[]) 
{
	// error handling: incorrect number of command line arguments
    if (argcount != 4) {
		fprintf(stderr, "Error: received %d arguments when there should have been 4", argcount);
		exit(EXIT_FAILURE);
	}

	// read files and convert arguments and value
	int monthProvided = myAtoi(argvalue[1], MODE_MONTH, 0, 11);
	estimatesLines = readFiles(argvalue[3], MODE_ESTIMATES);
	crontabLines = readFiles(argvalue[2], MODE_CRONTAB);

	// simulate month and get calculations
	int mostRunProgram = -1;
        int *pMostRunProgram = &mostRunProgram;
	int totalRunProcs = 0;
        int *pTotalRunProcs = &totalRunProcs;
	int maxRunningProcs = 0;
        int *pMaxRunningProcs = &maxRunningProcs;
	simMonth(pMostRunProgram, pTotalRunProcs, pMaxRunningProcs, monthProvided);

	printf("%s %d %d\n", estimatesFile[mostRunProgram].name, totalRunProcs, maxRunningProcs);
}

// reads the crontab and estimates files, converting their data values and storing into the struct arrays
// param filename: the filename to read from
// param mode: if the file is the crontab file (MODE_CRONTAB) or estimates file (MODE_ESTIMATES)
// return: the number of lines read into the struct arrays
int readFiles(char *filename, int mode) 
{
	FILE *dict = fopen(filename, "r");

    // error handling: file cannot be read
	if (dict == NULL) {
		fprintf(stderr, "error: cannot open '%s'\n", filename);
		exit(EXIT_FAILURE);
	}

    // looping through text file
	char line[MAX_FILENAME_LEN];
	char *token;
	
	int lineIndex = 0;
	while (fgets(line, sizeof line, dict) != NULL) {
		line[strcspn(line, "\r\n")] = 0;

		//Ignore empty lines
		if (strlen(line) == 0) {
			continue;
		}

		token = strtok(line, " \t");

		//Ignore comment lines
		if (token[0] == '#') {
			continue;
		}

        // store string tokens into arrays depending on if it is the crontab file or estimates file
		switch (mode) {
			case MODE_CRONTAB:
				crontabFile[lineIndex].minute = myAtoi(token, MODE_NONE, 0, 59);
				token = strtok(NULL, " \t");
				crontabFile[lineIndex].hour = myAtoi(token, MODE_NONE, 0, 23);
				token = strtok(NULL, " \t");
				crontabFile[lineIndex].date = myAtoi(token, MODE_NONE, 1, 31);
				token = strtok(NULL, " \t");
				crontabFile[lineIndex].month = myAtoi(token, MODE_MONTH, 0, 11);
				token = strtok(NULL, " \t");
				crontabFile[lineIndex].weekday = myAtoi(token, MODE_WEEKDAY, 0, 6);
				token = strtok(NULL, " \t");

				// error handling: month/weekday name not found or other values not an int
				if (crontabFile[lineIndex].minute == ERRORVALUE ||
					crontabFile[lineIndex].hour == ERRORVALUE ||
					crontabFile[lineIndex].date == ERRORVALUE ||
					crontabFile[lineIndex].month == ERRORVALUE ||
					crontabFile[lineIndex].weekday == ERRORVALUE) {

					fprintf(stderr, "Read line '%d' to ints as: %d %d %d %d %d", lineIndex, crontabFile[lineIndex].minute, crontabFile[lineIndex].hour, crontabFile[lineIndex].date,
							crontabFile[lineIndex].month, crontabFile[lineIndex].weekday);
					exit(EXIT_FAILURE);
				}

				//Since we receive month after the date, we need to check the date for validity now instead of in myAtoi
				int numDaysInMonth = daysInMonth(crontabFile[lineIndex].month);
				if (numDaysInMonth < crontabFile[lineIndex].date) {
					fprintf(stderr, "Invalid day of the month '%d' for month no. '%d'",
							crontabFile[lineIndex].date, crontabFile[lineIndex].month);
					exit(EXIT_FAILURE);
				}

				//If month, day and date are all specifically defined, we need to make sure it's valid
				if (crontabFile[lineIndex].date != ALLVALUES &&
					crontabFile[lineIndex].month != ALLVALUES &&
					crontabFile[lineIndex].weekday != ALLVALUES) {
				
					int firstDayInMonth = firstDayOfMonth(crontabFile[lineIndex].month);	
					int weekdayOfDate = (firstDayInMonth + crontabFile[lineIndex].date - 1) % 7;
					if (weekdayOfDate != crontabFile[lineIndex].weekday) {
						fprintf(stderr, "Month no. %d, Date no. %d, and Weekday no. %d do not match up in the year of 2022",
								crontabFile[lineIndex].month, crontabFile[lineIndex].date, crontabFile[lineIndex].weekday);
						exit(EXIT_FAILURE);
					}
				}

				//Links crontab line to estimate line. This allows many processes to have one estimate
				for (int j = 0; j < estimatesLines; j++) {
					if (strcmp(token, estimatesFile[j].name) == 0) {
						crontabFile[lineIndex].estimatesID = j;
						break;
					}
				}

				break;
			case MODE_ESTIMATES:
				strcpy(estimatesFile[lineIndex].name, token);
				token = strtok(NULL, " ");
				estimatesFile[lineIndex].runTime = myAtoi(token, MODE_NONE, 1, 1000000);

				if (estimatesFile[lineIndex].runTime == ERRORVALUE) {
					fprintf(stderr, "Invalid runtime of '%d' for estimate on line '%d'",
							estimatesFile[lineIndex].runTime, lineIndex);
				}

				break;
		}
		lineIndex++;
	}

	fclose(dict);
	return lineIndex;
}

// converts data values in the crontab file to an integer, from either a numeric argument, *, or string name
// param datastring: the string data to convert
// param mode: if the datastring represents a weekday (MODE_WEEKDAY), month (MODE_MONTH) or neither (MODE_NONE
// param lowerBound, upperBound: two inclusive bounds to determine what the value should be between
// return: the converted int value
int myAtoi(char *datastring, int mode, int lowerBound, int upperBound) 
{
	if (isdigit(datastring[0])) {
		int value = atoi(datastring);
		if (value >= lowerBound && value <= upperBound) {
			return value;
		}
		return ERRORVALUE;
	} 
	else if (datastring[0] == '*') {
		return ALLVALUES;
	} 
	else {
		// datastring is a 3-char string, convert to an integer based off the mode
		switch (mode) {
			case MODE_WEEKDAY:
				return weekdayToInt(datastring);
			case MODE_MONTH:
				return monthToInt(datastring);
			default:
				return ERRORVALUE;
		}
	}
}

// converts the month as a 3-char string to an integer
// param monthName: the month's name as a 3-char string
// return: the converted int value representing the month
int monthToInt(char *monthName)
{
	char months[12][4] = {"jan", "feb", "mar", "apr", "may", "jun", "jul", "aug", "sep", "oct", "nov", "dec"};
	for (int monthNum = 0; monthNum < 12; monthNum++) {
		if (strcmp(months[monthNum], monthName) == 0) {
			return monthNum;
		}
	}

	return ERRORVALUE;
}

// converts the weekday as a 3-char string to an integer
// param dayName: the weekday's name as a 3-char string
// return: the converted int value representing the month
int weekdayToInt(char *dayName)
{
	char days[7][4] = {"sun", "mon", "tue", "wed", "thu", "fri", "sat"};
	for (int dayNum = 0; dayNum < 7; dayNum++) {
		if (strcmp(days[dayNum], dayName)) {
			return dayNum;
		}
	}

	return ERRORVALUE;
}


// loops to simulate cron checking and execution throughout the month, recording calculations to the pointer parameters
// param mostRunProgram: pointer to the estimatesFile index/ command id of the most frequently run command/program that month
// param totalRunProcs: pointer to the total processes run during the month
// param maxRunningProcs: pointer to the maximum number of processes running simultaneously during month
// param monthProvided: the given month to calculate for
void simMonth(int *mostRunProgram, int *totalRunProcs, int *maxRunningProcs, int monthProvided)
{
	int currentRunningProcs = 0;
	int processEndTimes[MAX_CMDLINES];
	
    // initialising processEndTimes to have no running processes
	for (int pid = 0; pid < MAX_CMDLINES; pid++) {
		processEndTimes[pid] = -1;
	}

    // looping through the month by minutes
    int minutesInMonth = 60 * 24 * daysInMonth(monthProvided);
	for (int now = 0; now < minutesInMonth; now++) {
		// ending processes that terminate now, decrementing relevant counts
		for (int pid = 0; pid < MAX_CMDLINES; pid++) {
			if (processEndTimes[pid] == now) {
				processEndTimes[pid] = -1;
				currentRunningProcs--;
				printf("Ended a process with process id %d at time %d \n", pid, now);
			}
		}

		// if max number of processes already running
		if (currentRunningProcs == MAX_CMDLINES) {
			continue;
		}

		// checking if any process can be starting
		for (int cronid = 0; cronid < crontabLines; cronid++) {
			int firstDay = firstDayOfMonth(monthProvided);
			int nowMin = now % 60;
			int nowHour = now / 60 % 24;
			int nowDate = (now / 60 / 24) + 1;
			int nowWeekday = (firstDay + nowDate) % 7;
			if (crontabFile[cronid].date != ALLVALUES && crontabFile[cronid].date != nowDate) {
				continue;
			}
			if (crontabFile[cronid].weekday != ALLVALUES && crontabFile[cronid].weekday != nowWeekday) {
				continue;
			}
			if (crontabFile[cronid].month != ALLVALUES && crontabFile[cronid].month != monthProvided) {
				continue;
			}
			if (crontabFile[cronid].hour != ALLVALUES && crontabFile[cronid].hour != nowHour) {
				continue;
			}
			if (crontabFile[cronid].minute != ALLVALUES && crontabFile[cronid].minute != nowMin) {
				continue;
			}
			
			// invoke processes that are starting now, incrementing relevant counts
			int runTime = estimatesFile[crontabFile[cronid].estimatesID].runTime;
			int pid = invokeProcess(now + runTime, processEndTimes);
			int estimatesID = crontabFile[cronid].estimatesID;
			printf("Invoked process with process id %d at time %d with endtime %d. Command name: %s \n", pid, now, now + runTime, estimatesFile[estimatesID].name);
			
            		currentRunningProcs++;

			estimatesFile[estimatesID].runCount++;
			if (currentRunningProcs > *maxRunningProcs) {
				*maxRunningProcs = currentRunningProcs;
			}

			if (currentRunningProcs == MAX_CMDLINES) {
				break;
			}
		}
	}

    // getting the most frequently run program
	for (int i = 0; i < estimatesLines; i++) {
		*totalRunProcs += estimatesFile[i].runCount;
		if (estimatesFile[*mostRunProgram].runCount < estimatesFile[i].runCount) {
			*mostRunProgram = i;
		}
	}

}

// finding the number of days in a specified month
// param month: the month to calculate for as an integer
// return: the number of days in the month
int daysInMonth(int month) 
{
	if (month == FEBRUARY) {
		return 28;
	} 
	else if (month == APRIL || month == JUNE || month == SEPTEMBER || month == NOVEMBER) {
		return 30;
	} 
	else {
		return 31;
	}
}

// finding the weekday the first day of a specified month is on
// param month: the month to calculate for as an integer
// return: the weekday the first day of the month is on as an int
int firstDayOfMonth(int month)
{
	struct tm tm;

	memset(&tm, 0, sizeof(tm));

	tm.tm_mday = 1;
	tm.tm_mon = month;
	tm.tm_year = (2022 - 1900);

	mktime(&tm);

	return tm.tm_wday;
}


// invokes a process by finding available space to run and storing its endTime in the array of processes
// param endTime: the time the process will finish as the number of minutes since the month started
// param processEndTimes: the array storing the endTimes of currently running processes
// return: the processEndTimes index/ process id given to this process
int invokeProcess(int endTime, int *processEndTimes) {
	for (int i = 0; i < MAX_CMDLINES; i++) {
		if (processEndTimes[i] == -1) {
			processEndTimes[i] = endTime;
			return i;
		}
	}
	return ERRORVALUE;
}







