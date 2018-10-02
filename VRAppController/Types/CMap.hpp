#ifndef CHEL_MAP_HPP
#define CHEL_MAP_HPP

#include "CMap.h"

template<class K, class V> bool CMap<K, V>::hasKey(const K& key) const {
	bool bFound = false;
	for (int i = 0; i < m_map.length() && !bFound; i++)
		bFound = (key == m_map.get(i).key());
	return bFound;
}

template<class K, class V> bool CMap<K, V>::hasValue(const V& value) const {
	bool bFound = false;
	for (int i = 0; i < m_map.length() && !bFound; i++)
		bFound = (value == m_map.get(i).value());
	return bFound;
}

template<class K, class V> void CMap<K, V>::add(const K& key, const V& value) {
	//build a map pair and then add it to the list
	SMapPair<K, V> newPair(key, value);
	m_map.push(newPair);
}

template<class K, class V> V CMap<K, V>::remove(const K& key) {
	bool bNotFound = true;
	int i;
	for (i = 0; i < m_map.length() && bNotFound; i++)
		bNotFound = m_map.get(i).key() != key;

	return m_map.remove(--i).value();
}

template<class K, class V> void CMap<K, V>::flush() {
	while (size() > 0)
		m_map.pop();
}

template<class K, class V> V* CMap<K, V>::valuePtr(const K& key) const {
	bool bFound = false;
	int i;
	for (i = 0; i < m_map.length() && !bFound; i++)
		bFound = (key == m_map.get(i).key());

	return m_map.getPtr(--i)->valuePtr();
}

template<class K, class V> K CMap<K, V>::key(const V& value) const {
	bool bFound = false;
	int i;
	for (i = 0; i < m_map.length() && !bFound; i++)
		bFound = (value == m_map.get(i).value());

	return m_map.get(--i).key();
}

template<class K, class V> bool CMap<K, V>::operator ==(const CMap<K, V>& other) const {
	return this->m_map.length() == other.m_map.length() && this->m_map.containsAsSubSet(other.m_map);
}
#endif //CHEL_MAP_HPP