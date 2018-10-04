#ifndef CHEL_STRING_H
#define CHEL_STRING_H
#include "CDynList.hpp"

/**
 * @brief Measures length of null-terminated character string.
 * @param str - the string to measure
 * @return - the length of the string
 */
//int strlen(const char * str);

/**
 * @class String
 * @author Chel
 * @date 17/04/2017
 * @file String.h
 * @brief A dynamicly sizeable array-indexed list of characters. 
 * 			The character array is located on the heap, and is reallocated
 * 			to larger and smaller sizes as necessary.
 * 			The array reserves space on either end of itself to allow for
 * 			efficient popping/pushing from either end.
 * 			Allows for implicit conversions to and from {const char *}
 * 
 * 
 */
class String : public CDynList<char> {
private:
	inline void buildFromCStr(const char* str);
	
public:

	String() : CDynList<char>() {}
	
	String(int reservation) : CDynList<char>(reservation) {}

	String(int num, char initial) : CDynList<char>(num, initial) {}
	
	String(const char * str);
	
	String(const String& str);
	
	String(const CDynList<char>& str);
	
	static String fromInt(int toStr);
	
	static String EMPTY;
	
	/**
	 * @brief Removes spaces from the front and end of this
	 */
	inline void trimSpaces() { trimEnds(' '); } 
	
	/**
	 * @brief Removes underscores from the front and end of this
	 */
	inline void trimUnderscores() { trimEnds('_'); }
	
	/**
	 * @brief Updates this String in-place to replace all letters with their uppercase form.
	 * 		Does nothing to already uppercase letters and non-letter characters
	 */
	void toUpperCase();
	
	/**
	 * @brief Updates this String in-place to replace all letters with their lowercase form.
	 * 		Does nothing to already lowercase letters and non-letter characters
	 */
	void toLowerCase(); 
	
	/**
	 * @brief Updates this String in-place to replace all spaces with underscores
	 */
	inline void toUnderScore() { this->replaceMatches(' ', '_'); }
	
	String& operator=(const CDynList<char>& str);
	
	String& operator= (const char* str);
	
	bool operator==(const String& other) const;
	
	String operator+(const char* str);
	
	String& operator+=(const char* str);
	
	String& operator /=(const String& other);
	
	/**
	 * @brief Given a file and a size of that file, splits the file by line into the given destination.
	 * Newline characters are consumed.
	 */
	static void fromFile(FILE* pFile, uint64 fileSize, CDynList<String>& dest);
};

//inline int strlen(const String& str) { return str.length(); }

#endif //CHEL_STRING_H