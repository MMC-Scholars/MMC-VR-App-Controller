#ifndef CHEL_DYNLIST_HPP
#define CHEL_DYNLIST_HPP

#include "CDynList.h"
#include "stdio.h"

template<class T> CDynList<T>::CDynList(int num, T value) {
	m_array = new T[num];
	m_iReservedLength = num;
	m_iEndIndex = num;
	for (int i = 0; i < num; i++)
		m_array[i] = value;
}

template<class T> CDynList<T>::~CDynList() {
	//flush(); 
	delete[] m_array;
}

template<class T> void CDynList<T>::reserve(int newSpace) {
	m_iReservedLength += newSpace;
	T * newArray = new T[m_iReservedLength];
	for (int i = m_iStartIndex; i < m_iEndIndex; i++) {
		newArray[i] = m_array[i];
	}
	delete[] this->m_array;
	this->m_array = newArray;
}

template<class T> void CDynList<T>::reserveFront(int newSpace) {
	m_iReservedLength += newSpace;
	T * newArray = new T[m_iReservedLength];
	for (int i = m_iStartIndex; i < m_iEndIndex; i++) {
		newArray[i+newSpace] = m_array[i];
	}
	delete[] this->m_array;
	this->m_array = newArray;
	m_iStartIndex += newSpace;
	m_iEndIndex += newSpace;
}

template<class T> void CDynList<T>::trim() {
	//first check if we even need to trim
	int reservedBack = reservedLength() - m_iEndIndex;
	int reserveLimit = m_iReserveBuffer * 2;
	if (m_iStartIndex > reserveLimit || reservedBack > reserveLimit) {
		int reservedFront = m_iStartIndex;
		
		//build new array of correct size
		clampMax(reservedFront, m_iReserveBuffer);
		clampMax(reservedBack, m_iReserveBuffer);
		clampMin(reservedBack, 1);
		m_iReservedLength = reservedFront + length() + reservedBack;
		T * newArray = new T[m_iReservedLength];
		
		//copy contents of old array into new array
		for (int i = 0; i < length(); i++)
			newArray[reservedFront + i] = get(i);
			
		//restore indexes and delete old array
		m_iStartIndex = reservedFront;
		m_iEndIndex = m_iReservedLength - (reservedBack);
		if (m_iEndIndex < 0) m_iEndIndex = 0;
		delete[] m_array;
		m_array = newArray;
	}
}

template<class T> void CDynList<T>::add(int pos, T value) {
	//absolute position in the array
	int absolutePos = m_iStartIndex + pos;
	
	//first determine which side we're adding to
	bool bFrontHalf = pos < length() / 2;
	
	//then determine if we need to reserve new space
	if (bFrontHalf && m_iStartIndex == 0) {
		this->reserveFront(m_iReserveBuffer);
		absolutePos += m_iReserveBuffer;
	} else if (!bFrontHalf && m_iEndIndex == reservedLength()) {
		this->reserve(m_iReserveBuffer);
	}
	
	//shift the values left or right, and insert value
	if (bFrontHalf) {
		//decrement start index
		m_iStartIndex--;
		absolutePos--;
		
		for (int i = m_iStartIndex; i < absolutePos; i++)
			m_array[i] = m_array[i+1];
	} else {
		for (int i = m_iEndIndex; i > absolutePos; i--) 
			m_array[i] = m_array[i-1];
			
		//increment end index
		m_iEndIndex++;
	}
	
	//copy value into its position
	m_array[absolutePos] = value;
}

