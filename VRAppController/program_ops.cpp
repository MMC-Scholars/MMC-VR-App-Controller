#define _CRT_SECURE_NO_WARNINGS
#include "program_ops.h"
#include "stdio.h"
#include "Windows.h"

CMap<String, String> g_mVRApps;

// Given a path to a formated file containing a list of program-path pairs, loads the 
// into our registered lists.
// Returns false if file cannot be opened, true otherwise.
bool loadProgramsFromFile(const char* pszPath) {
	FILE* pFile = fopen(pszPath, "r");

	if (!pFile) {
		printf("Failed to open program list in %s", pszPath);
		return false;
	}

	//TODO interpret file and load key-value pairs into g_mVRApps
	//TODO check each read path to verify that the file exists, print error otherwise 

	fclose(pFile);
	return true;
}

// Returns true if the file exists and we have read permissions
bool fileExists(const char* pszPath) {
	bool result = false;
	FILE* pFile = fopen(pszPath, "r");
	if (pFile) {
		result = true;
		fclose(pFile);
	}
	return result;
}

// Registers a program, if the program exists at the given path
void registerProgram(const String& name, const String& path) {
	g_mVRApps.add(name, path);
}

// Prints all programs in an indexed list of their names and paths
void printProgramList() {
	for (int i = 0; i < g_mVRApps.size(); i++) {
		printf(String::fromInt(i) + ". " + g_mVRApps.keyByIndex(i) + " " + g_mVRApps.valueByIndex(i));
	}
}

// Given a name, runs the corresponding program and, if bWait, waits until that program terminates.
void runProgramByName(const String& name, bool bWait) {
	//get path
	String path = g_mVRApps.value(name);

	//TODO run program
}

// Reads from the Office-generated file which stores the name of the next program to run.
// Returns the name of the program to run, or returns empty string if there is no
// program to run.
String nextProgramName() {

}