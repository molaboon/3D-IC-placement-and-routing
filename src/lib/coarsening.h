#include "readfile.h"
#include <cstdlib>

#define cellNameSize 100

typedef struct netConnet
{
    int netIndex = 0;
    
    netConnet *connect = NULL;
};

typedef struct node
{
    char *name;

    int index = 0;
    int area = 0;    
    node *left;
    node *right;
    node *sibling = NULL;

    int numConnection = 0;

    netConnet *connection = NULL;
};


typedef struct nodeNet
{
    int numPins = 0;
    int netIndex = 0;
    
    node *head = NULL;

    nodeNet *nextNet;

    // vector < node* > net;
};



typedef struct nodeNets
{
    int numNet = 0;
    nodeNet *nets;
    // vector <nodeNet*> nets;

};

double returnCoarsenScore(node &firstNode, node &secondNode, nodeNets &nodeNests, double avgArea);

int returnDegree(nodeNets &nodeNets, int netIndex);

void popOutNode(vector < node* > &nodeForest, int firstNodeIndex, int secondNodeIndex, node *newNode);

void updateDataStucture(vector < node* > &nodeForest, nodeNets &nets);


void coarsen(vector <RawNet> rawNets, vector<Instance> &instances);