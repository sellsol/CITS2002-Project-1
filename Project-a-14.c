// CITS2002 Project 1 2022
// Student1: 	23643117	Leow	Hai Ning
// Student2:	23372032	Frayne	James

// program name: estimatecron.c
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
#define MAX_SIM_PROCS		20

#define ALLVALUES 		-2
#define ERRORVALUE		-1

#define MODE_ESTIMATES		1
#define MODE_CRONTAB		2
#define MODE_NONE 		0
#define MODE_MONTH		1
#define MODE_WEEKDAY		2

enum monthVal {JANUARY, FEBRUARY, MARCH, APRIL, MAY, JUNE, JULY, AUGUST, SEPTEMBER, OCTOBER, NOVEMBER, DECEMBER};
enum weekdayVal {SUNDAY, MONDAY, TUESDAY, WEDNESDAY, THURSDAY, FRIDAY, SATURDAY};

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
void checkStrtok(char *token, int mode, int lineNum, char *fullLine);
int myAtoi(char *dataString, int mode, int lowerBound, int upperBound);
int monthToInt(char *monthName);
int weekdayToInt(char *dayName);
void checkFileVals(int mode, int lineNum, int dataid, char *fullLine);

void simMonth(int *mostRunProgram, int *totalRunProcs, int *maxRunningProcs, int monthProvided);
int daysInMonth(int month);
int firstDayOfMonth(int month);
int invokeProcess(int endTime, int *processEndTimes);


void testfunc() // to be removed later
{
	for (int n = 0; n < crontabLines; n++) {
		printf("%i %i %i %i %i %i\n",
			crontabFile[n].minute,
			crontabFile[n].hour,
			crontabFile[n].date,
			crontabFile[n].month,
			crontabFile[n].weekday,
			crontabFile[n].estimatesID
			);
	}

	for (int n = 0; n < estimatesLines; n++) {
		printf("%s %i %i\n",
			estimatesFile[n].name,
			estimatesFile[n].runTime,
			estimatesFile[n].runCount
			);
	}
}


int main(int argcount, char *argvalue[]) 
{
	// error handling: incorrect number of command line arguments
    if (argcount != 4) {
		fprintf(stderr, "error: received %d arguments when there should have been 4\n", argcount);
		exit(EXIT_FAILURE);
	}

	// convert month argument and error handling for validity
	int monthProvided = myAtoi(argvalue[1], MODE_MONTH, 0, 11);
	if (monthProvided == ERRORVALUE) {
		fprintf(stderr, "error: month provided '%s' is not a valid month\n", argvalue[1]);
		exit(EXIT_FAILURE);
	}

	// read files and convert their values
	estimatesLines = readFiles(argvalue[3], MODE_ESTIMATES);
	crontabLines = readFiles(argvalue[2], MODE_CRONTAB);

	// create values needed for output and pointers to pass to simulate month function
	int mostRunProgram = -1;
        int *pMostRunProgram = &mostRunProgram;
	int totalRunProcs = 0;
        int *pTotalRunProcs = &totalRunProcs;
	int maxRunningProcs = 0;
        int *pMaxRunningProcs = &maxRunningProcs;

	// simulate month and get calculations
	simMonth(pMostRunProgram, pTotalRunProcs, pMaxRunningProcs, monthProvided);

	testfunc(); //test
	printf("month %i\n", monthProvided); //test
	printf("first day's weekday: %i\n", firstDayOfMonth(monthProvided)); // test
	printf("num days in month: %i\n", daysInMonth(monthProvided)); // test
	printf("\n"); //test


	printf("%s %d %d\n", estimatesFile[mostRunProgram].name, totalRunProcs, maxRunningProcs);
	exit(EXIT_SUCCESS);
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
	char fullLine[MAX_FILENAME_LEN];
	char *token;
	
	int lineNum = 0; // line in file, for reporting errors
	int dataid = 0; // non-comment line in file for storing
	while (fgets(line, sizeof line, dict) != NULL) {
		lineNum++;

		// strip and save original line into fullLine, get first string token
		line[strcspn(line, "\r\n")] = 0;
		strcpy(fullLine, line);

		token = strtok(line, " \t");
		checkStrtok(token, mode, lineNum, fullLine);

		// ignore comment lines
		if (token[0] == '#') {
			continue;
		}

        // store string tokens into arrays depending on if it is the crontab file or estimates file
		switch (mode) {
			case MODE_CRONTAB:
				crontabFile[dataid].minute = myAtoi(token, MODE_NONE, 0, 59);

				token = strtok(NULL, " \t");
				checkStrtok(token, mode, lineNum, fullLine);
				crontabFile[dataid].hour = myAtoi(token, MODE_NONE, 0, 23);

				token = strtok(NULL, " \t");
				checkStrtok(token, mode, lineNum, fullLine);
				crontabFile[dataid].date = myAtoi(token, MODE_NONE, 1, 31); // bound for specific month checked later
				
				token = strtok(NULL, " \t");
				checkStrtok(token, mode, lineNum, fullLine);
				crontabFile[dataid].month = myAtoi(token, MODE_MONTH, 0, 11);

				token = strtok(NULL, " \t");
				checkStrtok(token, mode, lineNum, fullLine);
				crontabFile[dataid].weekday = myAtoi(token, MODE_WEEKDAY, 0, 6);

				token = strtok(NULL, " \t");
				checkStrtok(token, mode, lineNum, fullLine);

				checkFileVals(mode, lineNum, dataid, fullLine); // check values for validity

				// links crontab line to estimates line. Allows multiple processes for same estimates line
				for (int j = 0; j < estimatesLines; j++) {
					if (strcmp(token, estimatesFile[j].name) == 0) {
						crontabFile[dataid].estimatesID = j;
						break;
					}
				}

				break;
			case MODE_ESTIMATES:
				strcpy(estimatesFile[dataid].name, token);

				token = strtok(NULL, " \t");
				checkStrtok(token, mode, lineNum, fullLine);
				estimatesFile[dataid].runTime = myAtoi(token, MODE_NONE, 1, 44640);

				checkFileVals(mode, lineNum, dataid, fullLine); // check runtime for validity

				break;
		}
		dataid++;
	}

	fclose(dict);
	return dataid;
}

