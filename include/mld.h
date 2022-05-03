/*
    Garbage Collector
    started on 7/2/22
    "another complaint on backpain" - Cinders22
*/

#ifndef _MLD_H_
#define _MLD_H_

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>

/* ======================================= Macros ======================================= */

#define MAX_STRUCT_NAME_SIZE  128
#define MAX_FIELD_NAME_SIZE   128

// possible pointer dereferencing, use inbuilt offsetof() from <stddef.h>
#define OFFSETOF(Struct, Member)  (size_t) &(((Struct *)0)->Member)   
// returns the size of the member
#define FIELDSIZE(Struct, Member) sizeof(((Struct *)0)->Member)
// converts data into an initialization array for FieldNode
#define FIELDINFO(Struct, Fieldname, type, nestedstruct) {#Fieldname, FIELDSIZE(Struct, Fieldname), offsetof(Struct, Fieldname), type, #nestedstruct} 
// adds a Fieldnode to a StructDbNode and appends it the the list (StructDblist)
#define REGSTRUCT(list, Struct, Fieldinfo)                              \
        do{                                                             \
            assert(!StructLookUp(list, #Struct));                       \
            StructDbNode * node = calloc(1, sizeof(StructDbNode));      \
            strncpy(node->StructName, #Struct, MAX_STRUCT_NAME_SIZE);   \
            node->StructSize = sizeof(Struct);                          \
            node->nFields    = sizeof(Fieldinfo) / sizeof(FieldsNode);  \
            node->Fields     = Fieldinfo;                               \
            if(StructInsertIntoDb(list, node)) { assert(0); }           \
        }while(0)

#define REGOBJ(list, pointer, count, StructureNode, Bool)               \
        do{                                                             \
            assert(!ObjectLookUp(list, pointer));                       \
            ObjectDbnode * node = calloc(1, sizeof(ObjectDbnode));      \
            node->ptr           = pointer;                              \
            node->n             = count;                                \
            node->StructNode    = StructureNode;                        \
            node->next          = NULL;                                 \
            node->root          = Bool;                                 \
            node->visited       = Bool;                                 \
            if(ObjInsertIntoDb(list, node)) { assert(0); }              \
        }while(0);

#define ACCESSFIELD(Object, Member) Object->Member



/* ======================================= Structure Definiations ======================================= */
typedef enum
{
    UINT8,
    UINT32,
    INT32,
    CHAR,
    OBJPTR,
    FLOAT,
    DOUBLE,
    OBJSTRUCT
}DataTypes;

typedef enum
{
    false,
    true
}bool;

// Represents a field present in a struct
typedef struct Fields
{
    char FieldName[MAX_FIELD_NAME_SIZE]; 
    size_t FieldSize;                           // size of the field in memory
    size_t Offset;                              // location in memory wrt the start of the struct
    DataTypes Type;                             // type of data the field holds
    char NestedStruct[MAX_STRUCT_NAME_SIZE];    // name of the nested structure

}FieldsNode;

// struct prototype
typedef struct StructDbNode StructDbNode;       

// each node in the database represents a structure
struct StructDbNode
{
    char StructName[MAX_STRUCT_NAME_SIZE];
    StructDbNode * next;                        // pointer to the next record in the database (pointer to the next node)
    size_t StructSize;                          // size of the struct in memory
    size_t nFields;                             // number of fields in the struct
    FieldsNode * Fields;                        // field info

};

// linked list that contains all the records of structures present in the application
typedef struct StructDb
{
    StructDbNode * head;                        // pointer to the first node in the list
    size_t size;                                // number of nodes present in the list

}StructDbList;


typedef struct ObjectDbnode ObjectDbnode; // struct prototype

// each node in the object data base represents a dynamically created object
struct ObjectDbnode
{
    void * ptr;                                 // pointer to the dynamic object
    ObjectDbnode * next;                        // pointer to the next record in the database
    size_t n;                                   // number of memory blocks assigned to the dynamic object
    StructDbNode * StructNode;                  // class of the dynamic object
    bool root;                                  // flag for root node
    bool visited;                               // flag for mdl algorithm

};

// linked list that contains all the record of dynamically created objects
typedef struct ObjectDb
{
    StructDbList * StructDb;                    // record of all structures present in the application
    ObjectDbnode * head;                        // pointer to the first node in the list
    size_t size;                                // number of nodes present in the list

}ObjectDbList;




/* ======================================= DataStructure Functions ======================================= */
// print functions for debugging
void printStructNode(StructDbNode * node);
void printStructList(StructDbList * list);
void printField(FieldsNode node);
void printObjNode(ObjectDbnode * node);
void printObjList(ObjectDbList * list);
// print the values of all supported fields by mld library for the object record passed as argument
void DumpObjectNode(ObjectDbnode * node); 


// initializes the object records and returns a pointer to the list
ObjectDbList * InitObjList(StructDbList * list);

// returns 0 if the node is successfully added otherwise 1.
int StructInsertIntoDb(StructDbList * list, StructDbNode * node);
int ObjInsertIntoDb(ObjectDbList * list, ObjectDbnode * node);
// searchs the table for a struct and returns the pointer to the node if found otherwise NULL
StructDbNode * StructLookUp(StructDbList * list, const char * StructName);
// searchs the table for an object pointer and returns the pointer if found otherwise NULL
ObjectDbnode * ObjectLookUp(ObjectDbList * list, void * ptr);

/* ======================================= API Functions ======================================= */

/* 
// corresponds to calloc(size_t, size_t)
// allocates 'n' units of contiguous memory blocks for an object of type "StructName"
// creates an object record for the object and adds it the record to the database
// link the object record to the structure record based on the "StructName"
// returns the pointer to the allocated object if successful otherwise NULL 
*/
void * xcalloc(ObjectDbList * ObjectDb, size_t n, const char * StructName);
/*
// corresponds to free(void * ptr)
// assert if the ptr passed as argument to xfree is not found in object database
// if found :
//  remove the object record from object db (but do not free it)
//  finally free the actual object by invoking free(obj_rec->ptr)
//  free object rec which is removed from object db 
*/
void xfree(void * ptr, ObjectDbList * list);

/*
// API to keep track of globally created objects
// creates a new ObjectNode of type 'StructName'
// marks it as a root
*/
void RegisterGlobalVar(ObjectDbList * list, void * ptr, const char * StructName, size_t n);

/*
// API to mark an existing dynamic object as the root
*/
void RegisterObjectasRoot(ObjectDbList * list, void * ptr);
void * AllocateObjectasRoot(ObjectDbList * objlist, StructDbList * structlist, void * ptr, const char * structname, size_t count);

// adds primitive data types into the StructDB for direct array allocation
void InitBasicMLD(StructDbList * list);

/*
// algorithm that detects memory leaks
// all data structures in an application take on the form of a Directed Cyclic Graph
// assumes that a root node is always reachable
// a root node is an object, from which, every other object could be reached
// the algorithm tries to visit all the nodes connecting to the root using depth first search
// at the end of the run, all nodes that are not visited are unreachable, i.e., leaked objects
*/ 
void MLDRun(ObjectDbList * list);

/* ======================================= MDL Functions ======================================= */

// clears visited flag on all the object nodes
static void InitMLD(ObjectDbList * list);

// returns the nearest root object to the current node otherwise NULL
static ObjectDbnode * GetNearestRoot(ObjectDbList * list, ObjectDbnode * node);

// explores nodes using dfs algorithm to find reachable nodes
static void ExploreNodesFrom(ObjectDbList * list, ObjectDbnode * node);

/* ======================================= EOF ======================================= */
#endif