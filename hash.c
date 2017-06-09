#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "hash.h"

static hashnode_t*
hash_node_find(hashnode_t* node, const void* key, size_t keylen)
{
    while (node)
    {
        if (keylen != node->keylen)
        {
            node = node->next;
            continue;
        }
        if (memcmp(node->key, key, keylen) == 0)
            return node;
        node = node->next;
    }
    return NULL;
}

static void hash_node_init(hashnode_t* node, void* key, size_t keylen)
{
    node->key = key;
    node->keylen = keylen;
    node->next = NULL;
}

static inline size_t hash_node_bin(size_t bins, size_t keyhash)
{
    return keyhash & (bins - 1);
}

int hashtable_init(hashtable_t* table,
                   size_t pow2size,
                   size_t (*hash)(const void* key, size_t keylen))
{
    assert(pow2size < sizeof(int) * 8);
    table->hash = hash;
    table->size = (1 << pow2size);
    table->nodes = calloc(sizeof(hashnode_t*), table->size);
    return table->nodes ? 0 : -1;
}

void hashtable_destroy(hashtable_t* table)
{
    table->hash = NULL;
    table->size = 0;
    free(table->nodes);
}

void hashtable_insert(hashtable_t* table,
                      hashnode_t* node,
                      void* key,
                      size_t keylen)
{
    hash_node_init(node, key, keylen);
    size_t hash = table->hash(node->key, node->keylen);
    size_t bin = hash_node_bin(table->size, hash);
    hashnode_t* head = table->nodes[bin];
    if (!head)
    {
        table->nodes[bin] = node;
        return;
    }
    if (hash_node_find(head, node->key, node->keylen))
        return;
    table->nodes[bin] = node;
    node->next = head;
}

hashnode_t*
hashtable_search(hashtable_t* table, const void* key, size_t keylen)
{
    size_t hash = table->hash(key, keylen);
    size_t bin = hash_node_bin(table->size, hash);
    hashnode_t* node = table->nodes[bin];
    return hash_node_find(node, key, keylen);
}

void hashtable_remove(hashtable_t* table, const void* key, size_t keylen)
{
    size_t hash = table->hash(key, keylen);
    size_t bin = hash_node_bin(table->size, hash);
    hashnode_t* current = table->nodes[bin];
    hashnode_t* prev = NULL;
    while (current)
    {
        if (keylen != current->keylen)
        {
            prev = current;
            current = current->next;
            continue;
        }
        if (memcmp(current->key, key, keylen) == 0)
        {
            if (prev)
                prev->next = current->next;
            else
                table->nodes[bin] = current->next;
            current->next = NULL;
            return;
        }
        prev = current;
        current = current->next;
    }
}
