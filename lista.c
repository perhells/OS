#include <stdio.h>
#include <stdlib.h>

typedef struct x {
    int data;
    struct x *prev, *next;
} elem, *elemPtr;

elemPtr newElem(int data)
{
    elemPtr tmp = malloc(sizeof(elem));
    tmp->data = data;
    tmp->prev = tmp->next = tmp;
    return tmp;
}

void insertElem(const elemPtr point, elemPtr newPtr)
{
    newPtr->next = point->next;
    point->next = newPtr;

    newPtr->next->prev = newPtr;
    newPtr->prev = point;
}

elemPtr removeElem(elemPtr ptr)
{
    ptr->next->prev = ptr->prev;
    ptr->prev->next = ptr->next;
    ptr->prev = ptr->next = ptr;
    return ptr;
}

void deleteElem(elemPtr ptr)
{
    if(ptr->next != NULL && ptr->prev != NULL) removeElem(ptr);
    free((void *) ptr);
}

void apply(const elemPtr head, void (*fun)(elemPtr))
{   
    if(head == NULL) return;
    elemPtr tmp = head;
    (*fun)(tmp);
    while((tmp = tmp->next) != head) (*fun)(tmp);
}

void printElem(elemPtr ptr)
{
    printf("%d\n", ptr->data);
}

int main()
{
    int tal;
    elemPtr head = NULL;
    
    printf("GE TAL: ");
    scanf("%d", &tal);
    head = newElem(tal);

    while(tal != 0)
    {
        printf("GE TAL: ");
        scanf("%d", &tal);
        insertElem(head->prev, newElem(tal));
    }
    
    apply(head, printElem);
}
