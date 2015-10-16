#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdio.h>
#include <string>

using namespace std;

const int DEFAULT_SIZE = 10000;
const string CHARSET = "abcdefghijklmnopqrstuvwxyz";

/* Nodes hold key/value pairs */
template<typename T>
struct node
{
    string key;
    T value;
    node() {}
    node(string key, T value) : key(key), value(value) {}
};

/* The Hashmap class */
template<typename T>
class Hashmap
{
    public:
        Hashmap();
        Hashmap(unsigned long map_size);
        ~Hashmap();
        unsigned long hash(string key);
        unsigned long hash2(string key);
        unsigned long get_index(string key);
        bool set(string key, T value);
        T get(string key);
        T delete_value(string key);
        float load();

    private:
        node<T> **slot; /* An array of node pointers */
        unsigned long size;
};

void fill(Hashmap<string>& map, int num_items);
string random_string(int length);

#endif
