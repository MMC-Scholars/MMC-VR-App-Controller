#define _CRT_SECURE_NO_WARNINGS
#include "program_ops.h"
#include "stdio.h"
#include "Windows.h"


static char g_aPrograms[NUM_MAX_PROGRAMS][MAX_PATH + 1];
static int g_iNumPrograms = 0;

inline bool isNewLine(char c) {
	return c == '\n' || c == '\r';
}

inline bool isWhitespace(char c) {
	return c == ' ' || c == '\t';
}

// Given a path to a formated file containing a list of program-path pairs, loads the 
// into our registered lists.
// Returns false if file cannot be opened, true otherwise.
bool loadProgramsFromFile(const char* pszPath) {
	FILE* pFile = fopen(pszPath, "r");

	if (!pFile) {
		printf("Failed to open program list in %s\n", pszPath);
		return false;
	}

	//setup loop
	char nameBuffer[MAX_PATH + 1];
	char pathBuffer[MAX_PATH + 1];
	uint16 nameIndex, pathIndex; //indexes of next empty spots in buffers
	nameIndex = pathIndex = 0;
	bool bName = true; //whether or not we are loading the name - used to switch between nameBuffer and pathBuffer
	memset(nameBuffer, 0, sizeof(nameBuffer));
	memset(pathBuffer, 0, sizeof(pathBuffer));

	// This lambda checks the buffers, registers program, and resets for next program
	auto registerCheck = [&]() {
		//ensure null-termination of what is already in buffers
		nameBuffer[nameIndex] = pathBuffer[pathIndex] = 0;

		//check if we we need to register what is already in the buffers
		//check for name without path
		if (nameIndex && !pathIndex) {
			printf("WARNING: Found program named \"%s\" without path!... ignoring\n", nameBuffer);
		}
		//otherwise ignore cases where buffers are empty
		if (nameIndex && pathIndex) {
			//verify path, then register
			if (fileExists(pathBuffer)) {
				registerProgram(nameBuffer, pathBuffer);
			}
			else {
				printf("WARNING: Found program named \"%s\" with invalid path!...ignoring\n", nameBuffer);
			}
		}

		//reset for next program
		nameIndex = pathIndex = 0;
		bName = true;
		memset(nameBuffer, 0, sizeof(nameBuffer));
		memset(pathBuffer, 0, sizeof(pathBuffer));
	};

	//Register programs one line at a time
	char c = getc(pFile);
	while (c != EOF) {
		//printf("Encountered: %c\n", c);
		//first check if it's a new line
		if (isNewLine(c)) {
			//printf("Register check\n");
			registerCheck();
		}
		//otherwise, check if we were loading the name of the program but then ran into a space...
		else if (bName && isWhitespace(c)) {
			//printf("Switching to path buffer\n");
			bName = false;
		}
		//ignore leading whitespace before loading path
		else if (!bName && pathIndex == 0 && isWhitespace(c)) {
			//printf("Ignoring whitespace\n");
			//do nothing
		}
		//otherwise, let's load into one of our buffers
		else if (bName) {
			//printf("\tLoading into name buffer\n");
			//load name
			nameBuffer[nameIndex++] = c;
		}
		else {
			//printf("\tLoading into path buffer\n");
			//load path
			pathBuffer[pathIndex++] = c;
		}

		//check against oversized paths
		if (pathIndex >= sizeof(pathBuffer) || nameIndex >= sizeof(pathBuffer)) {
			//printf("ERROR: Encountered name or path longer than %zi characters\n", sizeof(pathBuffer));
			//printf("Name=%s", nameBuffer);
			//printf("Path=%s", pathBuffer);
			return false;
		}

		//always advance to next character
		c = getc(pFile);
	}

	//in case there wasn't a new line at the end - run the registerCheck again
	registerCheck();

	// close file
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


// Returns the size of the file, in bytes
uint64 fileSize(FILE* pFile) {
	fseek(pFile, 0, SEEK_END); // seek to end of file
	uint64 size = ftell(pFile); // get current file pointer
	fseek(pFile, 0, SEEK_SET); // seek back to beginning of file
	return size;
}

// Registers a program, if the program exists at the given path
void registerProgram(const char* pszName, const char* pszPath) {
	printf("Registering program %-12s Path=%s\n", pszName, pszPath);
	strcpy_s(g_aPrograms[g_iNumPrograms * 2], pszName);
	strcpy_s(g_aPrograms[g_iNumPrograms * 2 + 1], pszPath);
	g_iNumPrograms++;
}

// Prints all programs in an indexed list of their names and paths
void printProgramList() {
	printf("%i program found\n", g_iNumPrograms);
	for (int i = 0; i < g_iNumPrograms; i++) {
		printf("%s: %s\n", g_aPrograms[2 * i], g_aPrograms[2 * i + 1]);
	}
}

// Given a name, runs the corresponding program and, if bWait, waits until that program terminates.
// Returns true on success, false on failure
bool runProgramByName(const char* pszName, bool bWait) {
	//get path
	const char* pszPath = NULL;
	for (int i = 0; i < g_iNumPrograms; i++) {
		if (strcmp(pszName, g_aPrograms[i * 2]) == 0) {
			pszPath = g_aPrograms[i * 2 + 1];
		}
	}

	if (!pszPath) {
		printf("ERROR: Failed to find path for program named %s\n", pszName);
		return false;
	}

	//use Windows API to start the process
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	printf("Running %s at \"%s\"\n", pszName, pszPath);
	if (CreateProcess(pszPath, NULL, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
		if (bWait) {
			WaitForSingleObject(pi.hProcess, INFINITE);
		}
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		return true;
	}
	else {
		return false;
	}
}

// Reads from the Office-generated file which stores the name of the next program to run.
// Returns the name of the program to run, or returns empty string if there is no
// program to run.
void nextProgramName(char dest[MAX_PATH + 1]) {
	//TODO implement
	
}

// Sleeps for the given number of seconds, counting down in the terminal
void sleepCountdown(uint8 numSeconds) {
	for (uint8 i = 0; i < numSeconds; i++) {
		printf("%i... ", numSeconds - i);
		Sleep(1000);
	}
	printf("\n");
}