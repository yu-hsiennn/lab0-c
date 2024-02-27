#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/* Create a new, empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (!head) {
        return NULL;  // Allocation failed
    }
    INIT_LIST_HEAD(head);  // Initialize the list
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head) {
        return;
    }
    struct list_head *current, *save;
    list_for_each_safe (current, save, head) {
        element_t *elem = list_entry(current, element_t, list);
        list_del(current);
        q_release_element(elem);
    }
    free(head);
}

/* Insert an element at the head of the queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head || !s) {
        return false;
    }
    element_t *new_elem = malloc(sizeof(element_t));
    if (!new_elem) {
        return false;  // Allocation failed
    }
    new_elem->value = strdup(s);  // Copy string
    if (!new_elem->value) {
        free(new_elem);
        return false;  // String copy failed
    }
    list_add(&new_elem->list, head);
    return true;
}

/* Insert an element at the tail of the queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head || !s) {
        return false;
    }
    return q_insert_head(head->prev, s);
}

/* Remove an element from the head of the queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || head->next == head) {
        return NULL;  // Queue is empty or NULL
    }
    struct list_head *first = head->next;
    element_t *elem = list_entry(first, element_t, list);
    if (sp && bufsize > 0) {
        strncpy(sp, elem->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    list_del_init(first);
    return elem;
}

/* Remove an element from the tail of the queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || head->prev == head) {
        return NULL;  // Queue is empty or NULL
    }
    return q_remove_head(head->prev->prev, sp, bufsize);
}

/* Return number of elements in the queue */
int q_size(struct list_head *head)
{
    if (!head) {
        return 0;  // Null queue has size 0
    }
    int count = 0;
    struct list_head *curr;
    list_for_each (curr, head) {
        count++;
    }
    return count;
}

bool q_delete_mid(struct list_head *head)
{
    if (!head || head->next == head || head->next->next == head) {
        return false;  // Empty or single element queue, nothing to delete
    }

    struct list_head *slow = head->next, *fast = head->next->next;
    while (fast != head && fast->next != head) {
        slow = slow->next;
        fast = fast->next->next;
    }

    element_t *middle_elem = list_entry(slow, element_t, list);
    list_del(slow);
    q_release_element(middle_elem);

    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return false;  // Empty queue, nothing to do
    }

    struct list_head *current = head->next;
    bool found = false;
    while (current != head && current->next != head) {
        element_t *current_entry = list_entry(current, element_t, list);
        element_t *next_entry = list_entry(current->next, element_t, list);

        if (strcmp(current_entry->value, next_entry->value) == 0) {
            list_del(current->next);
            q_release_element(next_entry);
            found = true;
        } else {
            current = current->next;
            if (found) {
                list_del(current->prev);
                q_release_element(current_entry);
                found = false;
            }
        }
    }
    if (found) {
        list_del(current);
        q_release_element(list_entry(current, element_t, list));
    }

    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head || head->next == head || head->next->next == head) {
        return;  // Empty or single element queue, nothing to swap
    }

    struct list_head *current = head->next;
    while (current != head && current->next != head) {
        struct list_head *next = current->next;
        struct list_head *nextNext = next->next;

        next->next = current;
        next->prev = current->prev;
        current->prev->next = next;
        current->next = nextNext;
        nextNext->prev = next;
        if (current->next != head) {
            current->next->prev = current;
        }
        current = nextNext;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head) {}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
