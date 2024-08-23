#include "coarsening.h"
#include <math.h>
#include <iostream>

#define weight 5.0
#define debugMode 1

struct node* createNode(int index) 
{
    node *newNode = (node*)malloc(sizeof(node));
    newNode->index = index;
    newNode->root = NULL;
    newNode->left = NULL;
    newNode->right = NULL;
    newNode->connection = NULL;
    newNode->numConnection = 0;

    return newNode;
}

struct netConnet* createNetConnect( int netIndex )
{
    netConnet *newConnect = (netConnet*) malloc(sizeof(netConnet));

    newConnect->netIndex = netIndex;

    newConnect->connect = NULL;

    return newConnect;
};


struct nodeNet *createNodeNet(int netIndex)
{
    nodeNet *newNodeNet = (nodeNet*)malloc(sizeof(nodeNet));

    newNodeNet->netIndex = netIndex;
    newNodeNet->nextNet = NULL;

    return newNodeNet;
}

void coarsenPreprocessing(vector <RawNet> rawNets, nodeNets &nodeNets, vector <Instance> instances, vector <node*> &nodes)
{   
    int numRawnet = rawNets.size();
    int numInstance = instances.size();

    nodeNet *position;

    for(int i = 0 ; i < numInstance; i++)
    {
        node *newNode = createNode( i );

        newNode->area = instances[i].area;

        nodes.push_back(newNode);
    }

    for(int netIndex = 0; netIndex < numRawnet; netIndex++)
    {
        int numPins = rawNets[netIndex].numPins;

        nodeNet *newNodeNet = createNodeNet( netIndex );

        newNodeNet->netIndex = netIndex;
        
        newNodeNet->numPins = numPins;

        for(int cellIndex = 0; cellIndex < numPins ; cellIndex++)
        {
            int index = rawNets[netIndex].Connection[cellIndex]->instIndex;

            netConnet *newConnect = createNetConnect( netIndex );

            nodes[index]->numConnection += 1;

            if(newNodeNet->head == NULL)
            {
                newNodeNet->head = nodes[index];
            }
            else
            {                
                node *tmp = newNodeNet->head;
                while ( tmp != NULL)
                {
                    tmp = tmp->sibling;
                }
                tmp = nodes[index];
            }

            netConnet *tmpPointer = nodes[index]->connection;

            if(tmpPointer == NULL)
            {
                nodes[index]->connection = newConnect;
            }
            else
            {
                while (tmpPointer->connect != NULL)
                {
                    tmpPointer = tmpPointer->connect;
                }
                tmpPointer->connect = newConnect;
            }
        }

        if(nodeNets.nets == NULL)
        {
            nodeNets.nets = newNodeNet;
            position = nodeNets.nets;
        }
        else
        {
            while (position->nextNet != NULL)
            {
                position = position->nextNet;
            }
            position->nextNet = newNodeNet;
        }
    }
}

int returnDegree(nodeNets &nodeNets, int netIndex)
{
    
    nodeNet *tmpPointer = nodeNets.nets;

    do
    {
        if(tmpPointer->netIndex == netIndex)
        {
            return tmpPointer->numPins;
        }
        else
        {
            tmpPointer = tmpPointer->nextNet;
        }

    } while ( tmpPointer != NULL );
    
    if(debugMode)
        cout << "return Degree error" << endl;
    
    return 0;
}

double returnCoarsenScore(node &firstNode, node &secondNode, nodeNets &nodeNests, double avgArea)
{
    int firstNodeNumConnect = firstNode.numConnection;

    int secondNodeNumConnect = secondNode.numConnection;

    double degree = 0;
    double score;

    netConnet *firstNetConnect = firstNode.connection;

    for(int i = 0; i < firstNodeNumConnect; i++ )
    {
        netConnet *secondNetConnect = secondNode.connection;

        for(int j = 0; j < secondNodeNumConnect; j++)
        {
            if( firstNetConnect->netIndex == secondNetConnect->netIndex )
            {
                // cout << firstNetConnect->netIndex << ", "<< secondNetConnect->netIndex <<endl;

                int tmp = returnDegree(nodeNests, firstNetConnect->netIndex);
                degree += 1.0 / (double) (tmp - 1);
            }

            secondNetConnect = secondNetConnect->connect;
        }

        firstNetConnect = firstNetConnect->connect;
    }
    

    score = exp(-(firstNode.area + secondNode.area) / avgArea) * degree;

    return score;

}

void coarsen(vector <RawNet> rawNets, vector<Instance> &instances)
{
    nodeNets nodeNets;
    vector < node* > nodes;

    int numInstance = instances.size();
    int rawnetSize = rawNets.size();
    double bestGrade = 0.0;
    node *bestChoice1, *bestChoice2;
    double gradeMap[numInstance][numInstance] = {}; // need optimize
    double avgArea = 0.0; 
    int instIndex = numInstance;

    for(int i = 0; i < numInstance; i++)
        avgArea += instances[i].area;

    avgArea = avgArea / double(numInstance);

    avgArea = avgArea * weight;

    coarsenPreprocessing(rawNets, nodeNets, instances, nodes); 

    nodeNet * tmptmp = nodeNets.nets;
     

    for (int firstNode = 0; firstNode < numInstance; firstNode++)
    {
        for(int secondNode = firstNode + 1; secondNode < numInstance; secondNode++ )
        {  
            double tmpGrade = returnCoarsenScore( *nodes[firstNode], *nodes[secondNode], nodeNets, avgArea);

            if (tmpGrade > bestGrade)
            {
                bestChoice1 = nodes[firstNode];

                bestChoice2 = nodes[secondNode];

                bestGrade = tmpGrade;

                cout << "cell: "<< bestChoice1->index << ", "<< bestChoice2->index << ", "<< bestGrade << endl;
            }            
        }
    }

    node *newNode = createNode(instIndex);

    newNode->left = bestChoice1;

    newNode->right = bestChoice2;

    newNode->area = bestChoice1->area + bestChoice2->area;

}

