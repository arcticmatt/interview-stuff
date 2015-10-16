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
    Hashmap<string> map(size);
    fill(map, size / 5);
}

/*** Test Functions ***/
/*
 * Fills the hashmap with random key/value pairs.
 */
void fill(Hashmap<string>& map, int num_items) {
    for (int i = 0; i < num_items; i++) {
        map.set(random_string(7), random_string(7));
    }
}

/*
 * Generates a random string.
 */
string random_string(int length) {
    string rand_string;
    rand_string.resize(length);

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
    first_hash_modifier = rand() % (size * 2);
    second_hash_modifier = rand() % (size * 3);
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
 * We are going to use Cuckoo hashing. So we need to have two different hash
 * functions. Which one we use is determined by the first parameter.
 */
template<typename T>
unsigned long Hashmap<T>::hash(bool first, string key) {
    unsigned long key_val = 0;
    const char *s = key.c_str();
    int c;

    while ((c = *s++))
        key_val += c;

    //cout << "key_val for key = " << key << " currently equals " << key_val <<
        //endl;

    if (first)
        return (key_val + first_hash_modifier) % size;
    else
        return (key_val + second_hash_modifier) % size;
}

/*
 * Rebuilds the hashmap using the nodes currently in it and the kicked node (the
 * node left out when breaking from the cuckoo insertion). Does so in-place.
 */
template<typename T>
bool Hashmap<T>::rebuild() {
    cout << "Rebuilding table" << endl;
    for (int i = 0; i < size; i++) {
        if (slot[i] != NULL) {
            set(slot[i]->key, slot[i]->value);
            //insert_into_first(slot[i], slot[i]->key);
        }
    }
    set(kicked_node->key, kicked_node->value);
    return true;
    cout << "Table rebuilt" << endl;
}

/*
 * Stores the given key/value pair in the hashmap. Returns a boolean value
 * indicating success/failure of the operation.
 *
 * We use the Cuckoo hashing method to insert values into our hashmap. The basic
 * idea is that we use two hash functions instead of one. This gives each key
 * two possible locations in our hashmap. So every time we insert a node, we check
 * to see if there is a node already there. If not, we just insert. If there is,
 * we kick the other node out, and insert the kicked-out node into its other
 * position. And this goes on.
 *
 * If we swap elements more than some max amount of times, we just return
 * false (instead of rehashing).
 */
template<typename T>
bool Hashmap<T>::set(string key, T value) {
    swap_count = 0;
    // If insert fails, rebuild table
    if (!insert_into_first(new node<T>(key, value), key)) {
        return rebuild();
    }
}

/*
 * Inserts a key/value pair using the first hash function.
 */
template<typename T>
bool Hashmap<T>::insert_into_first(node<T> *nd, string orig_key) {
    string key = nd->key;
    T value = nd->value;
    if (should_break()) {
        kicked_node = new node<T>(key, value);
        return false;
    }
    unsigned long index = hash(true, key);
    node<T> *prevNode = slot[index];
    // Insert new node
    slot[index] = new node<T>(key, value);
    // If we displaced a node with a different key, put it into its other position
    if (prevNode != NULL && prevNode->key.compare(key) != 0) {
        return insert_into_second(prevNode, orig_key);
    } else {
        return true;
    }
}

/*
 * Inserts a key/value pair using the second hash function.
 */
template<typename T>
bool Hashmap<T>::insert_into_second(node<T> *nd, string orig_key) {
    string key = nd->key;
    T value = nd->value;
    if (should_break()) {
        kicked_node = new node<T>(key, value);
        return false;
    }
    unsigned long index = hash(false, key);
    node<T> *prevNode = slot[index];
    // Insert new node
    slot[index] = new node<T>(key, value);
    // If we displaced a node with a different key, put it into its other position
    if (prevNode != NULL && prevNode->key.compare(key) != 0)
        return insert_into_first(prevNode, orig_key);
    else
        return true;
}

/*
 * If we have swapped elements more than some max number of times,
 * break out of the swapping loop.
 */
template<typename T>
bool Hashmap<T>::should_break() {
    swap_count++;
    return swap_count > MAX_SWAPS;
}

/*
 * Returns the value associated with the given key, or null if no value is set.
 */
template<typename T>
T Hashmap<T>::get(string key) {
    // Check first location
    unsigned long first_index = hash(true, key);
    node<T> *first_node = slot[first_index];
    if (first_node != NULL && key.compare(first_node->key) == 0)
        return first_node->value;

    // Check second location
    unsigned long second_index = hash(false, key);
    node<T> *second_node = slot[second_index];
    if (second_node != NULL && key.compare(second_node->key) == 0)
        return second_node->value;

    return NULL;
}

/*
 * Deletes the value associated with the given key, returning the value on
 * success or null if the key has no value.
 */
template<typename T>
T Hashmap<T>::delete_value(string key) {
    // Check first location
    unsigned long first_index = hash(true, key);
    node<T> *first_node = slot[first_index];
    if (first_node != NULL && key.compare(first_node->key) == 0) {
        // Delete node from array and from memory and return value
        slot[first_index] = NULL;
        T value = first_node->value;
        delete(first_node);
        return value;
    }

    // Check second location
    unsigned long second_index = hash(false, key);
    node<T> *second_node = slot[second_index];
    if (second_node != NULL && key.compare(second_node->key) == 0) {
        // Delete node from array and from memory and return value
        slot[second_index] = NULL;
        T value = second_node->value;
        delete(second_node);
        return value;
    }

    return NULL;
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

/*
 * Updates the modifiers to the hash functions. Used to rebuild the table.
 */
template<typename T>
void Hashmap<T>::update_hash_modifiers() {
    first_hash_modifier = rand() % (size * 2);
    second_hash_modifier = rand() % (size * 3);
}
