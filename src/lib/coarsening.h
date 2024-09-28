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
    int index = 0;
    int area = 0;    
    int numConnection = 0;

    node *left = NULL;
    node *right = NULL;
    node *sibling = NULL;

    netConnet *connection = NULL;
};

typedef struct nodeNet
{
    int numPins = 0;
    int netIndex = 0;
    
    vector < node* > *nodes;

    nodeNet *nextNet = NULL;
    nodeNet *lastNet = NULL;
};

typedef struct nodeNets
{
    int numNet = 0;
    nodeNet *nets = NULL;

    vector < nodeNet* > netnets;
};

double returnCoarsenScore(node &firstNode, node &secondNode, nodeNets &nodeNests, double avgArea);

int returnDegree(nodeNets &nodeNets, int netIndex);

void popOutNode(vector < node* > &nodeForest, int firstNodeIndex, int secondNodeIndex, node *newNode);

void updateConnection(vector < node* > &nodeForest, nodeNets &nets, node*newNode);

void updateDataStucture(vector < node* > &nodeForest, nodeNets &nets);

void bestChoice(vector < node* > &nodesForest, int avgArea, nodeNets &nets, node *newNode);

void nodesToInstances(vector < node* > &nodesForest, vector <instance> &newLevelInstance, int numNodes);

void coarsen(vector <RawNet> rawNets, vector<instance> &instances);