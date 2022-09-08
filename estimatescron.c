#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <ctype.h>

#define ALLVALUES	-1

//Month to interger definitions
#define JANUARY		0
#define FEBUARY		1
#define MARCH		2
#define APRIL		3
#define MAY		4
#define JUNE		5
#define JULY		6
#define AUGUST		7
#define SEPTEMBER	8
#define OCTOBER		9
#define NOVEMBER 	10
#define DECEMBER 	11

//--------------For days of the week in this code, 0 = Sunday, 1 = Monday, ... 6 = Saturday -------------------//

#define MAX_CMDLINES		20
#define MAX_CMDNAME_LEN		41
#define MAX_FILENAME_LEN	101

int weekdayToInt(char *day_name);
int monthToInt(char *month_name);

int my_atoi(char *datastring, int datafield) {
	if (isdigit(datastring[0])) {
		return atoi(datastring);
	}
	if (datastring[0] == '*') {
		return ALLVALUES;
	} else {
		switch (datafield) {
			case 1:;
			       //Months field
			       return monthToInt(datastring);
			
			case 2:
			       return weekdayToInt(datastring);
		
		}
		
	}
}

//Converts a 3-char weekday identifier to it's integer representation
int weekdayToInt(char *day_name) {
	char days[7][3] = {"sun", "mon", "tue", "wed", "thu", "fri", "sat"};
	for (int dayNum = 0; dayNum < 7; dayNum++) {
		if (strcmp(days[dayNum], day_name)) {
			return dayNum;
		}
	}

	return -2;
}

//Converts a 3-char month identifier to it's integer representation
int monthToInt(char *month_name) {
	char months[12][3] = {"jan", "feb", "mar", "apr", "may", "jun", "jul", "aug", "sep", "oct", "nov", "dec"};
	for (int monthNum = 0; monthNum < 12; monthNum++) {
		if (strcmp(months[monthNum], month_name) == 0) {
			return monthNum;
		}
	}

	return -2;
}

//Courtesy of Chris.McDonald
//Gets first day of the month in 2022 in it's integer representation
int firstDayOfMonth(int month) {
	struct tm tm;

	memset(&tm, 0, sizeof(tm));

	tm.tm_mday = 1;
	tm.tm_mon = month;
	tm.tm_year = (2022 - 1900);

	mktime(&tm);
	
	return tm.tm_wday;
}

//Returns the number of days in a month (which should be provided in it's integer representation)
int numDaysInMonth(int month) {
	if (month == FEBUARY) {
		return 28;
	} else if (month == APRIL || month == JUNE || month == SEPTEMBER || month == NOVEMBER) {
		return 30;
	} else {
		return 31;
	}
}

//Returns the number of week days of a given type in a specified month. The year is always 2022.
int numDaysOfTypeInMonth(int month, int dayType) {
	int firstDayType = firstDayOfMonth(month);
	int daysInMonth = numDaysInMonth(month);

	int firstWeekDayDate;
	if (firstDayType <= dayType) {
		firstWeekDayDate = dayType - firstDayType;
	} else {
		firstWeekDayDate = 7 - (dayType - firstDayType);
	}

	return (daysInMonth - firstWeekDayDate) / 7;
}

//Takes in the all the data in a command line, and outputs how many times this process is called in the specified month
//If any of the parameters (Excluding the month we're testing) are -1, this does not represent an error, but instead that every possible value is accepted (i.e is an asterisk)
int howManyTimesaMonth(int monthProvided, int minutes, int hours, int days, int months, int weekdays) {
	
	//First, make sure this process runs during the month we are measuring
	if (!(months == monthProvided || months == ALLVALUES)) {
		return 0;
	}
	
	//By being specified, the process will run at least once
	int times = 1;

	//Multiply how many times the process runs based off if it runs every minute/hour
	if (minutes == ALLVALUES) {
		times *= 60;
	}
	if (hours == ALLVALUES) {
		times *= 24;
	}


	if (days == ALLVALUES && weekdays == ALLVALUES) {
		
		//This process runs every day of the month, no matter what day of the week it is. Multiply the number of times it runs by the days in the month
		times *= numDaysInMonth(monthProvided);
	} else if (days == ALLVALUES) {

		//Count the number of weekdays within the month, and multiply times by this
		times *= numDaysOfTypeInMonth(months, weekdays);
	} else if (weekdays == ALLVALUES) {

		//This program only runs on one day.
		times *= 1;
	} else {

		//We've been provided a day of the week and a day of the month for when this process runs. Make sure they match, and return an error otherwise
		int firstDay = firstDayOfMonth(monthProvided);
		if ((firstDay + days) % 7 != weekdays) {
			return -1;
		}
	}

	return times;

}

struct {
	int minute;
	int hour;
	int date;
	int month;
	int weekday;
	char name[MAX_CMDNAME_LEN];
	int runtime;
} fileinfo[MAX_CMDLINES];

void readfiles(char *filename, int filetype) {
	FILE *dict = fopen(filename, "r");
	if (dict == NULL) {
		//Error handling: cannot open file
		printf("Error: cannot open '%s.'\n", filename);
		exit(EXIT_FAILURE);
	}

	//Declare line for loop and token for string split
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
	fclose(dict);
}

int main(int argcount, char *argvalue[]) {
	
	readfiles(argvalue[2], 1);
	readfiles(argvalue[3], 2);

	//printf("%i", howManyTimesaMonth(atoi(argvalue[1]), atoi(argvalue[2]), atoi(argvalue[3]), atoi(argvalue[4]), atoi(argvalue[5]), atoi(argvalue[6])));
	//printf(%i, howManyTimesAMonth(argvalue[1], argvalue[2], argvalue[3], argvalue[4], argvalue[5], argvalue[6]));

	exit(EXIT_SUCCESS);
}
