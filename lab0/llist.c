#include <stdio.h>
#include <stdlib.h>

typedef struct node {
    int value;
    struct node *next;
} Node;

/* Create a new node with the given value */
Node *node_new(int value) {
    Node *n = malloc(sizeof(Node));
    if (!n) return NULL;
    n->value = value;
    n->next = NULL;
    return n;
}

/* TODO: Append to end of list */
void list_append(Node **head, int value) {
    /* YOUR CODE HERE */
    if (*head == NULL) {
        *head = node_new(value);
	return;
    }
    
    Node *curr = *head;
    while (curr->next) {
        curr = curr->next;
    }
    curr->next = node_new(value);
    (void)head;
    (void)value;
}

/* TODO: Print all values, one per line */
void list_print(Node *head) {
    for (Node *curr = head; curr != NULL; curr = curr->next) {
        printf("%d\n", curr->value);
    }
    (void)head;
}

/* TODO: Free entire list (all nodes must be freed, no leaks) */
void list_free(Node *head) {
    /* YOUR CODE HERE */
    if (!head) return;

    Node *temp = head->next;
    while (temp) {
        free(head);
	head = temp;
	temp = temp->next;
    }
    free(head);
    (void)head;
}

int main(void) {
    Node *head = NULL;

    /* Create list [1, 2, 3, 4, 5] */
    for (int i = 1; i <= 5; i++) {
        list_append(&head, i);
    }

    printf("List contents:\n");
    list_print(head);

    list_free(head);
    head = NULL;

    printf("Done. Run with valgrind to verify no leaks.\n");
    return 0;
}
