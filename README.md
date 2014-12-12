### libintrusive
A collection of intrusive data-structures for C

#### Why?
Intrusive data structures are much better for cache locality, utilize less
allocations and allow for various optimizations like removing an object from a
doubly-linked list in constant time.

#### Supported
* Doubly-linked list
* AVL tree
* Red-Black tree
* Hash-table

### Using
Using an intrusive data structure is slightly different than a non-intrusive
data structure. The node for the structure becomes a member of the thing you
want to have a collection of. Here's an example using the AVL tree:

    struct street {
        const char *name;
        avl_tree_t houses;
    };

    struct house {
        int number; // house number
        avl_node_t node;
    };

    // Compare functions in libintrusive must follow the contract
    // return < 0 for <
    // return > 0 for >
    // return 0 for ==
    int house_compare(const avl_node_t *lhs, const avl_node_t *rhs, const void *aux) {
        const struct house *a = lhs;
        const struct house *b = rhs;
        if (a < b) return -1;
        if (b > a) return 1;
        return 0;
    }

    struct house houses[] = {{ 10 }, { 11 }, { 12 }, { 13 }}
    struct street street = { "Intrusive Ave" };

    // Insert them
    avl_init(&street.houses);
    for (size_t i = 0; i < sizeof(houses)/sizeof(*houses); i++) 
        avl_insert(&street.houses, &houses[i].node, house_compare);

    // Now find one
    struct house query;
    query.number = 12;
    avl_node_t *find = avl_search(&steet.houses, &query.node, house_compare);
    if (find) {
        struct house *house = avl_ref(find, struct house, node);
        printf("found %d\n", house->number);
    }

    // Iterate them
    avl_node_t *current = avl_head(&street.houses);
    while (current) {
        struct house *house = avl_ref(current, struct house, node);
        printf("%d\n", house->number);
        current = avl_next(current);
    }

Running s/avl_/rb_ will make the following example use a Red-black tree instead.
Check out the header files for documentation on others.
