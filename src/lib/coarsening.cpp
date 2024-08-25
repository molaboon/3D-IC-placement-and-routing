#include "coarsening.h"
#include <math.h>
#include <iostream>
#include <algorithm>


#define weight 5.0
#define debugMode 1
#define MAXNUM 9999
using namespace std;

struct node* createNode(int index) 
{
    node *newNode = (node*)malloc(sizeof(node));
    newNode->index = index;
    newNode->left = NULL;
    newNode->right = NULL;
    newNode->connection = NULL;
    newNode->area = 0;
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

void coarsenPreprocessing(vector <RawNet> rawNets, nodeNets &nodeNets, vector <Instance> instances, vector <node*> &nodesForest)
{   
    int numRawnet = rawNets.size();
    int numInstance = instances.size();

    nodeNet *position;

    for(int i = 0 ; i < numInstance; i++)
    {
        node *newNode = createNode( i );

        newNode->area = instances[i].area;

        nodesForest.push_back(newNode);
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

            nodesForest[index]->numConnection += 1;

            if(newNodeNet->head == NULL)
            {
                newNodeNet->head = nodesForest[index];
            }
            else
            {                
                node *tmp = newNodeNet->head;
                while ( tmp != NULL)
                {
                    tmp = tmp->sibling;
                }
                tmp = nodesForest[index];
            }

            netConnet *tmpPointer = nodesForest[index]->connection;

            if(tmpPointer == NULL)
            {
                nodesForest[index]->connection = newConnect;
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

        nodeNets.numNet += 1;
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

void popOutNode(vector < node* > &nodeForest, int firstNodeIndex, int secondNodeIndex, node *newNode)
{
    int size = nodeForest.size();

    node *tmp1 = nodeForest[size - 1];
    node *tmp2 = nodeForest[size - 2];

    for(int i = 0; i < size; i++)
    {
        if(nodeForest[i]->index == firstNodeIndex)
            nodeForest[i] = tmp1;
        
        else if(nodeForest[i]->index == secondNodeIndex)
            nodeForest[i] = tmp2;
    }

    nodeForest.pop_back();
    nodeForest.pop_back();

    nodeForest.push_back(newNode);

}

void updateConnection(vector < node* > &nodeForest, nodeNets &nets, node*newNode)
{
    int size = nodeForest.size();
    int numConnection1 = newNode->left->numConnection;
    int numConnection2 = newNode->right->numConnection;

    int numPopOut = 0;
    vector <int> array(numConnection1 + numConnection2);

    netConnet *tmp = newNode->left->connection;
    netConnet *lastTmp = tmp;
    netConnet *tmp2 = newNode->right->connection;
    netConnet *lastTmp2 = tmp2;

    for(int i = 0; i < numConnection1; i++)
    {
        array[i] = tmp->netIndex;
        lastTmp = tmp;
        tmp = tmp->connect;
        free(lastTmp);
    }

    for(int j = 0; j< numConnection2; j++)
    {
        array[j + numConnection1] = tmp2->netIndex;
        lastTmp2 = tmp2;
        tmp2 = tmp2->connect;
        free(lastTmp2);
    }
    free(tmp);
    free(tmp2);

    sort(array.begin(), array.end());

    for(int k = 1; k < (numConnection1 + numConnection2); k++)
    {
        if(array[k-1] == array[k])
        {   
            array[k-1] = MAXNUM;
            numPopOut += 1;
        }
    }

    sort(array.begin(), array.end());

    for(int l = 0; l < numPopOut; l++)
        array.pop_back();

    int arrSize = array.size();

    newNode->numConnection = arrSize;

    for(int i = 0; i < arrSize; i++)
    {
        netConnet *newConnect = createNetConnect( array[i] );
        netConnet *tmpPointer = newNode->connection;

        if( tmpPointer == NULL)
        {
            newNode->connection = newConnect;
        }
        else
        {
            while ( tmpPointer->connect != NULL )
            {
                tmpPointer = tmpPointer->connect;
            }
            tmpPointer->connect = newConnect;
        }
    }
    
}


void updateDataStucture(vector < node* > &nodeForest, nodeNets &Nets)
{
    int size = nodeForest.size();
    node *newNode = nodeForest[size - 1];
    updateConnection(nodeForest, Nets, newNode);
    nodeNet *tmp = Nets.nets;
    cout << Nets.numNet << endl;

    for(int i = 0; i < Nets.numNet; i++)
    {
        int numPin = tmp->numPins;
        node *head = tmp->head;
        cout << "Net"<<tmp->netIndex << ": ";
        for(int j = 0; j < numPin; j++)
        {
            cout << head->index << ", ";
        }
        
        cout << endl;

        tmp = tmp->nextNet;
    }
}

void coarsen(vector <RawNet> rawNets, vector<Instance> &instances)
{
    nodeNets nodeNets;
    vector < node* > nodesForest;

    int numInstance = instances.size();
    double bestGrade = 0.0;
    node *bestChoice1, *bestChoice2;
    double avgArea = 0.0; 
    int instIndex = numInstance;

    for(int i = 0; i < numInstance; i++)
        avgArea += instances[i].area;

    avgArea = avgArea / double(numInstance);

    avgArea = avgArea * weight;

    coarsenPreprocessing(rawNets, nodeNets, instances, nodesForest); 
     
    for (int firstNode = 0; firstNode < numInstance; firstNode++)
    {
        for(int secondNode = firstNode + 1; secondNode < numInstance; secondNode++ )
        {  
            double tmpGrade = returnCoarsenScore( *nodesForest[firstNode], *nodesForest[secondNode], nodeNets, avgArea);

            if (tmpGrade > bestGrade)
            {
                bestChoice1 = nodesForest[firstNode];

                bestChoice2 = nodesForest[secondNode];

                bestGrade = tmpGrade;
            }            
        }
    }

    node *newNode = createNode(instIndex);
    
    newNode->left = bestChoice1;

    newNode->right = bestChoice2;

    newNode->area = bestChoice1->area + bestChoice2->area;

    // cout << newNode->index <<", "<< newNode->left->index <<", "<< newNode->right->index <<", "<< newNode->area <<endl;

    popOutNode(nodesForest, newNode->left->index, newNode->right->index, newNode);

    updateDataStucture(nodesForest, nodeNets);

}

