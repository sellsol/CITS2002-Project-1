#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#define BUFFERSIZE 100

void readfile(char filename[])
{
	FILE *dict = fopen(filename, "r");
	if (dict == NULL) {
		printf("Error: cannot open '%s.'\n", filename);
		exit(EXIT_FAILURE);
	}

	char *fileinfo[100];
	int i = 0;

	char line[BUFFERSIZE];
	while (fgets(line, sizeof line, dict) != NULL) {
		char *thisline = strtok(line, " ");
		if (thisline[0] == '#') {
			continue;
		}
		fileinfo[i] = thisline;
		++i;
	}
	fclose(dict);
}

int main(int argc, char *argv[])
{
	return EXIT_SUCCESS;
}
