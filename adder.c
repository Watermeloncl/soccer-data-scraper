#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

/*

Adder: Sub program for Web Scraper of ESPN Soccer Data

Written by Devin Frandsen
November 2023

For educational use only.

Built to accept a very small snippet of an ESPN website, consisting of
  [wins]-[draws]-[losses], with each value being a scalar. Ths program
  extracts each piece and adds them together.

See write.c for more information.

*/

int main(int argc, char **argv) {
    unsigned int sum = 0;
    int bytesRead = 1;
    unsigned char buf[4];
    bzero(buf, 4);
    int offset = 0;

    while(bytesRead >= 0) {
	bytesRead = read(0, buf + offset, 1);

	if(bytesRead == -1) {
	    printf("Read Error\n");
	    exit(0);
	}

	if(buf[offset] == ' ' || bytesRead == 0) {
	    sum = sum + atoi(buf);
	    offset = 0;
	    bzero(buf, 4);

	    if(bytesRead == 0) {
		break;
	    }
	} else {
	    offset = offset + 1;
	}
    }

    printf("%d", sum);
    exit(0);
}
