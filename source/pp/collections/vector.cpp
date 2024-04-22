//
// Created by johno on 4/27/2020.
//

#include "pp/collections/vector.h"

namespace PP {
namespace Collections {


void* vector::operator[](u32 index) const {
    return Array[index];
}

bool vector::operator==(const vector& other) {
    if (size() != other.size()) {
        return false;
    }
    for (int i = 0; i < size(); i++) {
        if ((*this)[i] != other[i]) {
            return false;
        }
    }
    return true;
}


bool vector::operator!=(const vector& other) {
    return !(*this == other);
}

bool vector::set(u32 index, void* val) {
    if (index >= size()){
        return false;
    }

    Array[index] = val;
    return true;
}



/*bool vector<void*>::operator<(const vector<T>& other);
bool vector<void*>::operator>(const vector<T>& other);
bool vector<void*>::operator<=(const vector<T>& other);
bool vector<void*>::operator>=(const vector<T>& other);*/

bool vector::push(void* val) {
    if (length >= maxLength) {
        if (!reallocate(maxLength * 2)) {
            return false;
        };
    }
    Array[length] = val;
    length++;
    return true;
}

bool vector::insert(void* val, u32 index) {
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

size_t vector::size() const {
    return length;
}

void vector::forget() {
    length = 0;
}

// YOU STILL HAVE TO FREE WHATEVER THIS POINTS AT
void vector::erase(size_t index) {
    for (int i = index; i <= length; i++) {
        Array[i] = Array[i + 1];
    }
    length--;
}

bool vector::reallocate(size_t newSize) {
    void** temp = allocate(newSize);
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


size_t vector::index(const void* val) {
    for (size_t i = 0; i < this->size(); i++) {
        if (val == (*this)[i]) {
            return i;
        }
    }
    return -1;
}


/*template<class void*>
size_t vector<void*>::findIf(bool (*condition)(const T& x)) {
    for (size_t i = 0; i < this->size(); i++) {
        if (condition(*this[i])) {
            return i;
        }
    }
    return this->size();
}*/

void vector::clear() {
    // OSReport("length: %d\n", length);
    length = 0;
}

void* *vector::allocate(size_t newSize) {
    void** ptr = new void*[newSize];
    return ptr;
}

vector::vector() {
    length = 0;
    maxLength = 10;
    Array = allocate(maxLength);
}

vector::vector(size_t size) {
    length = 0;
    maxLength = size;
    Array = allocate(maxLength);
}

vector::vector(size_t size, void* val) {
    maxLength = size;
    Array = allocate(maxLength);
    for(int i = 0; i < size; i++) {
        Array[i] = val;
    }
    length = size;
}

vector::vector(const vector &other) {
    maxLength = other.size();
    length = other.size();
    Array = allocate(maxLength);
    for(int i = 0; i < other.size(); i++) {
        Array[i] = other[i];
    }
}

bool vector::empty() {
    return length == 0;
}

}} // namespace
