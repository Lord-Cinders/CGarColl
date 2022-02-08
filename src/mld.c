#include "mld.h"
#include <stdio.h>


/* ======================================= Print Functions ======================================= */


void printField(FieldsNode node)
{
    printf("Name of the Field             : %s\n", node.FieldName);
    printf("Size of the Field             : %ld\n", node.FieldSize);
    printf("Offset of the Field           : %ld\n", node.Offset);
    
    
    switch (node.Type)
    {
        case UINT8:
            printf("DataType of the Field         : UINT8\n");
            break;

        case UINT32:
            printf("DataType of the Field         : UINT32\n");
            break;

        case INT32:
            printf("DataType of the Field         : INT32\n");
            break;

        case CHAR:
            printf("DataType of the Field         : CHAR\n");
            break;

        case OBJPTR:
            printf("DataType of the Field         : OBJPTR\n");
            break;

        case FLOAT:
            printf("DataType of the Field         : FLOAT\n");
            break;

        case DOUBLE:
            printf("DataType of the Field         : DOUBLE\n");
            break;

        case OBJSTRUCT:
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

// helper for DumpObjectNode
void printVal(void * ptr, size_t offset, int type)
{
    switch (type)
    {
        case UINT8:
            printf("%ud", *(unsigned short int *)(ptr + offset));
            break;

        case UINT32:
            printf("%ud", *(unsigned int *)(ptr + offset));
            break;

        case INT32:
            printf("%d", *(int *)(ptr + offset));
            break;

        case CHAR:
            printf("%s", (char *)(ptr + offset));
            break;

        case OBJPTR:
            printf("pointer to 0x%x", (ptr + offset));
            break;

        case FLOAT:
            printf("%f", *(float *)(ptr + offset));
            break;

        case DOUBLE:
            printf("%ld", *(double *)(ptr + offset));
            break;

        case OBJSTRUCT:
            printf("Structure present at 0x%x", (ptr + offset));
            break;

        default:
            printf("UNSUPPORTED");
            break;
    }
}

// print the values of all supported fields by mld library for the object record passed as argument
void DumpObjectNode(ObjectDbnode * node)
{
    char * Structname = node->StructNode->StructName;
    for(int i = 0; i < node->n; i++)
    {
        void * temp = node->ptr + i;
        for(int j = 0; j < node->StructNode->nFields; j++)
        {
            FieldsNode Field = (node)->StructNode->Fields[j];
            printf("%s[%d]->%s: ", Structname, i, Field.FieldName);
            printVal(temp, Field.Offset, Field.Type);
            printf("\n");
        }
        printf("\n");
    }
    
}

/* ======================================= Structure Functions ======================================= */

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

// int FindIndexOf(ObjectDbnode * node, char * Structname, void * ptr)
// {
//     ObjectDbnode * temp = node;
//     int count = 0;
//     while(temp != NULL)
//     {
//         if(strcmp(temp->StructNode->StructName, Structname) == 0)
//         {
//             if(temp->ptr == ptr) { return count; }
//             count++;
//         }
//         temp = temp->next;
//     }
// }

/* ======================================= Memory Functions ======================================= */

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

/*
// corresponds to free(void * ptr)
// assert if the ptr passed as argument to xfree is not found in object database
// if found :
//  remove the object record from object db (but do not free it)
//  finally free the actual object by invoking free(obj_rec->ptr)
//  free object rec which is removed from object db 
*/
void xfree(void * ptr, ObjectDbList * list)
{
    assert(ptr);
    ObjectDbnode * temp = list->head;
    while(temp != NULL)
    {
        if(temp->next == ptr) { break; }
        temp = temp->next;
    }
    assert(temp);
    ObjectDbnode * caughtptr = temp->next;
    if (caughtptr->next == NULL) { temp->next = NULL; }
    else { temp->next = caughtptr->next; }
    free(caughtptr->ptr);
    free(caughtptr);
}