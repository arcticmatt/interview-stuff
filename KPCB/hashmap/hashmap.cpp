#include <cstdlib>
#include <ctime>
#include <cmath>
#include <cassert>
#include <iostream>
#include <type_traits>
#include "hashmap.h"

using namespace std;

int main(int argc, const char* argv[]) {
    // Can take the size of the hashmap as an argument
    unsigned long size;
    if (argc > 1)
        size = atoi(argv[1]);
    else
        size = DEFAULT_SIZE;

    vector<string> keys;
    vector<string> values;
    Hashmap<string> map(size);
    clock_t begin_time = clock();
    test_map(map, size / 2, size / 3, size);
    test_change_value(map);
    cout << "Test takes " <<
        float(clock() - begin_time) / CLOCKS_PER_SEC << " seconds" << endl;
}

/*** Test Functions ***/
/*
 * Tests hashmap by filling it with a bunch of values. Then calls the
 * delete method on a fraction of the keys, and makes sure the correct number
 * get deleted by comparing the fraction deleted to the final load value.
 *
 * For this test to work, all inserted keys need to be UNIQUE so that fill
 * always inserts new key/value pairs.
 */
void test_map(Hashmap<string>& map, int num_fill, int num_delete, int map_size) {
    cout << "Testing hashmap..." << endl;
    vector<string> fill_keys;
    vector<string> fill_vals;
    fill(map, num_fill, fill_keys, fill_vals);
    int num_keys = fill_keys.size();
    cout << "Inserted " << num_keys << " key/value pairs" << endl;

    num_delete = num_keys < num_delete ? num_keys : num_delete;
    vector<string> delete_keys;
    vector<string> delete_vals;
    for (int i = 0; i < num_delete; i++)
        delete_keys.push_back(fill_keys[i]);
    delete_values(map, num_delete, delete_keys, delete_vals);
    // Should be able to delete every key, because when filling, we only
    // inserted the key/value pairs that were successful into our vectors
    cout << "Deleted " << delete_keys.size() << " key/value pairs" << endl;
    assert (delete_keys.size() == num_delete);

    // Make sure the values we deleted match up with the values we inserted
    for (int i = 0; i < num_delete; i++) {
        //cout << fill_vals[i] << ", " << delete_vals[i] << endl;
        assert (fill_vals[i].compare(delete_vals[i]) == 0);
    }

    // Make sure the load value makes sense
    float practical_load = (float) (num_fill - num_delete) / map_size;
    float map_load = map.load();
    cout << "Practical load = " << practical_load << endl;
    cout << "Map load = " << map_load << endl;
    assert (abs(practical_load - map_load) < EPSILON);
}

/*
 * A simple test that makes sure changing the value for a key works.
 */
void test_change_value(Hashmap<string>& map) {
    string key = "matt";
    string val = "sucks";
    string val2 = "rocks";
    map.set(key, val);
    string orig = map.get(key);
    map.set(key, val2);
    string changed = map.get(key);

    assert (orig.compare("sucks") == 0);
    assert (orig.compare(changed) != 0);
    assert (changed.compare("rocks") == 0);
}

/*
 * Fills the hashmap with random key/value pairs. Puts the key/value pairs
 * that were successfully inserted into the hashmap into the passed-in vectors.
 */
void fill(Hashmap<string>& map, int num_items, vector<string>& keys,
        vector<string>& values) {
    for (int i = 0; i < num_items; i++) {
        string key = random_string(10);
        string value = random_string(10);
        if (map.set(key, value)) {
            keys.push_back(key);
            values.push_back(value);
        }
    }
}

/*
 * Deletes a certain number of keys from the hashmap. Puts the values that
 * were deleted into the passed-in vector.
 */
void delete_values(Hashmap<string>&map, int num_items, vector<string>& keys,
        vector<string>& values) {
    for (int i = 0; i < num_items; i++) {
        string key_to_del = keys[i];
        string del_value = map.delete_value(key_to_del);
        values.push_back(del_value);
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

/* This method searches the hashmap for an index to insert the passed-in key.
 * If the passed-in key already exists in the table, it returns that index
 * so that the key can be updated with the new value.
 *
 * We search for a vacant index using double hashing, which is similar to linear
 * probing. Basically, we search for an index by starting at the hash index of
 * our key, and then taking steps whose sizes are based on a different hash
 * function (hash2, in our case).
 */
template<typename T>
unsigned long Hashmap<T>::get_insert_index(string key) {
    unsigned long index = hash(key);
    unsigned long num_steps = 0;
    unsigned long incr = hash2(key);
    bool same_key = false;
    node<T> *curr_node = slot[index];

    // Search for vacant space, or for space with passed-in key
    while (curr_node != NULL) {
        if (num_steps == size) // have looped
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
 * This method searches the hashmap for the index of the passed-in key.
 * This index can be used to delete a key, or reference the value of a key.
 *
 * We search for keys using double hashing, which is similar to linear probing.
 * Basically, we search for an index by starting at the hash index of our key,
 * and then taking steps whose sizes are based on a different hash function (hash2,
 * in our case).
 */
template<typename T>
unsigned long Hashmap<T>::get_index(string key) {
    unsigned long index = hash(key);
    unsigned long num_steps = 0;
    unsigned long incr = hash2(key);
    node<T> *curr_node = slot[index];

    // Search for the key
    while (true) {
        if (num_steps == size) // have looped
            break;
        if (curr_node != NULL && key.compare(curr_node->key) == 0)
            return index;
        index = (index + incr) % size;
        curr_node = slot[index];
        num_steps++;
    }

    return -1;
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
    unsigned long index = get_insert_index(key);
    if (index == -1) {
        return false;
    } else if (slot[index] != NULL) { // Just change value
        assert (key.compare(slot[index]->key) == 0);
        slot[index]->value = value;
    } else {
        slot[index] = new node<T>(key, value);
    }
    return true;
}

/*
 * Returns the value associated with the given key, or null if no value is set.
 */
template<typename T>
T Hashmap<T>::get(string key) {
    unsigned long index = get_index(key);
    if (index == -1) {
        // NULL is not a compatible return type with every type in C++. So
        // we'll just initialize a default object of type T and return that.
        T obj = T();
        return obj;
    } else {
        return slot[index]->value;
    }
}

/*
 * Deletes the value associated with the given key, returning the value on
 * success or null if the key has no value or if the key doesn't exist.
 */
template<typename T>
T Hashmap<T>::delete_value(string key) {
    unsigned long index = get_index(key);
    if (index == -1) {
        // NULL is not a compatible return type with every type in C++. So
        // we'll just initialize a default object of type T and return that.
        T obj = T();
        return obj;
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
