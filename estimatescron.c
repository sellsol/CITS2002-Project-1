// CITS2002 Project 1 2022
// Student1: 	23643117	Leow	Hai Ning
// Student2:	TBD		Frayne	James

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
	char name[MAX_CMDNAME_LEN];
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
int myAtoi(char *dataString, int mode);
int monthToInt(char *monthName);
int weekdayToInt(char *dayName);

void simMonth(int *mostRunProgram, int *totalRunProcs, int *maxRunningProcs, int monthProvided);
int daysInMonth(int month);
int firstDayOfMonth(int month);
int invokeProcess(int endTime, int *processEndTimes);

int main(int argcount, char *argvalue[]) 
{
	if (argcount != 4) {
		fprintf(stderr, "Error: received %d arguments when there should have been 4", argcount);
		exit(EXIT_FAILURE);
	}

	// read files and convert arguments and value
	int monthProvided = myAtoi(argvalue[1], MODE_MONTH);
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

int readFiles(char *filename, int mode) 
{
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

		switch (mode) {
			case MODE_CRONTAB:
				crontabFile[lineIndex].minute = myAtoi(token, MODE_NONE);
				token = strtok(NULL, " ");
				crontabFile[lineIndex].hour = myAtoi(token, MODE_NONE);
				token = strtok(NULL, " ");
				crontabFile[lineIndex].date = myAtoi(token, MODE_NONE);
				token = strtok(NULL, " ");
				crontabFile[lineIndex].month = myAtoi(token, MODE_MONTH);
				token = strtok(NULL, " ");
				crontabFile[lineIndex].weekday = myAtoi(token, MODE_WEEKDAY);
				token = strtok(NULL, " ");

				//Test if there is any errors when reading the file

				if (crontabFile[lineIndex].minute == ERRORVALUE ||
					crontabFile[lineIndex].hour == ERRORVALUE ||
					crontabFile[lineIndex].date == ERRORVALUE ||
					crontabFile[lineIndex].month == ERRORVALUE ||
					crontabFile[lineIndex].weekday == ERRORVALUE) {

					printf("%d %d %d %d %d", crontabFile[lineIndex].minute, crontabFile[lineIndex].hour, crontabFile[lineIndex].date,
							crontabFile[lineIndex].month, crontabFile[lineIndex].weekday);
					fprintf(stderr, "Error reading non-comment line '%d': Make sure data is valid\n", lineIndex);
					exit(EXIT_FAILURE);
				}

				for (int j = 0; j < sizeof(estimatesFile)/ sizeof(estimatesFile[0]); j++) {
					if (strcmp(token, estimatesFile[j].name) == 0) {
						crontabFile[lineIndex].estimatesID = j;
						break;
					}
				}

				break;
			case MODE_ESTIMATES:
				strcpy(estimatesFile[lineIndex].name, token);
				token = strtok(NULL, " ");
				estimatesFile[lineIndex].runTime = atoi(token);

				break;
		}
		lineIndex++;
	}

	fclose(dict);
	return lineIndex;
}


int myAtoi(char *datastring, int mode) 
{
	if (isdigit(datastring[0])) {
		return atoi(datastring);
	} 
	else if (datastring[0] == '*') {
		return ALLVALUES;
	} 
	else {
		//At this point, convert a 3-char string to an integer based off the mode
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

void simMonth(int *mostRunProgram, int *totalRunProcs, int *maxRunningProcs, int monthProvided)
{
	int minutesInMonth = 60 * 24 * daysInMonth(monthProvided);
	int currentRunningProcs = 0;
	int processEndTimes[MAX_CMDLINES];
	//int firstDay
	
	for (int pid = 0; pid < MAX_CMDLINES; pid++) {
		processEndTimes[pid] = -1;
	}

	for (int now = 0; now < minutesInMonth; now++) {
		// ending processes
		for (int pid = 0; pid < MAX_CMDLINES; pid++) {
			if (processEndTimes[pid] == now) {
				processEndTimes[pid] = -1;
				currentRunningProcs--;
				printf("Ended a process with process id '%d' at time %d \n", pid, now);
			}
		}

		// max processes already running
		if (currentRunningProcs == MAX_CMDLINES) {
			continue;
		}

		// checking if any process can be starting
		for (int cronid = 0; cronid < crontabLines; cronid++) {
			//int valid = 1;// default true
			int firstDay = firstDayOfMonth(monthProvided);
			int nowMin = now % 60;
			int nowHour = now / 60 % 24;
			int nowDate = now / 60 / 24;
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
			
			// invoke process
			int runTime = estimatesFile[crontabFile[cronid].estimatesID].runTime;
			int pid = invokeProcess(now + runTime, processEndTimes);
			int estimatesID = crontabFile[cronid].estimatesID;
			printf("Invoked process with process id '%d' at time %d with endtime %d. Command id: %d \n", pid, now, now + runTime, estimatesID);
			currentRunningProcs += 1;
			estimatesFile[estimatesID].runCount++;

			if (currentRunningProcs > *maxRunningProcs) {
				*maxRunningProcs = currentRunningProcs;
			}
		}
	}

	for (int i = 0; i < estimatesLines; i++) {
		*totalRunProcs += estimatesFile[i].runCount;
		if (estimatesFile[*mostRunProgram].runCount < estimatesFile[i].runCount) {
			*mostRunProgram = i;
		}
	}

}

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

int invokeProcess(int endTime, int *processEndTimes) {
	//Find available space for the process to run
	for (int i = 0; i < MAX_CMDLINES; i++) {
		if (processEndTimes[i] == -1) {
			processEndTimes[i] = endTime;
			return i;
		}
	}
	return 0;
}
