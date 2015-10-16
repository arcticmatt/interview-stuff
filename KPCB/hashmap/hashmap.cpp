#include <cstdlib>
#include <ctime>
#include <cassert>
#include <iostream>
#include "hashmap.h"

using namespace std;

int main(int argc, const char* argv[]) {
    // Can take the size of the hashmap as an argument
    unsigned long size;
    if (argc > 1)
        size = atoi(argv[1]);
    else
        size = DEFAULT_SIZE;

    clock_t begin_time = clock();

    Hashmap<string> map(size);
    fill(map, size);
    cout << "Hashmap load is " << map.load() << endl;

    cout << "Fill time for " << size << " elements = " <<
        float(clock() - begin_time) / CLOCKS_PER_SEC << " seconds" << endl;
}

/*** Test Functions ***/
/*
 * Fills the hashmap with random key/value pairs.
 */
void fill(Hashmap<string>& map, int num_items) {
    for (int i = 0; i < num_items; i++) {
        string key = random_string(7);
        string value = random_string(7);
        map.set(key, value);
    }
}

/*
 * Generates a random string.
 */
string random_string(int length) {
    string rand_string(length, 'c');

    for (int i = 0; i < length; i++)
        rand_string[i] = CHARSET[rand() % CHARSET.length()];

    return rand_string;
}

/*** Constructors & Destructors ***/
template<typename T>
Hashmap<T>::Hashmap() {
    Hashmap(DEFAULT_SIZE);
}

template<typename T>
Hashmap<T>::Hashmap(unsigned long map_size) {
    srand(time(NULL));
    size = map_size;
    cout << "Constructing HM w/size = " << size << endl;
    slot = new node<T>*[size];
    for (int i = 0; i < size; i++)
        slot[i] = NULL;
}

template<typename T>
Hashmap<T>::~Hashmap() {
    cout << "Destructing" << endl;
    for (int i = 0; i < size; i++)
        delete slot[i];
    delete[](slot);
}

/*** Hashmap Functions ***/
/*
 * This is the DJB Hash Algorithm.
 */
template<typename T>
unsigned long Hashmap<T>::hash(string key) {
    unsigned long hash = 5381;
    int c;
    const char *s = key.c_str();

    while ((c = *s++))
        hash = ((hash << 5) + hash) + c;

    return hash % size;
}

/*
 * A simpler hash function, used for Double Hashing.
 */
template<typename T>
unsigned long Hashmap<T>::hash2(string key) {
    unsigned long sum = 0;
    int c;
    const char *s = key.c_str();
    while ((c = *s++))
        sum += c;

    return sum % size;
}

template<typename T>
unsigned long Hashmap<T>::get_index(string key) {
    unsigned long index = hash(key);
    unsigned long num_steps = 0;
    unsigned long incr = hash2(key);
    bool same_key = false;
    node<T> *curr_node = slot[index];
    // Search for vacant space, or for space with passed-in key
    while (curr_node != NULL) {
        if (num_steps == size)
            break;
        if (key.compare(curr_node->key) == 0) {
            same_key = true;
            break;
        }
        index = (index + incr) % size;
        curr_node = slot[index];
        num_steps++;
    }

    if (curr_node != NULL && !same_key)
        return -1;
    else
        return index;
}

/*
 * Stores the given key/value pair in the hashmap. Returns a boolean value
 * indicating success/failure of the operation.
 *
 * We use linear probing to find the index at which to insert our value.
 *
 * If there is no vacant space, we return false.
 */
template<typename T>
bool Hashmap<T>::set(string key, T value) {
    unsigned long index = get_index(key);
    if (index == -1) {
        return false;
    } else if (slot[index] != NULL) { // Just change value
        assert (key.compare(slot[index]->key) == 0);
        slot[index]->value = value;
    } else {
        slot[index] = new node<T>(key, value);
    }
}

/*
 * Returns the value associated with the given key, or null if no value is set.
 */
template<typename T>
T Hashmap<T>::get(string key) {
    unsigned long index = get_index(key);
    if (index == -1)
        return NULL;
    else
        return slot[index];

}

/*
 * Deletes the value associated with the given key, returning the value on
 * success or null if the key has no value.
 */
template<typename T>
T Hashmap<T>::delete_value(string key) {
    unsigned long index = get_index(key);
    if (index == -1) {
        return NULL;
    } else {
        // Delete node from array and memory, and return value
        T value = slot[index]->value;
        delete(slot[index]);
        slot[index] = NULL;
        return value;
    }
}

/*
 * Returns a float value representing the load factor:
 * (items in hashmap) / (size of hashmap)
 * of the data structure. Should never be greater than 1.
 */
template<typename T>
float Hashmap<T>::load() {
    int num_items = 0;
    for (int i = 0; i < size; i++) {
        if (slot[i] != NULL)
            num_items++;
    }
    return (float) num_items / size;
}
