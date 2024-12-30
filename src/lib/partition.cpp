#include <iostream>
#include <stdlib.h>
#include <time.h>

#include "partition.h"
#include "readfile.h"
#include "initial_placement.h"
#include "CG.h"
#include "legalization.h"

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
    int iii = -1;
    double gamma, penaltyWeight, totalScore = 0.0, newScore = 0.0;
    double wireLength, newWireLength;
    double lastCG[ numMacro * 3 ] = {0.0};
    double nowCG[ numMacro * 3 ] = {0.0};
    double lastGra[ numMacro * 3 ] = {0.0};
    double nowGra[ numMacro * 3 ] = {0.0};

    gamma = 0.05 * topDie.upperRightX;
    
    returnGridInfo(&topDie, &macroBinInfo, numMacro, macros);
    firstPlacement(macros, macroBinInfo, topDie);
    penaltyWeight = returnPenaltyWeight(netsOfMacros, gamma, macros, macroBinInfo);
    
    for(int i = 0; i < totalIter; i++)
    {
        totalScore = returnTotalScore(netsOfMacros, gamma, macroBinInfo, penaltyWeight, macros);
        updateGra(netsOfMacros, gamma, macros, macroBinInfo, lastGra, nowGra, penaltyWeight);
        CGandGraPreprocessing(macros, nowGra, nowCG, lastGra, lastCG);
    
        for(int j = 0; j < 40; j++)
        {
            iii++;
            conjugateGradient(nowGra, nowCG, lastCG, lastGra, numMacro, 1);
            newSolution(netsOfMacros, macros, penaltyWeight, gamma, nowCG, macroBinInfo);
            writeVisualFile(macros, iii, topDie);

            newScore = returnTotalScore( netsOfMacros, gamma, macroBinInfo, penaltyWeight, macros);

            updateGra(netsOfMacros, gamma, macros, macroBinInfo, lastGra, nowGra, penaltyWeight);
            
            if( newScore < totalScore )
                totalScore = newScore;
            else
                break;	                
        }
        penaltyWeight *= 2;
    }
    cout << iii << endl;
}
 
void macroLegalization(vector <instance> &macros, Die topDie, Die btmDie)
{

    cell2BestLayer(macros, topDie, btmDie);

    macroPlaceAndRotate(macros, topDie, btmDie);    
}

void updatePinsInMacroInfo( vector<instance> &macro, vector < vector<instance> > &pinsInMacros, vector<instance> &instances)
{
    int numMacro = macro.size();

    for(int i = 0; i < numMacro; i++)
    {
        int numPins = pinsInMacros[i].size();

        int x = macro[i].finalX;
        int y = macro[i].finalY;
        int w = macro[i].finalWidth;
        int h = macro[i].finalHeight;

        for(int j = 0; j < numPins; j++)
        {
            switch ( macro[i].rotate )
            {
                // if macro rotate = 0 
                case 0:
                    pinsInMacros[i].at(j).x = (double) x + pinsInMacros[i].at(j).finalX;
                    pinsInMacros[i].at(j).y = (double) y + pinsInMacros[i].at(j).finalY;
                    break;
                
                case 90:
                    pinsInMacros[i].at(j).x = (double) x + ( h - pinsInMacros[i].at(j).finalY );
                    pinsInMacros[i].at(j).y = (double) y + pinsInMacros[i].at(j).finalX;
                    break;
                
                case 180:
                    pinsInMacros[i].at(j).x = (double) x + ( w - pinsInMacros[i].at(j).finalX );
                    pinsInMacros[i].at(j).y = (double) y + ( h - pinsInMacros[i].at(j).finalY );
                    break;
                
                case 270:
                    pinsInMacros[i].at(j).x = (double) x + (pinsInMacros[i].at(j).finalY );
                    pinsInMacros[i].at(j).y = (double) y + (pinsInMacros[i].at(j).finalX );
                    break;

                default:
                    cout << "rotation error" << endl;
                    break;
            }
        }

        instances[ macro[i].instIndex ].width = (double) w;
        instances[ macro[i].instIndex ].height = (double) h;
        instances[ macro[i].instIndex ].x = double (macro[i].finalX + macro[i].finalWidth / 2);
        instances[ macro[i].instIndex ].y = double (macro[i].finalY + macro[i].finalHeight / 2);
        instances[ macro[i].instIndex ].z = double (macro[i].layer);
        instances[ macro[i].instIndex ].finalX = x;
        instances[ macro[i].instIndex ].finalY = y;
        instances[ macro[i].instIndex ].finalWidth = w;
        instances[ macro[i].instIndex ].finalHeight = h;
        instances[ macro[i].instIndex ].layer = macro[i].layer;

    }
}


