# A Garbage Collector for C
A tiny Grabage Collectior for C which works on the basic of reachability of objects to detect memory leaks.

## Useage
The Library contains 3 data structures to hold your object data.
1. The StructDbList - made out of StructDbNodes, where each node contains the information about a single `struct`. 

``
    struct StructDbNode
    {
        char StructName[MAX_STRUCT_NAME_SIZE];
        StructDbNode * next;                        // pointer to the next record in the database (pointer to the next node)
        size_t StructSize;                          // size of the struct in memory
        size_t nFields;                             // number of fields in the struct
        FieldsNode * Fields;                        // field info

    };
``


