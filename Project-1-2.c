#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#define MAX_CMDLINES 		20
#define MAX_CMDNAME_LEN 	41
#define MAX_FILELINE_LEN	101

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