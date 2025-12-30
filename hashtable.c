#include <stdio.h>
#include <stdlib.h>
#include "linkedlist.h"
#include "hashtable.h"

struct hashtable {
    // TODO: define hashtable struct to use linkedlist as buckets
    linkedlist_t **buckets;
    int bucket_count;
};

/**
 * Hash function to hash a key into the range [0, max_range)
 */
static int hash(int key, int max_range) {
    // TODO: feel free to write your own hash function (NOT REQUIRED)
    key = (key > 0) ? key : -key;
    return key % max_range;
}

hashtable_t *ht_init(int num_buckets) {
    // TODO: create a new hashtable
    hashtable_t *ht = malloc(sizeof(hashtable_t));
    linkedlist_t **buckets = malloc(sizeof(linkedlist_t*) * num_buckets);
    ht->buckets = buckets;
    for (int i = 0; i < num_buckets; i++)
    {
        ht->buckets[i] = NULL;
    }
    ht->bucket_count = num_buckets;
    return ht;
}

void ht_add(hashtable_t *table, int key, int value) {
    // TODO: create a new mapping from key -> value.
    // If the key already exists, replace the value.
    if (table == NULL) return;
    int target = hash(key, table->bucket_count);
    linkedlist_t *chosen_bucket = table->buckets[target];
    if (chosen_bucket == NULL) 
    {
        table->buckets[target] = ll_init();
    }
    ll_add(table->buckets[target], key, value);
}

int ht_get(hashtable_t *table, int key) {
    // TODO: retrieve the value mapped to the given key.
    // If it does not exist, return 0
    if (table == NULL) return 0;
    int target = hash(key, table->bucket_count);
    if (table->buckets[target] == NULL) return 0;
    return ll_get(table->buckets[target], key);
}

int ht_size(hashtable_t *table) {
    // TODO: return the number of mappings in this hashtable
    int sum = 0;
    for (int i = 0; i < table->bucket_count; i++)
    {
        if (table->buckets[i] == NULL) continue;
        sum += ll_size(table->buckets[i]);
    }
    return sum;
}
