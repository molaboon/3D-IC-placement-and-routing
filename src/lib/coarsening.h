#include "readfile.h"

#define cellNameSize 100

typedef struct cell
{
    char name[cellNameSize];

    int level;
    double area;

    struct cell *coarsenedCells; // link the cells;

}cell;

typedef struct hierarchyTree
{
    struct cell *root;
    struct cell *left;
    struct cell *right;

}hierarchyTree;


double clusteringScoreFunction( vector <RawNet> rawNets, bool haveMacro, double avgArea);

void coarsen(vector <RawNet> rawNets, vector<Instance> &instances);