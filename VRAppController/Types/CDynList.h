#ifndef CHEL_DYNLIST_H
#define CHEL_DYNLIST_H

#include "Clamps.h"

class String;
	/**
	 * @class CDynList
	 * @author Awesome
	 * @date 27/05/2017
	 * @file CDynList.h
	 * @brief A dynamically sizeable list. Instances of the template parameter
	 * 		are stored and managed by-value on the heap. The list reserves
	 * 		space on both ends to allow for efficient popping/pushing to either
	 * 		end. Passing by value performs deep copy.
	 * 		
	 * 		Removing elements from the list does not guarantee their destruction,
	 * 		especially in end cases.
	 */
template<class T> class CDynList {
	friend class String;
protected:
	int m_iReservedLength = 5;
	int m_iStartIndex = 0; //start index, inclusive
	int m_iEndIndex = 0; //end index, exclusive
	mutable T* m_array;
	
	int m_iReserveBuffer = 5; //how many spaces to add when reserving new space
	bool m_bAutoTrim = true;
	
	inline int reservedFront() const { return m_iStartIndex; }
	inline int reservedBack() const { return m_iReservedLength - m_iEndIndex + 1; }
	
	inline T* getPtr_noassert(int pos) const {
		return m_array + m_iStartIndex + pos;
	}
	
public:
	/**
	 * @brief Default constructor builds and empty list.
	 */
	CDynList<T>() {m_array = new T[m_iReservedLength];}
	
	/**
	 * @brief Constructors builds an empty list with the given amount of space reserved
	 * @param reservation
	 */
	CDynList<T>(int reservation) {m_iReservedLength = reservation; m_array = new T[m_iReservedLength];}

	/**
	 * @brief Constructor builds an dynamicly sizeable list
	 * @param num - the initial number of elements
	 * @param value - the initial value of all the elements
	 */
	CDynList<T>(int num, T value);
	
	/**
	 * @brief Destructor removes all items from the heap.
	 */
	~CDynList<T>();

	/**
	 * @brief Retrieves the number of elements in the list.
	 * @return 
	 */
	inline int length() const {return m_iEndIndex - m_iStartIndex;}
	
	/**
	 * @brief Checks whether or not the list is empty
	 * @return {length() == 0}
	 */
	inline bool isEmpty() const {return length() == 0;}
	
	/**
	 * @brief Retrieves the length of the list, including empty slots
	 * 			reserved on either side of the list.
	 * @return the number of allocated positions in the list 
	 */
	inline int reservedLength() const {return m_iReservedLength;}
	
	/**
	 * @brief Retrieves a pointer to the element at the given position.
	 * @param pos - the desired offset from the the list's first element
	 * @requires {0 <= pos < length()}
	 * @return - a pointer to the item
	 */
	inline T* getPtr(int pos) const {
		return m_array + m_iStartIndex + pos;
	}
	
	inline T* getPtrEnd(int revpos) const { return getPtr(length() - 1 - revpos); }
	
	/**
	 * @brief Retrieves the value contained at the given position
	 * @param pos - the position to look at
	 * @requires {0 <= pos < length()}
	 * @return the value contained at the position
	 */
	inline T get(int pos) const {return *getPtr(pos);}

	/**
	* @brief Retrieves the value contained at the given position
	* @param pos - the position to look at
	* @requires {0 <= pos < length()}
	* @return the value contained at the position
	*/
	inline T& getRef(int pos) const { return *getPtr(pos); }
	
	/**
	 * @brief Retrieves a value contained at the given index from the end
	 * @param revpos - the position from the end to look at. Ex.
	 * 			get(length()-1) == getEnd(0)
	 * @return the value at the position
	 */
	inline T getEnd(int revpos) const { return get(length() - 1 - revpos); }
	
	/**
	 * @brief Reports the first value in the list without modifying
	 * 		the list
	 * @return The value at the front of {this}
	 */
	inline T front() const { return get(0); }
	
	/**
	 * @brief Reports the last value in the list without modifying
	 * 		the list
	 * @return The value at the end of {this}
	 */
	inline T end() const { return get(length() - 1); }
	
	/**
	 * @brief Sets the number of spaces to add when reserving extra space on
	 * 			on either end of the array.
	 * @param bufferSize - the reserved space will increase by this amount on
	 * 			each reservation.
	 * @requires {bufferSize > 0}
	 * @perfwarn Larger values allow for faster growing and shrinking, but
	 * 				may take up more memory!
	 * @updates this
	 */
	inline void setReserveBuffer(int bufferSize) {m_iReserveBuffer = bufferSize;}
	
	/**
	 * @brief Allocates new array slots at the end of {this}.
	 * @param newSpace - the amount of slots to add
	 * @requires {newSpace > 0}
	 * @perfwarn Larger values allow for faster growing and shrinking, but
	 * 				may take up more memory!
	 * @updates this
	 */
	void reserve(int newSpace);
	
	/**
	 * @brief Allocates new array slots at the front of this {this}.
	 * @param newSpace
	 * @requires {newSpace > 0}
	 * @perfwarn Larger values allow for faster growing and shrinking, but
	 * 				may take up more memory!
	 * @updates this
	 */
	void reserveFront(int newSpace);
	
	/**
	 * @brief Asserts that the lengths of the reserved spaces on either side
	 * 			of the list does not exceed the size of the reserved buffer.
	 * 			Clamps the reserved length on either side, they may not end up
	 * 			as equal.
	 * @block Does nothing if reserved lengths are already within limits.
	 * @updates this
	 */
	void trim();
	
	/**
	 * @brief Sets whether or not to autotrim the list to remove empty reserved
	 * 			space
	 * @param setter - the override
	 * @updates this
	 */
	inline void setAutoTrim(bool setter) {m_bAutoTrim = setter;}
	
	/**
	 * @brief Checks whether or not the dynamic array is set to autotrim
	 * @return the current state of the autotrim
	 */
	inline bool getAutoTrim() const { return m_bAutoTrim;}
	
	/**
	 * @brief Adds a value to a given location, displacing other elements.
	 * @perfwarn May reallocate the list to fit the new size
	 * @param pos - the position to insert the element at
	 * @param value - the element to add
	 * @requires {0 <= pos < length()}
	 * @updates this
	 */
	void add(int pos, T value);
	
	/**
	 * @brief Inserts another list at the given location, displacing other elements
	 * @perfwarn May reallocate the list to fit the new size
	 * @param pos - the position in insert the list at
	 * @param other - the list to insert
	 */
	void add(int pos, const CDynList<T>& other);
	
	/**
	 * @brief Adds an value to the given location if an equal value
	 * 		is not already contained in the list
	 * @param pos - the position to add the value at
	 * @param value - the value to attempt adding
	 * @return - whether or not the value was added
	 */
	bool addUnique(int pos, T value);
	
	/**
	 * @brief Removes a value to a given location, displacing other elements
	 * 			to fill the gap.
	 * @perfwarn May reallocate the list to take up less memory
	 * @param pos - the position of remove from
	 * @requires {0 <= pos < length()}
	 * @updates this
	 * @return the value removed
	 */
	T remove(int pos);
	
	/**
	 * @brief Removes a sequence of values
	 * @perfwarn - May reallocate the list to take up less memory
	 * @param pos - the starting point, inclusive
	 * @param removedLength - the amount of values to remove
	 * @return - a list of copies of the removed values, in original order
	 */
	CDynList<T> remove(int pos, int removedLength);
	
	/**
	 * @brief Removes a sequence of values until the firstfound value which is equivalent
	 * 		to the given value, or until the end of the list.
	 * 		The firstfound value is popped off but it is not included in the result.
	 * @param value - the value to look for.
	 * @return - the sequence of values until the firstfound match.
	 */
	CDynList<T> removeUntil(T value);
	
	/**
	 * @brief Performs removeUntil(T) for the given number of iterations.
	 * 		For example, calling removeUntilIterated('\n', 3) on a String would
	 * 		remove 3 lines from the String and return three Strings in the returned list.
	 * @param value
	 * @param iterations
	 * @return - a list of the resulting sequences.
	 */
	CDynList<CDynList<T>> removeUntilIterated(T value, int iterations);
	
	/**
	 * @brief Removes a sequence of values
	 * @param start - starting point, inclusive
	 * @param xEnd - ending point, exclusive
	 * @return - a list of copies of the removed values, in original order
	 */
	inline CDynList<T> removeByIndexes(int start, int xEnd) { return remove(start, xEnd - start); }
	
	/**
	 * @brief Given a list of indexes, removes the value at each index
	 * @param indexList - the list of indexes to remove
	 * @updates indexList - indexes are modified as values at preceding indexes are removed
	 * @return - a list of the removed values, in original order.
	 */
	CDynList<T> removeByIndexList(CDynList<int>& indexList);
	
	/**
	 * @brief Sets the value at the given position to the given value
	 * @param pos - the position to replace
	 * @param value - the incoming value
	 * @requires {0 <= pos < length()}
	 * @return - the deleted value
	 */
	inline T set(int pos, T value) {T orig = get(pos); *(getPtr(pos)) = value; return orig;}
	
	/**
	 * @brief Replaces all values matching a given value to a given replacement value
	 * @param find - the value to be replaced
	 * @param replacement - the replacing value
	 * @return - the number of replacements made
	 */
	int replaceMatches(T find, T replacement);
	
	/**
	 * @brief Gets the position of a given value in the list, returning -1 otherwise.
	 * @param value - the value to look for.
	 * @return -1 if not found, otherwise the index of the first equal element 
	 */
	int indexOf(const T& value) const;
	int lastIndexOf(const T& value) const;
	int indexOf(const CDynList<T>& other) const;
	int lastIndexOf(const CDynList<T>& other) const;
	
	int indexOfAnyOf(const CDynList<T>& other) const;
	
	int indexOfBounded(T value, int start, int xEnd) const;
	int indexOfBounded(const CDynList<T>& other, int start, int xEnd) const;
	
	inline bool contains(T value) const { return indexOf(value) != -1; }
	inline bool contains(const CDynList<T>& other) const { return indexOf(other) != -1; }
	bool 		containsAnyOf(const CDynList<T>& other) const;
	
	/**
	 * @brief Tests that every element in other is also in this
	 * @param other - the list of values to look for
	 * @return - whether or not they were all found
	 */
	bool		containsAsSubSet(const CDynList<T>& other) const;
	
	bool 		containsAt(const CDynList<T>& other, int pos) const;
	
	/**
	 * @brief Returns by deep-copied value a sublist from two indexes.
	 * @param start - the starting point of the sub-list, inclusive
	 * @param end - the end point of the sublist, exclusive
	 * @requires {start} and {end} are valid indexes
	 * @return - a sub CDynList<T>
	 */
	CDynList<T> 		subString(int start, int end);
	inline CDynList<T> 	subString(int start) { return subString(start, m_iEndIndex); }
	inline CDynList<T> 	subStringByLength(int start, int length) { return subString(start, start + length); }
	
	/**
	 * @brief Pops the value from the end of this.
	 * @perfwarn May reallocate the list to take up less memory
	 * @requires {!isEmpty()}
	 * @return The removed value
	 */
	inline T pop() {return remove(length()-1);}
	
	/**
	 * @brief Pushes a value onto the end of this.
	 * @perfwarn May reallocate the list to fit new size
	 * @param value - the value to push
	 */
	inline void push(T value) {add(length(), value);}
	
	/**
	 * @brief Pops the value from the front of this
	 * @return 
	 */
	inline T popFront() {return remove(0);}
	
	/**
	 * @brief Pushes a value onto the front of this
	 * @param value - the value to add
	 */
	inline void pushFront(T value) {add(0, value);}
	
	/**
	 * @brief Flips the list about its center
	 */
	void flip();
	
	/**
	 * @brief Given an item, trims {this} list to ensure that
	 * 		no elements of equal value are the first or last
	 * 		elements in the list
	 * @param removedItem
	 */
	void trimEnds(T removedItem);
	
	/**
	 * @brief Rotates the list
	 * @param numRotations - the number of rotations to perform
	 * 		positive number rotates from front to end,
	 * 		negative number from end to front
	 */
	void rotate(int numRotations);
	
	/**
	 * @brief Empties the array of all its contents
	 */
	void flush();
	
	int removeBannedElement(const T& element);
	
	/**
	 * @brief Removes elements from {this} that are also contained
	 * 		in {bannedElements}
	 * @param bannedElements - the list of banned elements
	 * @return - the number of removed elements
	 */
	int removeBannedElements(const CDynList<T>& bannedElements);
	
	int removeBannedSequence(const CDynList<T>& bannedSequence);
	
	int removeBannedSequences(const CDynList<CDynList<T>>& bannedSequences);
	
	/**
	 * @brief Removes elements from the front of {this} which are
	 * 		also in {bannedElements} until such an element is not found
	 * @param bannedElements - the list of banned elements
	 * @return - the number of removed elements
	 */
	int removeBannedPrefixingElements(const CDynList<T>& bannedElements);
	
	/**
	 * @brief Given an element, counts the number of elements in the list
	 * 		which are equal to the given element
	 * @param element
	 * @return 
	 */
	int count(const T& element) const;
	
	/**
	 * @brief Given a sequence, counts the number of times the sequence appears
	 * 		in the list
	 * @param sequence - the sequence to look for
	 * @return 
	 */
	int count(const CDynList<T>& sequence) const;
	
	/**
	 * @brief Given a pointer to a void method which takes T* as a parameter,
	 * 		calls the function with a pointer to each element in the list
	 * 		Call order is from first element to last element.
	 * @param procedure - the procedure to call for each item
	 */
	void dispatchProcedure( void (*procedure)(T*) );
	//void dispatchProcedure( void (T::* procedure)());
	
	/**
	 * @brief Given a pointer to a function which takes T* as a parameter
	 * 		and returns R, calls the function with each element in the list,
	 * 		and dumps the return values into the given list
	 * 		Call order is from first element to last element.
	 * @param function - the function to call
	 * @param dest - where the results are dumped into
	 */
	template<class R> void dispatchFunction( R (*function)(T*), CDynList<R>& dest );
	//template<class R> CDynList<R> dispatchFunction( R (T::* function)());
	
	
	/**
	 * @brief Implicit conversion to a pointer to the first
	 * 		element in the list
	 */
	operator T*() const;
	
	bool operator==(const CDynList<T>& other) const;
	
	CDynList<T>& operator=(const CDynList<T>& str);
	
	CDynList<T> operator+(const CDynList<T>& other);
	
	CDynList<T>& operator+=(const CDynList<T>& other);
};
#endif //CHEL_DYNLIST_H