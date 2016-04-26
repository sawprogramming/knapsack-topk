#ifndef _INDEXED_VECTOR_
#define _INDEXED_VECTOR_

#include <map>
#include <vector>
#include <utility>
#include <algorithm>

template <typename Key, typename Value>
// ****************************************************************************
// Use this when you need a vector sorted in descending order by value 
// and random access to the values in the vector is needed.
//
// It behaves like a vector when used with the iterators.
// It behaves like a map when used with [] or at().
// ****************************************************************************
class indexed_vector {
public:
	typedef typename std::vector<std::pair<Key, Value>>::iterator iterator;
	typedef typename std::vector<std::pair<Key, Value>>::const_iterator const_iterator;

public:
	indexed_vector();

	void push_back(std::pair<Key, Value> row);

	void   clear();
	bool   HasKey(const Key& key)    const;
	size_t size()                  const;
	const Value& at(const size_t& pos) const;

	typename iterator       erase(const Key& key);
	typename iterator       begin(const bool& sort = true);
	typename iterator       end();
	typename const_iterator begin() const;
	typename const_iterator end() const;

	Value& operator[](const Key& i);
	const Value& operator[](const Key& i) const;

private:
	void GenerateIndexes() const;

	mutable bool                       dirty_;
	mutable std::map<Key, size_t>      indexes_;
	mutable std::vector<std::pair<Key, Value>> values_;
};

// ********************************************************************************************************************
template <typename Key, typename Value>
indexed_vector<Key, Value>::indexed_vector() : dirty_(false) {}

template <typename Key, typename Value>
void indexed_vector<Key, Value>::push_back(std::pair<Key, Value> row) {
	values_.push_back(row);
	indexes_[row.first] = values_.size() - 1;
	dirty_ = true;
}

template <typename Key, typename Value>
void indexed_vector<Key, Value>::clear() {
	dirty_ = false;
	indexes_.clear();
	values_.clear();
}

template <typename Key, typename Value>
bool indexed_vector<Key, Value>::HasKey(const Key& key) const {
	return indexes_.find(key) != indexes_.end();
}

template <typename Key, typename Value>
size_t indexed_vector<Key, Value>::size() const {
	return values_.size();
}

template <typename Key, typename Value>
const Value& indexed_vector<Key, Value>::at(const size_t& pos) const {
	if (dirty_) GenerateIndexes();
	return values_[pos].second;
}

template <typename Key, typename Value>
typename indexed_vector<Key, Value>::iterator indexed_vector<Key, Value>::erase(const Key& key) {
	indexed_vector<Key, Value>::iterator itr = values_.erase(values_.begin() + indexes_[key]);
	indexes_.erase(key);
	GenerateIndexes();
	return itr;
}


template <typename Key, typename Value>
// ****************************************************************************
// Returns an iterator to the beginning of the vector. If you do not require 
// the vector to be sorted before returning the iterator, pass false as the 
// parameter (true by default). This is useful when you are using the iterator
// to do a on a large number of modifications to the vector that do not require
// the vector to be sorted to function correctly.
// ****************************************************************************
typename indexed_vector<Key, Value>::iterator indexed_vector<Key, Value>::begin(const bool& sort) {
	if (dirty_ && sort) GenerateIndexes();
	dirty_ = true;
	return values_.begin();
}

template <typename Key, typename Value>
typename indexed_vector<Key, Value>::iterator indexed_vector<Key, Value>::end() {
	return values_.end();
}

template <typename Key, typename Value>
typename indexed_vector<Key, Value>::const_iterator indexed_vector<Key, Value>::begin() const {
	if (dirty_) GenerateIndexes();
	return values_.begin();
}

template <typename Key, typename Value>
typename indexed_vector<Key, Value>::const_iterator indexed_vector<Key, Value>::end() const {
	return values_.end();
}

template <typename Key, typename Value>
Value& indexed_vector<Key, Value>::operator[](const Key& i) {
	dirty_ = true;
	return values_[indexes_[i]].second;
}

template <typename Key, typename Value>
const Value& indexed_vector<Key, Value>::operator[](const Key& i) const {
	dirty_ = true;
	return values_[indexes_[i]].second;
}


template <typename Key, typename Value>
void indexed_vector<Key, Value>::GenerateIndexes() const {
	indexes_.clear();

	sort(values_.begin(), values_.end(), [](const std::pair<Key, Value>& a, const std::pair<Key, Value>& b) {
		return b.second < a.second;
	});

	for (size_t i = 0; i < values_.size(); ++i) {
		indexes_[values_[i].first] = i;
	}

	dirty_ = false;
}

#endif