#ifndef CHEL_CHARACTER_H
#define CHEL_CHARACTER_H

namespace Character {
	
	/**
	 * @brief Given a character which is a letter, ensures that it is uppercase
	 * 			if it is not a letter, nothing happens.
	 * @param ldChar - the ASCII character to ensure is uppercase
	 */
	void toUpperCase(char& ldChar);
	inline void toUpperCase(char* pChar) { toUpperCase(*pChar); }
	
	/**
	 * @brief Given a character which is a letter, ensures that it is lowercase
	 * @param ldChar - the ASCII character to ensure is lowercase
	 */
	void toLowerCase(char& ldChar);
	inline void toLowerCase(char* pChar) { toLowerCase(*pChar); }
	
	/**
	 * @brief Given a character, ensures that if it is a space, it becomes an underscore
	 * @param ldChar - the ASCII character to check
	 */
	inline void toUnderScore(char& ldChar) {
		if (ldChar == ' ')
			ldChar = '_';
	}
	inline void toUnderScore(char* pChar) { toUnderScore(*pChar); }
	
	inline bool isLetter(char c) {
		return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
	}
	
	inline bool isNumber(char c) {
		return ( c >= '0' &&  c <= '9');
	}
	
	inline bool isSymbol(char c) {
		return !isLetter(c) && !isNumber(c) && c != ' ' && c != '_';
	}
	
	int numDigitsFor(int n);
}
#endif //CHEL_CHARACTER_H