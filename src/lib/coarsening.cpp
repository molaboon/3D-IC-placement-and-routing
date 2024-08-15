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
    int numInstance = instances.size();
    int rawnetSize = rawNets.size();

    double gradeMap[numInstance][numInstance] = {};

    double avgArea = 0.0; 

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
            double tmp= 0.0;
            bool key = 0;

            for(int nets = 0; nets < rawnetSize; nets++)
            {   
                if( instances[firstCell].netsConnect[nets] & instances[secondCell].netsConnect[nets])
                {
                    degree += (double) (rawNets[nets].numPins - 1);
                    cout << degree << "\n";
                }

            }
            if(degree != 0.0 )
            {
                tmp = exp(-(instances[firstCell].area + instances[secondCell].area) / avgArea);
                gradeMap[firstCell][secondCell] =  tmp * (1.0 / degree);
            }

            cout << "cell("<<firstCell+1 << ", "<< secondCell+1 <<"): "<< gradeMap[firstCell][secondCell] <<"\n";

        }
    }

    
}