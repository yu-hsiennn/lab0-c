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
    int s_len = strlen(s);

    if (!new_elem) {
        return false;  // Allocation failed
    }
    new_elem->value = (char *) malloc((s_len + 1) * sizeof(char));
    if (!new_elem->value) {
        free(new_elem);
        return false;  // String copy failed
    }

    strncpy(new_elem->value, s, (s_len + 1));
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
        struct list_head *prev = current->prev;
        list_move(current->next, prev);
        current = current->next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || head->next == head || head->next->next == head) {
        return;  // Empty or single element queue, nothing to reverse
    }

    for (struct list_head *current = head->next;
         current != head && current->next != head;) {
        list_move(current->next, head);
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    if (!head || list_empty(head) || k < 2) {
        return;
    }
    int q_length = q_size(head), times = q_length / k;

    if (times == 0) {
        return;
    }

    struct list_head *start = head, *current = NULL;
    while (times--) {
        int count = k;
        for (current = start->next; --count > 0;)
            list_move(current->next, start);
        start = current;
    }
}

void merge(struct list_head *head,
           struct list_head *left,
           struct list_head *right,
           bool descend)
{
    struct list_head *current = head;
    while (!list_empty(left) && !list_empty(right)) {
        element_t *left_entry = list_entry(left->next, element_t, list);
        element_t *right_entry = list_entry(right->next, element_t, list);

        if (descend ? strcmp(left_entry->value, right_entry->value) >= 0
                    : strcmp(left_entry->value, right_entry->value) <= 0) {
            list_move(left->next, current);
        } else {
            list_move(right->next, current);
        }
        current = current->next;
    }
    struct list_head *remaining = list_empty(left) ? right : left;
    list_splice_tail(remaining, current->next);
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || head->next->next == head) {
        return;
    }

    struct list_head *slow = head->next, *fast = head->next->next;
    while (fast != head && fast->next != head) {
        slow = slow->next;
        fast = fast->next->next;
    }

    LIST_HEAD(left);   // left half
    LIST_HEAD(right);  // right half
    list_cut_position(&left, head, slow);
    list_splice_init(head, &right);

    q_sort(&left, descend);
    q_sort(&right, descend);

    merge(head, &left, &right, descend);
}

int process_list(struct list_head *head, bool ascend)
{
    if (!head || list_empty(head)) {
        return 0;
    }

    element_t *extreme_entry = list_entry(head->prev, element_t, list);
    int result = 1;
    struct list_head *current = head->prev->prev;

    while (current != head) {
        element_t *current_entry = list_entry(current, element_t, list);
        struct list_head *prev = current->prev;

        bool condition =
            ascend ? (strcmp(current_entry->value, extreme_entry->value) >= 0)
                   : (strcmp(current_entry->value, extreme_entry->value) <= 0);

        if (condition) {
            list_del_init(current);
            q_release_element(current_entry);
        } else {
            result++;
            extreme_entry = current_entry;
        }
        current = prev;
    }

    return result;
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    return process_list(head, true);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    return process_list(head, false);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    if (!head || list_empty(head))
        return 0;
    else if (list_is_singular(head))
        return list_entry(head->next, queue_contex_t, chain)->size;
    queue_contex_t *target = list_entry(head->next, queue_contex_t, chain);
    queue_contex_t *current = NULL;
    list_for_each_entry (current, head, chain) {
        if (current == target)
            continue;
        list_splice_init(current->q, target->q);
        target->size = target->size + current->size;
        current->size = 0;
    }
    q_sort(target->q, descend);

    return target->size;
}
