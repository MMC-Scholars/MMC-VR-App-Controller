#include "String.h"
#include "string"
#include "Character.h"
#include "Types.h"

/**
 * @brief Measures length of null-terminated character string.
 * @param str - the string to measure
 * @return - the length of the string
 */
/*int strlen(const char * str) {
	int length = 0;
	while (*(str++)) length++;
	return length;
}*/

template<> CDynList<char>::operator char*() const {
	//assert that the string is null-terminated
	m_array[m_iEndIndex] = 0;
	return m_array + m_iStartIndex;
}

inline void String::buildFromCStr(const char* str) {
	int num = (int) strlen(str);
	m_iReservedLength = num+1;
	m_array = new char[num+1];
	m_iReserveBuffer = (int) (num * 1.3);
	m_iEndIndex = num;
	for (int i = 0; i < num; i++)
		m_array[i] = str[i];
}

String::String(const char * str) {
	buildFromCStr(str);
}

String& String::operator =(const CDynList<char>& str) {
	if (this != &str) {
		delete[] m_array;
		m_iReservedLength 	= str.m_iReservedLength;
		m_array 			= new char[m_iReservedLength];
		m_iReserveBuffer 	= str.m_iReserveBuffer;
		m_iEndIndex 		= str.m_iEndIndex;
		m_iStartIndex 		= str.m_iStartIndex;
		for (int i = 0; i < str.length(); i++) {
			m_array[i] = str.m_array[i];
		}
	}
	return *this;
}

String::String(const CDynList<char>& str) {
	m_iReservedLength 	= str.m_iReservedLength;
	m_array 			= new char[m_iReservedLength];
	m_iReserveBuffer 	= str.m_iReserveBuffer;
	m_iEndIndex 		= str.m_iEndIndex;
	m_iStartIndex 		= str.m_iStartIndex;
	m_bAutoTrim			= str.m_bAutoTrim;
	for (int i = m_iStartIndex; i < m_iEndIndex; i++) {
		m_array[i] = str.m_array[i];
	}
}

String::String(const String& str) {
	m_iReservedLength 	= str.m_iReservedLength;
	m_array 			= new char[m_iReservedLength];
	m_iReserveBuffer 	= str.m_iReserveBuffer;
	m_iEndIndex 		= str.m_iEndIndex;
	m_iStartIndex 		= str.m_iStartIndex;
	m_bAutoTrim			= str.m_bAutoTrim;
	for (int i = m_iStartIndex; i < m_iEndIndex; i++) {
		m_array[i] = str.m_array[i];
	}
}

#define STRING_MAX_INT_SIZE 33
String String::fromInt(int toStr) {
	char pStr[33];
	sprintf_s(pStr, "%i", toStr);
	return String(pStr);
}

String String::EMPTY = String();

void String::toUpperCase() {
	for (int i = 0; i < length(); i++)
		Character::toUpperCase(getPtr(i));
}

void String::toLowerCase() {
	for (int i = 0; i < length(); i++)
		Character::toLowerCase(getPtr(i));
}

String& String::operator=(const char * str) {
	flush();
	delete[] m_array;
	
	buildFromCStr(str);
		
	return *this;
}

bool String::operator ==(const String& other) const {
	return this->length() == other.length() && (this->length() == 0 || this->indexOf(other) != -1);
}

String String::operator+(const char * str) {
	String copy = *this;
	
	int reservedBack = reservedLength() - m_iEndIndex;
	int otherLength = (int) strlen(str);
	if (otherLength >= reservedBack) {
		copy.reserve(otherLength + m_iReserveBuffer);
	}
	for (int i = 0; i < otherLength; i++)
		copy.push(str[i]);
	
	return copy;
}
		
String& String::operator +=(const char* str) {
	int reservedBack = reservedLength() - m_iEndIndex;
	int otherLength = (int) strlen(str);
	if (otherLength >= reservedBack) {
		this->reserve(otherLength + m_iReserveBuffer);
	}
	for (int i = 0; i < otherLength; i++)
		this->push(str[i]);
		
	return *this;
}

String& String::operator /=(const String& other) {
	if (this->end() == '\\' || this->end()  == '/')
		this->pop();
		
	if (other.front() != '\\' && other.front() != '/')
		this->push('/');
		
	return (*this) += other;
}

inline bool isNewLine(char c) {
	return c == '\n';
}

void String::fromFile(FILE* pFile, uint64 fileSize, CDynList<String>& dest) {
	
	//first, clear and reserve
	dest.flush();

	//estimate reservation based on filesize.
	//assume average of 30 line length
	const int ESTIMATED_LINE_LENGTH = 30;
	dest.reserve(fileSize / ESTIMATED_LINE_LENGTH + 1);

	//setup loop
	char c = getc(pFile);
	if (c != EOF && !isNewLine(c)) dest.push(""); dest.getRef(0).reserve(ESTIMATED_LINE_LENGTH);

	while (c != EOF) {
		if (isNewLine(c)) {
			//trim previous, setup next string
			dest.getPtrEnd(0)->trim();
			dest.push("");
			dest.getPtrEnd(0)->reserve(ESTIMATED_LINE_LENGTH);
		}
		else {
			//otherwise push into current line
			dest.getPtrEnd(0)->push(c);
		}
		c = getc(pFile);
	}
	dest.trim();
}
