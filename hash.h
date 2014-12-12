#ifndef HASH_HDR
#define HASH_HDR
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hashnode_s hashnode_t;
typedef struct hashtable_s hashtable_t;

struct hashnode_s {
    hashnode_t *next;
    size_t keylen;
    void *key;
};

struct hashtable_s {
    size_t (*hash)(const void *key, size_t keylen);
    size_t size;
    hashnode_t **nodes;
};

int hashtable_init(hashtable_t *table, size_t pow2size, size_t (*hash)(const void *key, size_t keylen));
void hashtable_destroy(hashtable_t *table);
void hashtable_insert(hashtable_t *table, hashnode_t *node, void *key, size_t keylen);
hashnode_t *hashtable_search(hashtable_t *table, const void *key, size_t keylen);
void hashtable_remove(hashtable_t *table, const void *key, size_t keylen);

#define hashtable_ref(ELEMENT, TYPE, MEMBER) \
    ((TYPE *)((unsigned char *)(ELEMENT) - offsetof(TYPE, MEMBER)))

#ifdef __cplusplus
}
#endif

#endif