template<class T> void CDynList<T>::add(int pos, const CDynList<T>& other) {
	int addedLength = other.length(); //so we don't access over and over again
	
	//first determine which side we'd prefer to add to
	bool bFrontHalf = pos < length() / 2;
	
	//check if we can save a copy by using the other side instead
	if (bFrontHalf && reservedFront() < addedLength && reservedBack() > addedLength)
		bFrontHalf = false;
	else if (!bFrontHalf && reservedBack() <= addedLength && reservedFront() >= addedLength)
		bFrontHalf = true;
	
	//determine if we need to reserve space
	if (bFrontHalf && reservedFront() < addedLength)
		reserveFront(addedLength);
	else if (!bFrontHalf && reservedBack() <= addedLength) //we check equivalance too to allow null-termination
		reserve(addedLength);
	
	//displace values to create gap 
	if (bFrontHalf) {
		m_iStartIndex -= addedLength;
		int endCopy = m_iStartIndex + pos;
		for (int i = m_iStartIndex; i < endCopy; i++)
			m_array[i] = m_array[i+addedLength];
	} else {
		m_iEndIndex += addedLength;
		int endCopy = m_iStartIndex + pos + addedLength;
		for (int i = m_iEndIndex - 1; i > endCopy; i--) {
			m_array[i] = m_array[i - addedLength];
		}
	}
	
	//copy values into place
	for (int i = 0; i < addedLength; i++)
		*getPtr_noassert(pos + i) = other.get(i);
}

template<class T> bool CDynList<T>::addUnique(int pos, T value) {
	bool bAdd = indexOf(value) == -1;
	if (bAdd) {
		this->add(pos, value);
	}
	return bAdd;
}

template<class T> T CDynList<T>::remove(int pos) {
	
	//absolute position in the array
	int absolutePos = m_iStartIndex + pos;
	
	//first determine which side we're removing from
	bool bFrontHalf = pos < length() / 2;
	
	T removed = get(pos);
	
	//shift the values left or right, and forgetting value
	if (bFrontHalf) {
		for (int i = absolutePos; i > m_iStartIndex; i--)
			m_array[i] = m_array[i-1];
		
		//increment start index
		m_iStartIndex++;
	} else {
		//decrement end index
		m_iEndIndex--;
		for (int i = absolutePos; i < m_iEndIndex; i++) 
			m_array[i] = m_array[i+1];
	}
	
	//now check if we need to autotrim
	if (m_bAutoTrim)
		trim();
		
	return removed;
}

template<class T> CDynList<T> CDynList<T>::remove(int pos, int removedLength) {
	
	//build returned list
	CDynList<T> result = this->subStringByLength(pos, removedLength);
	
	//determine which side to remove from, so there will be less copying later on.
	bool bFrontHalf = pos < (length() / 2);
	
	//perform actual removal and adjust indexes
	if (bFrontHalf) {
		int absoluteEnd = m_iStartIndex + pos + removedLength; //exlusive
		m_iStartIndex += removedLength;
		for (int i = absoluteEnd - 1; i >= m_iStartIndex; i--) {
			m_array[i] = m_array[i - removedLength];
		}
	} else {
		int absoluteStart = m_iStartIndex + pos; //inclusive
		m_iEndIndex -= removedLength;
		for (int i = absoluteStart; i < m_iEndIndex; i++) {
			m_array[i] = m_array[i + removedLength];
		}
	}
	
	//check if we need to trim
	if (m_bAutoTrim)
		trim();
		
	return result;
}

template<class T> CDynList<T> CDynList<T>::removeUntil(T value) {
	int endIndex = indexOf(value);
	if (endIndex == -1) {
		return remove(0, length());
	} else {
		remove(endIndex);
		return removeByIndexes(0, endIndex);
	}
}

template<class T> CDynList<CDynList<T>> CDynList<T>::removeUntilIterated(T value, int iterations) {
	CDynList<CDynList<T>> result;
	while (this->length() > 0 && iterations > 0) {
		result.push(removeUntil(value));
		
		iterations--;
	}
	return result;
}

