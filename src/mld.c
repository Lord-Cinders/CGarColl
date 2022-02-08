#include "mld.h"
#include <stdio.h>

void printField(FieldsNode node)
{
    printf("Name of the Field             : %s\n", node.FieldName);
    printf("Size of the Field             : %ld\n", node.FieldSize);
    printf("Offset of the Field           : %ld\n", node.Offset);
    
    
    switch (node.Type)
    {
        case 0:
            printf("DataType of the Field         : UINT8\n");
            break;

        case 1:
            printf("DataType of the Field         : UINT32\n");
            break;

        case 2:
            printf("DataType of the Field         : INT32\n");
            break;

        case 3:
            printf("DataType of the Field         : CHAR\n");
            break;

        case 4:
            printf("DataType of the Field         : OBJPTR\n");
            break;

        case 5:
            printf("DataType of the Field         : FLOAT\n");
            break;

        case 6:
            printf("DataType of the Field         : DOUBLE\n");
            break;

        case 7:
            printf("DataType of the Field         : OBJSTRUCT\n");
            break;

        default:
            printf("DataType of the Field         : UNSUPPORTED\n");
            break;
    }

    if(node.NestedStruct != NULL) 
    { 
         printf("Structure its nested in       : %s", node.NestedStruct);
    }
    printf("\n\n\n");
}

void printStructNode(StructDbNode * node)
{
    printf("Name of the Structure             : %s\n", node->StructName);
    printf("Size of the Structure in memory   : %ld\n", node->StructSize);
    printf("Number of fields in the Structure : %ld\n\n", node->nFields);
    for(int i = 0; i < node->nFields; i++)
    {
        printField(node->Fields[i]);
    }
}

void printStructList(StructDbList * list)
{
    if(list->size == 0)
    {
        printf("The list is empty\n");
        return;
    }

    StructDbNode * temp = list->head;
    while((temp) != NULL)
    {
        printStructNode(temp);
        temp = temp->next;
    }
}

void printObjNode(ObjectDbnode * node)
{
    printf("Addr of the Object                : 0x%x\n", node->ptr);
    printf("No of blocks in memory            : %ld\n", node->n);
    printf("Type of the Object                : %s\n\n", node->StructNode->StructName);
}

void printObjList(ObjectDbList * list)
{
    if(list->size == 0)
    {
        printf("The list is empty\n");
        return;
    }
    ObjectDbnode * temp = list->head;
    while(temp != NULL)
    {
        printObjNode(temp);
        temp = temp->next;
    }
}

int StructInsertIntoDb(StructDbList * list, StructDbNode * node)
{
    if(list->head == NULL)
    {
        list->head = node;
        list->size++;
        return 0;
    }
    StructDbNode * temp = list->head;
    int s = 0;
    while(temp->next != NULL)
    {
        temp = temp->next;
        if(++s > list->size)
        {
            return 1;
        }
    }
    temp->next = node;
    list->size++;
    return 0;
}

StructDbNode * StructLookUp(StructDbList * list, const char * structName)
{
    StructDbNode * temp = list->head;
    while(temp != NULL)
    {
        if(strcmp(temp->StructName, structName) == 0)
        {
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

ObjectDbList * InitObjList(StructDbList * list)
{
    ObjectDbList * temp = calloc(1, sizeof(ObjectDbList));
    temp->StructDb = list;
    return temp;
}

int ObjectLookUp(ObjectDbList * list, void * ptr)
{
    ObjectDbnode * temp = list->head;
    while (temp != NULL)
    {
        if(ptr == temp->ptr)
        {
            return 1;
        }
        temp = temp->next;
    }
    return 0;
}

int ObjInsertIntoDb(ObjectDbList * list, ObjectDbnode * node)
{
    if(list->head == NULL)
    {
        list->head = node;
        list->size++;
        return 0;
    }
    ObjectDbnode * temp = list->head;
    int count = 0;
    while(temp->next != NULL)
    {
        temp = temp->next;
        if(++count > list->size)
        {
            return 1;
        }
    }
    temp->next = node;
    list->size++;
    return 0;
}

/* 
// corresponds to calloc
// allocates 'n' units of contiguous memory blocks for an object of type "StructName"
// creates an object record for the object and adds it the record to the database
// link the object record to the structure record based on the "StructName"
// returns the pointer to the allocated object if successful otherwise NULL 
*/
void * xcalloc(ObjectDbList * ObjectDb, size_t n, const char * StructName)
{
    StructDbNode * temp = StructLookUp(ObjectDb->StructDb, StructName);
    void * pointer = calloc(n, temp->StructSize);
    if(pointer == NULL) { return NULL; }
    REGOBJ(ObjectDb, pointer, n, temp);
    return pointer;
}