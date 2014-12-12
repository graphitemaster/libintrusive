#ifndef RB_HDR
#define RB_HDR
#include <stddef.h>

#ifndef __cplusplus
extern "C" {
#endif

typedef struct rb_node_s rb_node_t;
typedef struct rb_tree_s rb_tree_t;

struct rb_node_s {
    unsigned long parent;
    rb_node_t *right;
    rb_node_t *left;
};

struct rb_tree_s {
    rb_node_t *root;
    void *aux;
};

typedef int (*rb_compare_t)(const rb_node_t *lhs, const rb_node_t *rhs, void *aux);

void rb_init(rb_tree_t *tree, void *aux);

void rb_insert(rb_tree_t *tree, rb_node_t *node, rb_compare_t compare);
void rb_remove(rb_tree_t *tree, rb_node_t *node);

rb_node_t *rb_search(rb_tree_t *tree, rb_node_t *node, rb_compare_t compare);

rb_node_t *rb_head(const rb_tree_t *tree);
rb_node_t *rb_tail(const rb_tree_t *tree);

rb_node_t *rb_next(const rb_node_t *tree);
rb_node_t *rb_prev(const rb_node_t *tree);

#define rb_ref(ELEMENT, TYPE, MEMBER) \
    ((TYPE *)((unsigned char *)(ELEMENT) - offsetof(TYPE, MEMBER)))

#ifdef __cplusplus
}
#endif

#endif
