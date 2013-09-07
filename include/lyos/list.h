/*  This file is part of Lyos.

    Lyos is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Lyos is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Lyos.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef _LIST_H_
#define _LIST_H_

/* linked list facility */

struct list_head {
    struct  list_head * prev, * next;
};

#define LIST_INIT(name) { &(name), &(name) }
#define DEF_LIST(name) struct list_head name = LIST_INIT(name)
#define INIT_LIST_HEAD(ptr) do { \
    (ptr)->next = (ptr); (ptr)->prev = (ptr); \
} while (0)


#define list_entry(ptr, type, member) ({          \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})

#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

PUBLIC int list_empty(struct list_head * list);
PUBLIC inline void list_add(struct list_head * new, struct list_head * head);
PUBLIC inline void list_del(struct list_head * node);

#define prefetch(x) __builtin_prefetch(&x)

#define list_for_each_entry(pos, head, member)                          \
        for (pos = list_entry((head)->next, typeof(*pos), member);      \
             prefetch(pos->member.next), &(pos->member) != (head);        \
             pos = list_entry(pos->member.next, typeof(*pos), member))

#endif