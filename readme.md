# A Garbage Collector for C
A tiny Grabage Collectior for C which works on the basic of reachability of objects to detect memory leaks.

## Useage
The Library contains 3 data structures to hold your object data.
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
To link the fields of a `struct` with its node, create an array of `FieldNodes` containing the initialization arrays of individual nodes using the macro `FIELDINFO(Struct, Fieldname, type, nestedstruct)`

````
FieldsNode fieldarray[] = {
        FIELDINFO(struct1, name1, DataType, nestedstruct),
        FIELDINFO(struct1, name2, DataType, nestedstruct)
    };
````

