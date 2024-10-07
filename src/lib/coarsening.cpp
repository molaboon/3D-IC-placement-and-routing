#include <math.h>
#include <iostream>
#include <algorithm>
#include <time.h>

#include "coarsening.h"

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
    newNode->sibling = NULL;

    return newNode;
}

struct netConnet* createNetConnect( int netIndex )
{
    netConnet *newConnect = (netConnet*) malloc(sizeof(netConnet));

    newConnect->netIndex = netIndex;
    newConnect->connect = NULL;

    return newConnect;
};

struct nodeNet *createNodeNet(int netIndex, int numPins)
{
    /*
        the nodeNet is store the 
    */
    nodeNet *newNodeNet = (nodeNet*)malloc(sizeof(nodeNet));

    newNodeNet->netIndex = netIndex;
    newNodeNet->numPins = numPins;

    newNodeNet->nextNet = NULL;
    newNodeNet->lastNet = NULL;

    return newNodeNet;
}

void coarsenPreprocessing(vector <RawNet> rawNets, nodeNets &nodeNets, vector <instance> instances, vector <node*> &nodesForest)
{   
    int numRawnet = rawNets.size();
    int numInstance = instances.size();

    nodeNet *position = nodeNets.nets;

    for(int i = 0 ; i < numInstance; i++)
    {
        node *newNode = createNode( i );

        newNode->area = instances[i].area;

        nodesForest.push_back(newNode);
    }

    for(int netIndex = 0; netIndex < numRawnet; netIndex++)
    {
        int numPins = rawNets[netIndex].numPins;

        nodeNet *newNodeNet = createNodeNet(netIndex, numPins);

        vector <node*> *tmpNodes = new vector <node*>();

        for(int cellIndex = 0; cellIndex < numPins ; cellIndex++)
        {
            int index = rawNets[netIndex].Connection[cellIndex]->instIndex;
        
            netConnet *newConnect = createNetConnect( netIndex );

            netConnet *tmpPointer = nodesForest[index]->connection;

            nodesForest[index]->numConnection += 1;

            tmpNodes->push_back( nodesForest[index] );

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

        newNodeNet->nodes = tmpNodes;

        if(nodeNets.nets == NULL)
        {   
            nodeNets.nets = newNodeNet;
            newNodeNet->lastNet = NULL;

            position = nodeNets.nets;
        }
        else
        {
            while (position->nextNet != NULL)
            {
                position = position->nextNet;
            }
            newNodeNet->lastNet = position;
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

    double Ddegree = 0;
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

                if( tmp < 2)
                {}
                else
                {
                    Ddegree += 1.0 / (double) (tmp - 1);
                }
            }

            secondNetConnect = secondNetConnect->connect;
        }

        firstNetConnect = firstNetConnect->connect;
    }    
    score = exp(-(firstNode.area + secondNode.area) / avgArea) * Ddegree;

    return score;
}

