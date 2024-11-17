#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fillCommands.h"

// Prepares all basic commands for the scraper.
void FillBasicCommands(unsigned char** commands, int numCmds) {
    for(int i = 0; i < numCmds + 1; i++) {
	unsigned char* cmd = (unsigned char*)malloc(256);
	bzero(cmd, 256);
	commands[i] = (cmd);
    }

    sprintf(commands[0], "curl https://www.espn.com/soccer/matchstats/_/gameId/");
    sprintf(commands[1], " | grep -i \"datawrapper\" | awk 'BEGIN{FS=\"ScoreCell__TeamName \"} {print $2}' | awk -F'>' '{print $2}' | awk -F'<' '{printf \"%%s\",$1}'");
    sprintf(commands[2], " | grep -i \"datawrapper\" | awk 'BEGIN{FS=\"ScoreCell__TeamName \"} {print $3}' | awk -F'>' '{print $2}' | awk -F'<' '{printf \"%%s\",$1}'");
    sprintf(commands[3], " | grep -i \"datawrapper\" | awk 'BEGIN{FS=\"Gamestrip__Score \"} {print $2}' | awk -F'>' '{print $2}' | awk -F'<' '{printf \"%%s\",$1}'");
    sprintf(commands[4], " | grep -i \"datawrapper\" | awk 'BEGIN{FS=\"Gamestrip__Score \"} {print $3}' | awk -F'>' '{print $2}' | awk -F'<' '{printf \"%%s\",$1}'");
    sprintf(commands[5], " | grep -i \"datawrapper\" | awk 'BEGIN{FS=\"ScoreCell__GameNote\"} {print $2}' | awk -F'>' '{print $2}' | awk -F'<' '{printf \"%%s\",$1}'");
    sprintf(commands[6], " | grep -i \"datawrapper\" | awk 'BEGIN{FS=\"Gamestrip__Record\"} {print $2}' | awk -F'>' '{print $2}' | awk -F'<' '{printf \"%%s\",$1}' | awk -F'-' '{printf \"%%s,%%s,%%s\",$1,$2,$3}'");
    sprintf(commands[7], " | grep -i \"datawrapper\" | awk 'BEGIN{FS=\"Gamestrip__Record\"} {print $3}' | awk -F'>' '{print $2}' | awk -F'<' '{printf \"%%s\",$1}' | awk -F'-' '{printf \"%%s,%%s,%%s\",$1,$2,$3}'");
    sprintf(commands[8], " | grep -i \"datawrapper\" | awk 'BEGIN{FS=\"Gamestrip__Record\"} {print $2}' | awk -F'>' '{print $2}' | awk -F'<' '{printf \"%%s\",$1}' | awk -F'-' '{print $1, $2, $3}' | ./adder");

}

// Prepares a few commands for the scraper to speed things up through
//   temporary files.
void FillSpeedCommands(unsigned char** speedCommands, int numSpeedCommands) {
    for(int i = 0; i < numSpeedCommands; i++) {
	unsigned char* cmd = (unsigned char*)malloc(32);
	bzero(cmd, 32);
	speedCommands[i] = cmd;
    }

    sprintf(speedCommands[0], " > formText.txt");
    sprintf(speedCommands[1], "cat formText.txt");
    sprintf(speedCommands[2], " > homeTeam.txt");
    sprintf(speedCommands[3], "cat homeTeam.txt");
    sprintf(speedCommands[4], " >> formData.txt");
}

// Prepare a few commands for scraping statistics based on a team's form.
void FillFormCommands(unsigned char** formCommands, int numFormCommands) {
    for(int i = 0; i < numFormCommands; i++) {
	unsigned char* cmd = (unsigned char*)malloc(256);
	bzero(cmd, 256);
	formCommands[i] = cmd;
    }

    sprintf(formCommands[0], " | grep -i \"dataWrapper\" | awk 'BEGIN{FS=\">COMPETITION<\"} {print $%%d}' | awk 'BEGIN{FS=\"</tr>\"} {print $%%d}'");
    sprintf(formCommands[1], " | awk 'BEGIN{FS=\"</td>\"} {print $%%d}'");
    sprintf(formCommands[2], " | awk 'BEGIN{FS=\"soccer\"} {print $2}' | awk -F'>' '{print $2}' | awk -F'<' '{print $1}' >> formData.txt");
    sprintf(formCommands[3], " | awk 'BEGIN{FS=\"gameId\"} {print $2}' | awk -F'>' '{print $2}' | awk -F'<' '{print $1}' | awk 'BEGIN{FS=\" - \"} {printf \"%%s\\n%%s\\n\",$1,$2}' >> formData.txt");

    formCommands[4] = formCommands[2];
}

