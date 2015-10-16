#ifndef HASHMAP_H
#define HASHMAP_H

#include <vector>
#include <stdio.h>
#include <string>

using namespace std;

const float EPSILON = .1;
const string STRING_NULL = string();
const int INT_NULL = -1;
const int DEFAULT_SIZE = 1000000;
const string CHARSET = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";

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
        unsigned long get_insert_index(string key);
        unsigned long get_index(string key);
        bool set(string key, T value);
        T get(string key);
        T delete_value(string key);
        float load();

    private:
        node<T> **slot; /* An array of node pointers */
        unsigned long size;
};

/* Test functions */
void test_map(Hashmap<string>& map, int num_fill, int num_delete, int map_size);
void test_change_value(Hashmap<string>& map);
void fill(Hashmap<string>& map, int num_items, vector<string>& keys,
        vector<string>& values);
void delete_values(Hashmap<string>&map, int num_items, vector<string>& keys,
        vector<string>& values);
;
string random_string(int length);

#endif
