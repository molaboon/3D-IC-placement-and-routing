#include "coarsening.h"
#include <math.h>
#include <iostream>

#define weight 5.0

struct node* createNode(int index) 
{
    node *newNode = (node*)malloc(sizeof(node));
    newNode->index = index;
    newNode->root = NULL;
    newNode->left = NULL;
    newNode->right = NULL;
    newNode->connection = NULL;

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

    return newNodeNet;
}

void coarsenPreprocessing(vector <RawNet> rawNets, nodeNets &nodeNets, vector <Instance> instances, vector <node*> &nodes)
{   
    int numRawnet = rawNets.size();
    int numInstance = instances.size();

    nodeNet *position = nodeNets.nets;

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

            if(nodes[index]->connection == NULL)
            {
                nodes[index]->connection = newConnect;
                // cout << nodes[index]->connection <<endl<<endl;
            }
            else
            {
                netConnet *tmpPointer = nodes[index]->connection;

                cout <<newConnect <<endl;

                while (tmpPointer != NULL)
                {
                    tmpPointer = tmpPointer->connect;
                }

                tmpPointer = newConnect;

                cout << tmpPointer << endl<<endl;
            }
        }

        position = newNodeNet;

        position = newNodeNet->nextNet;
    }
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

    for(int i = 0; i < numInstance; i++)
    {
        netConnet *tmp = nodes[i]->connection;

        cout << tmp << endl;

        if(tmp->connect == NULL)
        {
            cout << "NO " << i <<endl;
        }
        // while ( tmp != NULL )
        // {           
        //     cout << "cell: "<< i << ",  "<< tmp->netIndex << " " << tmp->connect->netIndex << endl;
        //     tmp = tmp->connect;
        // }
        
    }
     
    for (int firstNode = 0; firstNode < numInstance; firstNode++)
    {
        for(int secondNode = firstNode + 1; secondNode < numInstance; secondNode++ )
        {  
            double tmpGrade = returnCoarsenScore( *nodes[firstNode], *nodes[secondNode], nodeNets, avgArea);

            if (tmpGrade > bestGrade)
            {
                bestChoice1 = nodes[firstNode];

                bestChoice2 = nodes[firstNode];

                bestGrade = tmpGrade;
            }            
        }
    }

    node *newNode = createNode(instIndex);

    newNode->left = bestChoice1;

    newNode->right = bestChoice2;

    newNode->area = bestChoice1->area + bestChoice2->area;

}

double returnCoarsenScore(node &firstNode, node &secondNode, nodeNets nodeNests, double avgArea)
{
    // int firstNodeNumConnect = firstNode.connection.size();

    // int secondNodeNumConnect = secondNode.connection.size();

    // double degree = 0;
    // double score;

    // for(int i = 0; i < firstNodeNumConnect; i++ )
    // {
    //     for(int j = 0; j < secondNodeNumConnect; j++)
    //     {
    //         if(firstNode.connection[i] == secondNode.connection[j])
    //         {
    //             int tmp = firstNode.connection[i];

    //             degree +=  (double) 1.0 / (double) ();
    //         }
    //     }
    // }
    // score = exp(-(firstNode.area + secondNode.area) / avgArea) * degree;

    return 0.0;

}


void clusteringScoreFunction(vector <RawNet> rawNets, vector<Instance> &instances)
{
    vector <RawNet> tmpRawNets;

    
    int numInstance = instances.size();
    int rawnetSize = rawNets.size();
    double bestGrade = 0.0;
    int bestChoice1, bestChoice2;
    double gradeMap[numInstance][numInstance] = {}; // need optimize
    double avgArea = 0.0; 

    for(int i = 0; i < numInstance; i++)
        avgArea += instances[i].area;

    avgArea = avgArea / double(numInstance);

    avgArea = avgArea * weight;

    // cout << avgArea << endl;

    
}