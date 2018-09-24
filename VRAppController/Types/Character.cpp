#include "Character.h"

#define CHARACTER_CASE_OFFSET 32

void Character::toLowerCase(char& ldChar) {
	if (ldChar >= 'A' && ldChar <= 'Z')
		ldChar += CHARACTER_CASE_OFFSET;
}

void Character::toUpperCase(char& ldChar) {
	if (ldChar >= 'a' && ldChar <= 'z')
		ldChar -= CHARACTER_CASE_OFFSET;
}

int Character::numDigitsFor(int n) {
	int digits = n < 0 ? 2 : 1;
	n = n < 0 ? -n : n;
	
	while (n > 9) {
		n /= 10;
		digits++;
	}
	return digits;
}