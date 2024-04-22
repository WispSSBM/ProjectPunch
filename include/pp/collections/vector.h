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

#include <types.h>
#include <cstddef>

namespace PP {
namespace Collections {

// the most scuffed vector implementation of all time
class vector {
public:
    typedef bool (*unaryPredicate)(const void* v);

    vector();
    explicit vector(size_t size);
    vector(size_t size, void* val);
    vector(const vector& other);
    ~vector() { delete[] Array; };

    void* operator[](u32 index) const;
    bool set(u32 index, void* val);
    bool operator==(const vector& other);
    bool operator!=(const vector& other);
    /*bool operator<(const vector<void*>& other);
    bool operator>(const vector<void*>& other);
    bool operator<=(const vector<void*>& other);
    bool operator>=(const vector<void*>& other);*/
    bool push(void* val);
    bool insert(void* val, u32 index);
    void pop_back();
    size_t size() const;
    bool reallocate(size_t newSize);
    //void insert(size_t index, const void*& val);
    void erase(size_t index);
    //destroys elements from start, up to (but not including) end
    //void erase(size_t start, size_t end);

    // sets the length to 0 without trying to free the elements
    void forget();
    void clear();
    bool empty();

    void** allocate(size_t newSize);

    size_t index(const void* val);
    //size_t findIf(bool (*condition)(const void*& x));

private:
    size_t length; //  = 0;
    size_t maxLength; // = 10;
    void** Array;
};

}} // namespace

#endif