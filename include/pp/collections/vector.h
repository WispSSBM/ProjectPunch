#ifndef PP_COLLECTIONS_VECTOR
#define PP_COLLECTIONS_VECTOR

//
// Created by johno on 4/27/2020.
// ported to syriinge by wisp on 3/2024
//

/*
This vector differs from the stl one in BrawlHeaders because
it has deletion capabilities. At some point, perhaps the
implementations should be merged.
*/

#include <memory.h>
#include "stddef.h"

namespace ProjectPunch {
namespace Collections {
// the most scuffed vector implementation of all time
template<class T>
class vector {
public:
    typedef bool (*unaryPredicate)(const T& v);

    vector();
    explicit vector(size_t size);
    vector(size_t size, const T& val);
    vector(const vector<T>& other);
    ~vector<T>();

    T& operator[](u32 index) const;
    bool set(u32 index, const T& val);
    bool operator==(const vector<T>& other);
    bool operator!=(const vector<T>& other);
    /*bool operator<(const vector<T>& other);
    bool operator>(const vector<T>& other);
    bool operator<=(const vector<T>& other);
    bool operator>=(const vector<T>& other);*/
    bool push(const T& val);
    bool insert(const T& val, u32 index);
    void pop_back();
    size_t size() const;
    bool reallocate(size_t newSize);
    //void insert(size_t index, const T& val);
    void erase(size_t index);
    //destroys elements from start, up to (but not including) end
    //void erase(size_t start, size_t end);

    // sets the length to 0 without trying to free the elements
    void forget();
    void clear();
    bool empty();

    T* allocate(size_t newSize);

    size_t index(const T& val);
    //size_t findIf(bool (*condition)(const T& x));

private:
    size_t length; //  = 0;
    size_t maxLength; // = 10;
    T* Array;
};


template<class T>
T& vector<T>::operator[](u32 index) const {
    return Array[index];
}

template<class T>
bool vector<T>::set(u32 index, const T& val) {
    if (index >= size()){
        return false;
    }

    Array[index] = val;
    return true;
}


template<class T>
vector<T>::~vector<T>() {
    delete[] Array;
}


template<class T>
bool vector<T>::operator==(const vector<T>& other) {
    if (size() != other.size()) {
        return false;
    }
    for (int i = 0; i < size(); i++) {
        if (*this[i] != other[i]) {
            return false;
        }
    }
    return true;
}


template<class T>
bool vector<T>::operator!=(const vector<T>& other) {
    return !(*this == other);
}


/*bool vector<T>::operator<(const vector<T>& other);
bool vector<T>::operator>(const vector<T>& other);
bool vector<T>::operator<=(const vector<T>& other);
bool vector<T>::operator>=(const vector<T>& other);*/

template<class T>
bool vector<T>::push(const T& val) {
    if (length >= maxLength) {
        if (!reallocate(maxLength * 2)) {
            return false;
        };
    }
    Array[length] = val;
    length++;
    return true;
}

template<class T>
bool vector<T>::insert(const T& val, u32 index) {
    if (length >= maxLength) {
        if (!reallocate(maxLength * 2)) {
            return false;
        };
    }
    for (u32 i = length; i > index; i--) {
        Array[i + 1] = Array[i];
    }
    Array[index] = val;
    length++;
    return true;
}

template<class T>
void vector<T>::pop_back() {
    delete Array[--length];
}

template<class T>
size_t vector<T>::size() const {
    return length;
}

template<class T>
void vector<T>::forget() {
    length = 0;
}

template<class T>
void vector<T>::erase(size_t index) {
    delete Array[index];
    for (int i = index; i <= length; i++) {
        Array[i] = Array[i + 1];
    }
    length--;
}

template<class T>
bool vector<T>::reallocate(size_t newSize) {
    T* temp = allocate(newSize);
    if (temp == 0) { return false; }

    if(newSize < length) {
        length = newSize;
    }
    for(int i = 0; i < length; i++) {
        temp[i] = Array[i];
    }
    delete[] Array;
    Array = temp;
    maxLength = newSize;
    return true;
}


template<class T>
size_t vector<T>::index(const T& val) {
    for (size_t i = 0; i < this->size(); i++) {
        if (val == *this[i]) {
            return i;
        }
    }
    return -1;
}


/*template<class T>
size_t vector<T>::findIf(bool (*condition)(const T& x)) {
    for (size_t i = 0; i < this->size(); i++) {
        if (condition(*this[i])) {
            return i;
        }
    }
    return this->size();
}*/

template<class T>
void vector<T>::clear() {
    // OSReport("length: %d\n", length);
    for(int i = 0; i < length; i++) {
        delete Array[i];
    }
    length = 0;
}

template<class T>
inline T *vector<T>::allocate(size_t newSize) {
    T* ptr = new T[newSize];
    return ptr;
}

template<class T>
vector<T>::vector() {
    length = 0;
    maxLength = 10;
    Array = allocate(maxLength);
}

template<class T>
vector<T>::vector(size_t size) {
    length = 0;
    maxLength = size;
    Array = allocate(maxLength);
}

template<class T>
vector<T>::vector(size_t size, const T &val) {
    maxLength = size;
    Array = allocate(maxLength);
    for(int i = 0; i < size; i++) {
        Array[i] = val;
    }
    length = size;
}

template<class T>
vector<T>::vector(const vector<T> &other) {
    maxLength = other.size();
    length = other.size();
    Array = allocate(maxLength);
    for(int i = 0; i < other.size(); i++) {
        Array[i] = other[i];
    }
}

template<class T>
bool vector<T>::empty() {
    return length == 0;
}
}
}

#endif