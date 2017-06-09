#ifndef LIST_HDR
#define LIST_HDR

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct link_s link_t;
typedef struct list_s list_t;

struct link_s
{
    link_t* prev;
    link_t* next;
};

struct list_s
{
    link_t* head;
    link_t* tail;
};

void list_init(list_t* list);

void list_push_front(list_t* list, link_t* link);
void list_push_back(list_t* list, link_t* link);
void list_insert_before(list_t* list, link_t* before, link_t* link);
void list_insert_after(list_t* list, link_t* after, link_t* link);

link_t* list_pop_front(list_t* list);
link_t* list_pop_back(list_t* list);

link_t* list_head(const list_t* list);
link_t* list_tail(const list_t* list);

link_t* list_next(const link_t* link);
link_t* list_prev(const link_t* link);

#define list_ref(ELEMENT, TYPE, MEMBER)                                      \
    ((TYPE*) ((unsigned char*) (ELEMENT) -offsetof(TYPE, MEMBER)))

#ifdef __cplusplus
}
#endif

#endif
