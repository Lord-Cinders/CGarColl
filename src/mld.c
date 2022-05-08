#include "mld.h"
#include <stdio.h>


/* ======================================= Print Functions ======================================= */


void printField(FieldsNode node)
{
    printf("Name of the Field             : %s\n", node.FieldName);
    printf("Size of the Field             : %ld\n", node.FieldSize);
    printf("Offset of the Field           : %ld\n", node.Offset);
    
    
    switch(node.Type)
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
    printf("Type of the Object                : %s\n", node->StructNode->StructName);
    printf("State of the Object               : %d\n\n", node->root);
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
    switch(type)
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
        void * temp = (node->ptr + (i * node->StructNode->StructSize));
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

StructDbNode * StructLookUp(StructDbList * list, const char * StructName)
{
    StructDbNode * temp = list->head;
    while(temp != NULL)
    {
        if(strcmp(temp->StructName, StructName) == 0)
        {
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

ObjectDbList * InitObjList(StructDbList * list)
{
    assert(list);
    ObjectDbList * temp = calloc(1, sizeof(ObjectDbList));
    temp->StructDb = list;
    return temp;
}

ObjectDbnode * ObjectLookUp(ObjectDbList * list, void * ptr)
{
    ObjectDbnode * temp = list->head;
    while (temp != NULL)
    {
        if(ptr == temp->ptr)
        {
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

int ObjInsertIntoDb(ObjectDbList * list, ObjectDbnode * node)
{
    if(list->head == NULL)
    {
        list->head = node;
        list->head->next = NULL;
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

/* ======================================= API Functions ======================================= */

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
    REGOBJ(ObjectDb, pointer, n, temp, false);
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
        if(temp->next->ptr == ptr) { break; }
        temp = temp->next;
    }
    assert(temp);
    ObjectDbnode * caughtptr = temp->next;
    if (caughtptr->next == NULL) { temp->next = NULL; }
    else { temp->next = caughtptr->next; }
    free(caughtptr->ptr);
    free(caughtptr);
}

/*
// API to mark an existing dynamic object as the root
*/
void RegisterObjectasRoot(ObjectDbList * list, void * ptr)
{
    ObjectDbnode * node = ObjectLookUp(list, ptr);
    assert(node);
    node->root = true;
}

// void * AllocateObjectasRoot(ObjectDbList * objlist, StructDbList * structlist, void * ptr, const char * structname, size_t count)
// {
//     StructDbNode * node = StructLookUp(structlist, structname);
//     assert(node);
//     REGOBJ(objlist, ptr, count, node, true);
// }

/*
// API to keep track of globally created objects
// creates a new ObjectNode of type 'StructName'
// marks it as a root
*/
void RegisterGlobalVar(ObjectDbList * list, void * ptr, const char * StructName, size_t n)
{
    StructDbNode * StructPtr = StructLookUp(list->StructDb, StructName);
    assert(StructPtr);
    REGOBJ(list, ptr, n, StructPtr, true);

}

// adds primitive data types into the StructDB for direct array allocation
void InitBasicMLD(StructDbList * list)
{
    // FieldsNode intfield    = {"int",    sizeof(int),    0, INT32,  "NULL"};
    // FieldsNode charfield   = {"char",   sizeof(char),   0, CHAR,   "NULL"};
    // FieldsNode floatfield  = {"float",  sizeof(float),  0, FLOAT,  "NULL"};
    // FieldsNode doublefield = {"double", sizeof(double), 0, DOUBLE, "NULL"};

    // REGSTRUCT(list, int,    &intfield);
    // REGSTRUCT(list, char,   &charfield);
    // REGSTRUCT(list, float,  &floatfield);
    // REGSTRUCT(list, double, &doublefield);

    REGSTRUCT(list, int,    0);
    REGSTRUCT(list, char,   0);
    REGSTRUCT(list, float,  0);
    REGSTRUCT(list, double, 0);

}

/* ======================================= MDL Functions ======================================= */
// clears visited flag on all the object nodes
static void InitMLD(ObjectDbList * list)
{

    assert(list);
    ObjectDbnode * temp = list->head;
    while(temp != NULL)
    {
        temp->visited = false;
        temp = temp->next;
    }
}

static ObjectDbnode * GetNearestRoot(ObjectDbList * list, ObjectDbnode * node)
{
    ObjectDbnode * temp = node ? node->ptr : list->head;
    while(temp != NULL)
    {
        if(temp->root == true)
        {
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

static void ExploreNodesFrom(ObjectDbList * list, ObjectDbnode * node)
{
    size_t i = 0, j = 0;
    FieldsNode * field = node->StructNode->Fields;

    assert(node->visited);

    if(node->StructNode->nFields == 0) { return; } // if primitive, there are no leaks
    while(i < node->n) // obj may be initialized with multiple blocks in memory
    {   
        void * ptr = (node->ptr + (i * node->StructNode->StructSize)); 
        while(j < node->StructNode->nFields)
        {
            if(field[j].Type == OBJPTR) // obj can only point to other object if it points to other objects 
            {
                void * ObjPtr = (ptr + field[j].Offset);
                if(ObjPtr == NULL) { continue; }
                ObjectDbnode * temp = ObjectLookUp(list, ObjPtr); // get the pointer to the object referred in the current object
                assert(temp);
                if(temp->visited == false) 
                { 
                    temp->visited = true; 
                    DumpObjectNode(temp);
                    ExploreNodesFrom(list, temp);
                }
            }
            j++; 
        }
        i++;
    }
}

void MLDRun(ObjectDbList * list)
{
    InitMLD(list);
    ObjectDbnode * temp = GetNearestRoot(list, NULL);
    while (temp != NULL)
    {
        if(temp->visited == true)   
        {
            GetNearestRoot(list, temp);
            continue;
        }
        temp->visited = true;
        ExploreNodesFrom(list, temp);
        temp = GetNearestRoot(list, temp);
    }
    temp = list->head;

    printf("Leaked Nodes are: \n");
    while(temp != NULL)
    {
        if(temp->visited == false)
        {
            printObjNode(temp);
        }
        temp = temp->next;
    }    
}
