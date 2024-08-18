#include "readfile.h"
#include <cstdlib>

#define cellNameSize 100

typedef struct node
{
    char *name;

    int index = 0;
    int area = 0;    
    node *root;
    node *sibling;

}node;

struct node* createNode(int index) 
{
    node *newNode = (node*)malloc(sizeof(node));
    newNode->index = index;
    newNode->root = NULL;
    newNode->sibling = NULL;
    return newNode;
}

typedef struct _nodeNet
{
    int numNode = 0;
    int netIndex = 0;
    node *nextNode;

}nodeNet;

struct _nodeNet *createNodeNet(int netIndex)
{
    nodeNet *newNodeNet = (nodeNet*)malloc(sizeof(nodeNet));
    newNodeNet->netIndex = netIndex;
    newNodeNet->nextNode = NULL;

    return newNodeNet;
}

typedef struct _nodeNets
{
    int numNet = 0;
    nodeNet *nextNodeNet;

}nodeNets;


double clusteringScoreFunction( vector <RawNet> rawNets, bool haveMacro, double avgArea);

void coarsen(vector <RawNet> rawNets, vector<Instance> &instances);