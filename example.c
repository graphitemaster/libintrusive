#include <stdio.h>

#include "rb.h"
#include "avl.h"
#include "list.h"
#include "hash.h"

// RB TREE EXAMPLE
typedef struct {
    int number;
    rb_node_t node;
} test_rb_t;

int test_rb_compare(const rb_node_t *lhs, const rb_node_t *rhs, const void *aux) {
    const test_rb_t *a = (const test_rb_t*)lhs;
    const test_rb_t *b = (const test_rb_t*)rhs;
    if (a->number < b->number) return -1;
    if (a->number > b->number) return 1;
    return 0;
}

void rb() {
    printf(">> RB\n");
    rb_tree_t tree;
    rb_init(&tree, NULL);

    test_rb_t a = { 1 };
    test_rb_t b = { 2 };
    test_rb_t c = { 3 };

    rb_insert(&tree, &a.node, &test_rb_compare);
    rb_insert(&tree, &b.node, &test_rb_compare);
    rb_insert(&tree, &c.node, &test_rb_compare);

    // Display them
    rb_node_t *node = rb_head(&tree);
    while (node) {
        rb_node_t *next = rb_next(node);
        rb_node_t *prev = rb_prev(node);
        test_rb_t *c = rb_ref(node, test_rb_t, node);
        test_rb_t *n = next ? rb_ref(next, test_rb_t, node) : NULL;
        test_rb_t *p = prev ? rb_ref(prev, test_rb_t, node) : NULL;
        printf("current: %d, next: %d, prev: %d\n",
                c->number,
                n ? n->number : -1,
                p ? p->number : -1);
        node = next;
    }
}

// AVL TREE EXAMPLE
typedef struct {
    int number;
    avl_node_t node;
} test_avl_t;

int test_avl_compare(const avl_node_t *lhs, const avl_node_t *rhs, const void *aux) {
    const test_avl_t *a = (const test_avl_t*)lhs;
    const test_avl_t *b = (const test_avl_t*)rhs;
    if (a->number < b->number) return -1;
    if (a->number > b->number) return 1;
    return 0;
}

void avl() {
    printf(">> AVL\n");
    avl_tree_t tree;
    avl_init(&tree, NULL);

    test_avl_t a = { 1 };
    test_avl_t b = { 2 };
    test_avl_t c = { 3 };

    avl_insert(&tree, &a.node, &test_avl_compare);
    avl_insert(&tree, &b.node, &test_avl_compare);
    avl_insert(&tree, &c.node, &test_avl_compare);

    // Display them
    avl_node_t *node = avl_head(&tree);
    while (node) {
        avl_node_t *next = avl_next(node);
        avl_node_t *prev = avl_prev(node);
        test_avl_t *c = avl_ref(node, test_avl_t, node);
        test_avl_t *n = next ? avl_ref(next, test_avl_t, node) : NULL;
        test_avl_t *p = prev ? avl_ref(prev, test_avl_t, node) : NULL;
        printf("current: %d, next: %d, prev: %d\n",
                c->number,
                n ? n->number : -1,
                p ? p->number : -1);
        node = next;
    }
}

// LIST EXAMPLE
typedef struct {
    const char *message;
    link_t link;
} test_list_t;

void list() {
    printf(">> LIST\n");
    list_t list;
    list_init(&list);

    test_list_t a = { "Hello" };
    test_list_t b = { "Intrusive" };
    test_list_t c = { "World" };

    list_push_back(&list, &a.link);
    list_push_back(&list, &b.link);
    list_push_back(&list, &c.link);

    link_t *node = list_head(&list);
    while (node) {
        link_t *next = list_next(node);
        link_t *prev = list_prev(node);
        test_list_t *c = list_ref(node, test_list_t, link);
        test_list_t *n = next ? list_ref(next, test_list_t, link) : NULL;
        test_list_t *p = prev ? list_ref(prev, test_list_t, link) : NULL;
        printf("current: %s, next: %s, prev: %s\n",
                c->message,
                n ? n->message : "(None)",
                p ? p->message : "(None)");
        node = next;
    }
}

typedef struct {
    const char *message;
    hashnode_t node;
} test_hash_t;

size_t hash_func(const void *key, size_t keylen) {
    // One character keys for test, you can implement a more complex hash
    // function here
    return ((const char *)key)[0];
}

void hash() {
    printf(">> HASH\n");
    hashtable_t hash;

    test_hash_t a = { "Hello" };
    test_hash_t b = { "Intrusive" };
    test_hash_t c = { "World" };

    hashtable_init(&hash, 10, &hash_func);

    hashtable_insert(&hash, &a.node, "a", 2);
    hashtable_insert(&hash, &b.node, "b", 2);
    hashtable_insert(&hash, &c.node, "c", 2);

    hashnode_t *ga = hashtable_search(&hash, "a", 2);
    hashnode_t *gb = hashtable_search(&hash, "b", 2);
    hashnode_t *gc = hashtable_search(&hash, "c", 2);

    test_hash_t *aa = hashtable_ref(ga, test_hash_t, node);
    test_hash_t *bb = hashtable_ref(gb, test_hash_t, node);
    test_hash_t *cc = hashtable_ref(gc, test_hash_t, node);

    printf("%s\n", aa->message);
    printf("%s\n", bb->message);
    printf("%s\n", cc->message);

    hashtable_destroy(&hash);
}

int main() {
    rb();
    avl();
    list();
    hash();
}

