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
int currentRunningProcesses;

struct {
	int minute;
	int hour;
	int date;
	int month;
	int weekday;
	char name[MAX_CMDNAME_LEN];
	int runtime;
} fileinfo[MAX_CMDLINES];

//Struct array of all running processes
struct {
	int startTime;
	int endTime;
} processes[MAX_CMDLINES];

//Returns whether it succeeded or failed to find an available space to put the process
int invokeProcess(int startTime, int endTime) {
	//Find available space for the process to run
	for (int i = 0; i < MAX_CMDLINES; i++) {
		if (processes[i].endTime == 0) { //Setting the end time to 0 indicates the process is invalid
			processes[i].startTime = startTime;
			processes[i].endTime = endTime;
			return 1;
		}
	}
	return 0;
}
//'weekday' is a bad way to decribe this, and vague. meed a better way to describe days of the week
int weekdayToInt(char *day_name) {
	char days[7][3] = {"sun", "mon", "tue", "wed", "thu", "fri", "sat"};
	for (int dayNum = 0; dayNum < 7; dayNum++) {
		if (strcmp(days[dayNum], day_num)) {
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

//For now, assume currentMinute is the time the process last ran
int GetProcessNextStartingTime(int currentMinute, int processNum) {
	//Start by getting the next day that the process will start on

	if (fileinfo.date == '*' && fileinfo.weekday == '*') {
		//We need to check if there exists a minute this process can run which is greater than the current minute

		if (fileinfo.hour == '*' && fileinfo.minute == '*') {
			//If the process runs every minute of every hour of every day, next minute it should run again
			return currentMinute + 1;
		} else if (fileinfo.hour == '*') {
			//Like above, the process should run on the next hour
			return currentMinute + 60;
		} else if (fileinfo.minute == '*') {
			//We need to check if the next hour 
			if (IsOnTheSameHour(currentMinute, currentMinute + 1)) {
				return currentMinute + 1;
			}
			//Otherwise we're on the last minute of the hour
			return currentMinute - 59 + (60 * 24);
		} else {
			//Since the process only runs once per day, we need to find the next day it runs. We know this will be tomorrow, so we can add one day
			return currentMinute + (60 * 24);
		}
	} else if (fileinfo.date == '*') {
		//The process will run on the next week

		if (fileinfo.hour == '*' && fileinfo.minute == '*') {
			//Runs every minute. We need to make sure the next minute is not tomorrow
			if (IsOnTheSameDay(currentMinute, currentMinute + 1)) {
				//Because it's only one day a week, it will run next week
				return currentMinute + 1;
			}
			return currentMinute + (60 * 24 * 7);
		} else if (fileinfo.hour == '*') {
			//Runs on every hour of a specified minute of a specified day of the week
			if (IsOnTheSameDay(currentMinute, currentMinute + 60)) {
				return currentMinute + 60;
			}
			return currentMinute + (60 * 24 * 7);
		} else if (fileinfo.minute == '*'){
			//Runs on every minute of a specifed hour of a specified day of the week
			if (IsOnTheSameHour(currentMinute, currentMinute + 1)) {
				return currentMinute + 1;
			}
			return currentMinute - 59 + (60 * 24 * 7);
		} else {
			//Runs on a specified hour, specified minute, on a specified day
			return currentMinute + (60 * 24 * 7);
		}
	} else if (fileinfo.weekday == '*') {
		//This program runs on a specific day
		
		if (fileinfo.hour == '*' && fileinfo.minute == '*') {
			//Runs only on a specific date
			if (IsOnTheSameDay(currentMinute, currentMinute + 1)) {
				return currentMinute + 1;
			}
		} else if (fileinfo.hour == '*') {
			//Runs on a specific minute every hour for the day
			if (IsOnTheSameDay(currentMinute, currentMinute + 60)) {
				return currentMinute + 60;
			}
		} else if (fileinfo.minute == '*') {
			if (IsOnTheSameHour(currentMinute, currentMinute + 1)) {
				return currentMinute + 1;
			}
		}
		//If none of the above apply, the program will only run once
	}
	//This program only runs once in this month, return no
	return 100000;

}

int GetNextStartingTime() {
	
}

int GetNextEndingTime() {
	for (int i = 0; i < MAX_CMD_LENGTH; i++) {
		
	}
}

int main(int argcount, char *argvalue[]) {
	if (argcount != 4) {
		//stderr/print? I dunno
		exit(EXIT_FAILURE);
	}

	int month = my_atoi(argvalue[1], 1);
	int minutesInMonth = 60 * 24 * GetDaysInMonth(month);

	currentRunningProcesses = 0;
	while true {
		//Get next starting process/Get next ending process
		//If current processes are 20, 

		int nextEventTime = 0;

		for (int i = 0; i < fileinfoLength; i++) {
			//Get the start time
		}

		if (currentRunningProcesses == 20) {
			
		}
	}
}
