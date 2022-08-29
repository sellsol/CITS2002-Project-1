#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//Courtesy of Chris.McDonald
//Month is inputedd so 0 = january, 1 = febuary,... etc
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
	if (month == 1) { //Month equals Febuary
		return 28;
	} else if (month == 3 || month == 5 || month == 8 || month == 10) {
		return 30;
	} else {
		return 31;
	}
}

int howManyTimesaMonth(int monthProvided, int minutes, int hours, int days, int months, int weekdays) {
	if (months != monthProvided || monthProvided != -1) {
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

		int firstDay = firstDayOfMonth(monthProvided);
		int daysInMonth = numDaysInMonth(monthProvided);
		int firstWeekDay;
		if (firstDay < weekdays) {
			firstWeekDay = weekdays - firstDay;
		} else {
			firstWeekDay = 7 - (weekdays - firstDay);
		}
	
		int count = 1;
		for (int i = firstWeekDay; i < daysInMonth; i += 7) {
			count++;
		}

		times *= count;

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
	
	printf("%i", firstDayOfMonth(atoi(argvalue[1])));
	//printf(%i, howManyTimesAMonth(argvalue[1], argvalue[2], argvalue[3], argvalue[4], argvalue[5], argvalue[6]));

	exit(EXIT_SUCCESS);
}
