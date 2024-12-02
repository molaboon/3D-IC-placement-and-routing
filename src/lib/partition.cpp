#include <iostream>
#include <stdlib.h>
#include <time.h>

#include "partition.h"
#include "readfile.h"
#include "initial_placement.h"
#include "CG.h"


void macroPartition( vector <instance> &macros, vector <RawNet> &netsOfMacros, Die topDie)
{
    srand( time(NULL) );

    int numMacros = int ( macros.size() );
    int cnt = 0;
    int cut = 0;
    int bestCut = 9999;

    double topDieArea = 0;
    double btmDieArea = 0;
    double dif = 0;
    double ratio = 0.5;

    int finalLayer[numMacros] = {0};
    
    while (cnt < 10)
    {
        topDieArea = 0;
        btmDieArea = 0;
        
        for(int i = 0; i < numMacros; i++)
        {
            int layer = rand() % 2;
            macros[i].layer = layer;

            if(layer)
                topDieArea += macros[i].area;
            
            else
                btmDieArea += macros[i].inflateArea;

            dif = abs(topDieArea - btmDieArea);

            if( dif > topDie.upperRightX * topDie.upperRightY * ratio )
            {
                if(layer)
                {
                    topDieArea -= macros[i].area;
                    btmDieArea += macros[i].inflateArea;
                }
                else
                {
                    topDieArea += macros[i].area;
                    btmDieArea -= macros[i].inflateArea;
                }
            }

            if(ratio > 0.1)
                ratio *= 0.8;    
        }  

        cut = returnCut(netsOfMacros);
        cnt++;
        // printf("%lf, %lf, %d\n", topDieArea, btmDieArea, cut);

        if(cut < bestCut)
        {
            for(int i = 0; i < numMacros; i++)
                finalLayer[numMacros] = macros[i].layer;    
        }    
    }
    // calculate the area and the best cut 

}

int returnCut(vector <RawNet> &netsOfMacros)
{
    int numNetsOfMacro = int( netsOfMacros.size() );
    int cut = 0;

    for(int i = 0; i < numNetsOfMacro ; i ++ )
    {
        int numPins = netsOfMacros[i].Connection.size();
        int firstlayer = netsOfMacros[i].Connection[0]->layer;

        for(int j = 1; j < numPins; j++)
        {
            if(netsOfMacros[i].Connection[j]->layer != firstlayer)
            {
                cut++;
                break;
            }
        }    
    }

    return cut;
}

void macroRotation(vector <instance> &macros, vector <RawNet> &netsOfMacros, Die topDie)
{
    
}

void macroGradient( vector <instance> &macros, vector <RawNet> &netsOfMacros, Die topDie, int totalIter)
{
    gridInfo macroBinInfo;

    int numMacro = macros.size();
    double gamma, penaltyWeight, totalScore = 0.0, newScore = 0.0;
    double wireLength, newWireLength;
    double lastCG[ numMacro * 3 ] = {0.0};
    double nowCG[ numMacro * 3 ] = {0.0};
    double lastGra[ numMacro * 3 ] = {0.0};
    double nowGra[ numMacro * 3 ] = {0.0};

    gamma = 0.05 * topDie.upperRightX;
    
    returnGridInfo(&topDie, &macroBinInfo, numMacro);
    firstPlacement(macros, macroBinInfo, topDie);
    penaltyWeight = returnPenaltyWeight(netsOfMacros, gamma, macros, macroBinInfo);
    totalScore = returnTotalScore(netsOfMacros, gamma, macroBinInfo, penaltyWeight, macros);
    CGandGraPreprocessing(macros, nowGra, nowCG, lastGra, lastCG);

    penaltyWeight = 1.0;

    for(int i = 0; i < totalIter; i++)
    {
        for(int j = 0; j < 20; j++)
        {
            conjugateGradient(nowGra, nowCG, lastCG, lastGra, numMacro, i);

            if(j == 0)
                totalScore = newSolution(netsOfMacros, macros, penaltyWeight, gamma, nowCG, macroBinInfo);

            else
                newScore = newSolution(netsOfMacros, macros, penaltyWeight, gamma, nowCG, macroBinInfo);

            updateGra(netsOfMacros, gamma, macros, macroBinInfo, lastGra, nowGra, penaltyWeight);

            if( newScore * 0.8 < totalScore)
                totalScore = newScore;

            else
                break;	
        }

        cout << i << endl;
        penaltyWeight *= 2;
    }

}

void macroLegalization(vector <instance> &macros)
{
    int numMacro = macros.size();

    for (int i = 0; i < numMacro; i++)
    {
        macros[i].finalX = macros[i].x - macros[i].width/2;
        macros[i].finalY = macros[i].y - macros[i].height/2;
        if(macros[i].z < 0.5)
            macros[i].layer = 0;
        
        else
            macros[i].layer = 1;
    }
}