void popOutNode(vector <node*> &nodeForest, int firstNodeIndex, int secondNodeIndex, node *newNode)
{
    int size = nodeForest.size();

    // if(firstNodeIndex > secondNodeIndex)
    // {
    //     int tmp = secondNodeIndex;
    //     secondNodeIndex = firstNodeIndex;
    //     firstNodeIndex = tmp;
    // }
    for(int i = 0; i < size ; i++)
    {
        if(nodeForest[i]->index == firstNodeIndex)
        {
            if(nodeForest[i+1]->index == secondNodeIndex)
            {
                for(int j = i; j < size - 2; j++)
                {
                    nodeForest[j] = nodeForest[j+2];
                }
            }
            else
            {
                for(int j = i; j < size; j++)
                {
                    nodeForest[j] = nodeForest[j+1];

                    if(nodeForest[j + 1]->index == secondNodeIndex)
                    {
                        nodeForest[j] = nodeForest[j+2];

                        for(int k = j + 1; k < size - 1; k++)
                        {
                            nodeForest[k] = nodeForest[k+2];
                        }
                        break;
                    }
                }
            }
            break;
        }
    }
    nodeForest.pop_back();
    nodeForest[size-2] = newNode;
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

void updateDataStucture(vector < node* > &nodeForest, nodeNets &ndNets)
{
    int size = nodeForest.size();
    int numNet = ndNets.numNet;
    node *newNode = nodeForest[size - 1];
    nodeNet *pOfNets = ndNets.nets;

    for(int i = 0; i <= numNet; i++)
    {
        bool nodeChanged = false;
        bool haveDual = false;
        int dual = 0;

        for (int j = 0; j < pOfNets->numPins; j++)
        {
            if( pOfNets->nodes->at(j)->index == newNode->left->index || pOfNets->nodes->at(j)->index == newNode->right->index)
            {   
                if(nodeChanged)
                {
                    dual = j ;
                    haveDual = 1;
                }
                else
                {
                    nodeChanged = true;
                    pOfNets->nodes->at(j) = newNode;
                }
            }
        }

        if(haveDual)
        {
            pOfNets->nodes->erase(pOfNets->nodes->begin() + dual);
            pOfNets->numPins = int( pOfNets->nodes->size() );

            if(pOfNets->numPins == 1)
            {
                nodeNet *freeNet = pOfNets;
                ndNets.numNet--;
                numNet--;

                if( pOfNets->lastNet == NULL)
                {
                    ndNets.nets = pOfNets->nextNet;
                    pOfNets = ndNets.nets;
                    pOfNets->lastNet = NULL;
                }
                else if ( pOfNets->nextNet == NULL)
                {
                    pOfNets->lastNet->nextNet = NULL;
                }
                else
                {
                    nodeNet *tmpLastNet = pOfNets->lastNet;

                    tmpLastNet->nextNet = pOfNets->nextNet;
                    pOfNets = tmpLastNet->nextNet;
                }
                free(freeNet);
            }
        }
        else
            pOfNets = pOfNets->nextNet;
    }


    
    updateConnection(nodeForest, ndNets, newNode);
}

void bestChoice(vector < node* > &nodesForest, int avgArea, nodeNets &nets, node *newNode)
{
    int numInstance = nodesForest.size();
    
    double bestGrade = 0.0;

    double bestDegree = 0.0;

    node *bestChoice1, *bestChoice2;

    for (int firstNode = 0; firstNode < numInstance; firstNode++)
    {
        for(int secondNode = firstNode + 1; secondNode < numInstance; secondNode++ )
        {  
            double degree = 0.0;
            double tmpGrade = 0.0;
            
            tmpGrade = returnCoarsenScore( *nodesForest[firstNode], *nodesForest[secondNode], nets, avgArea);

            if (tmpGrade > bestGrade)
            {
                bestChoice1 = nodesForest[firstNode];

                bestChoice2 = nodesForest[secondNode];

                bestGrade = tmpGrade;
            }            
        }
    }
    newNode->left = bestChoice1;

    newNode->right = bestChoice2;

    newNode->area = bestChoice1->area + bestChoice2->area;
}

void nodesToInstances(vector < node* > &nodesForest, vector <instance> &newLevelInstance, int numNodes)
{
    /*
        3. write the function to nodes to instances
    */
    for(int i = 0; i < numNodes; i++)
    {
        instance newInstance;

        newInstance.instIndex = nodesForest[i]->index;

        newInstance.area = nodesForest[i]->area;

        newLevelInstance.push_back(newInstance);
    }
}

void printWhichMerged()
{
    // for(int i = 0; i < nodeNets.numNet; i++)
        // {
        //     for (int j = 0 ; j < tmp->numPins; j++)
        //     {
        //         cout << tmp->nodes->at(j)->index<< " ";
        //     }
        //     cout << endl;
        //     tmp = tmp->nextNet;
        // }
        // cout << "Cell: ";
        // for(int i = 0; i < nodesForest.size(); i++)
        // {
        //     cout << nodesForest[i]->index << " ";
        // }
        // cout << endl << endl;
}

void coarsen(vector <RawNet> rawNets, vector<instance> &instances)
{
    double START, END; 
    START = clock();
    
    nodeNets nodeNets;
    nodeNets.nets = NULL;
    vector < node* > nodesForest;

    int numInstance = instances.size();  
    int instIndex = numInstance;

    double bestGrade = 0.0;
    double totalArea = 0.0;
    double avgArea = 0.0; 

    for(int i = 0; i < numInstance; i++)
        totalArea += instances[i].area;

    totalArea *= weight;
    coarsenPreprocessing(rawNets, nodeNets, instances, nodesForest);

    nodeNet *tmp = nodeNets.nets;

    for(int i = 0; i < 10 ; i++)
    {
        node *newNode = createNode(instIndex);

        avgArea = totalArea / double(numInstance);

        bestChoice(nodesForest, avgArea, nodeNets, newNode);

        popOutNode(nodesForest, newNode->left->index, newNode->right->index, newNode);

        updateDataStucture(nodesForest, nodeNets);

        instIndex++;

        numInstance--;

        nodeNet *tmp = nodeNets.nets;
        
        // for(int i = 0; i < nodeNets.numNet; i++)
        // {
        //     for (int j = 0 ; j < tmp->numPins; j++)
        //     {
        //         cout << tmp->nodes->at(j)->index<< " ";
        //     }
        //     cout << endl;
        //     tmp = tmp->nextNet;
        // }
        // cout << "Cell: ";
        // for(int i = 0; i < nodesForest.size(); i++)
        // {
        //     cout << nodesForest[i]->index << " ";
        // }
        // cout << endl << endl;
        // cout << "merge: " << newNode->left->index << ", " << newNode->right->index << " to "  << newNode->index << endl;
        cout << "iter: "<< i << endl;
    }

    END = clock();
    cout << (END - START) / CLOCKS_PER_SEC << endl;

}

