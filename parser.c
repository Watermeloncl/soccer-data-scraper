#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>

/*

Parser: Sub program for Web Scraper of ESPN Soccer Data

Written by Devin Frandsen
November 2023

For educational use only.

Built to accept a specific piece of an ESPN website, known as the "form".
  The form is the last five games for each time in question, including
  final scores and winning team. This program extracts this information
  from the website snippet (from a file), and calcuates how many points
  were scored and conceded in the form for each team.

See write.c for more information.

*/


// Data structure for holding each game of data within the forms. (One form
//   per team, one row per game.)
typedef struct formRow {
	unsigned char home[64];
	unsigned char away[64];
	int homePoints;
	int awayPoints;
} formRow;

int main(int argc, char **argv) {

    FILE* dataFile = fopen(argv[2], "r");
    if(dataFile == NULL) {
	exit(0);
    }

    int fd = fileno(dataFile);
    unsigned char buf[64];
    bzero(buf, 64);
    int offset = 0;
    int dataItem = 0;

    unsigned char emptyBuf[64];
    bzero(emptyBuf, 64);

    formRow* rows[5];
    int currRow = 0;

    formRow tempRow;
    bzero(tempRow.home, 64);
    bzero(tempRow.away, 64);
    tempRow.homePoints = -1;
    tempRow.awayPoints = -1;

    unsigned char homeTeam[64];
    bzero(homeTeam, 64);

    int nread = 1;
    for(;;) {
	nread = read(fd, buf + offset, 1);

	if(nread < 0) {
	    printf("Parser Error\n");
	    exit(0);
	}

	if(nread == 0) {
	    if(buf[offset] == '\n') {
		buf[offset] = 0;

		formRow* newRow;
		newRow = malloc(sizeof(formRow));
		memcpy(newRow->home, tempRow.home, 64);
		newRow->homePoints = tempRow.homePoints;
		newRow->awayPoints = tempRow.awayPoints;
		memcpy(newRow->away, buf, 64);

		rows[currRow] = newRow;
	    }
	    break;
	}

	if(buf[offset] == '\n') {
	    buf[offset] = 0;
	    switch(dataItem) {
		case 0:
		    dataItem = 1;
		    memcpy(homeTeam, buf, 64);
		    break;
		case 1:
		    dataItem = 2;
		    memcpy(tempRow.home, buf, 64);
		    break;
		case 2:
		    dataItem = 3;
		    tempRow.homePoints = atoi(buf);
		    break;
		case 3:
		    dataItem = 4;
		    tempRow.awayPoints = atoi(buf);
		    break;
		case 4:
		default:
		    dataItem = 1;
		    memcpy(tempRow.away, buf, 64);

		    formRow* newRow;
		    newRow = malloc(sizeof(formRow));
		    memcpy(newRow->home, tempRow.home, 64);
		    newRow->homePoints = tempRow.homePoints;
		    newRow->awayPoints = tempRow.awayPoints;
		    memcpy(newRow->away, tempRow.away, 64);

		    rows[currRow] = newRow;
		    currRow = currRow + 1;

		    bzero(tempRow.home, 64);
		    bzero(tempRow.away, 64);
		    tempRow.homePoints = -1;
		    tempRow.awayPoints = -1;
		    break;
	    }

	    bzero(buf, 64);
	    offset = 0;
	} else {
	    offset = offset + 1;
	}
    }

    int opcode = atoi(argv[1]);
    if(opcode == 1 || opcode == 2) {
	unsigned char team1[64];
	unsigned char team2[64];
	memcpy(team1, rows[0]->home, 64);
	memcpy(team2, rows[0]->away, 64);
	int count1 = 1;
	int points1 = rows[0]->homePoints;
	int count2 = 1;
	int points2 = rows[0]->awayPoints;
	float games = 1.0;

	for(int i = 1; i < 5; i++) {
	    if(rows[i]->homePoints == -1) {
		break;
	    }
	    games = games + 1.0;
	    if(strcmp(team1, rows[i]->home) == 0) {
		count1 = count1 + 1;
		points1 = points1 + rows[i]->homePoints;
	    }

	    if(strcmp(team1, rows[i]->away) == 0) {
		count1 = count1 + 1;
		points1 = points1 + rows[i]->awayPoints;
	    }

	    if(strcmp(team2, rows[i]->home) == 0) {
		count2 = count2 + 1;
		points2 = points2 + rows[i]->homePoints;
	    }

	    if(strcmp(team2, rows[i]->away) == 0) {
		count2 = count2 + 1;
		points2 = points2 + rows[i]->awayPoints;
	    }
	}

	if(count1 > count2) {
	    printf("%.1f", points1 / games);
	} else {
	    printf("%.1f", points2 / games);
	}
    } else {
	int homePoints = 0;
	int awayPoints = 0;
	for(int i = 0; i < 5; i++) {
	    if(rows[i]->homePoints == -1) {
		break;
	    }
	    if(strcmp(homeTeam, rows[i]->home) == 0) {
		homePoints = homePoints + rows[i]->homePoints;
		awayPoints = awayPoints + rows[i]->awayPoints;
	    } else {
		homePoints = homePoints + rows[i]->awayPoints;
		awayPoints = awayPoints + rows[i]->homePoints;
	    }
	}

	printf("%d,%d", homePoints, awayPoints);

    }

    for(int i = 0; i < 5; i++) {
	free(rows[i]);
    }

    exit(0);
}
