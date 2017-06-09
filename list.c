#include "list.h"

void list_init(list_t* list)
{
    list->head = NULL;
    list->tail = NULL;
}

void list_push_front(list_t* list, link_t* link)
{
    list_remove(list, link);

    if (list->head)
    {
        list->head->prev = link;
        link->prev = NULL;
        link->next = list->head;
        list->head = link;
    }
    else
    {
        list->head = link;
        list->tail = link;
        link->next = NULL;
        link->prev = NULL;
    }
}

void list_push_back(list_t* list, link_t* link)
{
    list_remove(list, link);

    if (list->tail)
    {
        list->tail->next = link;
        link->prev = list->tail;
        link->next = NULL;
        list->tail = link;
    }
    else
    {
        list->head = link;
        list->tail = link;
        link->next = NULL;
        link->prev = NULL;
    }
}

void list_insert_before(list_t* list, link_t* before, link_t* link)
{
    if (before != link)
    {
        list_remove(list, link);

        link->prev = before->prev;
        link->next = before;
        if (before->prev)
        {
            before->prev->next = link;
        }
        else
        {
            list->head = link;
        }
        before->prev = link;
    }
}

void list_insert_after(list_t* list, link_t* after, link_t* link)
{
    if (after != link)
    {
        list_remove(list, link);

        link->next = after->next;
        link->prev = after;
        if (after->next)
        {
            after->next->prev = link;
        }
        else
        {
            list->tail = link;
        }
        after->next = link;
    }
}

link_t* list_pop_front(list_t* list)
{
    link_t* link = list->head;
    if (!link)
    {
        return NULL;
    }
    if (link->next)
    {
        link->next->prev = link->prev;
    }
    if (link->prev)
    {
        link->prev->next = link->next;
    }
    if (list->head == link)
    {
        list->head = link->next;
    }
    if (list->tail == link)
    {
        list->tail = link->prev;
    }
    return link;
}

link_t* list_pop_back(list_t* list)
{
    link_t* link = list->tail;
    if (!link)
    {
        return NULL;
    }
    if (link->next)
    {
        link->next->prev = link->prev;
    }
    if (link->prev)
    {
        link->prev->next = link->next;
    }
    if (list->head == link)
    {
        list->head = link->next;
    }
    if (list->tail == link)
    {
        list->tail = link->prev;
    }
    return link;
}

void list_remove(list_t* list, link_t* link)
{
    if (!link)
    {
        return;
    }
    if (link->next)
    {
        link->next->prev = link->prev;
    }
    if (link->prev)
    {
        link->prev->next = link->next;
    }
    if (list->head == link)
    {
        list->head = link->next;
    }
    if (list->tail == link)
    {
        list->tail = link->prev;
    }
}

link_t* list_head(const list_t* list)
{
    return list->head;
}

link_t* list_tail(const list_t* list)
{
    return list->tail;
}

link_t* list_next(const link_t* link)
{
    return link->next;
}

link_t* list_prev(const link_t* link)
{
    return link->prev;
}
