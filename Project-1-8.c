#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <string.h>

#define MAX_CMDLINES 		20
#define MAX_CMDNAME_LEN 	41
#define MAX_FILELINE_LEN	101

// creating array of structures for each crontab command
// contains data from both files
struct {
	int	minute;
	int	hour;
	int 	date;
	int	month;
	int	weekday;
	char	name[MAX_CMDNAME_LEN];
	int 	runtime;
} fileinfo[MAX_CMDLINES];

// reads both files to store data in the struct array
void readfiles(char *filename, int filetype)
{
	// opening text file
	FILE *dict = fopen(filename, "r");
	if (dict == NULL) {
		// error handling: cannot open file
		printf("Error: cannot open '%s.'\n", filename);
		exit(EXIT_FAILURE);
	}
	
	// declare line for loop and token for string split
	char line[MAX_FILELINE_LEN];
	char *token;

	printf("point 1 passed.\n"); //test

	// loop for lines in the text file
	int i = 0;
	while (fgets(line, sizeof line, dict) != NULL) {
		line[strcspn(line, "\r\n")] = 0; // strip line
		if (strlen(line) == 0) {
			// ignore empty line
			continue;
		}
		token = strtok(line, " ");
            	if (token[0] == '#') {
			// ignore comment lines
			continue;
		}
		printf("point 2 passed.\n"); //test
		
		// store data, switching method used based on which file
		switch (filetype) {
		case 1: 
			// crontab file
			// gets string tokens from line and stores into struct array
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

			printf("point 3 passed.\n"); //test
			break;
		case 2: 
			//estimates file
			for (int i = 0; i < sizeof(fileinfo)/sizeof(fileinfo[0]); i++) {
				// matches crontab and estimates names to get and store runtime data
				if (strcmp(fileinfo[i].name, token) == 0) {
					token = strtok(NULL, " ");
					fileinfo[i].runtime = atoi(token);
					break;
				}
			}

			printf("point 4 passed.\n"); //test
			break;	
		}
		i++;
	}
	// close file after loop ends
	fclose(dict);
}

void testfunc()
{
	// currently to test the implementation of structs
	// to be removed later
	int nlines = sizeof(fileinfo)/sizeof(fileinfo[0]);

	for (int n = 0; n < nlines; n++) {
		if (strlen(fileinfo[n].name) == 0) {
			// ignores any empty structs
			break;
		}
		printf("%i %i %i %i %i %s %i\n", 
			fileinfo[n].minute, 
			fileinfo[n].hour, 
			fileinfo[n].date, 
			fileinfo[n].month, 
			fileinfo[n].weekday, 
			fileinfo[n].name, 
			fileinfo[n].runtime);
	}
}

int main(int argc, char *argv[])
{
	
	readfiles(argv[2], 1);
	readfiles(argv[3], 2);
	
	testfunc();

	return EXIT_SUCCESS;
}
