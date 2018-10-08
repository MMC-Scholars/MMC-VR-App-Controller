#ifndef PROGRAM_OPS_H
#define PROGRAM_OPS_H

#include "Types/Types.h"
#include "stdio.h"
#include "Windows.h"

//Increase this number to increase the amount of programs this app supports.
#define NUM_MAX_PROGRAMS 64

//How long, in seconds, to wait for SteamVR to open before first opening the office
#define STEAMVR_WAITTIME 12

#define APP_LIST_FILENAME "vr_apps_list.dat"

#define NAME_STEAMVR	"STEAMVR"
#define NAME_OFFICE		"OFFICE"

// Given a path to a formated file containing a list of program-path pairs, loads the 
// into our registered lists.
// Returns false if file cannot be opened, true otherwise.
bool loadProgramsFromFile(const char* pszPath);

// Returns true if the file exists and we have read permissions
bool fileExists(const char* pszPath);

// Returns the size of the file, in bytes
uint64 fileSize(FILE*);

// Registers a program, if the program exists at the given path
void registerProgram(const char* pszName, const char* pszPath);

// Prints all programs in an indexed list of their names and paths
void printProgramList();

// Given a name, runs the corresponding program and, if bWait, waits until that program terminates.
// Returns true on success, false on failure
bool runProgramByName(const char* pszName, bool bWait = true);

// Reads from the Office-generated file which stores the name of the next program to run.
// Returns the name of the program to run, or returns empty string if there is no
// program to run.
void nextProgramName(char dest[MAX_PATH + 1]);

// Sleeps for the given number of seconds, counting down in the terminal
void sleepCountdown(uint8 numSeconds);

#endif //PROGRAM_OPS_H
