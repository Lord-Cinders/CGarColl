# A Garbage Collector for C
A tiny Grabage Collectior for C which works on the basic of reachability of objects to detect memory leaks.

## Useage
The library Provides two APIs for Dynamic memory allocation, they are:

````
/* 
// corresponds to calloc(size_t, size_t)
// allocates 'n' units of contiguous memory blocks for an object of type "StructName"
// creates an object record for the object and adds it the record to the database
// link the object record to the structure record based on the "StructName"
// returns the pointer to the allocated object if successful otherwise NULL 
*/
void * xcalloc(ObjectDbList * ObjectDb, size_t n, const char * StructName);
````

````
/*
// corresponds to free(void * ptr)
// assert if the ptr passed as argument to xfree is not found in object database
// if found :
//  remove the object record from object db (but do not free it)
//  finally free the actual object by invoking free(obj_rec->ptr)
//  free object rec which is removed from object db 
*/
void xfree(void * ptr, ObjectDbList * list);
````


The Library contains 3 data structures to keep track of and hold your object data.
1. `StructDbList` - made out of StructDbNodes, where each node contains the information about a single `struct`. 

````
struct StructDbNode
{
    char StructName[MAX_STRUCT_NAME_SIZE];
    StructDbNode * next;                        // pointer to the next record in the database (pointer to the next node)
    size_t StructSize;                          // size of the struct in memory
    size_t nFields;                             // number of fields in the struct
    FieldsNode * Fields;                        // field info

};
````

````
typedef struct StructDb
{
    StructDbNode * head;                        // pointer to the first node in the list
    size_t size;                                // number of nodes present in the list

}StructDbList;
````
To store your structure, first, create a new `StructDbList` object and then initialize it
````
StructDbList * structlist = calloc(1, sizeof(StructDbList));
InitBasicMLD(strlist);
````
this snippet of code, creates a basic structure database list with primitive data types.
the primitive data types allowed with this library are:
````
uint8_t
uint32_t
int
float
double
char
````

utility fuctions available for this structure are:
````
// print functions for debugging
void printStructNode(StructDbNode * node);
void printStructList(StructDbList * list);
// returns 0 if the node is successfully added otherwise 1.
int StructInsertIntoDb(StructDbList * list, StructDbNode * node);
// searchs the table for a struct and returns the pointer to the node if found otherwise NULL
StructDbNode * StructLookUp(StructDbList * list, const char * StructName);
````

2. `Fields` - This data structure allows to store information about individual data members of a `struct`

````
typedef struct Fields
{
    char FieldName[MAX_FIELD_NAME_SIZE]; 
    size_t FieldSize;                           // size of the field in memory
    size_t Offset;                              // location in memory wrt the start of the struct
    DataTypes Type;                             // type of data the field holds
    char NestedStruct[MAX_STRUCT_NAME_SIZE];    // name of the nested structure

}FieldsNode;
````
To link the fields of a `struct` with its node, first, create an array of `FieldNodes` containing the initialization arrays of individual nodes using the macro `FIELDINFO(Struct, Fieldname, type, nestedstruct)`

````
FieldsNode fieldarray[] = {
        FIELDINFO(struct1, name1, DataType, nestedstruct),
        FIELDINFO(struct1, name2, DataType, nestedstruct)
    };
````

utility fuctions available for this structure are:
````
void printField(FieldsNode node);
````

To register your Node, use the macro `#define REGSTRUCT(list, Struct, Fieldinfo)`.

For example, take a structure:
````
typedef struct stdd
{
    char name[20];
    int age;
}stdd;
````

We create and register a the above struct as a node by, 
````
int main()
{
    StructDbList * strlist = calloc(1, sizeof(StructDbList));
    InitBasicMLD(strlist);
    
    FieldsNode fieldarray[] = {
        FIELDINFO(stdd, name, CHAR, NULL),
        FIELDINFO(stdd, age, INT32, NULL)
    };
    REGSTRUCT(strlist, stdd, fieldarray);
}

````




