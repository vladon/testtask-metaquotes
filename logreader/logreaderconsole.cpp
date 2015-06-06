// logreader console program

#include <cstring>
#include "LogReader.h"

void ShowHelp()
{
	printf("Usage\n");
	printf("\tlogreader <logfile> <wildcardmask>\n");
	printf("\nExample:\n");
	printf("\tlogreader test.log order*closed");
}

int main(int argc, char* argv[])
{
	char * myfilename;
	char * myfilter;

	if (argc < 3) {
		ShowHelp();
	}
	else {
		myfilename = argv[1];
		myfilter = argv[2];

		CLogReader mylogreader(myfilename);

		if (mylogreader.Open()) {
			mylogreader.SetFilter(myfilter);

			char buf[4096];
			while (mylogreader.GetNextLine(buf, sizeof(buf))) {
				printf("%s\n", buf);
			}
		}
		else {
			printf("Error opening file: %s\n", myfilename);
		}
	}

	return 0;
}
