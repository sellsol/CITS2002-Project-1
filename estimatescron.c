#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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



//Courtesy of Chris.McDonald
//Gets first day of the month in 2022. 0 = Sunday, 1 = Monday, ... , 6 = Saturday
int firstDayOfMonth(int month) {
	struct tm tm;

	memset(&tm, 0, sizeof(tm));

	tm.tm_mday = 1;
	tm.tm_mon = month;
	tm.tm_year = (2022 - 1900);

	mktime(&tm);
	
	return tm.tm_wday;
}

//Month starts from 0. No error handling
int numDaysInMonth(int month) {
	if (month == FEBUARY) {
		return 28;
	} else if (month == APRIL || month == JUNE || month == SEPTEMBER || month == NOVEMBER) {
		return 30;
	} else {
		return 31;
	}
}

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

int howManyTimesaMonth(int monthProvided, int minutes, int hours, int days, int months, int weekdays) {
	if (!(months == monthProvided || months == -1)) {
		return 0;
	}
	
	int times = 1;
	if (minutes == -1) {
		times *= 60;
	}
	if (hours == -1) {
		times *= 24;
	}
	if (days == -1 && weekdays == -1) {
		times *= numDaysInMonth(monthProvided);
	} else if (days == -1) {
		//Count the number of weekdays within the month, and multiply times by this
		times *= numDaysOfTypeInMonth(months, weekdays);

	} else if (weekdays == -1) {
		times *= 1; //Just goes for one day
	} else {
		//Check if the weekday matches the day
		int firstDay = firstDayOfMonth(monthProvided);
		if ((firstDay + days) % 7 != weekdays) {
			return 0;
		}
	}

	return times;

}

int main(int argcount, char *argvalue[]) {
	
	printf("%i", howManyTimesaMonth(atoi(argvalue[1]), atoi(argvalue[2]), atoi(argvalue[3]), atoi(argvalue[4]), atoi(argvalue[5]), atoi(argvalue[6])));
	//printf(%i, howManyTimesAMonth(argvalue[1], argvalue[2], argvalue[3], argvalue[4], argvalue[5], argvalue[6]));

	exit(EXIT_SUCCESS);
}
