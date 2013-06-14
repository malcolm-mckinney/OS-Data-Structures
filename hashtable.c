//
//  hashtable.c
//  
//
//  Created by Malcolm McKinney on 11/8/12.
//  Copyright 2012 Cornell University. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "hashtable.h"

#define LOAD_FACTOR .7


/* Allocates memory for a generic hashtable: consisting of an array of key value pairs.
 * The number of buckets allotted are of size 'size' */

enum bucket_status {
    VACANT = 2, OCCUPIED, DELETED
};

struct tuple{
    enum bucket_status status;
    void* key;
    void* value;
    
};


struct hashtable {
    tuple_t* buckets;
    unsigned int num_buckets;
    double total_items;
    int multiplier;
    unsigned int (*hash_function)(void*);
    
};

/* Returns a pointer to a hashtable, given the initial size and a pointer to a hash function */

hashtable_t new_hashtable(int size, unsigned int (*hash_fnc)(void*)) {
    
    tuple_t* buckets;
    hashtable_t hashtable;
    int i;
    
    if (size <= 0) {
        return NULL;
    }
    
    buckets = (tuple_t*)calloc(size, sizeof(struct tuple));
    
    if (buckets == NULL) {
        return NULL;
    }
    
    for (i = 0; i < size; i++) {
        buckets[i] = (tuple_t)malloc(sizeof(struct tuple));
        buckets[i]->status = VACANT; 
    }
    
    hashtable = (hashtable_t)malloc(sizeof(struct hashtable));
    
    if (hashtable == NULL) {
        free(buckets);
        return NULL;
    }
  
   
    hashtable->multiplier = 1;
    hashtable->buckets = buckets;
    hashtable->num_buckets = size;
    hashtable->total_items = 0;
    hashtable->hash_function = hash_fnc;
    return hashtable;
   
}

/* Inserts a value at the index given by hashing the key with the hashing function. 
 * If there is an insert with the same key, then the function fails. 
 * Resizes the hashtable if the new load is larger than the load factor. */

int hashtable_insert(hashtable_t hashtable, void* key, void* value) {
    
    unsigned int index;
    unsigned short num_iter;
    unsigned short size; 
    unsigned int multiplier;
    tuple_t fetched;
    
    
    if (hashtable == NULL || key == NULL || value == NULL) {
        return -1;
    }
    
    
    multiplier = hashtable->multiplier;
    num_iter = 0;
    size = hashtable->num_buckets;
    index = hashtable->hash_function(key) * hashtable->multiplier;
    index = index % size;
    
    fetched = hashtable->buckets[index];
       
    while (fetched->status == OCCUPIED && num_iter++ < size) {
        
        index = (++index == size) ? 0: index;// 1 % size;
        fetched = hashtable->buckets[index];
    
    }
    
    
    if (num_iter == size) {
        return -1;
    }
    
    
    fetched->key = key;
    fetched->value = value;
    

    hashtable->buckets[index]->status = OCCUPIED;
    
    if (++hashtable->total_items/hashtable->num_buckets > LOAD_FACTOR) {
        hashtable_resize(&hashtable);
    }
    return 0;
}
/* Because there are no duplicates in the hashtable, it may be necessary to
 * update the value of a tuple with an equivalant key. 
 * Returns 0 if successful. Otherwise returns -1 upon error. */

int hashtable_update(hashtable_t hashtable, void* key, void* value, void ** old_value) {
    
    
    unsigned int index;
    unsigned short num_iter;
    unsigned short size; 
    tuple_t kv_pair;
    tuple_t fetched;
    
    
    if (hashtable == NULL || key == NULL || value == NULL || old_value == NULL) {
        return -1;
    }
    
    kv_pair = (tuple_t)malloc(sizeof(struct tuple));
    
    if (kv_pair == NULL) {
        return -1;
    }
    
    num_iter = 0;
    size = hashtable->num_buckets;
    index = hashtable->hash_function(key) * hashtable->multiplier;
    index = index % size;
    fetched = hashtable->buckets[index]; 
    
    while (fetched->status != VACANT && num_iter++ < size) {
              
        if (key == fetched->key) {
            kv_pair->value = value;
            kv_pair->key = key;
            hashtable->buckets[index]= kv_pair; 
            *old_value = fetched->value;
            return 0;
        }
        
        index = (++index == size) ? 0: index;
        fetched = hashtable->buckets[index];        
    }
    
    return -1;
}


/* Removes the tuple from the hashtable. 
 * Returns 0 if found and deleted. Returns -1 if error. */

