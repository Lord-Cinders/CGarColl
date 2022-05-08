# A Garbage Collector for C
A tiny Grabage Collector for C which works on the basis of reachability of objects to detect memory leaks.

## Usage
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
And two other APIs to record an object as a root object, they are:

````
/*
// API to keep track of globally created objects
// creates a new ObjectNode of type 'StructName'
// marks it as a root
*/
void RegisterGlobalVar(ObjectDbList * list, void * ptr, const char * StructName, size_t n);
````
````
/*
// API to mark an existing dynamic object as the root
*/
void RegisterObjectasRoot(ObjectDbList * list, void * ptr);

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
utility fuctions available for this structure are:
````
void printField(FieldsNode node);
````

To link the fields of a `struct` with its node, first, create an array of `FieldNodes` containing the initialization arrays of individual nodes using the macro `FIELDINFO(Struct, Fieldname, type, nestedstruct)`

````
FieldsNode fieldarray[] = {
        FIELDINFO(struct1, name1, DataType, nestedstruct),
        FIELDINFO(struct1, name2, DataType, nestedstruct)
    };
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

We create and register the above struct as a node by, 
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

3. `ObjectDbList` - Similar to `StructDbList`, is made out of `ObjectDbNode`, where each node represents a dynamically created object in your application.

````
struct ObjectDbnode
{
    void * ptr;                                 // pointer to the dynamic object
    ObjectDbnode * next;                        // pointer to the next record in the database
    size_t n;                                   // number of memory blocks assigned to the dynamic object
    StructDbNode * StructNode;                  // class of the dynamic object
    bool root;                                  // flag for root node
    bool visited;                               // flag for mdl algorithm

};
````

````
typedef struct ObjectDb
{
    StructDbList * StructDb;                    // record of all structures present in the application
    ObjectDbnode * head;                        // pointer to the first node in the list
    size_t size;                                // number of nodes present in the list

}ObjectDbList;
````
The List is created and initialized using `InitObjList(StructDbList * list)` 

````
ObjectDbList * objlist = InitObjList(strlist);
````

The nodes are dynamically added and deleted when the application calls `xcalloc` and `xfree`.

utility fuctions available for this structure are:
````
// print functions for debugging
void printObjNode(ObjectDbnode * node);
void printObjList(ObjectDbList * list

// print the Object with all its information stored in it
void DumpObjectNode(ObjectDbnode * node); 

// returns 0 if the node is successfully added otherwise 1.
int ObjInsertIntoDb(ObjectDbList * list, ObjectDbnode * node);

// searchs the table for an object pointer and returns the pointer if found otherwise NULL
ObjectDbnode * ObjectLookUp(ObjectDbList * list, void * ptr);
````

To detect Memory Leaks, The library provides the function `MLDRun(ObjectDbList * list)`, which detects and reports leaked objects inside memory.

For example, consider
````
typedef struct stdd
{
    char name[20];
    int age;
}stdd;
typedef struct emp empt;

int main()
{
    StructDbList * strlist = calloc(1, sizeof(StructDbList));
    InitBasicMLD(strlist);
    
    FieldsNode fieldarray[] = {
        FIELDINFO(stdd, name, CHAR, NULL),
        FIELDINFO(stdd, age, INT32, NULL)
    };
    REGSTRUCT(strlist, stdd, fieldarray);

    ObjectDbList * objlist = InitObjList(strlist);

    stdd * temp = xcalloc(objlist, 1, "stdd");
    stdd * t    = xcalloc(objlist, 2, "stdd"); // we are intentionally leaking this object

    t[0].age = 69;
    t[1].age = 100;

    RegisterObjectasRoot(objlist, temp);

    MLDRun(objlist);

    xfree(temp, objlist);
    xfree(t, objlist);

    return 0;
}

````

At the end of the run, the funtion `MLDRun` reports the object we intentionally leaked along with all of its information.

## Building and Linking
To create an object file of the library just use the Makefile provided along with the code and run the `make` command. link the object file to your compiler path and include the header file to your application to start using the library.
