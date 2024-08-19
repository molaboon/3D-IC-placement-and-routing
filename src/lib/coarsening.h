#include "readfile.h"
#include <cstdlib>

#define cellNameSize 100

typedef struct node
{
    char *name;

    int index = 0;
    int area = 0;    
    node *root;
    node *left;
    node *right;
    node *sibling = NULL;

    vector <int> connection;
};



typedef struct _nodeNet
{
    int numPins = 0;
    int netIndex = 0;
    
    node *head = NULL;

    nodeNet *nextNet;

    // vector < node* > net;


}nodeNet;



typedef struct _nodeNets
{
    int numNet = 0;
    nodeNet *nets;
    // vector <nodeNet*> nets;

}nodeNets;

double returnCoarsenScore(node &firstNode, node &secondNode, nodeNets nodeNests, double avgArea);

void clusteringScoreFunction(vector <RawNet> rawNets, vector<Instance> &instances);

void coarsen(vector <RawNet> rawNets, vector<Instance> &instances);