// error handling/check for if a string token returned by strtok() read from the files is NULL - blank line/ not enough columns
// param token: the token returned by strtok()
// param mode: if the file is the crontab file (MODE_CRONTAB) or estimates file (MODE_ESTIMATES)
// param lineNum: the number of this line in the file (including comment lines, starting from 1)
// param fullLine: the original string line from the file that the token comes from
void checkStrtok(char *token, int mode, int lineNum, char *fullLine)
{
	if (token == NULL) {
		if (mode == MODE_CRONTAB) {
			fprintf(stderr, "error: line %d incomplete in crontab file: '%s'\n", lineNum, fullLine);
			exit(EXIT_FAILURE);
		}
		if (mode == MODE_ESTIMATES) {
			fprintf(stderr, "error: line %d incomplete in estimates file: '%s'\n", lineNum, fullLine);
			exit(EXIT_FAILURE);
		}
	} 
}

// converts data values in the crontab file to an integer, from either a numeric argument, *, or string name
// param datastring: the string data to convert
// param mode: if the datastring represents a weekday (MODE_WEEKDAY), month (MODE_MONTH) or neither (MODE_NONE
// param lowerBound, upperBound: 2 inclusive bounds to determine what the value should be between
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
		// datastring is a 3-char word, convert to an integer based off the mode
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
		if (strcmp(days[dayNum], dayName) == 0) {
			return dayNum;
		}
	}

	return ERRORVALUE;
}

