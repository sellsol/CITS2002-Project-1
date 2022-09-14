// bits and pieces file, latest full before this is v10 from me and overall on the github

#include <stdio.h>
#include <stdlib.h>

struct event {
	int eventtime;
	int countchange;
} eventlog[8]; // size not known in actual program, add more later with malloc?

// sorts the eventlog struct array
int cmpevents(const void *a, const void *b)
{
	const struct event *pa = a;
	const struct event *pb = b;
	
	if (pa->eventtime != pb->eventtime) {
		return (pa->eventtime > pb->eventtime) - (pa->eventtime < pb->eventtime);
	}
	else {
		return (pa->countchange > pb->countchange) - (pa->countchange < pb->countchange);
	}
}

// takes an array of starttimes and endtimes, and simulates the month's time
// returns the maximum no. of cmds running at the same time
int simMonth(struct event *eventlog, int length)
{
        int count = 0;
	int maxcount = 0;

	for (int i = 0; i < length; i++) {
		count += eventlog[i].countchange;
	
		if (count > maxcount) {
			maxcount = count;
		}
		if (maxcount == 20) {
			return 20;
		}
	}

	return maxcount;
}

void placehfunc() 
{
	eventlog[0] = (struct event) {120332, 1};
        eventlog[1] = (struct event) {124300, -1};
        eventlog[2] = (struct event) {45099, 1};
        eventlog[3] = (struct event) {120332, -1};
        eventlog[4] = (struct event) {120332, 1};
        eventlog[5] = (struct event) {171230, -1};
        eventlog[6] = (struct event) {41230, 1};
        eventlog[7] = (struct event) {150000, -1};
	 // test eventlog
	 // in actual program size will have to be grown before each new storing?
}

int main(int argc, char *argv[])
{

	placehfunc(); // placeholder test function

	int numEvents = sizeof(eventlog) / sizeof(*eventlog);
	qsort(eventlog, numEvents, sizeof *eventlog, cmpevents);

	for (int n = 0; n < numEvents; n++) {
		printf("%d, %d\n", eventlog[n].eventtime, eventlog[n].countchange);
	} // testing block

	int maxSimEvents = simMonth(eventlog, numEvents);
	printf("%d\n", maxSimEvents);

	return EXIT_SUCCESS;
}
