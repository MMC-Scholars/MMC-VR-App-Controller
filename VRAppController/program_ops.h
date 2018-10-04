#ifndef PROGRAM_OPS_H
#define PROGRAM_OPS_H

#include "Types/String.h"
#include "Types/CMap.hpp"
#include "Types/Types.h"

//How long, in seconds, to wait for SteamVR to open before first opening the office
#define STEAMVR_WAITTIME 12.f

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
void registerProgram(const String& name, const String& path);

// Prints all programs in an indexed list of their names and paths
void printProgramList();

// Given a name, runs the corresponding program and, if bWait, waits until that program terminates.
void runProgramByName(const String& name, bool bWait = true);

// Reads from the Office-generated file which stores the name of the next program to run.
// Returns the name of the program to run, or returns empty string if there is no
// program to run.
void nextProgramName(String&);

#endif //PROGRAM_OPS_H
