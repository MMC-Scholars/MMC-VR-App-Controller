/* Written by Michael Trunk */
/* This program acts as a pseudo-shell meant to switch between different VR applications */

#include "program_ops.h"
#include "string"
#include "iostream"

int main(int argc, const char* argv[]) {

	String defaultProgramListPath = "vr_apps_list.dat";

	//Load up our list of apps
	if (!loadProgramsFromFile(defaultProgramListPath)) {
		printf(String("ERROR: ") + defaultProgramListPath + " could not be opened, terminating...\n");
	}

	char buffer[8];
	bool bTerminate = false;
	while (!bTerminate) {
		//Display list of runnable programs
		printProgramList();

		//Get index from user
		printf("Enter the index of the VR app you wish to run,\nor anything else to exit: ");
		std::string temp;
		std::getline(std::cin, temp);
		uint8 index = atoi(temp.c_str());

		// Validate and run program at the given index
		if (index == 0) {
			bTerminate = true;
		}
		else {
			printf(String("Running program named ") + g_aProgramNames.getRef(index));
			runProgramByIndex(index);
		}
	}
}