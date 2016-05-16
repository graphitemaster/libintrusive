#include <stdint.h>
#include "avl.h"

#ifndef __has_feature
#define __has_feature(x) 0
#endif

#if __STDC_VERSION__ >= 201112L || (__has_feature(c_static_assert) && __has_feature(c_alignof))
_Static_assert(_Alignof(avl_node_t) >= 4, "Incompatible avl_node_t alignment");
#elif __GNUC__
typedef char static_assertion_avl_node_t_alignment[!!(__alignof__(avl_node_t)>4)*2-1];
#else
typedef char static_assertion_avl_node_t_alignment[!!(offsetof(struct {char c;avl_node_t t;}, t)>4)*2-1];
#endif

static int avl_abs(int n) {
    int mask = n >> ((sizeof(int) * 8) - 1);
    return (mask + n) ^ mask;
}

static inline int avl_max(int lhs, int rhs) {
    return lhs > rhs ? lhs : rhs;
}

static inline void avl_set_parent(avl_node_t *node, avl_node_t *parent) {
    node->parent = (uintptr_t)parent | (node->parent & 3);
}

static inline avl_node_t *avl_get_parent(const avl_node_t *const node) {
    return (avl_node_t*)(node->parent & ~3);
}

static inline void avl_set_factor(avl_node_t *node, int factor) {
    node->parent = (uintptr_t)avl_get_parent(node) | (uintptr_t)(factor + 1);
}

static inline int avl_get_factor(const avl_node_t *const node) {
    return ((int)(node->parent & 3)) - 1;
}

static inline int avl_get_balance(const avl_node_t *const node) {
    return node ? avl_get_factor(node) : 0;
}

static avl_node_t *avl_rotate_ll(avl_node_t *parent, int pfactor, int *cfactor, int *difference) {
    avl_node_t *child = parent->left;
    
    int cleft = !!child->left;
    int cright = !!child->right;
    int pright = 0;
    if (*cfactor < 0) {
        cleft = cright - *cfactor;
        pright = cleft + 1 + pfactor;
        if (difference) *difference = avl_max(cleft, avl_max(cright, pright) + 1) - (cleft + 1);
    } else {
        cright = cleft + *cfactor;
        pright = cright + 1 + pfactor;
        if (difference) *difference = avl_max(cleft, avl_max(cright, pright) + 1) - (cright + 1);
    }
    *cfactor = (avl_max(cright, pright) + 1) - cleft;
    avl_set_factor(parent, pright - cright);

    parent->left = child->right;
    if (child->right) avl_set_parent(child->right, parent);
    child->right = parent;
    avl_set_parent(child, avl_get_parent(parent));
    avl_set_parent(parent, child);

    return child;
}

static avl_node_t *avl_rotate_rr(avl_node_t *parent, int pfactor, int *cfactor, int *difference) {
    avl_node_t *child = parent->right;
    
    int cleft = !!child->left;
    int cright = !!child->right;
    int pleft = 0;
    if (*cfactor < 0) {
        cleft = cright - *cfactor;
        pleft = cleft + 1 - pfactor;
        if (difference) *difference = avl_max(cright, avl_max(cleft, pleft) + 1) - (cleft - 1);
    } else {
        cright = cleft + *cfactor;
        pleft = cright + 1 - pfactor;
        if (difference) *difference = avl_max(cright, avl_max(cleft, pleft) + 1) - (cright + 1);
    }
    *cfactor = cright - (avl_max(cleft, pleft) + 1);
    avl_set_factor(parent, cleft - pleft);

    parent->right = child->left;
    if (child->left) avl_set_parent(child->left, parent);
    child->left = parent;
    avl_set_parent(child, avl_get_parent(parent));
    avl_set_parent(parent, child);

    return child;
}

static inline avl_node_t *avl_rotate_lr(avl_node_t *parent, int pfactor) {
    avl_node_t *child = parent->left;
    
    int cfactor = 0;
    int difference = 0;
    if (child->right) {
        cfactor = avl_get_factor(child->right);
        parent->left = avl_rotate_rr(child, avl_get_factor(child), &cfactor, &difference);
    } else {
        cfactor = avl_get_factor(child);
    }
    
    avl_node_t *node = avl_rotate_ll(parent, pfactor - difference, &cfactor, NULL);
    avl_set_factor(node, cfactor);
    return node;
}

static inline avl_node_t *avl_rotate_rl(avl_node_t *parent, int pfactor) {
    avl_node_t *child = parent->right;

    int cfactor = 0;
    int difference = 0;
    if (child->left) {
        cfactor = avl_get_factor(child->left);
        parent->right = avl_rotate_ll(child, avl_get_factor(child), &cfactor, &difference);
    } else {
        cfactor = avl_get_factor(child);
    }

    avl_node_t *node = avl_rotate_rr(parent, pfactor + difference, &cfactor, NULL);
    avl_set_factor(node, cfactor);
    return node;
}

static avl_node_t *avl_balance(avl_node_t *node, int factor) {
    int difference = avl_get_balance(node) + factor;
    if (!node) return NULL;
    if (difference < -1 && node->left) {
        if (avl_get_balance(node->left) <= 0) {
            int child = avl_get_factor(node->left);
            node = avl_rotate_ll(node, difference, &child, NULL);
            avl_set_factor(node, child);
        } else {
            node = avl_rotate_lr(node, difference);
        }
    } else if (difference > 1 && node->right) {
        if (avl_get_balance(node->right) >= 1) {
            int child = avl_get_factor(node->right);
            node = avl_rotate_rr(node, difference, &child, NULL);
            avl_set_factor(node, child);
        } else {
            node = avl_rotate_rl(node, difference);
        }
    } else {
        avl_set_factor(node, avl_get_factor(node) + factor);
    }
    return node;
}