int hashtable_remove(hashtable_t hashtable, void* key, void** old_value) {
    
    
    int index;
    unsigned short num_iter;
    unsigned short size; 
    unsigned int multiplier;    
    tuple_t fetched;
    
    
    if (hashtable == NULL || key == NULL || old_value == NULL) {
        return -1;
    }
    
    num_iter = 0;
    multiplier = hashtable->multiplier;
    size = hashtable->num_buckets;
    index = hashtable->hash_function(key) * multiplier;
    index = index % size;
    
    fetched = hashtable->buckets[index];
    
    while (fetched->status != VACANT && num_iter++ < size) {
        
        if (fetched->status == OCCUPIED) {
        
            
            
            if (fetched->key == key) {
                fetched->key = NULL;
                
                //It is the programmer's responsibility to free the value
                 
                *old_value = fetched->value;
                fetched->value = NULL;
                fetched->status = DELETED;
                index = (++index == size) ? 0: index;
        
                //If the next bucket is vacant, we want to be vacant
            if (hashtable->buckets[index]->status == VACANT) {
                fetched->status = VACANT;
                index = index - 2;
                // NEGATIVE MOD in C gives negative values
                if (index < 0) {
                    index = size - 1;
                }
                // If we are vacant, and the previous is deleted, we want him to be vacant
                if (hashtable->buckets[index]->status == DELETED) {
                    hashtable->buckets[index]->status = VACANT;
                }
            }
                hashtable->total_items--; 
                return 0;
            }
            
        }
        
        index = (++index == size) ? 0: index;
        fetched = hashtable->buckets[index];
        
    }

    return -1;

}
/* Returns the value at the index given by the hash of the key */

int hashtable_get(hashtable_t hashtable, void* key, void** value) {
    
    unsigned int index;
    unsigned short num_iter;
    unsigned short size; 
    unsigned int multiplier;
    tuple_t fetched;
    
    if (hashtable == NULL || key == NULL || value == NULL) {
        return -1;
    }
    num_iter = 0;
    multiplier = hashtable->multiplier;
    size = hashtable->num_buckets;
    index = hashtable->hash_function(key) * multiplier;
    index = index % size;
    fetched = hashtable->buckets[index];
    
    
    while (fetched->status != VACANT && num_iter++ < size) {
       
        if (fetched->key == key) {
           *value = fetched->value;
            return 0;
        }
        
       index = (++index == size) ? 0: index;
        fetched = hashtable->buckets[index];
    }
    
    return -1;
    
}


/* A check to see if the hashtable contains the exact key.
 * Returns 0 if found. Returns -1 upon error. */

int hashtable_contains_key(hashtable_t hashtable, void* key) {
  
    
    unsigned short index;
    unsigned short num_iter;
    unsigned short size;
    tuple_t fetched;
    
    
    if (hashtable == NULL ||  key == NULL) {
        return -1;
    }
    
    num_iter = 0;
    size = hashtable->num_buckets;
    index = hashtable->hash_function(key);
    index = index % size;
   
    while (fetched->status != VACANT && num_iter++ < size) {
        
        if (fetched == key) {
            return 0;
        }
        
        index = (++index == size) ? 0: index;
        fetched = hashtable->buckets[index];
        
    }   
    return -1;
    
}

/* Increases the size of the hashtable by a factor of 2. 
 * Increases the multiplier of the hash function by 2.
 * Returns 0 if successful. Returns -1 if fails. */
                             
int hashtable_resize(hashtable_t* table) {
   
    hashtable_t hashtable;
    int size;
    hashtable_t hashtable_resized;
    int i;
    int multiplier;
    
    
    if (table == NULL) {
        return -1;
    }
    
    
    hashtable = *table;
    multiplier = hashtable->multiplier * 2;
    size = hashtable->num_buckets;
    hashtable_resized = new_hashtable(size * 2, hashtable->hash_function);
    
  
    if (hashtable_resized == NULL) {
        return -1;
    }
    
    for(i = 0; i < size; i++) {
        if (hashtable->buckets[i] != NULL) {
            hashtable_resized->buckets[i/multiplier] = hashtable->buckets[i];
            
        } 
    
    }
   
    hashtable_resized->total_items = hashtable->total_items;
    hashtable_resized->multiplier = multiplier;
    hashtable_free(hashtable);
    *table = hashtable_resized;
    return 0;

}
/* Frees the hashtable */

void hashtable_free(hashtable_t hashtable) {
    int size;
    int i;
    
    if (hashtable == NULL) {
        return;
    }
    size = hashtable->num_buckets;
    
    for (i = 0; i < size; i++) {
        free(hashtable->buckets[i]);
    }
    
    free(hashtable);
}

