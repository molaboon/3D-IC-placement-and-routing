#include "coarsening.h"
#include <math.h>
#include <iostream>

#define weight 5.0

double clusteringScoreFunction( vector <RawNet> rawNets, bool haveMacro, double avgArea)
{
    return 0.0;
}

void coarsen(vector <RawNet> rawNets, vector<Instance> &instances)
{
    vector <RawNet> tmpRawNets;

    
    int numInstance = instances.size();
    int rawnetSize = rawNets.size();
    double bestGrade = 0.0;
    int bestChoice1, bestChoice2;
    double gradeMap[numInstance][numInstance] = {}; // need optimize
    double avgArea = 0.0; 

    for (int i = 0; i < rawnetSize; i++)
    {
        RawNet tmpNet;

        tmpNet.numPins = rawNets[i].numPins;

        tmpNet.Connection = rawNets[i].Connection;

        tmpRawNets.emplace_back(tmpNet);
    }

    
    for(int i = 0; i < numInstance; i++)
        avgArea += instances[i].area;

    avgArea = avgArea / double(numInstance);

    avgArea = avgArea * weight;

    cout << avgArea << endl;

    
    for (int firstCell = 0; firstCell < numInstance; firstCell++)
    {
        for(int secondCell = firstCell + 1; secondCell < numInstance; secondCell++ )
        {   
            double degree = 0.0;
            double tmpGrade= 0.0;
            bool key = 0;

            for(int nets = 0; nets < rawnetSize; nets++)
            {   
                if( instances[firstCell].netsConnect[nets] & instances[secondCell].netsConnect[nets])
                {
                    degree += (double) (rawNets[nets].numPins - 1);
                }

            }
            if(degree != 0.0 )
            {
                tmpGrade = exp(-(instances[firstCell].area + instances[secondCell].area) / avgArea) * (1.0 / degree);
                gradeMap[firstCell][secondCell] =  tmpGrade ;

                if (tmpGrade > bestGrade)
                {
                    bestChoice1 = firstCell;

                    bestChoice2 = secondCell;

                    bestGrade = tmpGrade;
                }
            }
        }
    }

    
}