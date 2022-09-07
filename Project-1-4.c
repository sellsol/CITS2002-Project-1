#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <string.h>

#define MAX_CMDLINES 		20
#define MAX_CMDNAME_LEN 	41
#define MAX_FILELINE_LEN	101

//Converts months to integer values for passing
int monthToInt(char *monthname) {
	//This feels like it should be passed outside of the function
	if (isalpha(monthname == 0)) {
		return atoi(monthname);
	}
	else {
		char months[12][4] = {"jan", "feb", "mar", "apr", "may", "jun",
			"jul", "aug", "sep", "oct", "nov", "dec"};
		for (int i = 0; i < 12; i++) {
			if (strcmp(months[i], monthname) == 0) {
				return i;
			}
		}
		return -1;
	}
} 

int weekdayToInt(char *dayname) {
	if (isalpha(dayname == 0)) {
		return atoi(dayname);
	}
	else {
		char days[7][4] = {"sun", "mon", "tue", "wed", "thu",
			"fri", "sat"};
		for (int i = 0; i < 12; i++) {
			if (strcmp(days[i], dayname) == 0) {
				return i;
			}
		}
		return -1;
	}
}

struct {
	int	minute;
	int	hour;
	int 	date;
	int	month;
	int	weekday;
	char	name[MAX_CMDNAME_LEN];
	int 	runtime;
} fileinfo[MAX_CMDLINES];

void readfiles(char *filename, int filetype)
{
	FILE *dict = fopen(filename, "r");
	if (dict == NULL) {
		printf("Error: cannot open '%s.'\n", filename);
		exit(EXIT_FAILURE);
	}
	
	char line[MAX_FILELINE_LEN];
	char *token;

	int i = 0;
	while (fgets(line, sizeof line, dict) != NULL) {
		token = strtok(line, " ");
		if (token[0] == '#') {
			continue;
		}

		switch (filetype) {
		case 1:
			fileinfo[i].minute = atoi(token);
			token = strtok(NULL, " ");
			fileinfo[i].hour = atoi(token);
			token = strtok(NULL, " ");
			fileinfo[i].date = atoi(token);
			token = strtok(NULL, " ");
			fileinfo[i].month = atoi(token);
			token = strtok(NULL, " ");
			fileinfo[i].weekday = atoi(token);
			token = strtok(NULL, " ");
			strcpy(fileinfo[i].name, token);
		case 2:;
			int cmdindex;
			for (int i = 0; i < sizeof(fileinfo)/sizeof(fileinfo[0]); i++) {
				if (fileinfo[i].name == token) {
					cmdindex = i;
					break; // does this do anything
				}
			}
			token = strtok(NULL, " ");
			fileinfo[cmdindex].runtime = atoi(token);
		}
		i++;
	}
	
	fclose(dict);
}

int main(int argc, char *argv[])
{
	
	readfiles(argv[3], 1);
	readfiles(argv[4], 2);

	return EXIT_SUCCESS;
}