// error handling/checks for if currently stored values are errorvalues, for both files
// param mode: if the file is the crontab file (MODE_CRONTAB) or estimates file (MODE_ESTIMATES)
// param lineNum: the number of this line in the file (including comment lines, starting from 1)
// param dataid: the number of this line in the file array / number of this line in file not including comments
void checkFileVals(int mode, int lineNum, int dataid, char *fullLine)
{
	switch (mode) {
		case MODE_CRONTAB:
			// error handling: month/weekday name not found/ required values not an int/ values out of bounds
			if (crontabFile[dataid].minute == ERRORVALUE ||
				crontabFile[dataid].hour == ERRORVALUE ||
				crontabFile[dataid].date == ERRORVALUE ||
				crontabFile[dataid].month == ERRORVALUE ||
				crontabFile[dataid].weekday == ERRORVALUE) {

				fprintf(stderr, "error: line %d of crontab file contains invalid values for fields they represent: '%s'\n", lineNum, fullLine);
				exit(EXIT_FAILURE);
			}

			// error handling: since we receive month after the date, we need to check the date for validity in bounds now again after myAtoi
			int numDaysInMonth = daysInMonth(crontabFile[dataid].month);
			if (numDaysInMonth < crontabFile[dataid].date) {
				fprintf(stderr, "error: line %d of crontab file contains invalid values for fields they represent: '%s'\n", lineNum, fullLine);
				exit(EXIT_FAILURE);
			}

			break;
		case MODE_ESTIMATES:
			// error handling: runTime is invalid - 0 or negative
			if (estimatesFile[dataid].runTime == ERRORVALUE) {
				fprintf(stderr, "error: invalid runtime of '%d' line '%d' of estimates file\n", estimatesFile[dataid].runTime, lineNum);
			}
			break;
	}
}


// loops to simulate cron checking and execution throughout the month, recording calculations to the pointer parameters
// param mostRunProgram: pointer to the estimatesFile index/ command id of the most frequently run command/program that month
// param totalRunProcs: pointer to the total processes run during the month
// param maxRunningProcs: pointer to the maximum number of processes running simultaneously during month
// param monthProvided: the given month to calculate for
void simMonth(int *mostRunProgram, int *totalRunProcs, int *maxRunningProcs, int monthProvided)
{
	int currentRunningProcs = 0;
	int processEndTimes[MAX_SIM_PROCS];
	
    // initialising processEndTimes to have no running processes
	for (int pid = 0; pid < MAX_SIM_PROCS; pid++) {
		processEndTimes[pid] = -1;
	}

    // looping through the month by minutes
    int minutesInMonth = 60 * 24 * daysInMonth(monthProvided);
	for (int now = 0; now < minutesInMonth; now++) {
		// ending processes that terminate now, decrementing relevant counts
		for (int pid = 0; pid < MAX_SIM_PROCS; pid++) {
			if (processEndTimes[pid] == now) {
				processEndTimes[pid] = -1;
				currentRunningProcs--;
				printf("ended a process with process id %d at time %d\n", pid, now);
			}
		}

		// checking if any process can be starting
		for (int cronid = 0; cronid < crontabLines; cronid++) {
			// checking if max number of simultaneous processes already running
			if (currentRunningProcs == MAX_SIM_PROCS) {
				printf("cannot invoke new process, maximum simultaneous processes = %d\n", MAX_SIM_PROCS);
				break;
			}

			// ignore processes not starting now
			int firstDay = firstDayOfMonth(monthProvided);
			int nowMin = now % 60;
			int nowHour = now / 60 % 24;
			int nowDate = (now / 60 / 24) + 1;
			int nowWeekday = (firstDay + (nowDate - 1)) % 7;
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
			printf("invoked process with process id %d at time %d with endtime %d. Command name: %s\n", pid, now, now + runTime, estimatesFile[estimatesID].name);
			
            currentRunningProcs++;
			estimatesFile[estimatesID].runCount++;
			if (currentRunningProcs > *maxRunningProcs) {
				*maxRunningProcs = currentRunningProcs;
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
// return: the weekday the first day of the month is on as an int (from current year)
int firstDayOfMonth(int month)
{    
    // get current time as tm for current year
    time_t secondsNow = time(NULL);
    struct tm *currentTime = localtime(&secondsNow);

    // find weekday, using current year
    struct tm firstDay;
    memset(&firstDay, 0, sizeof(firstDay));

    firstDay.tm_mday = 1;
    firstDay.tm_mon = month;
    firstDay.tm_year = currentTime->tm_year;

    mktime(&firstDay);

    return firstDay.tm_wday;
}


// invokes a process by finding available space to run and storing its endTime in the array of processes
// param endTime: the time the process will finish as the number of minutes since the month started
// param processEndTimes: the array storing the endTimes of currently running processes
// return: the processEndTimes index/ process id given to this process
int invokeProcess(int endTime, int *processEndTimes) {
	for (int i = 0; i < MAX_SIM_PROCS; i++) {
		if (processEndTimes[i] == -1) {
			processEndTimes[i] = endTime;
			return i;
		}
	}
	return 0;
}





