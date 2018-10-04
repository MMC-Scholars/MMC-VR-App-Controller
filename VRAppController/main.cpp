/* Written by Michael Trunk */
/* This program acts as a pseudo-shell meant to switch between different VR applications */

#include "program_ops.h"
#include "string"
#include "iostream"
#include "stdio.h"
#include "Windows.h"



int main(int argc, const char* argv[]) {

	const String defaultProgramListPath = APP_LIST_FILENAME;

	//Load up our list of apps
	if (!loadProgramsFromFile(defaultProgramListPath)) {
		printf(String("ERROR: ") + defaultProgramListPath + " could not be opened, terminating...\n");
	}

	//Start up SteamVR and wait a short time for it to get set up
	printf("Starting SteamVR...\n");
	runProgramByName(NAME_STEAMVR, false);
	Sleep(1000 * STEAMVR_WAITTIME);

	//Now run the office
	runProgramByName(NAME_OFFICE);

	String nextProgramName = 
}