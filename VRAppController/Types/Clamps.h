#ifndef CLAMPS_H
#define CLAMPS_H

template<class T>
inline void clampMin(T& value, T min) {
	if (value < min) value = min;
}

template<class T>
inline void clampMax(T& value, T max) {
	if (value > max) value = max;
}

#endif //CLAMPS_H