template<class T> CDynList<T> CDynList<T>::removeByIndexList(CDynList<int>& indexList) {
	//TODO implement!
	//Sort the list and then remove indexes at the end of the list first
	const int finalNumRemovals = indexList.length();
	
	//build empty result list
	CDynList<T> result(finalNumRemovals);
	
	int nextIndex;
	int curLength;
	for (int i = 0; i < finalNumRemovals; i++) {
		nextIndex = indexList.pop();
		
		//decrement indexes greater than ours
		curLength = indexList.length();
		for (int i = 0; i < curLength; i++)
			if (indexList.get(i) > nextIndex)
				indexList.set(i, get(i) - 1);
				
		//now actually remove it and add it to the result list
		result.push(indexList.remove(nextIndex));
	}
	
	return result;
}

template<class T> int CDynList<T>::replaceMatches(T find, T replacement) {
	int nReplacemnts = 0;
	for (int i = 0; i < length(); i++)
		if (get(i) == find) {
			*getPtr(i) = replacement;
			nReplacemnts++;
		}
	return nReplacemnts;
}

template<class T> int CDynList<T>::indexOf(const T& value) const {
	int foundIndex = -1;
	for (int i = 0; i < length(); i++) {
		if (get(i) == value) {
			foundIndex = i;
			break; //I will break you!
		}
	}
	return foundIndex;
}

template<class T> int CDynList<T>::lastIndexOf(const T& value) const {
	int foundIndex = -1;
	for(int i = length()-1; i >= 0; i--) {
		if(get(i) == value) {
			foundIndex = i;
			break; //I will break you!
		}
	}
	return foundIndex;
}

template<class T> int CDynList<T>::indexOf(const CDynList<T>& other) const {
	bool foundFirst = false;
	int foundIndex = -1;
	bool foundLast = false;
	
	if (other.length() == 1)
		foundIndex = this->indexOf(other.get(0));
	else if (other.length() <= length() && !other.isEmpty()) {
		for (int i = 0; i < length() && !foundLast; i++) {
			//if we've already started checking matches
			if (foundFirst) {
				if (!(get(i) == other.get(i - foundIndex))) {
					foundFirst = false;
					foundIndex = -1;
				}
				else if ((i - foundIndex + 1) == other.length()) {
					foundLast = true; //completion
				}
			}
			//check for start of next match
			if (get(i) == other.get(0) && !foundFirst) {
				foundFirst = true;
				foundIndex = i;
			}
		}
		if (!foundLast)
			foundIndex = -1;
	}
	return foundIndex;
}
//Direct reverse of indexOf. Has not been checked. Use with caution. -Joy
template<class T> int CDynList<T>::lastIndexOf(const CDynList<T>& other) const {
	bool foundFirst = false;
	int foundIndex = -1;
	bool foundLast = false;
	
	if(other.length() == 1) {
		foundIndex = this->indexOf(other.get(0));
	}
	else if (other.length() <= length() && !other.isEmpty()) {
		for(int i = length()-1; i >= 0 && !foundFirst; i--) {
			//If identified potential match
			if(foundLast) {
				if(!get(i) == other.get(i - foundIndex)) {
					foundLast = false;
					foundIndex = -1;
				}
				else if((i - foundIndex + 1) == other.length()) {
					foundFirst = true;
				}
			}
			if (get(i) == other.get(0) && !foundLast) {
				foundLast = true;
				foundIndex = i;
			}
		}
		if(!foundFirst)
			foundIndex = -1;
	}
	return foundIndex;
}
template<class T> int CDynList<T>::indexOfAnyOf(const CDynList<T>& other) const {
	int foundIndex = -1;
	for (int i = 0; i < this->length() && foundIndex == -1; i++) {
		if (other.contains(this->get(i))) {
			foundIndex = i;
		}
	}
	
	return foundIndex;
}

template<class T> int CDynList<T>::indexOfBounded(T value, int start, int xEnd) const {
	int foundIndex = -1;
	for (int i = start; i < xEnd; i++) {
		if (get(i) == value) {
			foundIndex = i;
			break; //I will break you!
		}
	}
	return foundIndex;
}