void avl_init(avl_tree_t *tree, void *aux) {
    tree->root = NULL;
    tree->aux = aux;
}

avl_node_t *avl_head(const avl_tree_t *tree) {
    avl_node_t *head = NULL;
    for (avl_node_t *node = tree->root; node; node = node->left)
        head = node;
    return head;
}

avl_node_t *avl_tail(const avl_tree_t *tree) {
    avl_node_t *tail = NULL;
    for (avl_node_t *node = tree->root; node; node = node->right)
        tail = node;
    return tail;
}

avl_node_t *avl_next(const avl_node_t *node) {
    return node ? node->next : NULL;
}

avl_node_t *avl_prev(const avl_node_t *node) {
    return node ? node->prev : NULL;
}

avl_node_t *avl_search(avl_tree_t *tree, avl_node_t *node, avl_compare_t compare) {
    for (avl_node_t *parent = tree->root; parent; ) {
        int cmp = compare(parent, node, tree->aux);
        if (cmp > 0) parent = parent->left;
        else if (cmp < 0) parent = parent->right;
        else return parent;
    }
    return NULL;
}

void avl_insert(avl_tree_t *tree, avl_node_t *node, avl_compare_t compare) {
    avl_node_t *parent = NULL;
    for (avl_node_t *current = tree->root; current; ) {
        int cmp = compare(current, node, tree->aux);
        parent = current;
        if (cmp > 0) current = current->left;
        else if (cmp < 0) current = current->right;
        else return;
    }

    avl_set_parent(node, parent);
    avl_set_factor(node, 0);

    node->left = NULL;
    node->right = NULL;
    node->prev = NULL;
    node->next = NULL;

    if (parent) {
        if (compare(parent, node, tree->aux) > 0) {
            parent->left = node;
            node->next = parent;
            node->prev = parent->prev;
            if (parent->prev) parent->prev->next = node;
            parent->prev = node;
        } else {
            parent->right = node;
            node->prev = parent;
            node->next = parent->next;
            if (parent->next) parent->next->prev = node;
            parent->next = node;
        }
    } else {
        tree->root = node;
    }

    int factor = 0;
    while (node) {
        parent = avl_get_parent(node);
        if (parent) {
            int oldfactor = avl_get_factor(node);
            if (parent->right == node) {
                node = avl_balance(node, factor);
                parent->right = node;
            } else {
                node = avl_balance(node, factor);
                parent->left = node;
            }

            if (!node->left && !node->right) {
                factor = parent->left == node ? -1 : 1;
            } else {
                factor = 0;
                if (avl_abs(oldfactor) < avl_abs(avl_get_factor(node)))
                    factor = parent->left == node ? -1 : 1;
            }
        } else if (node == tree->root) {
            tree->root = avl_balance(tree->root, factor);
            break;
        }
        if (factor == 0) break;
        node = parent;
    }
}

void avl_remove(avl_tree_t *tree, avl_node_t *node) {
    if (!node) return;
    if (node->prev) node->prev->next = node->next;
    if (node->next) node->next->prev = node->prev;

    avl_node_t *next = avl_head(&((avl_tree_t){ node->right, NULL }));
    avl_node_t *current = NULL;
    avl_node_t *parent = NULL;
    int factor = 0;
    if (next) {
        if (avl_get_parent(next)) {
            if (avl_get_parent(next) != node) {
                avl_get_parent(next)->left = next->right;
                if (next->right) avl_set_parent(next->right, avl_get_parent(next));
            }
        }
        if (avl_get_parent(node)) {
            if (avl_get_parent(node)->left == node) {
                avl_get_parent(node)->left = next;
            } else {
                avl_get_parent(node)->right = next;
            }
        }

        if (node->right != next) {
            node->right = node->right;
            if (node->right) avl_set_parent(node->right, next);
            current = avl_get_parent(next);
            factor = 1;
        } else {
            current = next;
            factor = -1;
        }

        next->left = node->left;
        if (node->left) avl_set_parent(node->left, next);
        avl_set_parent(next, avl_get_parent(node));
        avl_set_factor(next, avl_get_factor(node));
    } else {
        parent = avl_get_parent(node);
        if (parent) {
            if (parent->left == node) {
                parent->left = node->left;
                factor = 1;
            } else {
                parent->right = node->left;
                factor = -1;
            }
        }
        if (node->left) avl_set_parent(node->left, parent);
        current = avl_get_parent(node);
    }

    if (tree->root == node) {
        tree->root = next;
        if (!next && node->left) tree->root = node->left;
    }

    while (current) {
        parent = avl_get_parent(current);
        if (parent) {
            int oldfactor = avl_get_factor(current);
            if (parent->right == current) {
                current = avl_balance(current, factor);
                parent->right = current;
            } else {
                current = avl_balance(current, factor);
                parent->left = current;
            }

            if (!current->left && !current->right) {
                factor = parent->left == current ? 1 : -1;
            } else {
                factor = 0;
                if (avl_abs(oldfactor) > avl_abs(avl_get_factor(current)))
                    factor = parent->left == current ? 1 : -1;
            }
        } else if (current == tree->root) {
            tree->root = avl_balance(tree->root, factor);
            break;
        }
        if (factor == 0) break;
        current = parent;
    }
}
