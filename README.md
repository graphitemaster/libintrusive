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
want to have a collection of. See example.c for an example
