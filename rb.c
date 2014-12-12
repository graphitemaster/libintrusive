#include <stdint.h>
#include "rb.h"

#define	RB_RED   0
#define	RB_BLACK 1

#define rb_parent(R) ((rb_node_t *)((uint64_t)((R)->parent) & ~3))
#define rb_color(R) ((uint64_t)((R)->parent) & 1)
#define rb_is_red(R) (!rb_color(R))
#define rb_is_black(R)  rb_color(R)
#define rb_set_red(R)   do { rb_set_color((R), RB_RED); } while(0)
#define rb_set_black(R) do { rb_set_color((R), RB_BLACK); } while (0)

static inline void rb_set_parent(rb_node_t *rb, rb_node_t *p) {
    rb->parent = (rb_node_t*)(uint64_t)(((uint64_t)rb->parent & 3) | (uint64_t)p);
}
static inline void rb_set_color(rb_node_t *rb, int color) {
    rb->parent = (rb_node_t*)(uint64_t)(((uint64_t)rb->parent & ~1) | (uint64_t)color);
}

static inline void rb_tree_init(rb_tree_t *tree, rb_node_t *node) {
    tree->root = node;
    if (node) {
        rb_set_color(node, RB_BLACK);
        node->left  = NULL;
        node->right = NULL;
    }
}

static inline void rb_init_node(rb_node_t *rb) {
    rb->parent = 0;
    rb->right = NULL;
    rb->left = NULL;
    rb_set_parent(rb, rb);
}

static inline void rb_link_node(rb_node_t *node, rb_node_t *parent, rb_node_t **link) {
    node->parent = parent;
    node->left = NULL;
    node->right = NULL;
    *link = node;
}

static void rb_rotate_left(rb_node_t *node, rb_tree_t *tree) {
    rb_node_t *right = node->right;
    rb_node_t *parent = rb_parent(node);

    if ((node->right = right->left))
        rb_set_parent(right->left, node);
    right->left = node;

    rb_set_parent(right, parent);

    if (parent) {
        if (node == parent->left) parent->left = right;
        else parent->right = right;
    } else tree->root = right;
    rb_set_parent(node, right);
}

static void rb_rotate_right(rb_node_t *node, rb_tree_t *tree) {
    rb_node_t *left = node->left;
    rb_node_t *parent = rb_parent(node);

    if ((node->left = left->right))
        rb_set_parent(left->right, node);
    left->right = node;
    rb_set_parent(left, parent);

    if (parent) {
        if (node == parent->right) parent->right = left;
        else parent->left = left;
    } else tree->root = left;
    rb_set_parent(node, left);
}

static void rb_insert_color(rb_node_t *node, rb_tree_t *tree) {
    rb_node_t *parent;
    while ((parent = rb_parent(node)) && rb_is_red(parent)) {
        rb_node_t *gparent = rb_parent(parent);
        if (parent == gparent->left) {
            rb_node_t *uncle = gparent->right;
            if (uncle && rb_is_red(uncle)) {
                rb_set_black(uncle);
                rb_set_black(parent);
                rb_set_red(gparent);
                node = gparent;
                continue;
            }
            if (parent->right == node) {
                rb_node_t *tmp;
                rb_rotate_left(parent, tree);
                tmp = parent;
                parent = node;
                node = tmp;
            }
            rb_set_black(parent);
            rb_set_red(gparent);
            rb_rotate_right(gparent, tree);
        } else {
            rb_node_t *uncle = gparent->left;
            if (uncle && rb_is_red(uncle)) {
                rb_set_black(uncle);
                rb_set_black(parent);
                rb_set_red(gparent);
                node = gparent;
                continue;
            }
            if (parent->left == node) {
                rb_node_t *tmp;
                rb_rotate_right(parent, tree);
                tmp = parent;
                parent = node;
                node = tmp;
            }
            rb_set_black(parent);
            rb_set_red(gparent);
            rb_rotate_left(gparent, tree);
        }
    }
    rb_set_black(tree->root);
}

#define rb_empty_black(NODE) (!(NODE) || rb_is_black((NODE)))

static int rb_erase_left(rb_node_t *node, rb_node_t *parent, rb_tree_t *tree) {
    rb_node_t *other = parent->right;
    if (rb_is_red(other)) {
        rb_set_black(other);
        rb_set_red(parent);
        rb_rotate_left(parent, tree);
        other = parent->right;
    }
    if (rb_empty_black(other->left) || rb_empty_black(other->right)) {
        rb_set_red(other);
        node = parent;
        parent = rb_parent(node);
    } else {
        if (rb_empty_black(other->right)) {
            rb_set_black(other->left);
            rb_set_red(other);
            rb_rotate_right(other, tree);
            other = parent->right;
        }
        rb_set_color(other, rb_color(parent));
        rb_set_black(parent);
        rb_set_black(other->right);
        rb_rotate_left(parent, tree);
        node = tree->root;
        return 1;
    }
    return 0;
}

