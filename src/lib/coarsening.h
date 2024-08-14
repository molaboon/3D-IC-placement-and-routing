#include "readfile.h"

#define cellNameSize 100

typedef struct cell
{
    char name[cellNameSize];

}cell;

typedef struct hierarchyTree
{
    struct cell *root;
    struct cell *left;
    struct cell *right;

}hierarchyTree;



double clusteringScoreFunction( vector <RawNet> rawNets, bool haveMacro);
