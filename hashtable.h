//
//  hashtable.h
//  
//
//  Created by Malcolm McKinney on 11/11/12.
//  Copyright 2012 Cornell University. All rights reserved.
//


#ifndef _hashtable_h
#define _hashtable_h
/* A generic definition hashtable of size 'size'.
 * Uses linear probing instead of chaining to decrease memory overhead 
 * No duplicate keys allowed */

typedef struct tuple *tuple_t;

typedef struct hashtable *hashtable_t;

/* Allocates memory for a generic hashtable: consisting of an array of key value pairs.
 * The number of buckets allotted are of size 'size' */


extern hashtable_t new_hashtable(int size, unsigned int (*hash_fnc)(void*));

/* Inserts a value at the index given by hashing the key with the hashing function. 
 * If there is an insert with the same key, then the function fails. 
 * Resizes the hashtable if the new load is larger than the load factor. */

extern int hashtable_insert(hashtable_t hashtable, void* key, void* value);
/* Because there are no duplicates in the hashtable, it may be necessary to
 * update the value of a tuple with an equivalant key. 
 * Returns 0 if successful. Otherwise returns -1 upon error. */

extern int hashtable_update(hashtable_t hashtable, void* key, void* value);

/* Removes the tuple from the hashtable. 
 * Returns 0 if found and deleted. Returns -1 if error. */

extern int hashtable_remove(hashtable_t hashtable, void* key, void** value);

/* Returns the value at the index given by the hash of the key */

extern void* hashtable_get(hashtable_t hashtable, void* key);

/* A check to see if the hashtable contains the exact key.
 * Returns 0 if found. Returns -1 upon error. */

extern int hashtable_contains_key(hashtable_t hashtable, void* key);
/* A linear check to see if the hashtable contains the value specified.
 * Returns 0 if found. Returns -1 if error. */

extern int hashtable_contains_value(hashtable_t hashtable, void* value);

/* Increases the size of the hashtable by a factor of 2. 
 * Increases the multiplier of the hash function by 2.
 * Returns 0 if successful. Returns -1 if fails. */

extern int hashtable_resize(hashtable_t* table);

/* Frees the hashtable */

extern void hashtable_free(hashtable_t hashtable);


#endif