template<class T> int CDynList<T>::indexOfBounded(const CDynList<T>& other, int start, int xEnd) const {
	bool foundFirst = false;
	int foundIndex = -1;
	bool foundLast = false;
	
	if (other.length() == 1)
		foundIndex = this->indexOfBounded(other.get(0), start, xEnd);
	else if (other.length() <= length() && !other.isEmpty()) {
		for (int i = start; i < xEnd && !foundLast; i++) {
			//if we've already started checking matches
			if (foundFirst) {
				if (!(get(i) == other.get(i - foundIndex))) {
					foundFirst = false;
					foundIndex = -1;
				}
				else if ((i - foundIndex + 1) == other.length()) {
					foundLast = true; //completion
				}
			}
			//check for start of next match
			if (get(i) == other.get(0) && !foundFirst) {
				foundFirst = true;
				foundIndex = i;
			}
		}
		if (!foundLast)
			foundIndex = -1;
	}
	return foundIndex;
}

template<class T> bool CDynList<T>::containsAnyOf(const CDynList<T>& other) const {
	bool bFound = false;
	int index = 0;
	while( index < other.length() && !bFound) {
		bFound = this->contains(other.get(index));
		index++;
	}
	return bFound;
}

template<class T> bool CDynList<T>::containsAsSubSet(const CDynList<T>& other) const {
	bool bIsSubSet = true;
	
	for (int i = 0; i < other.length() && bIsSubSet; i++) {
		bIsSubSet = this->contains(other.get(i));
	}
	
	return bIsSubSet;
}

/*template<class T> bool CDynList<T>::containsAt(const CDynList<T>& other, int pos) {
	if (pos + other.length() > this->length())
		return false;
	
	bool bMatches = true;
	for (int i = 0; i < other.length() && bMatches; i++)
		bMatches = this->get(pos + i) == other.get(i);
		
	return bMatches;
}*/

template<class T> CDynList<T> CDynList<T>::subString(int start, int end) {
	CDynList<T> result(end - start + 2);
	//can't use null termination, in case we're storing objects by value
	for (int i = 0; i < end - start; i++)
		result.m_array[i] = m_array[i + start];
	result.m_iEndIndex = end - start;
	return result;
}

template<class T> void CDynList<T>::flip() {
	int num = length();
	for (int i = 0; i < num / 2; i++) {
		swap(getPtr(i), getPtr(num-i-1));
	}
}

template<class T> void CDynList<T>::trimEnds(T removedItem) {
	while (length() > 0 && front() == removedItem)
		popFront();
	
	while (length() > 0 && end() == removedItem)
		pop();
}

template<class T> void CDynList<T>::rotate(int numRotations) {
	//rotate front to end
	while (numRotations > 0) {
		push(popFront());
		numRotations--;
	}
	//rotate end to front
	while (numRotations < 0) {
		pushFront(pop());
		numRotations++;
	}
}

template<class T> void CDynList<T>::flush() {
	if (length() > 0) {
		bool initialTrimState = getAutoTrim();
		setAutoTrim(false);
		remove(0, length());
		trim();
		setAutoTrim(initialTrimState);
	}
}

template<class T> int CDynList<T>::removeBannedElement(const T& element) {
	int removedCount = 0;
	int index = 0;
	while (index < this->length()) {
		if (element == get(index)) {
			remove(index);
			removedCount++;
		} else {
			index++;
		}
	}
	return removedCount;
}

template<class T> int CDynList<T>::removeBannedElements(const CDynList<T>& bannedElements) {
	int removedCount = 0;
	int index = 0;
	while (index < this->length()) {
		T curVal = this->get(index);
		if (bannedElements.indexOf(curVal) != -1) {
			this->remove(index);
			removedCount++;
		} else {
			index++;
		}
	}
	return removedCount;
}

template<class T> int CDynList<T>::removeBannedSequence(const CDynList<T>& bannedSequence) {
	int removedCount = 0;
	int removedLength = bannedSequence.length();
	int nextFoundIndex = this->indexOf(bannedSequence);
	while (nextFoundIndex != -1) {
		this->remove(nextFoundIndex, removedLength);
		nextFoundIndex = indexOfBounded(bannedSequence, nextFoundIndex, length());
		removedCount++;
	}
	return removedCount;
}

