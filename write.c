#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "fillCommands.h"

/*

Web Scraper of ESPN Soccer Data

Written by Devin Frandsen
November 2023

For educational use only.

Was originally built to scrape 2018/19 - 2023/24 seasons of the premier league and Serie A.
  It is not necessarily the best way to do this task, but it was a fun project, and was useful
  for other projects, particularly creating datasets for machine learning. You will find that
  it is unfortunately fragile. If ESPN changes something about their site, there is a chance it
  might break. However, this project was mostly for educational, one-time use, and this
  restriction is acceptable.

This file writes a bash script which does the actual data scraping using two other programs
  and several Linux system libraries. At a high level, the bash script does the following:

1. Use curl to pull a webpage down based on game ID
2. Parse individual statistics out of the webpage
3. Parse the last five games data using ./parser
4. Parse total games using ./adder
5. Output extracted information
6. Repeat

Keep in mind, ESPN stores season data in sequential ID order, with each game being given
  a unique ID.

To run this program:
- Make the executables using the given makefile
- run ./writer [filename] [lower-bound] [upper-bound]
  - filename: the name of the bash script you wish to create
  - lower-bound: the lower bound of the ID values you would like scraped
  - upper-bound: the upper bound of the ID values you would like scraped
- run bash [filename]
  - filename: the name of the bash script you created earlier

For example:
./writer scraperBash 513460-513839;
bash scraperBash > data.csv

By default, sends all output to the terminal, in csv format, in this order:
{id, home team, away team, home points, away points, league, home wins, home draws,
  home losses, away wins, away draws, away losses, matchday, avg points in form,
  goals scored in the last 5 games, goals conceded in the last 5 games}

If there are any empty rows, i.e. "513460,,,,,,0,,,,,; then the curl command failed. Simply
  try that ID again. One day I may update this program to handle that.

*/

void Write(int fd, const void *buf, size_t count);

int main(int argc, char **argv) {
    if(argc < 4) {
	printf("Provide filename, lower bound, upper bound\n");
	exit(0);
    }

    int lowerBound = atoi(argv[2]);
    int upperBound = atoi(argv[3]);

    if(lowerBound <= 0 || upperBound < lowerBound) {
	printf("Provide proper bounds\n");
	exit(0);
    }

    printf("Writing bashScript: %s\n", argv[1]);
    printf("Lower Bound %d\n", lowerBound);
    printf("Upper Bound %d\n", upperBound);

    FILE* bashScript = fopen(argv[1], "w");

    if(bashScript == NULL) {
	printf("fopen Error\n");
	exit(0);
    }

    int fd = fileno(bashScript);

    // The following are the bash commands, placed into helper arrays
    // Basic commands array

    int numCmds = 8;
    unsigned char* commands[numCmds + 1];

    int numSpeedCommands = 5;
    unsigned char* speedCommands[numSpeedCommands];

    int numFormCommands = 5;
    unsigned char* formCommands[numFormCommands];

    //Prepare all bash commands
    FillBasicCommands(commands, numCmds);
    FillSpeedCommands(speedCommands, numSpeedCommands);
    FillFormCommands(formCommands, numFormCommands);

    Write(fd, "#!/bin/bash\n\n", 13);

    for(int i = lowerBound; i <= upperBound; i++) {
	//First Comment
	Write(fd, "#id=", 4);
	unsigned char idstr[8];
	bzero(idstr, 8);
	sprintf(idstr, "%d", i);
	Write(fd, idstr, 6);
	Write(fd, "\n", 1);

	Write(fd, "echo -n '", 9);
	Write(fd, idstr, 6);
	Write(fd, ",'\n", 3);

	unsigned char startCmd[256];
	bzero(startCmd, 256);
	sprintf(startCmd, "%s%d%s\n", commands[0], i, speedCommands[0]);
	Write(fd, startCmd, strlen(startCmd));

	unsigned char teamCmd[256];
	bzero(teamCmd, 256);
	sprintf(teamCmd, "%s%s%s\n", speedCommands[1], commands[1], speedCommands[2]);
	Write(fd, teamCmd, strlen(teamCmd));

	unsigned char cmd[256];
	for(int j = 1; j <= numCmds; j++) {
	    bzero(cmd, 256);
	    sprintf(cmd, "%s%s\n", speedCommands[1], commands[j]);
	    Write(fd, cmd, strlen(cmd));

	    Write(fd, "echo -n ','\n", 12);
	}

	unsigned char chainCmd[1024];
	unsigned char homeCmd[256];
	unsigned char cmd9b[256];
	unsigned char cmd10b[256];
	unsigned char parseCmd[64];

	for(int form = 1; form <= 3; form++) {
	    Write(fd, "touch formData.txt\n", 19);

	    bzero(homeCmd, 256);
	    sprintf(homeCmd, "%s%s\n", speedCommands[3], speedCommands[4]);

	    Write(fd, homeCmd, strlen(homeCmd));
	    Write(fd, "echo '' >> formData.txt\n", 24);

	    for(int row = 0; row < numFormCommands; row++) {
		for(int j = 1; j <= 3; j++) {

		    bzero(chainCmd, 1024);
		    bzero(cmd9b, 256);
		    bzero(cmd10b, 256);

		    sprintf(cmd9b, formCommands[0], form + 1, row + 2);

		    if(form == 3) {
			sprintf(cmd10b, formCommands[1], j*2 - 1);
		    } else {
			sprintf(cmd10b, formCommands[1], j*2);
		    }

		    sprintf(chainCmd, "%s%s%s%s\n", speedCommands[1], cmd9b, cmd10b, formCommands[j + 1]);
		    Write(fd, chainCmd, strlen(chainCmd));
		}
	    }

	    bzero(parseCmd, 64);
	    sprintf(parseCmd, "./parser %d formData.txt\n", form);

	    Write(fd, parseCmd, strlen(parseCmd));
	    Write(fd, "rm -f formData.txt\n", 19);
	    Write(fd, "echo -n ','\n", 12);
	}

    Write(fd, "echo ''\n", 8);
    }

    Write(fd, "rm -f formText.txt\n", 19);
    Write(fd, "rm -f homeTeam.txt\n", 19);

    printf("Finished.\n");

    exit(0);
}

// A simplified error handler for the Write system call.
//
// To minimize complexity, this simply exits if the number written
//   to the file descriptor is less than intended. This is not the
//   only way this could have been handled.
//
// fd: file descriptor to write to
// buf: buffer to write from
// count: number of bytes to write
void Write(int fd, const void *buf, size_t count) {

    int success = write(fd, buf, count);
    if(success != count) {
	printf("Bad write\n");
	exit(0);
    }
    return;
}
