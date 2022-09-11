#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <ctype.h>
//You're going to have to trim these libraries later, but this is what the old project had

#define ALLVALUES		-1

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

int processEndTimes[MAX_CMDLINES];

//Returns whether it succeeded or failed to find an available space to put the process
int invokeProcess(int endTime) {
	//Find available space for the process to run
	for (int i = 0; i < MAX_CMDLINES; i++) {
		if (processEndTimes[i] == -1) { //Setting the end time to -1 indicates the process is invalid
			processEndTimes[i] = endTime;
			return 1;
		}
	}
	return 0;
}
//'weekday' is a bad way to decribe this, and vague. meed a better way to describe days of the week
int weekdayToInt(char *day_name) {
	char days[7][3] = {"sun", "mon", "tue", "wed", "thu", "fri", "sat"};
	for (int dayNum = 0; dayNum < 7; dayNum++) {
		if (strcmp(days[dayNum], day_name)) {
			return dayNum;
		}
	}

	return -2;
}

//Converts month string to int code
int monthToInt(char *month_name) {
	char months[12][3] = {"jan", "feb", "mar", "apr", "may", "jun", "jul", "aug", "sep", "oct", "nov", "dec"};
	for (int monthNum = 0; monthNum < 12; monthNum++) {
		if (strcmp(months[monthNum], month_name) == 0) {
			return monthNum;
		}
	}

	return -2;
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

int my_atoi(char *datastring, int datafield) {
	if (isdigit(datastring[0])) {
		return atoi(datastring);
	}

	if (datastring[0] == '*') {
		return ALLVALUES;
	} else {
		switch (datafield) {
			case 1:
				//Months field
				return monthToInt(datastring);
			case 2:
				return weekdayToInt(datastring);
		}
	}
}

void readfiles(char *filename, int filetype) {
	FILE *dict = fopen(filename, "r");
	if (dict == NULL) {
		//Should use stderr
		printf("error: cannot open '%s' \n", filename);
		exit(EXIT_FAILURE);
	}

	char line[MAX_FILENAME_LEN];
	char *token;
	
	int i = 0;
	while (fgets(line, sizeof line, dict) != NULL) {
		line[strcspn(line, "\r\n")] = 0;

		//Ignore empty lines
		if (strlen(line) == 0) {
			continue;
		}

		token = strtok(line, " ");

		//Ignore comment lines
		if (token[0] == '#') {
			continue;
		}

		switch (filetype) {
			case 1:
				fileinfo[i].minute = my_atoi(token, 0);
				token = strtok(NULL, " ");
				fileinfo[i].hour = my_atoi(token, 0);
				token = strtok(NULL, " ");
				fileinfo[i].date = my_atoi(token, 0);
				token = strtok(NULL, " ");
				fileinfo[i].month = my_atoi(token, 1);
				token = strtok(NULL, " ");
				fileinfo[i].weekday = my_atoi(token, 2);
				token = strtok(NULL, " ");
				strcpy(fileinfo[i].name, token);
				
				break;
			case 2:
				for (int i = 0; i < sizeof(fileinfo)/sizeof(fileinfo[0]); i++) {
					if (strcmp(fileinfo[i].name, token) == 0) {
						token = strtok(NULL, " ");
						fileinfo[i].runtime = atoi(token);
						break;
					}
				}
				break;
		}
		i++;
	}
	fileinfoLength = i;

	fclose(dict);
}

//Tests if two minutes are on the same day
int IsOnTheSameDay(int minute1, int minute2) {
	//Divide each minute value by 60 * 24 rounded down to get the number of days in the minute value
	int day1 = minute1 / (60 * 24);
	int day2 = minute2 / (60 * 24);

	return (day1 == day2);
}

//Tests if two minutes are in the same hour
int IsOnTheSameHour(int minute1, int minute2) {
	int hour1 = minute1 / 60;
	int hour2 = minute2 / 60;

	return (hour1 == hour2);
}

void NewGetNextStartingTime(struct tm currentTime, int processNum, int month) {
	int isDone = 0;
	while (isDone) {
		if (fileinfo[processNum].minute != '*' && currentTime.tm_min != fileinfo[processNum].minute) {
			if (currentTime.tm_min > fileinfo[processNum].minute) {
				currentTime.tm_hour += 1;
			}
			currentTime.tm_min = fileinfo[processNum].minute;
		}
		if (fileinfo[processNum].hour != '*' && currentTime.tm_hour != fileinfo[processNum].hour) {
			if (currentTime.tm_hour > fileinfo[processNum].hour) {
				currentTime.tm_hour = fileinfo[processNum].hour;
				currentTime.tm_mday += 1;
				currentTime.tm_min = 0;
				continue;
			}
			currentTime.tm_hour = fileinfo[processNum].hour;
			currentTime.tm_min = 0;
			continue;
		}
		if (fileinfo[processNum].weekday != '*' && currentTime.tm_wday != fileinfo[processNum].weekday) {
			int deltaDays = fileinfo[processNum].weekday - currentTime.tm_wday;
			if (deltaDays < 0) { deltaDays += 7; }
			currentTime.tm_mday += deltaDays;
			currentTime.tm_hour = 0;
			currentTime.tm_min = 0;
			continue;
		}
		if (fileinfo[processNum].date != '*' && currentTime.tm_mday != fileinfo[processNum].date) {
			if (currentTime.tm_mday > fileinfo[processNum].date || GetDaysInMonth(month) < currentTime.tm_mday) {
				currentTime.tm_mon += 1;
				currentTime.tm_mday = 1;
				currentTime.tm_hour = 0;
				currentTime.tm_min = 0;
				continue;
			}
			currentTime.tm_mday = fileinfo[processNum].date;
			currentTime.tm_hour = 0;
			currentTime.tm_min =0;
			continue;
		}
		isDone = 1;
	}	
}

//Invalid end times are -1. -1 is returned if there are no processes running
int GetNextEndingTime() {
	int min = -1;
	for (int i = 0; i < MAX_CMDLINES; i++) {
		if (min == -1 || processEndTimes[i] < min) {
			min = i;
		}
	}
	return min;
}

int main(int argcount, char *argvalue[]) {
	if (argcount != 4) {
		//stderr/print? I dunno
		exit(EXIT_FAILURE);
	}

	readfiles(argvalue[2], 1);
	readfiles(argvalue[3], 2);

	int month = my_atoi(argvalue[1], 1);
	int minutesInMonth = 60 * 24 * GetDaysInMonth(month);

	int currentRunningProcesses = 0;
	int maxRunningProcesses = 0;

	/*
	int currentTime = 0;
	while true {
		//Get next starting process/Get next ending process
		//If current processes are 20
		
		int nextEnding = GetNextEndingTime();

		//There are too many processes running, one can only end
		if (cuurentRunningProcesses == 20) {
			//The next event is a process ending
			currentTime = processEndTimes[i];
			processEndTimes[i] = -1; //Kill process
			currentRunningProcesses -= 1;
		}

		int nextStart = NewGetNextStartingTime();
		//Make sure you account for the edge case where one program ends as another begins
	}*/

	struct tm currentTime;

	currentTime.tm_min = 0;		currentTime.tm_hour = 0;	currentTime.tm_mday = 0;
	currentTime.tm_mon = month;
	
	NewGetNextStartingTime(currentTime, 0, month);

	printf("%d, %d, %d, %d, %d", currentTime.tm_min, currentTime.tm_hour, currentTime.tm_mday, currentTime.tm_mon, currentTime.tm_wday);
}
