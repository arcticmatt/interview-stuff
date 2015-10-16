#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdio.h>
#include <string>

using namespace std;

const int DEFAULT_SIZE = 100;
const string CHARSET = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
const int MAX_SWAPS = 30;

/* This is the node to be used in the linked list */
template<typename T>
struct node
{
    string key;
    T value;
    node() {}
    node(string key, T value) : key(key), value(value) {}
    node(node& other) : key(other.key), value(other.value) {}
};

template<typename T>
class Hashmap
{
    public:
        Hashmap();
        Hashmap(unsigned long map_size);
        ~Hashmap();
        unsigned long hash(bool first, string key);
        bool set(string key, T value);
        bool insert_into_first(node<T> *nd, string orig_key);
        bool insert_into_second(node<T> *nd, string orig_key);
        bool should_break();
        bool rebuild();
        T get(string key);
        T delete_value(string key);
        float load();

    private:
        node<T> **slot; /* An array of node pointers */
        unsigned long first_hash_modifier;
        unsigned long second_hash_modifier;
        unsigned long size;
        int swap_count = 0;
        node<T> *kicked_node;
        void update_hash_modifiers();
};

void fill(Hashmap<string>& map, int num_items);
string random_string(int length);

#endif