template<class T> int CDynList<T>::removeBannedSequences(const CDynList<CDynList<T>>& bannedSequences) {
	int removedCount = 0;
	for (int i = 0; i < bannedSequences.length(); i++)
		removedCount += this->removeBannedSequence(*(bannedSequences.getPtr(i)));
	
	return removedCount;
}

template<class T> int CDynList<T>::removeBannedPrefixingElements(const CDynList<T>& bannedElements) {
	int removedCount = 0;
	bool bTerminate = false;
	while (!bTerminate) {
		T curVal = this->get(0);
		if (bannedElements.indexOf(curVal) != -1) {
			this->popFront();
			removedCount++;
		} else {
			bTerminate = true;
		}
	}
	return removedCount;
}

template<class T> int CDynList<T>::count(const T& element) const {
	int count = 0;
	int iLength = length();
	for (int i = 0; i < iLength; i++)
		if (get(i) == element)
			count++;
	return count;
}

template<class T> int CDynList<T>::count(const CDynList<T>& sequence) const {
	int count = 0;
	int lastFoundIndex = indexOf(sequence);
	int iLength = length();
	while (lastFoundIndex != -1) {
		count++;
		lastFoundIndex = indexOfBounded(sequence, lastFoundIndex, length);
	}
	return count;
}

template<class T> void CDynList<T>::dispatchProcedure(void (*procedure)(T*)) {
	int len = length();
	for (int i = 0; i < len; i++)
		(*procedure)(getPtr(i));
}

/*template<class T> void CDynList<T>::dispatchProcedure(void (T::* procedure)()) {
	int len = length();
	for (int i = 0; i < len; i++)
		(getPtr(i))->(*procedure)();
}*/

template<class T> template<class R> void CDynList<T>::dispatchFunction(R (*function)(T*), CDynList<R>& dest) {
	//build result list
	dest.flush();
	dest.reserve(this->length() - dest.reservedLength());
	int len = length();
	for (int i = 0; i < len; i++)
		dest.push((*function)(getPtr(i)));
}

/*template<class T> template<class R> CDynList<R> CDynList<T>::dispatchProcedure((R)(T::* function)()) {
	//build result list
	CDynList<R> result(this->length());
	int len = length();
	for (int i = 0; i < len; i++)
		result.set(i, getPtr(i)->(*function)());
	return result;
}*/

/**
 * @brief Implicit conversion to char* for will null-terminate the string
 */
template<> CDynList<char>::operator char*() const;

template<class T> CDynList<T>::operator T*() const {
	return m_array + m_iStartIndex;
}

template<class T> bool CDynList<T>::operator ==(const CDynList<T>& other) const {
	return this->length() == other.length() && (this->length() == 0 ||this->indexOf(other) != -1);
}

template<class T>CDynList<T>& CDynList<T>::operator =(const CDynList<T>& str) {
	if (&str != this) {
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
	return *this;
}

template<class T> CDynList<T> CDynList<T>::operator +(const CDynList<T>& other){
	CDynList<T> copy = *this;
	
	int reservedBack = reservedLength() - m_iEndIndex;
	int otherLength = other.length();
	if (otherLength >= reservedBack) {
		copy.reserve(otherLength + m_iReserveBuffer);
	}
	for (int i = 0; i < otherLength; i++)
		copy.push(other.get(i));
		
	return copy;
}

template<class T> CDynList<T>& CDynList<T>::operator +=(const CDynList<T>& other) {
	int reservedBack = reservedLength() - m_iEndIndex;
	int otherLength = other.length();
	if (otherLength >= reservedBack) {
		this->reserve(otherLength + m_iReserveBuffer);
	}
	for (int i = 0; i < otherLength; i++)
		this->push(other.get(i));
		
	return *this;
}

#endif //CHEL_DYNLIST_HPP