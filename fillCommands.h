#ifndef FILL_COMMANDS_H
#define FILL_COMMANDS_H

// Prepares all basic commands for the scraper.
void FillBasicCommands(unsigned char** commands, int numCmds);

// Prepares a few commands for the scraper to speed things up through
//   temporary files.
void FillSpeedCommands(unsigned char** speedCommands, int numSpeedCommands);

// Prepare a few commands for scraping statistics based on a team's form.
void FillFormCommands(unsigned char** formCommands, int numFormCommands);

#endif