static int rb_erase_right(rb_node_t *node, rb_node_t *parent, rb_tree_t *tree) {
    rb_node_t *other = parent->left;
    if (rb_is_red(other)) {
        rb_set_black(other);
        rb_set_red(parent);
        rb_rotate_right(parent, tree);
        other = parent->left;
    }
    if (rb_empty_black(other->left) || rb_empty_black(other->right)) {
        rb_set_red(other);
        node = parent;
        parent = rb_parent(node);
    } else {
        if (rb_empty_black(other->left)) {
            rb_set_black(other->right);
            rb_set_red(other);
            rb_rotate_left(other, tree);
            other = parent->left;
        }
        rb_set_color(other, rb_color(parent));
        rb_set_black(parent);
        rb_set_black(other->left);
        rb_rotate_right(parent, tree);
        node = tree->root;
        return 1;
    }
    return 0;
}

static void rb_erase_color(rb_node_t *node, rb_node_t *parent, rb_tree_t *tree) {
    while ((!node || rb_is_black(node)) && node != tree->root) {
        if (parent->left == node && rb_erase_left(node, parent, tree)) break;
        else if (rb_erase_right(node, parent, tree)) break;
    }
    if (node) rb_set_black(node);
}

static rb_node_t *rb_insert_try(rb_tree_t *tree, rb_node_t *node, rb_compare_t compare) {
    rb_node_t **p = &tree->root;
    rb_node_t *parent = NULL;
    while (*p) {
        parent = *p;
        int cmp = compare(node, *p, tree->aux);
        if (cmp < 0) p = &(*p)->left;
        else if (cmp > 0) p = &(*p)->right;
        else return *p;
    }
    if (parent) rb_link_node(node, parent, p);
    else rb_tree_init(tree, node);
    return NULL;
}

void rb_init(rb_tree_t *tree, void *aux) {
    tree->root = NULL;
    tree->aux = aux;
}

void rb_insert(rb_tree_t *tree, rb_node_t *node, rb_compare_t compare) {
    if (rb_insert_try(tree, node, compare)) return;
    rb_insert_color(node, tree);
}

rb_node_t *rbwrap_search(rb_tree_t *tree, rb_node_t *node, rb_compare_t compare) {
    rb_node_t *n = tree->root;
    while (n) {
        int cmp = compare(node, n, tree->aux);
        if (cmp < 0) n = n->left;
        else if (cmp > 0) n = n->right;
        else return n;
    }
    return NULL;
}

void rb_remove(rb_tree_t *tree, rb_node_t *node) {
    rb_node_t *child, *parent;
    int color;

    if (!node->left) child = node->right;
    else if (!node->right) child = node->left;
    else {
        rb_node_t *old = node;
        rb_node_t *left;

        node = node->right;
        while ((left = node->left) != NULL) node = left;
        if (rb_parent(old)) {
            if (rb_parent(old)->left == old) rb_parent(old)->left = node;
            else rb_parent(old)->right = node;
        } else tree->root = node;

        child = node->right;
        parent = rb_parent(node);
        color = rb_color(node);

        if (parent == old) parent = node;
        else {
            if (child) rb_set_parent(child, parent);
            parent->left = child;
            node->right = old->right;
            rb_set_parent(old->right, node);
        }
        node->parent = old->parent;
        node->left = old->left;
        rb_set_parent(old->left, node);
        goto color;
    }

    parent = rb_parent(node);
    color = rb_color(node);

    if (child) rb_set_parent(child, parent);
    if (parent) {
        if (parent->left == node) parent->left = child;
        else parent->right = child;
    }
    else tree->root = child;

color:
    if (color == RB_BLACK) rb_erase_color(child, parent, tree);
}

rb_node_t *rb_head(const rb_tree_t *tree) {
    rb_node_t *n = tree->root;
    if (!n) return NULL;
    while (n->left) n = n->left;
    return n;
}

rb_node_t *rb_tail(const rb_tree_t *tree) {
    rb_node_t *n = tree->root;
    if (!n) return NULL;
    while (n->right) n = n->right;
    return n;
}

rb_node_t *rb_next(const rb_node_t *node) {
    rb_node_t *parent;
    if (rb_parent(node) == node) return NULL;
    if (node->right) {
        node = node->right;
        while (node->left) node=node->left;
        return (rb_node_t *)node;
    }
    while ((parent = rb_parent(node)) && node == parent->right) node = parent;
    return parent;
}

rb_node_t *rb_prev(const rb_node_t *node) {
    rb_node_t *parent;
    if (rb_parent(node) == node) return NULL;
    if (node->left) {
        node = node->left;
        while (node->right) node=node->right;
        return (rb_node_t *)node;
    }
    while ((parent = rb_parent(node)) && node == parent->left) node = parent;
    return parent;
}
